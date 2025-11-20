#include "se050_sss.h"
#include "se050_config.h"
#include <stdio.h>

sss_session_t g_session;
sss_object_t g_tls_key;
#include "se050_sss.h"
#include "se050_config.h"
#include <stdio.h>
#include "fsl_sss_api.h"

sss_session_t g_session;
sss_object_t g_tls_key;
sss_asymmetric_t g_asym;

sss_status_t se050_init(void)
{
    sss_status_t status;

    // 打开SE050 session
    status = sss_session_open(&g_session, kSSS_ConnectionType_I2C,
                              SE050_I2C_ADDRESS, SE050_USE_SCP03);
    if (status != kStatus_SSS_Success)
        return status;

    // 初始化 crypto context
    status = sss_key_store_context_init(&(g_session.ks), &g_session);
    if (status != kStatus_SSS_Success)
        return status;

    // 加载 TLS key
    return se050_load_tls_key(&g_tls_key);
}

sss_status_t se050_load_tls_key(sss_object_t* key)
{
    sss_status_t status;

    sss_object_t obj;
    sss_key_store_t* ks = &(g_session.ks);

    sss_object_init(&obj, ks);

    // 在 SE050 key slot 创建 ECC key pair (P-256)
    status = sss_key_object_allocate_handle(&obj,
                                            SE050_KEY_SLOT,
                                            kSSS_KeyPart_Default,
                                            kSSS_CipherType_EC_NIST_P,
                                            256,
                                            kSSS_KeyUsage_Sign);
    if (status != kStatus_SSS_Success)
        return status;

    // 如果需要从外部导入私钥，也可以使用 sss_key_store_set_key()
    printf("TLS key loaded in SE050 slot 0x%02X\n", SE050_KEY_SLOT);

    *key = obj;

    // 初始化 ECC 上下文
    status = sss_asymmetric_context_init(&g_asym, &obj,
                                         kSSS_Algorithm_ECDSA, kSSS_Mode_Sign);
    return status;
}
