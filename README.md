# WMP
Water Meter Pulser counts the pulsses from water meter and send it to server every day for the 24 last houres. It also expects command from HES on short window time after sending data. 
Please use e2studo to build and debug the firmware.
renesasa chip is R7FA2E1A72DFM.
LTE QUECTEL chip is ec200s we use AT commands to control this chip.
production version is 4. hex file for programming is \phliporiginal\Debug\wmp_segger_v104.hex it includs already the boot loader+appklication.(wmp_v104) is only the hex file application.
Meter id is located at address 0x10e00. Enterring Meter number and flashing firmware use segger jlink version 11.0 at least. configuration file wpm_yos1.jflash is in \phliporiginal\Debug.
SMS is used only after powerup and for configuration only. 
Upgrading the firmware by OTAP is described in document "WMP_Protocol_V6." 


