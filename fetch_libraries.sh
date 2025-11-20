#!/bin/bash
set -e

echo "=== Fetching CMSIS, mbedTLS, SE050 Plug&Trust middleware ==="

# 项目目录
BASE_DIR=$(pwd)

# 1. CMSIS
if [ ! -d "CMSIS_5" ]; then
    echo "--- Cloning CMSIS_5 ---"
    git clone --depth 1 git@github.com:ARM-software/CMSIS_5.git
else
    echo "CMSIS_5 already exists, skipping"
fi

# 2. mbedTLS
if [ ! -d "mbedtls" ]; then
    echo "--- Cloning mbedTLS ---"
    git clone --depth 1 git@github.com:Mbed-TLS/mbedtls.git
else
    echo "mbedtls already exists, skipping"
fi

# 3. SE050 Plug&Trust
if [ ! -d "plug-and-trust" ]; then
    echo "--- Cloning NXP Plug&Trust middleware ---"
    git clone --depth 1 git@github.com:NXP/plug-and-trust.git
else
    echo "plug-and-trust already exists, skipping"
fi

echo "=== All libraries downloaded ==="
echo "CMSIS path: $BASE_DIR/CMSIS_5"
echo "mbedTLS path: $BASE_DIR/mbedtls"
echo "SE050 middleware path: $BASE_DIR/plug-and-trust"
