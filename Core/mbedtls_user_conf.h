/**
 * @file mbedtls_user_conf.h
 * @brief mbedTLS user configuration
 */

#ifndef MBEDTLS_USER_CONF_H
#define MBEDTLS_USER_CONF_H

/* System support */
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_HAVE_TIME_DATE
#define MBEDTLS_PLATFORM_MEMORY

/* mbed TLS feature support */
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_SSL_PROTO_TLS1_2

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ECP_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_ERROR_C
#define MBEDTLS_ECDSA_C
#define MBEDTLS_GCM_C
#define MBEDTLS_MD_C
#define MBEDTLS_NET_C
#define MBEDTLS_OID_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_USE_C

/* ALT implementations */
#define MBEDTLS_ECDSA_SIGN_ALT
#define MBEDTLS_ECDSA_VERIFY_ALT

/* For test certificates */
#define MBEDTLS_CERTS_C
#define MBEDTLS_PEM_PARSE_C

/* Memory buffer allocator */
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C

#endif /* MBEDTLS_USER_CONF_H */