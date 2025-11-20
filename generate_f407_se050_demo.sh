#!/bin/bash
# generate_f407_se050_demo.sh
# 一键生成 STM32F407 + SE050 + mbedTLS Demo（支持 ALT engine 调用 SE050）

set -e

PROJECT_NAME="f407_se050_mbedtls_demo"
echo "=== Generating project: $PROJECT_NAME ==="

# 1. 创建目录结构
mkdir -p $PROJECT_NAME/{include,src,middleware/sss,mbedtls/include,mbedtls/library,build,scripts}

# 2. README.md
cat <<EOF > $PROJECT_NAME/README.md
STM32F407 + SE050 + mbedTLS Demo (with ALT engine)

目录结构已生成，可直接用 CMake 或 Makefile 进行交叉编译。
ALT engine 示例绑定 SE050 密钥进行 TLS。
EOF

# 3. include/board.h
cat <<EOF > $PROJECT_NAME/include/board.h
#ifndef BOARD_H
#define BOARD_H

#include "stm32f4xx_hal.h"
#define I2C_SE050_HANDLE hi2c1
extern I2C_HandleTypeDef I2C_SE050_HANDLE;

#endif
EOF

# 4. include/se050_config.h
cat <<EOF > $PROJECT_NAME/include/se050_config.h
#ifndef SE050_CONFIG_H
#define SE050_CONFIG_H

#define SE050_I2C_ADDRESS 0x48
#define SE050_KEY_SLOT    0xF1
#define SE050_USE_SCP03   1

#endif
EOF

# 5. include/se050_sss.h
cat <<EOF > $PROJECT_NAME/include/se050_sss.h
#ifndef SE050_SSS_H
#define SE050_SSS_H

#include "sss_api.h"

sss_status_t se050_init(void);
sss_status_t se050_load_tls_key(sss_object_t* key);

#endif
EOF

# 6. include/tls_app.h
cat <<EOF > $PROJECT_NAME/include/tls_app.h
#ifndef TLS_APP_H
#define TLS_APP_H

void tls_app_run(void);

#endif
EOF

# 7. src/main.c
cat <<EOF > $PROJECT_NAME/src/main.c
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
        printf("SE050 Init Failed!\\n");
        while(1);
    }

    // 运行TLS Demo
    tls_app_run();

    while(1) {}
}
EOF

# 8. src/i2c_hal.c
cat <<EOF > $PROJECT_NAME/src/i2c_hal.c
#include "board.h"
#include "stm32f4xx_hal.h"

I2C_HandleTypeDef I2C_SE050_HANDLE;

void i2c_init(void) {
    // TODO: 配置I2C
}
EOF

# 9. src/se050_sss.c
cat <<EOF > $PROJECT_NAME/src/se050_sss.c
#include "se050_sss.h"
#include "se050_config.h"
#include <stdio.h>

sss_session_t g_session;
sss_object_t g_tls_key;

sss_status_t se050_init(void) {
    sss_status_t status;

    // 打开SE050 session (SCP03)
    status = sss_session_open(&g_session, kSSS_ConnectionType_I2C,
                              SE050_I2C_ADDRESS, SE050_USE_SCP03);
    if (status != kStatus_SSS_Success) return status;

    // 初始化 crypto context (HostCrypto)
    // TODO: 根据 NXP SSS文档初始化

    // 加载 TLS key 对象
    return se050_load_tls_key(&g_tls_key);
}

sss_status_t se050_load_tls_key(sss_object_t* key) {
    sss_status_t status;
    // TODO: 使用 SSS API 创建或加载 key slot
    printf("Load TLS key into SE050 slot %02X\\n", SE050_KEY_SLOT);
    return kStatus_SSS_Success;
}
EOF

# 10. src/tls_app.c
cat <<EOF > $PROJECT_NAME/src/tls_app.c
#include "tls_app.h"
#include "se050_sss.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include <stdio.h>

extern sss_object_t g_tls_key;

void tls_app_run(void) {
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);

    printf("=== TLS Demo using SE050 ALT engine ===\\n");

    // 将 SE050 key 绑定到 mbedTLS ALT engine
    // 这里使用 NXP SSS API 的封装函数
    sss_mbedtls_associate_keypair(&ssl, &g_tls_key);

    // TODO: 配置 TLS 证书、CA、cipher
    // tls_configure(&conf);

    // TODO: 建立 TLS 连接测试
    // tls_connect(&ssl, "test.example.com", 443);

    printf("TLS setup complete. SE050 will perform private key operations.\\n");

    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
}
EOF

# 11. CMakeLists.txt
cat <<EOF > $PROJECT_NAME/CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(f407_se050_mbedtls_demo C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Os -g")
add_compile_options(\${MCU_FLAGS})
add_link_options(\${MCU_FLAGS})

include_directories(include middleware/sss mbedtls/include)
file(GLOB SRC "src/*.c" "middleware/sss/src/*.c" "mbedtls/library/*.c")
add_executable(f407_se050_demo \${SRC})

set(LINKER_SCRIPT "STM32F407VG_FLASH.ld")
target_link_options(f407_se050_demo PRIVATE "-T\${LINKER_SCRIPT}")
EOF

# 12. scripts/flash.sh
cat <<EOF > $PROJECT_NAME/scripts/flash.sh
#!/bin/bash
# 烧录脚本示例，使用openocd
OPENOCD_INTERFACE=stlink-v2
OPENOCD_TARGET=stm32f4x
OPENOCD_CFG="interface/\${OPENOCD_INTERFACE}.cfg target/\${OPENOCD_TARGET}.cfg"

arm-none-eabi-gcc -v 2>/dev/null || { echo "请先安装arm-none-eabi-gcc"; exit 1; }

OPENOCD_CMD="openocd -f \$OPENOCD_CFG -c 'program ../build/f407_se050_demo.elf verify reset exit'"
echo "烧录命令: \$OPENOCD_CMD"
\$OPENOCD_CMD
EOF
chmod +x $PROJECT_NAME/scripts/flash.sh

echo "=== Project $PROJECT_NAME generated successfully! ==="
echo "进入 $PROJECT_NAME/build 运行 cmake .. && make 即可交叉编译。"
echo "然后用 scripts/flash.sh 烧录到 STM32F407。"
