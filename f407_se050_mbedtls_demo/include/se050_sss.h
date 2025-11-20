#ifndef SE050_SSS_H
#define SE050_SSS_H

#include "sss_api.h"

sss_status_t se050_init(void);
sss_status_t se050_load_tls_key(sss_object_t* key);

#endif
