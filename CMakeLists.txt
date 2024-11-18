cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(twt-testbench)

# Include directories for each module
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coap
    ${CMAKE_CURRENT_SOURCE_DIR}/src/profiler
    ${CMAKE_CURRENT_SOURCE_DIR}/src/wifi
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tests
)

# Main source file
target_sources(app PRIVATE src/main.c)

# CoAP module
target_sources(app PRIVATE src/coap/coap.c)

# Profiler module
target_sources(app PRIVATE src/profiler/profiler.c)

# WiFi module
target_sources(app PRIVATE
    src/wifi/wifi_ps.c
    src/wifi/wifi_sta.c
    src/wifi/wifi_twt.c
    src/wifi/wifi_utils.c
)

# Tests module
target_sources(app PRIVATE 
    src/tests/test_runner.c
    src/tests/test_sensor_twt.c
    src/tests/test_sensor_ps.c
    src/tests/test_sensor_twt_public_server.c
    src/tests/test_sensor_ps_public_server.c
    src/tests/test_large_packet_ps.c
    src/tests/test_large_packet_ps_public_server.c
)
