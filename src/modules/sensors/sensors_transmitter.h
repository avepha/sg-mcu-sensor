#include <Task.h>
extern TaskManager taskManager;

class Sensors : public Task
{
  public:
    static Sensors *s_instance;
    Sensors() : Task(MsToTaskTime(1000)){};
    static Sensors *instance()
    {
      if (!s_instance)
      s_instance = new Sensors;
      return s_instance;
    }

  private:
    virtual bool OnStart()
    {
      return true;
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
      int bucketSize = 9;
      float sensorBucket[9];
      sensorBucket[0] = Air::instance()->GetTemperature();
      sensorBucket[1] = Air::instance()->GetHumidity();
      sensorBucket[2] = Air::instance()->GetVpd();
      sensorBucket[3] = Soil::instance()->GetSoilTemperature();
      sensorBucket[4] = Soil::instance()->GetSoil();
      sensorBucket[5] = Soil::instance()->GetSoilPotential();
      sensorBucket[6] = CO2::instance()->GetCO2();
      sensorBucket[7] = Light::instance()->GetLight();
      sensorBucket[8] = Par::instance()->GetPar();
      
      String sensorPayload = craftSensorPayload(sensorBucket, bucketSize);
      Serial.println(sensorPayload);
      sensors.println(sensorPayload);
    }

    String craftSensorPayload(float *sensorBucket, int bucketSize) {
      String payload = "{";
      for(int i = 0; i < bucketSize; i++) {
        payload += String(sensorBucket[i]);
        payload = (i != bucketSize - 1) ? payload + "," : payload + "}";
      }
      
      return payload;
    }
};
Sensors *Sensors::s_instance = NULL;