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
  * DRY
  * WET
  * SENSOR_PIN
  * RELAY_PIN

* Modify under esp8266/configNet.h the following values:
  * WIFI_SSID
  * WIFI_PASSWORD
  * SERVER_URL
  * BOT_TOKEN
  * CHAT_ID

* Upload the code to the NodeMCU

## How to use the server

* Go to http://localhost:3000 to see the data
* Go to http://localhost:3000/json to see the data in JSON format
  * You can filter the data by adding the following parameters:
    * `?sensor=name-of-the-sensor`
    * `?from=2022-01-01`
    * `?to=2022-12-31`
    * `?limit=10`
    * `?sort=asc`
    * `?sort=desc`
    * `?name=name-of-the-sensor&from=2022-01-01&to=2022-12-31&limit=10&sort=asc....`
* To push data to the server, you can use the following command:
  * `curl -X POST -H "Content-Type: application/json" -d '{"sensor": "sensor-name", "data": {"moisture": 22}}' http://localhost:3000/send-data`
