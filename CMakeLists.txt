
if (DEFINED PICO_BOARD) 
    message(STATUS "WiFi for Raspberry Pi Pico (RP2040, RP2350)")
    INCLUDE (RPxxxx_CMakeLists.txt)
elseif ( MCULIB_TARGET STREQUAL "MCUXPRESSO" )
    message(STATUS "WiFi for MCUXpresso SDK *NOT* supported yet")
    # INCLUDE (MCUXpresso_CMakeLists.txt)
else ()
    message(STATUS "Wifi for ESP32")
    # Espressif IDF CMake files are special: cannot include sub file here.
    # Configuration below has been tested with IDF V6.0.2
    idf_component_register(
          SRCS 
              "McuDNSResolver.c"	     
          INCLUDE_DIRS
              "./"
          REQUIRES
              esp_driver_gpio
         )
endif ()

