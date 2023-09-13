################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/r_wdt/r_wdt.c 

C_DEPS += \
./ra/fsp/src/r_wdt/r_wdt.d 

OBJS += \
./ra/fsp/src/r_wdt/r_wdt.o 

SREC += \
phliporiginal.srec 

MAP += \
phliporiginal.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/r_wdt/%.o: ../ra/fsp/src/r_wdt/%.c
	$(file > $@.in,-mcpu=cortex-m23 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -D_RA_CORE=CM23 -D_RENESAS_RA_ -D_RA_BOOT_IMAGE -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/src" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/inc" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/inc" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/bsp/mcu/ra2e1" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_flash_lp" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/bsp/cmsis/Device/RENESAS/Include" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/bsp/mcu" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_wdt" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/bsp" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_sci_uart" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_lvd" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/bsp/cmsis/Device" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_icu" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_rtc" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/bsp/cmsis" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/bsp/mcu/all" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_adc" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_cgc" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/bsp/cmsis/Device/RENESAS/Source" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/bsp/cmsis/Device/RENESAS" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_agt" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_lpm" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/src/r_ioport" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/inc/api" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/fsp/inc/instances" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra/arm/CMSIS_5/CMSIS/Core/Include" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra_gen" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra_cfg/fsp_cfg/bsp" -I"C:/yosif/wtaer_pulser_RA2E1/water_meter_bulk/phliporiginal/ra_cfg/fsp_cfg" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

