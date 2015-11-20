# ESP8266-setup
Sketch to configure your ESP8266 &amp; connect it to your WiFi network from a graphical interface

## Description

This project was made to have a way to easily configure your ESP8266 WiFi chip, and connect it to your WiFi network without the need of hard-coding your WiFi name & password in the sketch.

## Usage

1. Upload the code to your board
* Open the Serial monitor to get the IP of the board
* Connect to the WiFi network called **esp8266-setup** with your computer
* Navigate to the IP with your web browser
* Enter your WiFi name & password
* Connect to your main WiFi network again
* Get the new IP address of the board from the Serial monitor

You can of course modify the code inside the section called **Your code goes here**.

## Future Updates

* Allow the user to choose from a list of WiFi networks
* Use mDNS
* Release as an independent library
* Include the aREST library into the project
* Include jQuery in the project
