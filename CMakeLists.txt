cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(webScraper C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(webScraperClient main.cpp webScraperClient.cpp webScraperClient.h)
	target_compile_definitions(
        webScraperClient PRIVATE
	        WIFI_SSID="<network_ssid>" # ENTER YOUR CREDENTIALS
                WIFI_PASSWORD="<network_password>" # ENTER YOUR CREDENTIALS
        )
	target_include_directories(
        webScraperClient PRIVATE
                ${CMAKE_CURRENT_LIST_DIR}
                ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
	target_link_libraries(
        webScraperClient
                pico_cyw43_arch_lwip_threadsafe_background
                pico_stdlib
        )

	pico_add_extra_outputs(webScraperClient)
