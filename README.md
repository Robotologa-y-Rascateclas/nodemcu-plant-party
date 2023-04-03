NodeMCU Plant Party
===================

This is a project to monitor the health of your plants and watering them when
necessary, using a NodeMCU, a Capacity Soil Moisture Sensor v1.2, and several
other things that we will document later.

It will also send the data to a server and display it in a web page.

## Hardware

* NodeMCU
* Capacity Soil Moisture Sensor v1.2
* rPi

## How to configure and run the server

* Install Docker and Docker Compose
* Modify file `server/init-mongo.js` and change user and password there
* Go to the server directory and run `docker-compose up -d`

## How to configure and run the NodeMCU

* Modify under esp8266/config.h the following values:
  * WIFI_SSID
  * WIFI_PASSWORD
  * SERVER_URL
  * DRY
  * WET
  * SENSOR_PIN
  * RELAY_PIN

* Upload the code to the NodeMCU

