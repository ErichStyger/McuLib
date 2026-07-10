
if (DEFINED PICO_BOARD) 
    message(STATUS "McuLib WiFi for Raspberry Pi Pico (RP2040, RP2350)")
    INCLUDE (RPxxxx_CMakeLists.cmake)
elseif ( MCULIB_TARGET STREQUAL "MCUXPRESSO" )
    message(STATUS "McuLib WiFi for MCUXpresso SDK")
    INCLUDE (MCUXpresso_CMakeLists.cmake)
else ()
    message(STATUS "McuLib WiFi for ESP32")
    # Espressif IDF CMake files are special: cannot include sub file here.
    # Configuration below has been tested with IDF V6.0.2
    idf_component_register(
          SRCS 
              "McuWiFi.c"
              "McuUdpServerShell.c"
              "McuUdpServer.c"
              "McuUdpClientShell.c"
              "McuUdpClient.c"
              "McuTcpServer.c"
              "McuNtpClient.c"
              "McuMqttClient.c"
              "McuMqtt.c"
              "McuDnsResolver.c"
              "cJSON.c"
          INCLUDE_DIRS
              "./"
          REQUIRES
              McuLib
              esp_wifi
              wpa_supplicant
              nvs_flash
         )
endif ()
