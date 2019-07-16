#include <Arduino.h>

#include "Task.h"
#include <Wire.h>
#include "SoftwareSerial.h"
#include "SHT1x.h"

SoftwareSerial sensors(8,9);

#include "./modules/sensors/soil_moisture.h"
#include "./modules/sensors/par.h"
#include "./modules/sensors/air.h"
#include "./modules/sensors/co2.h"
#include "./modules/sensors/light.h"
#include "./modules/sensors/sensors_transmitter.h"

TaskManager taskManager;

void setup(){
    Wire.begin();
    
    Serial.begin(9600);
    sensors.begin(9600);

    taskManager.StartTask(Air::instance());
    taskManager.StartTask(Soil::instance());
    taskManager.StartTask(CO2::instance());
    taskManager.StartTask(Par::instance());
    taskManager.StartTask(Sensors::instance());
}
void loop(){
    taskManager.Loop();
}
