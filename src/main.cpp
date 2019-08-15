#define SENSOR_STA 1
#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_TIMECRITICAL

#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TaskScheduler.h>
#include "util/haftFloat.h"
#include "util/converter.h"

#include <SHT1x.h>
#include "par.h"

float temperature = 0;
float humidity = 0;
float soilTemperature = 0;
float soil = 0;
float par = 0;
float parAccumulation = 0;

SoftwareSerial outletPort(8, 9);

SHT1x airSensor(2, 3); // air
SHT1x soilSensor(6, 7); // soil
void restoreBytesArrayToSensorPayload(byte *payload, int size, float *dePayload);
void craftBytesArrayOfSensorPayload(byte sta, uint16_t *sensorBucket, int bucketSize, byte* payload);

Scheduler scheduler;
void getSensors();
void publishSensors();
Task tSensors(2000L, TASK_FOREVER, &getSensors, &scheduler, true);
Task tPublish(0, TASK_ONCE, &publishSensors, &scheduler, false);

Par parSensor(A0);

void setup() {
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
  soilTemperature = soilSensor.readTemperatureC();
  soil = soilSensor.readHumidity();
  par = parSensor.getPar();
  parAccumulation = (parAccumulation < 10e6) ? parAccumulation + parSensor.getPar() : 0;


  Serial.print("Actual: ");
  Serial.print(String(temperature) + " ");
  Serial.print(String(humidity) + " ");
  Serial.print(String(soilTemperature) + " ");
  Serial.print(String(soil) + " ");
  Serial.print(String(par) + " ");
  Serial.print(String(parAccumulation) + " ");
  Serial.println();

  tPublish.restart();
}

void publishSensors() {
//  Serial.print(", start delayed by ");
//  Serial.println(tPublish.getStartDelay());
  int bucketSize = 6;
  uint16_t sensorBucket[bucketSize];
  sensorBucket[0] = Float16Compressor::compress(temperature);
  sensorBucket[1] = Float16Compressor::compress(humidity);
  sensorBucket[2] = Float16Compressor::compress(soilTemperature);
  sensorBucket[3] = Float16Compressor::compress(soil);
  sensorBucket[4] = Float16Compressor::compress(par);
  sensorBucket[5] = Float16Compressor::compress(parAccumulation / 1000);

  byte payload_sta_1[bucketSize * sizeof(uint16_t) + sizeof(byte) * 4];
  craftBytesArrayOfSensorPayload(SENSOR_STA, sensorBucket, bucketSize, payload_sta_1);
  outletPort.write(payload_sta_1, sizeof(payload_sta_1) / sizeof(payload_sta_1[0]));

  printBytes(payload_sta_1, sizeof(payload_sta_1) / sizeof(payload_sta_1[0]));

  float dePayload[6];
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