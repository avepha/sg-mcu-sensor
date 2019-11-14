/* There are still bugs in the system
 *  - when checksum = stop or start byte, system will be crashed.
 *  - we still can not identify whenever sensor is inactive (currently 0 is identify as inactive sensor -> not good solution)
 *  TODO: revise all logic again!!!
 *  TODO: set sensor station in build flag
 */

//#define SG_TEST
#define SENSOR_STA 1
#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_TIMECRITICAL

#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TaskScheduler.h>
#include "util/haftFloat.h"
#include "util/converter.h"
#include "./config/config.h"

#include "./external_lib/sht1x/SHT1x.h"
#include "par.h"

float temperature = 0;
float humidity = 0;
float soilTemperature = 0;
float vpd = 0;
float soil = 0;
float par = 0;
float parAccumulation = 0;
float co2 = 0;

#define DIR_485_PIN 8
SoftwareSerial outletPort(SG_SENSOR_RX, SG_SENSOR_TX);

SHT1x airSensor(AIR_SENSOR_DATA_PIN, AIR_SENSOR_CLK_PIN); // air
SHT1x soilSensor(SOIL_SENSOR_DATA_PIN, SOIL_SENSOR_CLK_PIN); // soil
void restoreBytesArrayToSensorPayload(byte *payload, int size, float *dePayload);
void craftBytesArrayOfSensorPayload(byte sta, uint16_t *sensorBucket, int bucketSize, byte* payload);

Scheduler scheduler;
float getVpd(float, float);
void getSensors();
void publishSensors();
Task tSensors(2000L, TASK_FOREVER, &getSensors, &scheduler, true);
Task tPublish(0, TASK_ONCE, &publishSensors, &scheduler, false);

Par parSensor(PAR_PIN);

void setup() {
  analogReference(EXTERNAL);
  pinMode(DIR_485_PIN, OUTPUT);
  digitalWrite(DIR_485_PIN, HIGH);
  Wire.begin();
  Serial.begin(9600);
  outletPort.begin(9600);

  tSensors.enableDelayed();
}

void loop() {
  scheduler.execute();
}

void getSensors() {
  temperature = airSensor.readTemperatureC();
  humidity = airSensor.readHumidity();
  vpd = getVpd(temperature, humidity) / 1000;
  soilTemperature = soilSensor.readTemperatureC();
  soil = soilSensor.readHumidity();
  par = parSensor.getPar();
  parAccumulation = (parAccumulation < 10e6) ? parAccumulation + parSensor.getPar() : 0;
  co2 = 0;

#ifdef SG_TEST
  temperature = (float)random(280, 288) / 10;
  humidity = (float)random(500, 510) / 10;
  soilTemperature = (float)random(290, 298) / 10;
  soil = (float)random(510, 520) / 10;
  par = (float)random(10, 15) / 10;
  parAccumulation = (parAccumulation < 10e6) ? parAccumulation + par : 0;
  co2 = random(1000, 1200);
#endif


  Serial.print("Actual: ");
  Serial.print(String(temperature) + " ");
  Serial.print(String(humidity) + " ");
  Serial.print(String(vpd) + " ");
  Serial.print(String(soilTemperature) + " ");
  Serial.print(String(soil) + " ");
  Serial.print(String(par) + " ");
  Serial.print(String(parAccumulation) + " ");
  Serial.print(String(co2) + " ");
  Serial.println();

  tPublish.restart();
}

void publishSensors() {
  int bucketSize = 7;
  uint16_t sensorBucket[bucketSize];
  sensorBucket[0] = Float16Compressor::compress(temperature);
  sensorBucket[1] = Float16Compressor::compress(humidity);
  sensorBucket[2] = Float16Compressor::compress(vpd);
  sensorBucket[3] = Float16Compressor::compress(soilTemperature);
  sensorBucket[4] = Float16Compressor::compress(soil);
  sensorBucket[5] = Float16Compressor::compress(par);
  sensorBucket[6] = Float16Compressor::compress(parAccumulation / 1000);
  sensorBucket[7] = Float16Compressor::compress(co2);

  byte payload_sta_1[bucketSize * sizeof(uint16_t) + sizeof(byte) * 4];
  craftBytesArrayOfSensorPayload(SENSOR_STA, sensorBucket, bucketSize, payload_sta_1);
  outletPort.write(payload_sta_1, sizeof(payload_sta_1) / sizeof(payload_sta_1[0]));

#ifdef SG_TEST
  craftBytesArrayOfSensorPayload(7, sensorBucket, bucketSize, payload_sta_1);
  outletPort.write(payload_sta_1, sizeof(payload_sta_1) / sizeof(payload_sta_1[0]));
#endif

  printBytes(payload_sta_1, sizeof(payload_sta_1) / sizeof(payload_sta_1[0]));

  float dePayload[bucketSize];
  restoreBytesArrayToSensorPayload(payload_sta_1, bucketSize * sizeof(uint16_t) + sizeof(byte) * 4, dePayload);

  Serial.print("Decompress: ");
  for(unsigned int i = 0; i < sizeof(dePayload) / sizeof(dePayload[0]); i++) {
    Serial.print(String(dePayload[i]) + " ");
  }
  Serial.println();
  Serial.println();
}

void craftBytesArrayOfSensorPayload(byte sta, uint16_t *sensorBucket, int bucketSize, byte* payload) {
  byte payloadSize = bucketSize * sizeof(uint16_t) + sizeof(byte) * 4;
  payload[0] = 0xEE;
  payload[1] = sta;
  // payload[2-12] = sensor_payload
  int indexPayload = 2;

  for(int i = 0; i < bucketSize; i++) {
    byte value[2];
    Uint16ToBytes(sensorBucket[i], value);
    memcpy(payload + indexPayload + sizeof(uint16_t) * i, value, 2);
  }

  payload[payloadSize - 1] = 0xEF;
  // get only payload to calculate checksum
  payload[payloadSize - 2] = modsum(payload + 1, payloadSize - 3);
}

float getVpd(float _temperature , float _humidity) {
  float spv = 610.7 * pow(10, ((7.5 * _temperature) / (237.3 + _temperature)));
  return (1 - (_humidity / 100)) * spv;
}