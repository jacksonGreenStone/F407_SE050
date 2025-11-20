#!/bin/bash
# 烧录脚本示例，使用openocd
OPENOCD_INTERFACE=stlink-v2
OPENOCD_TARGET=stm32f4x
OPENOCD_CFG="interface/${OPENOCD_INTERFACE}.cfg target/${OPENOCD_TARGET}.cfg"

arm-none-eabi-gcc -v 2>/dev/null || { echo "请先安装arm-none-eabi-gcc"; exit 1; }

OPENOCD_CMD="openocd -f $OPENOCD_CFG -c 'program ../build/f407_se050_demo.elf verify reset exit'"
echo "烧录命令: $OPENOCD_CMD"
$OPENOCD_CMD
