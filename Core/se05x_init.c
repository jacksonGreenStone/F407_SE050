/**
 * @file se05x_init.c
 * @brief SE05x initialization functions
 */

#include "se05x_init.h"
#include "board.h"
#include "fsl_sss_api.h"
#include <stdio.h>

/* Global variables for SE05x context */
sss_session_t g_session;        //会话对象，用于标记当前通信会话
sss_key_store_t g_key_store;    //密钥存储对象
sss_object_t g_tls_key;         //tls加密对象

/**
 * @brief Initialize SE05x secure element
 * @retval 0 if successful, non-zero otherwise
 * 
 * @usage:
 *  1.使用NXP的plug-and-trust SDK 软件框架抽象了SE050的硬件抽象层，
 *  2.即 加解密对象 mbedtls 直接与 plug-and-trust 抽象层进行通信,plug提供操作AIP；底层封装具体的SE050硬件操作。
 * 
 */
int se05x_init(void)
{
    sss_status_t status;
    
    printf("Initializing SE05x secure element...\n");
    
    /* Open SE05x session */
    status = sss_session_open(&g_session, 
                             kSSS_ConnectionType_I2C, /* I2C connection type, host com with SEO50 i2c ,maybe spi*/
                             0x48,     /* I2C address for SE050 */
                             0);       /* No SCP03 */
    
    if (status != kStatus_SSS_Success) {
        printf("ERROR: Failed to open SE05x session (status = 0x%X)\n", status);
        return -1;
    }
    
    /* Initialize key store */
    status = sss_key_store_context_init(&g_key_store, &g_session);
    if (status != kStatus_SSS_Success) {
        printf("ERROR: Failed to initialize key store (status = 0x%X)\n", status);
        sss_session_close(&g_session);
        return -1;
    }
    
    /* Load key store */
    /*
        说明：此处为抽象接口
        sss_key_store_load（）： 函数是 NXP Secure Subsystem (SSS) API 的一部分，用于加载密钥存储

        映射到：
            sss_status_t sss_se05x_key_store_load(sss_se05x_key_store_t *keyStore);
    */
    status = sss_key_store_load(&g_key_store);/* 加载密匙 from  SE050*/
    if (status != kStatus_SSS_Success) {
        printf("ERROR: Failed to load key store (status = 0x%X)\n", status);
        sss_session_close(&g_session);
        return -1;
    }
    
    /*
        加载CERT from  SE050
    
    */
    printf("SE05x secure element initialized successfully\n");
    return 0;
}

/**
 * @brief Create or load TLS key in SE05x
 * @param key_id Key identifier in SE05x
 * @retval 0 if successful, non-zero otherwise
 */
int se05x_create_tls_key(uint32_t key_id)
{
    sss_status_t status;
    
    /* Initialize key object */
    status = sss_key_object_init(&g_tls_key, &g_key_store);
    if (status != kStatus_SSS_Success) {
        printf("ERROR: Failed to initialize key object (status = 0x%X)\n", status);
        return -1;
    }
    
    /* Allocate key handle */
    status = sss_key_object_allocate_handle(&g_tls_key,
                                          key_id,
                                          kSSS_KeyPart_Pair,
                                          kSSS_CipherType_EC_NIST_P,
                                          256,
                                          kSSS_KeyUsage_Sign | kSSS_KeyUsage_Verify);
    if (status != kStatus_SSS_Success) {
        printf("ERROR: Failed to allocate key handle (status = 0x%X)\n", status);
        return -1;
    }
    
    /* Check if key already exists */
    status = sss_key_store_get_key(&g_key_store, &g_tls_key, NULL, NULL, NULL);
    if (status == kStatus_SSS_Success) {
        printf("TLS key already exists in SE05x (ID: 0x%08X)\n", key_id);
        return 0;
    }
    
    /* Generate new ECC key pair */
    status = sss_key_store_generate_key(&g_key_store, &g_tls_key, 256, NULL);
    if (status != kStatus_SSS_Success) {
        printf("ERROR: Failed to generate key (status = 0x%X)\n", status);
        return -1;
    }
    
    printf("Generated new TLS key in SE05x (ID: 0x%08X)\n", key_id);
    return 0;
}


