#Build with open source toolcain

## Prerequires

For generate project needed [premake5](https://premake.github.io)
For build arg-none-eabi [GCC toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)

## Configuration
Create file config.mk with conten
```bash
TOOLCHAINPATH="path to gcc-arm-none-eabi installed" 
```
for example:
```
TOOLCHAINPATH=$(HOME)/sdk/gcc-arm-none-eabi-9-2019-q4-major
```

## Build firmare
### Generate project
```bash
make project
```
### Build firmware binary
```bash
make release
```
### Create distribution folder
```bash
make distr
```
## Flash
For update place content of distr on root of USB drive, insert to board and power it up.