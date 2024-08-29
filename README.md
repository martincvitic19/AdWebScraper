# WebScraperClient for Raspberry Pi Pico W and WebScraperServer Linux-hosted application

## Overview

**IMOPRTANT:** This is a project I was working on when I was trying to be up to date with relevant ads. It ended up as a proof-of-concept project for educational purposes only.

Always be aware of the web scraping regulations and limitations! 

**NOTE:** I refer to the application running on host machine as a server application, while the application running on Rasperry Pi Pico W as a client application. 

This project implements a web scraper client running on a Raspberry Pi Pico W and a web scraper server running on Linux host machine. 

The server application demonstrates a web scraper that fetches and extracts data from a website using a CSS selector, saves selected entries to text files, and sends notifications via UDP to a Raspberry Pi Pico W device. The client is connected to a Wi-Fi network, listens for UDP messages from the server. When it receives a specific message indicating a new entry, it processes the message and provides visual feedback through the onboard LED.

The main problem I wanted to solve is that is scrapes the web for possible new entries on sites like Njuškalo and blinks if a new entry occurs within acceptable monetary boundaries so that I don't have to search manually check it myself. Another motivation behind doing this project was to learn mroe about the Raspberry Pi Pico W ecosystem, networking in C++ and Rust.

## Possible future improvements
- **IMPORTANT**: watch out for the scraping limitations because most sites have protections against it which may cause unexepected errors like prompting "No new elements found matching the selector." even though new entries are available
- add bash script that runs every 24 hours and cheks if new entries have arrived instead of the user running the script himself

## Features

- Connects to a specified Wi-Fi network.
- Listens for UDP messages on a specific port.
- Processes received UDP messages and triggers visual feedback.
- Handles network initialization and error reporting.

## Components

- `main.cpp`: Main application file that initializes the system, handles UDP message reception, and manages the main loop.
- `webScraperClient.cpp`: Contains functions for printing IP addresses and sending UDP beacon messages.
- `webScraperClient.h`: Header file with function prototypes and necessary includes.
- `CMakeLists.txt`: Build configuration for the project.
- `lwipopts.h`: Configuration file for the LWIP library.

## Prerequisites

- Raspberry Pi Pico W
- Raspberry Pi Debug Probe
- CMake (version 3.13 or higher)
- Pico SDK
- A Wi-Fi network to connect to

## Main concepts

1. **Initialization**: The Pico W initializes its network stack and connects to the specified Wi-Fi network.
2. **UDP Listening**: The device binds to a UDP port and listens for incoming messages from the server.
3. **Message Handling**: Upon receiving a UDP message, the device checks if it indicates a new entry. If so, it processes the message and blinks the onboard LED.

## Building the project

1. **Install the Pico SDK**: Follow the official Raspberry Pi Pico SDK setup guide.
2. **Clone the Repository**: Clone this repository to your local machine.

3. **Configure runtime variables**: open main.rs and enter the URL you want to scrape. On sites like Njuškalo, adding filters to your entries changes the URL `let url = "https://...";` and `let receiver_addr = "<IP_of_the_raspberry_pi_pico_w_client>:4444";`. Then, open main.cpp and modify the following:
`char ssid[] = "<network_ssid>";
char pass[] = "<network_password>";` 
Finnaly, modify the following

3. **Configure the Build**: Navigate to the project's build directory and run:
   ```sh
   cmake -DPICO_BOARD=pico_w -DCMAKE_BUILD_TYPE=Debug -DWIFI_SSID=<network_ssid> -DWIFI_PASSWORD=<network_password> ..
    ``` 
4. **Build the Project**: Navigate to the project directory and run:  
    ```sh
   make webScraperClient
    ``` 

## Flashing the firmware
1. **Connect Debug Probe**: Connect the Raspberry Pi Debug Probe to your Raspberry Pi Pico W.
2. **Open serial console**: I used picocom and I ran the following in one terminal:
    ```sh
    sudo picocom -b 115200 /dev/ttyACM0
    ``` 
3. **Flash the Firmware**: Open another terminal window and use the following command to flash the firmware and observe the outp:
    ```sh
    openocd -f raspberrypi-swd.cfg -f target/rp2040.cfg -c "program webScraperClient.elf reset exit"
    ``` 

## Running the server
To run the server, open a new terminal window and change directory to `web-scraper-server/src` and run `cargo run` and observe the output on the serial and watch the onboard LED blinking if new entries of interest arrived.


