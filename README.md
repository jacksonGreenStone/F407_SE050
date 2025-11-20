# STM32F407 + Plug & Trust + SE050 + mbedTLS TLS Connection Example

This example demonstrates how to establish a TLS connection using:
- STM32F407 microcontroller
- NXP Plug & Trust middleware
- SE050 secure element
- mbedTLS library

The private key operations are performed inside the SE050 secure element, providing enhanced security for TLS communications.

## Directory Structure

```
.
├── Core/                 # Core application files
│   ├── main.c           # Main entry point
│   ├── se05x_init.c     # SE050 initialization
│   └── mbedtls_user_conf.h # mbedTLS configuration
├── Drivers/              # Hardware abstraction layer drivers
│   ├── CMSIS/           # Cortex Microcontroller Software Interface Standard
│   └── STM32F4xx_HAL_Driver/ # STM32F4 HAL drivers
├── Middlewares/          # Middleware components
│   ├── mbedtls/         # mbedTLS library
│   └── plug-and-trust/  # NXP Plug & Trust middleware
│       ├── hostlib/     # Host library
│       ├── sss/         # Secure Subsystem
│       └── se05x/       # SE05x specific implementations
└── board/               # Board support files
    ├── board_I2C.c      # I2C implementation
    └── board_log.c      # Logging functions
```

## How It Works

1. The application initializes the STM32F407 hardware
2. I2C interface is configured for communication with SE050
3. SE050 secure element is initialized via Plug & Trust middleware
4. An ECC key pair is either generated or loaded in the SE050
5. mbedTLS is configured to use the SE050 key via ALT API callbacks
6. A TLS connection is established to a remote server
7. Private key operations (signing) are performed securely within SE050

## Key Features

- Secure key storage in SE050 secure element
- Hardware-accelerated cryptographic operations
- mbedTLS integration with SE050 via ALT APIs
- Example TLS client implementation

## Building the Project

### Prerequisites

1. ARM GCC toolchain
2. CMake 3.15 or higher
3. STM32CubeProgrammer (for flashing)

### Build Steps

1. Clone the repository with submodules:
   ```
   git clone --recursive https://github.com/your-repo/stm32-se050-tls-example.git
   ```

2. Create build directory:
   ```
   mkdir build && cd build
   ```

3. Configure with CMake:
   ```
   cmake ..
   ```

4. Build the project:
   ```
   make
   ```

This will generate `.elf`, `.bin`, and `.hex` files that can be flashed to the STM32F407.

## Flashing the Application

To flash the application to your STM32F407 board:

```
STM32_Programmer_CLI -c port=SWD -w build/stm32_se050_tls_client.bin 0x08000000 -v -s
```

## Hardware Requirements

- STM32F407 development board
- SE050 Arduino shield or equivalent
- Proper wiring between STM32F407 and SE050 (typically I2C lines)

## Customization

You can customize the following parameters in the code:

1. TLS server endpoint in [tls_client.c](file:///home/qcc01264/mygit/nxp_se50_demo/Core/tls_client.c):
   ```c
   #define SERVER_NAME "httpbin.org"
   #define SERVER_PORT "443"
   ```

2. SE050 key ID in [se05x_init.h](file:///home/qcc01264/mygit/nxp_se50_demo/Core/se05x_init.h):
   ```c
   #define TLS_KEY_ID 0xF0000001
   ```

3. mbedTLS configuration in [mbedtls_user_conf.h](file:///home/qcc01264/mygit/nxp_se50_demo/Core/mbedtls_user_conf.h)

## Troubleshooting

1. If the TLS handshake fails, check:
   - Network connectivity
   - SE050 connection and initialization
   - Certificate validation settings

2. If the SE050 initialization fails, check:
   - I2C wiring
   - Power supply to SE050
   - Correct I2C address (default is 0x48)

3. If you encounter build issues:
   - Ensure all submodules are properly cloned
   - Verify ARM GCC toolchain installation
   - Check that all required dependencies are present