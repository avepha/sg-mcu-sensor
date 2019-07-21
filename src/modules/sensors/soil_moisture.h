#include <Task.h>
#include<math.h>
extern TaskManager taskManager;

#define dataPin 6
#define clockPin 7
SHT1x soilSensor(dataPin, clockPin);

class Soil : public Task
{
public:
  static Soil *s_instance;
  Soil() : Task(MsToTaskTime(100)){
    soil = 0;
  };
  static Soil *instance()
  {
    if (!s_instance)
      s_instance = new Soil;
    return s_instance;
  }

  float GetSoil()
  {
    return soil;
  }

  float GetSoilTemperature()
  {
    return soilTemp;
  }

  float GetSoilPotential()
  {
    float tempK = celsiusToKelvin(soilTemp);
    float soilFraction = (soil > 0) ? log(soil / 100) : 0;
    return ((8.3143 * tempK)/ (1.8 * 10E-5)) * soilFraction;
  }

  float celsiusToKelvin(int celsius) {
    return celsius + 273.15;
  }

private:
  float soil;
  float soilTemp;
  virtual bool OnStart()
  {
    return true;
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    soil = soilSensor.readHumidity();
    soilTemp = soilSensor.readTemperatureC() - 3;
  }
};
Soil *Soil::s_instance = NULL;
