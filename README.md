# slash-adss

## Rpi Config

Had to configure the Raspberry to be a wireless access point because the esp32
cannot find the CODE wifi network
(It is also more reliable to use an access point as it is wifi-independent).

Tutorial link:

https://www.raspberrypi.org/documentation/configuration/wireless/access-point-routed.md

## ESP32 Config

Used aREST API to send the data over to the Rpi.
The ESP serves a server, the Rpi gets the data over a http request.