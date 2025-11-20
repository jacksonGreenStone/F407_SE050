#include "tls_app.h"
#include "se050_sss.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/error.h"
#include <stdio.h>

extern sss_object_t g_tls_key;
extern sss_asymmetric_t g_asym;

void tls_app_run(void)
{
    int ret;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_net_context server_fd;

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_net_init(&server_fd);

    printf("=== TLS Demo using SE050 ALT engine ===\n");

    // 配置默认 TLS client
    if ((ret = mbedtls_ssl_config_defaults(&conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        printf("mbedtls_ssl_config_defaults failed: -0x%04X\n", -ret);
        return;
    }

    // 绑定 SE050 key 到 ALT engine
    // 这里 NXP 提供的函数：sss_mbedtls_associate_keypair()
    sss_mbedtls_associate_keypair(&ssl, &g_tls_key);

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
        printf("mbedtls_ssl_setup failed: -0x%04X\n", -ret);
        return;
    }

    // 连接到服务器 (示例：TLS 443)
    if ((ret = mbedtls_net_connect(&server_fd, "example.com", "443",
                                   MBEDTLS_NET_PROTO_TCP)) != 0) {
        printf("mbedtls_net_connect failed: -0x%04X\n", -ret);
        return;
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd,
                        mbedtls_net_send, mbedtls_net_recv, NULL);

    // TLS 握手
    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
            ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            char err_buf[100];
            mbedtls_strerror(ret, err_buf, 100);
            printf("mbedtls_ssl_handshake failed: %s\n", err_buf);
            return;
        }
    }

    printf("TLS handshake complete. SE050 performed private key operations.\n");

    // 可以发送测试数据
    const char* msg = "GET / HTTP/1.0\r\n\r\n";
    mbedtls_ssl_write(&ssl, (const unsigned char*)msg, strlen(msg));

    // 读取服务器响应
    unsigned char buf[512];
    ret = mbedtls_ssl_read(&ssl, buf, sizeof(buf)-1);
    if (ret > 0) {
        buf[ret] = 0;
        printf("Server response:\n%s\n", buf);
    }

    mbedtls_ssl_close_notify(&ssl);
    mbedtls_net_free(&server_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
}
