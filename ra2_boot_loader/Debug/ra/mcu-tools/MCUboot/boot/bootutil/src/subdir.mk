################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/mcu-tools/MCUboot/boot/bootutil/src/boot_record.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/bootutil_misc.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/caps.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/encrypted.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/image_ec.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/image_ec256.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/image_ed25519.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/image_rsa.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/image_validate.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/loader.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/swap_misc.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/swap_move.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/swap_scratch.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/tlv.c 

C_DEPS += \
./ra/mcu-tools/MCUboot/boot/bootutil/src/boot_record.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/bootutil_misc.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/caps.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/encrypted.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_ec.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_ec256.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_ed25519.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_rsa.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_validate.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/loader.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_misc.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_move.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_scratch.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/tlv.d 

OBJS += \
./ra/mcu-tools/MCUboot/boot/bootutil/src/boot_record.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/bootutil_misc.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/caps.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/encrypted.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_ec.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_ec256.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_ed25519.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_rsa.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_validate.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/loader.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_misc.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_move.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_scratch.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/tlv.o 

SREC += \
ra_mcuboot_ra2e1.srec 

MAP += \
ra_mcuboot_ra2e1.map 


# Each subdirectory must supply rules for building sources it contributes
ra/mcu-tools/MCUboot/boot/bootutil/src/%.o: ../ra/mcu-tools/MCUboot/boot/bootutil/src/%.c
	$(file > $@.in,-mcpu=cortex-m23 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -D_RENESAS_RA_ -D_RA_CORE=CM23 -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/src" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/fsp/inc" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/fsp/inc/api" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/fsp/inc/instances" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/arm/CMSIS_5/CMSIS/Core/Include" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_gen" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_cfg/fsp_cfg/bsp" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_cfg/fsp_cfg" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/mcu-tools/MCUboot/boot/bootutil/src" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_cfg/mcu-tools/include" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/mcu-tools/MCUboot/boot/bootutil/include" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/fsp/src/rm_mcuboot_port" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra/intel/tinycrypt/lib/include" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_cfg/mcu-tools/include/mcuboot_config" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/RA2_secure_bootloader/overwrite_no_signature/ra_mcuboot_ra2e1/ra_cfg/mcu-tools/include/sysflash" -std=c99 -w -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

