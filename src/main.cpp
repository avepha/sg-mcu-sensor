/* There are still bugs in the system
 *  - when checksum = stop or start byte, system will be crashed.
 *  - we still can not identify whenever sensor is inactive (currently 0 is identify as inactive sensor -> not good solution)
 *  TODO: revise all logic again!!!
 *  TODO: set sensor station in build flag
 */

#define SLAVE_ID 1
#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_TIMECRITICAL
#define _TASK_PRIORITY
#define VERSION "1.0.7"

#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TaskScheduler.h>
#include <FastCRC.h>
#include "util/converter.h"
#include "./config/config.h"
#include "./util/packetUtil.h"

#include "./external_lib/k30/K30_I2C.h"
#include "./external_lib/sht1x/SHT1x.h"
#include "par.h"

#define DIR_485_PIN 8
FastCRC16 crc16;
SoftwareSerial outletPort(SG_SENSOR_RX, SG_SENSOR_TX);

Scheduler schCom, schMain;
SHT1x airSensor(AIR_SENSOR_DATA_PIN, AIR_SENSOR_CLK_PIN); // air
SHT1x soilSensor(SOIL_SENSOR_DATA_PIN, SOIL_SENSOR_CLK_PIN); // soil
Par parSensor(PAR_PIN);
K30 k30(0x34);

float temperature = 0;
float humidity = 0;
float soilTemperature = 0;
float vpd = 0;
float soil = 0;
float par = 0;
float parAccumulation = 0;
float co2 = 0;


float getVpd(float _temperature, float _humidity) {
  float spv = 610.7 * pow(10, ((7.5 * _temperature) / (237.3 + _temperature)));
  return (1 - (_humidity / 100)) * spv;
}

void fGetTemperature() {
  temperature = airSensor.readTemperatureC();
  vpd = getVpd(temperature, humidity) / 1000;
}

void fGetHumidity() {
  humidity = airSensor.readHumidity();
  vpd = getVpd(temperature, humidity) / 1000;
}

void fGetSoilTemperature() {
  soilTemperature = soilSensor.readTemperatureC();
}

void fGetSoil() {
  soil = soilSensor.readHumidity();
}

void fGetPar() {
  par = parSensor.getPar();
  parAccumulation = (parAccumulation < 10e6) ? parAccumulation + parSensor.getPar() : 0;
}

void fGetCO2() {
  int _co2;
  int rc = k30.readCO2(_co2);
  if (rc == 0) {
    co2 = _co2;
  }
}

void fCheckRequestAndResponse() {
  if (!outletPort.available()) {
    return;
  }

  byte requestPacket[50];
  uint16_t size = 0;
  uint32_t timestamp = 0;
  while (true) {
    if (outletPort.available()) {
      requestPacket[size] = outletPort.read();
      size++;

      uint32_t timeDiff = micros() - timestamp;
      if (timeDiff > 1500 && timeDiff < 4000) {
        Serial.println("[Error] invalid packet, 1.5ms > timediff < 4ms");
      }

      timestamp = micros();
    }
    else {
      if (size == 0) {
        break;
      }

      uint32_t timeDiff = micros() - timestamp;
      if (size > 0 && timeDiff >= 4000) {
        if (requestPacket[0] != SLAVE_ID) {
          break;
        }

        Serial.print("[Info] Got data: ");
        printBytes(requestPacket, size);

        byte crcByte[2] = {requestPacket[size - 2], requestPacket[size - 1]};
        uint16_t packetCrc;
        memcpy(&packetCrc, crcByte, sizeof(packetCrc));

        byte data[size - 4];
        memcpy(data, &requestPacket[2], sizeof(data));

        uint16_t recalCrc = crc16.ccitt(data, sizeof(data));

        if (recalCrc != packetCrc) {
          // crc is not match
          // response error
          Serial.println("[Error] Crc is not match");
          break;
        }
        else {
          Serial.println("[Info] Got valid packet, func: " + String(requestPacket[1], HEX));
        }

        byte funcCode = requestPacket[1];
        switch (funcCode) {
          case 0x04: {
            uint32_t sensors[8];
            sensors[0] = temperature;
            sensors[1] = humidity;
            sensors[2] = vpd;
            sensors[3] = soilTemperature;
            sensors[4] = soil;
            sensors[5] = par;
            sensors[6] = parAccumulation;
            sensors[7] = co2;
#ifdef SG_TEST
            sensors[0] = 25;
            sensors[1] = 50;
            sensors[2] = getVpd(25, 50);
            sensors[3] = 25.5;
            sensors[4] = 60.5;
            sensors[5] = 105;
            sensors[6] =+ sensors[5];
            sensors[7] = 1500;
#endif
            // response sensors
            byte packets[100];
            byte data[100];
            data[0] = 0x01; // 0x01 = type gsensor
            uint16_t dataIndex = 1;
            for (uint16_t i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
              memcpy(&data[dataIndex], &sensors[i], sizeof(sensors[i]));
              dataIndex += 4;
            }

            uint16_t packetSize = generatePacket(packets, SLAVE_ID, 0x04, data, sizeof(sensors));
            digitalWrite(SG_SENSOR_DIR, SG_SENSOR_SEND_MODE);
            outletPort.write(packets, packetSize);
            digitalWrite(SG_SENSOR_DIR, SG_SENSOR_RECV_MODE);

            Serial.print("[Info] write data: ");
            printBytes(packets, packetSize);

            break;
          }
          case 0x17: {
            // response slave id
            byte packets[100];
            byte data[100] = {SLAVE_ID};

            uint16_t packetSize = generatePacket(packets, SLAVE_ID, 0x04, data, 4);
            outletPort.write(packets, packetSize);
          }
        }

        size = 0;
      }
    }
  }
}

void fPrintSensor() {
  uint32_t sensors[8];
  sensors[0] = temperature;
  sensors[1] = humidity;
  sensors[2] = vpd;
  sensors[3] = soilTemperature;
  sensors[4] = soil;
  sensors[5] = par;
  sensors[6] = parAccumulation;
  sensors[7] = co2;

  Serial.print("read sensor:");
  for (int i = 0 ; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
    Serial.print(" ");
    Serial.print(String((float)sensors[i]));
  }
  Serial.println();
}
Task tPrintSensor(2000L, TASK_FOREVER, &fPrintSensor, &schMain, true);

Task tGetTemperature(2000L, TASK_FOREVER, &fGetTemperature, &schMain, true);
Task tGetHumidity(2050L, TASK_FOREVER, &fGetHumidity, &schMain, true);
Task tGetSoilTemperature(2200L, TASK_FOREVER, &fGetSoilTemperature, &schMain, true);
Task tGetSoil(2100L, TASK_FOREVER, &fGetSoil, &schMain, true);
Task tGetPar(2150L, TASK_FOREVER, &fGetPar, &schMain, true);
Task tGetCO2(2200L, TASK_FOREVER, &fGetCO2, &schMain, true);
Task tCheckRequestAndResponse(50, TASK_FOREVER, &fCheckRequestAndResponse, &schCom, true);


void setup() {
  analogReference(EXTERNAL);
  pinMode(SG_SENSOR_DIR, OUTPUT);
  digitalWrite(SG_SENSOR_DIR, SG_SENSOR_RECV_MODE);

  Wire.begin();
  Serial.begin(9600);
  outletPort.begin(9600);

  schMain.setHighPriorityScheduler(&schCom);

  Serial.println("initializing...");
  Serial.println("SG Version: " + SG_VERSION);
  Serial.println("BUILD VERSION: " + String(VERSION));
}

void loop() {
  schMain.execute();
}
