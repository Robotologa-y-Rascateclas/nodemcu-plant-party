NodeMCU Plant Party
===================

This is a project to monitor the health of your plants and watering them when
necessary, using a NodeMCU, a Capacity Soil Moisture Sensor v1.2, and several
other things that we will document later.

It will also send the data to a server and display it in a web page.

You will see a graph with the moisture of the plant

<img width="600" alt="image" src="https://user-images.githubusercontent.com/709594/235131310-063c0790-97c1-4e50-b008-6ed17378c3c9.png">

A table with the sensor data sent

<img width="600" alt="image" src="https://user-images.githubusercontent.com/709594/232586280-2d8a2815-c675-48e9-9ae7-ff0f2ec3878a.png">

And a table with the events sent from the telegram bot, to note what you did, like changing deepSleep time, changing the battery or watering the plant

<img width="600" alt="image" src="https://user-images.githubusercontent.com/709594/232586758-4770dcd7-c05d-4d11-9a6a-41409981c88f.png">

## Hardware

* NodeMCU
* Capacity Soil Moisture Sensor v1.2
* Battery (not a powerbank, they usually poweroff and can't work with DeepSleep)
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
  * NUM_MEASURES
  * RELAY_PIN
  * DELAY_MEASURE_TIME
  * DELAY_DEEPSLEEP_TIME

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

## Diagrams

![image](https://github.com/Robotologa-y-Rascateclas/nodemcu-plant-party/assets/709594/91287d70-88bc-44a3-bdb5-a242cf68ec3b)
