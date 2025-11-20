/**
 * @file se05x_init.h
 * @brief SE05x initialization functions header
 */

#ifndef SE05X_INIT_H
#define SE05X_INIT_H

#include <stdint.h>

/* Default key ID for TLS key */
#define TLS_KEY_ID 0xF0000001

/**
 * @brief Initialize SE05x secure element
 * @retval 0 if successful, non-zero otherwise
 */
int se05x_init(void);

/**
 * @brief Create or load TLS key in SE05x
 * @param key_id Key identifier in SE05x
 * @retval 0 if successful, non-zero otherwise
 */
int se05x_create_tls_key(uint32_t key_id);

#endif /* SE05X_INIT_H */