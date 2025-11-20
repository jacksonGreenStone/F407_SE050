/**
 * @file tls_client.c
 * @brief TLS client implementation using SE05x for private key operations
 */

#include "tls_client.h"
#include "se05x_init.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/error.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "sss_mbedtls.h"
#include <stdio.h>
#include <string.h>

/* Server configuration */
#define SERVER_NAME "httpbin.org"
#define SERVER_PORT "443"

/* Global variables for mbed TLS contexts */
static mbedtls_net_context server_fd;
static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;
static mbedtls_ssl_context ssl;
static mbedtls_ssl_config conf;

/**
 * @brief Initialize mbed TLS contexts
 * @retval 0 if successful, non-zero otherwise
 */
static int tls_init(void)
{
    int ret;
    const char *pers = "ssl_client";
    
    printf("Initializing mbed TLS contexts...\n");
    
    /* Initialize contexts */
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    
    /* Seed the RNG */
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    (const unsigned char *) pers,
                                    strlen(pers))) != 0) {
        printf("ERROR: mbedtls_ctr_drbg_seed returned -0x%04X\n", -ret);
        return -1;
    }
    
    printf("mbed TLS contexts initialized successfully\n");
    return 0;
}

/**
 * @brief Configure mbed TLS for TLS connection
 * @retval 0 if successful, non-zero otherwise
 */
static int tls_configure(void)
{
    int ret;
    sss_status_t status;
    
    printf("Configuring mbed TLS for TLS connection...\n");
    
    /* Setup TLS defaults */
    if ((ret = mbedtls_ssl_config_defaults(&conf,
                                          MBEDTLS_SSL_IS_CLIENT,
                                          MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        printf("ERROR: mbedtls_ssl_config_defaults returned -0x%04X\n", -ret);
        return -1;
    }
    
    /* Set RNG callback */
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    
    /* Associate SE05x key with mbed TLS */
    status = sss_mbedtls_associate_keypair(&ssl, &g_tls_key);
    if (status != kStatus_SSS_Success) {
        printf("ERROR: Failed to associate SE05x key with mbed TLS (status = 0x%X)\n", status);
        return -1;
    }
    
    /* Setup SSL context */
    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
        printf("ERROR: mbedtls_ssl_setup returned -0x%04X\n", -ret);
        return -1;
    }
    
    /* Set hostname for SNI */
    if ((ret = mbedtls_ssl_set_hostname(&ssl, SERVER_NAME)) != 0) {
        printf("ERROR: mbedtls_ssl_set_hostname returned -0x%04X\n", -ret);
        return -1;
    }
    
    printf("mbed TLS configured successfully\n");
    return 0;
}

/**
 * @brief Connect to TLS server
 * @retval 0 if successful, non-zero otherwise
 */
static int tls_connect(void)
{
    int ret;
    
    printf("Connecting to %s:%s...\n", SERVER_NAME, SERVER_PORT);
    
    /* Connect to server */
    if ((ret = mbedtls_net_connect(&server_fd, SERVER_NAME, SERVER_PORT, 
                                  MBEDTLS_NET_PROTO_TCP)) != 0) {
        printf("ERROR: mbedtls_net_connect returned -0x%04X\n", -ret);
        return -1;
    }
    
    /* Set BIO callbacks */
    mbedtls_ssl_set_bio(&ssl, &server_fd, 
                       mbedtls_net_send, mbedtls_net_recv, NULL);
    
    printf("Connected to server successfully\n");
    return 0;
}

/**
 * @brief Perform TLS handshake
 * @retval 0 if successful, non-zero otherwise
 */
static int tls_handshake(void)
{
    int ret;
    
    printf("Performing TLS handshake...\n");
    
    /* Perform handshake */
    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && 
            ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf("ERROR: mbedtls_ssl_handshake returned -0x%04X\n", -ret);
            return -1;
        }
    }
    
    /* Check certificate verification */
    uint32_t flags = mbedtls_ssl_get_verify_result(&ssl);
    if (flags != 0) {
        char vrfy_buf[512];
        printf("Certificate verification failed\n");
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
        printf("%s\n", vrfy_buf);
        return -1;
    } else {
        printf("Certificate verification passed\n");
    }
    
    printf("TLS handshake completed successfully\n");
    printf("Cipher suite: %s\n", mbedtls_ssl_get_ciphersuite(&ssl));
    return 0;
}

/**
 * @brief Exchange data over TLS connection
 * @retval 0 if successful, non-zero otherwise
 */
static int tls_exchange_data(void)
{
    int ret;
    size_t len;
    unsigned char buf[1024];
    
    printf("Exchanging data over TLS connection...\n");
    
    /* Send HTTP GET request */
    const char *http_request = "GET / HTTP/1.1\r\n"
                               "Host: " SERVER_NAME "\r\n"
                               "Connection: close\r\n"
                               "\r\n";
    
    printf("Sending HTTP request...\n");
    while ((ret = mbedtls_ssl_write(&ssl, (const unsigned char *)http_request, 
                                   strlen(http_request))) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && 
            ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf("ERROR: mbedtls_ssl_write returned -0x%04X\n", -ret);
            return -1;
        }
    }
    
    len = ret;
    printf("Sent %d bytes\n", len);
    
    /* Read HTTP response */
    printf("Receiving HTTP response...\n");
    do {
        memset(buf, 0, sizeof(buf));
        ret = mbedtls_ssl_read(&ssl, buf, sizeof(buf) - 1);
        
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || 
            ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;
        }
        
        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            printf("Connection was closed gracefully\n");
            break;
        }
        
        if (ret < 0) {
            printf("ERROR: mbedtls_ssl_read returned -0x%04X\n", -ret);
            break;
        }
        
        if (ret == 0) {
            printf("Connection closed by server\n");
            break;
        }
        
        len = ret;
        printf("Received %d bytes\n", len);
        
        /* Print first 256 bytes of response */
        if (len > 256) len = 256;
        buf[len] = '\0';
        printf("Response:\n%s\n", buf);
        break; /* Just read first chunk for demo */
        
    } while (1);
    
    return 0;
}

/**
 * @brief Cleanup TLS connection
 */
static void tls_cleanup(void)
{
    printf("Cleaning up TLS connection...\n");
    
    mbedtls_ssl_close_notify(&ssl);
    mbedtls_net_free(&server_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    
    printf("TLS connection cleaned up\n");
}

/**
 * @brief Run TLS client example
 * @retval 0 if successful, non-zero otherwise
 * 
 * @usage：
 *      1. 客户端使用TLS进行加密通信
 *      2. 私钥：直接存储为密钥对象，通过SE050的密钥管理API进行操作
 *      3. 证书：存储为二进制数据对象，需要通过专门的API进行存储和检索
 *      4. 使用：在TLS连接中，证书数据从SE050读取到主机内存中进行处理，而私钥操作则在SE050内部完成
 *      5. 在SE050中，证书通常以二进制数据的形式存储，而不是像密钥那样有专门的密钥对象类型。证书可以存储在SE050的通用数据对象中
 */
/*
证书存储方式
使用二进制数据对象：

证书通常存储为二进制文件数据对象
使用特定的对象ID来标识证书
可以通过sss_key_store_set_binary_data和sss_key_store_get_binary_data函数来存储和检索
证书与密钥的关联：

证书中的公钥需要与SE050中存储的私钥相关联
这通常通过使用相同的密钥ID来实现
*/
int tls_client_run(void)
{
    int ret;
    
    /* Create TLS key in SE05x */
    if (se05x_create_tls_key(TLS_KEY_ID) != 0) {
        printf("ERROR: Failed to create TLS key\n");
        return -1;
    }
    
    /* Initialize mbed TLS */
    if (tls_init() != 0) {
        printf("ERROR: Failed to initialize mbed TLS\n");
        return -1;
    }
    
    /* Configure mbed TLS */
    if (tls_configure() != 0) {
        printf("ERROR: Failed to configure mbed TLS\n");
        tls_cleanup();
        return -1;
    }
    
    /* Connect to server */
    if (tls_connect() != 0) {
        printf("ERROR: Failed to connect to server\n");
        tls_cleanup();
        return -1;
    }
    
    /* Perform TLS handshake */
    if (tls_handshake() != 0) {
        printf("ERROR: TLS handshake failed\n");
        tls_cleanup();
        return -1;
    }
    
    /* Exchange data */
    if (tls_exchange_data() != 0) {
        printf("ERROR: Data exchange failed\n");
        tls_cleanup();
        return -1;
    }
    
    /* Clean up */
    tls_cleanup();
    
    printf("TLS client example completed successfully\n");
    return 0;
}


/*
以下是一个如何在SE050中存储证书的示例：
*/
// 存储证书到SE050
sss_status_t se05x_store_certificate(sss_key_store_t *keyStore, 
                                     uint32_t certId, 
                                     const uint8_t *certData, 
                                     size_t certLen)
{
    sss_status_t status;
    sss_object_t certObject;
    
    // 初始化证书对象
    status = sss_key_object_init(&certObject, keyStore);
    if (status != kStatus_SSS_Success) {
        return status;
    }
    
    // 分配证书对象句柄（作为二进制数据）
    status = sss_key_object_allocate_handle(&certObject,
                                          certId,
                                          kSSS_KeyPart_Default,
                                          kSSS_CipherType_Binary,
                                          certLen,
                                          kSSS_KeyUsage_Preserve);
    if (status != kStatus_SSS_Success) {
        return status;
    }
    
    // 存储证书数据
    status = sss_key_store_set_binary_data(keyStore, 
                                         &certObject, 
                                         certData, 
                                         certLen);
    
    // 释放对象
    sss_key_object_free(&certObject);
    
    return status;
}

// 从SE050获取证书
sss_status_t se05x_get_certificate(sss_key_store_t *keyStore,
                                   uint32_t certId,
                                   uint8_t *certData,
                                   size_t *certLen)
{
    sss_status_t status;
    sss_object_t certObject;
    
    // 初始化证书对象
    status = sss_key_object_init(&certObject, keyStore);
    if (status != kStatus_SSS_Success) {
        return status;
    }
    
    // 获取证书对象句柄
    status = sss_key_object_get_handle(&certObject, certId);
    if (status != kStatus_SSS_Success) {
        return status;
    }
    
    // 获取证书数据
    status = sss_key_store_get_binary_data(keyStore,
                                         &certObject,
                                         certData,
                                         certLen);
    
    // 释放对象
    sss_key_object_free(&certObject);
    
    return status;
}


// 在TLS客户端配置中使用SE050中的证书和密钥
int configure_tls_with_se050_certificates(mbedtls_ssl_config *conf,
                                          sss_key_store_t *keyStore,
                                          uint32_t certId,
                                          sss_object_t *privateKey)
{
    sss_status_t status;
    mbedtls_x509_crt client_cert;
    uint8_t certData[2048]; // 根据证书大小调整
    size_t certLen = sizeof(certData);
    
    // 初始化证书结构
    mbedtls_x509_crt_init(&client_cert);
    
    // 从SE050获取证书
    status = se05x_get_certificate(keyStore, certId, certData, &certLen);
    if (status != kStatus_SSS_Success) {
        return -1;
    }
    
    // 解析证书
    int ret = mbedtls_x509_crt_parse(&client_cert, certData, certLen);
    if (ret != 0) {
        return ret;
    }
    
    // 配置客户端证书
    ret = mbedtls_ssl_conf_own_cert(conf, &client_cert, NULL);
    if (ret != 0) {
        return ret;
    }
    
    // 关联SE050私钥到mbedTLS（通过ALT API）
    sss_mbedtls_associate_keypair(conf, privateKey);
    
    return 0;
}