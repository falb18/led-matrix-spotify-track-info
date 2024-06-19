# LED matrix Spotify track info

This project displays the current track played on Spotify on a LED matrix.
The device gets the track information from the topics published by the program [spotify-mqtt-publisher](https://github.com/falb18/spotify-mqtt-publisher).
It is only displayed the **title of the song**, the **name of the artist** and the **name of the album**.

The project uses the following Arduino libraries:
- [MD_Parola v3.7.3](https://github.com/MajicDesigns/MD_Parola)
- [MD_MAX72XX v3.5.1](https://github.com/MajicDesigns/MD_MAX72xx)
- [MQTT v2.5.2](https://github.com/256dpi/arduino-mqtt)
- [NoDelay v2.5.2](https://github.com/M-tech-Creations/NoDelay)

The following projects were useful to develop this project:
- [ESP8266_WiFiSmartConfig](https://github.com/usemodj/ESP8266_WiFiSmartConfig)

## Setup the network

You need to install the [ESPTouch](https://github.com/EspressifApp/EsptouchForAndroid) app on your smarphone to connect
the device to the WiFi network.

The first time the device waits for the WiFi credentials. Once it has established a connection, the next time after the
reset it will try 10 times to connect to the same WiFi network. In case it fails to connect, then it will wait for the
WiFi credentials.

If the problem has to be with the WiFi network, then after solving the issue just reset the device and it will successfully
connect to the network.

## Setup the MQTT broker

You have to setup a MQTT broker. You can run it on your PC or in an external microcomputer like a Raspberry Pi.

In the Arduino code modify the broker's IP:
```arduino
IPAddress mqttBroker(xxx,xxx,xxx,xxx);
```