################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/intel/tinycrypt/lib/source/aes_decrypt.c \
../ra/intel/tinycrypt/lib/source/aes_encrypt.c \
../ra/intel/tinycrypt/lib/source/cbc_mode.c \
../ra/intel/tinycrypt/lib/source/ccm_mode.c \
../ra/intel/tinycrypt/lib/source/cmac_mode.c \
../ra/intel/tinycrypt/lib/source/ctr_mode.c \
../ra/intel/tinycrypt/lib/source/ctr_prng.c \
../ra/intel/tinycrypt/lib/source/ecc.c \
../ra/intel/tinycrypt/lib/source/ecc_dh.c \
../ra/intel/tinycrypt/lib/source/ecc_dsa.c \
../ra/intel/tinycrypt/lib/source/ecc_platform_specific.c \
../ra/intel/tinycrypt/lib/source/hmac.c \
../ra/intel/tinycrypt/lib/source/hmac_prng.c \
../ra/intel/tinycrypt/lib/source/sha256.c \
../ra/intel/tinycrypt/lib/source/utils.c 

C_DEPS += \
./ra/intel/tinycrypt/lib/source/aes_decrypt.d \
./ra/intel/tinycrypt/lib/source/aes_encrypt.d \
./ra/intel/tinycrypt/lib/source/cbc_mode.d \
./ra/intel/tinycrypt/lib/source/ccm_mode.d \
./ra/intel/tinycrypt/lib/source/cmac_mode.d \
./ra/intel/tinycrypt/lib/source/ctr_mode.d \
./ra/intel/tinycrypt/lib/source/ctr_prng.d \
./ra/intel/tinycrypt/lib/source/ecc.d \
./ra/intel/tinycrypt/lib/source/ecc_dh.d \
./ra/intel/tinycrypt/lib/source/ecc_dsa.d \
./ra/intel/tinycrypt/lib/source/ecc_platform_specific.d \
./ra/intel/tinycrypt/lib/source/hmac.d \
./ra/intel/tinycrypt/lib/source/hmac_prng.d \
./ra/intel/tinycrypt/lib/source/sha256.d \
./ra/intel/tinycrypt/lib/source/utils.d 

OBJS += \
./ra/intel/tinycrypt/lib/source/aes_decrypt.o \
./ra/intel/tinycrypt/lib/source/aes_encrypt.o \
./ra/intel/tinycrypt/lib/source/cbc_mode.o \
./ra/intel/tinycrypt/lib/source/ccm_mode.o \
./ra/intel/tinycrypt/lib/source/cmac_mode.o \
./ra/intel/tinycrypt/lib/source/ctr_mode.o \
./ra/intel/tinycrypt/lib/source/ctr_prng.o \
./ra/intel/tinycrypt/lib/source/ecc.o \
./ra/intel/tinycrypt/lib/source/ecc_dh.o \
./ra/intel/tinycrypt/lib/source/ecc_dsa.o \
./ra/intel/tinycrypt/lib/source/ecc_platform_specific.o \
./ra/intel/tinycrypt/lib/source/hmac.o \
./ra/intel/tinycrypt/lib/source/hmac_prng.o \
./ra/intel/tinycrypt/lib/source/sha256.o \
./ra/intel/tinycrypt/lib/source/utils.o 

SREC += \
ra_mcuboot_ra2e1.srec 

MAP += \
ra_mcuboot_ra2e1.map 


# Each subdirectory must supply rules for building sources it contributes
ra/intel/tinycrypt/lib/source/%.o: ../ra/intel/tinycrypt/lib/source/%.c
	$(file > $@.in,-mcpu=cortex-m23 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -D_RENESAS_RA_ -D_RA_CORE=CM23 -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/src" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/fsp/inc" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/fsp/inc/api" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/fsp/inc/instances" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/arm/CMSIS_5/CMSIS/Core/Include" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_gen" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_cfg/fsp_cfg/bsp" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_cfg/fsp_cfg" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/mcu-tools/MCUboot/boot/bootutil/src" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_cfg/mcu-tools/include" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/mcu-tools/MCUboot/boot/bootutil/include" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/fsp/src/rm_mcuboot_port" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/intel/tinycrypt/lib/include" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_cfg/mcu-tools/include/mcuboot_config" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_cfg/mcu-tools/include/sysflash" -std=c99 -w -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

