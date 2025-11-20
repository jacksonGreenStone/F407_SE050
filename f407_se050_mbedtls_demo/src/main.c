#include "board.h"
#include "se050_sss.h"
#include "tls_app.h"

int main(void) {
    HAL_Init();
    // SystemClock_Config(); // 用户自行实现

    // 初始化I2C
    i2c_init();

    // 初始化SE050
    if (se050_init() != kStatus_SSS_Success) {
        printf("SE050 Init Failed!\n");
        while(1);
    }

    // 运行TLS Demo
    tls_app_run();

    while(1) {}
}
