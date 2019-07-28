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
      int bucketSize = 6;
      float sensorBucket[bucketSize];
      sensorBucket[0] = Air::instance()->GetTemperature();
      sensorBucket[1] = Air::instance()->GetHumidity();
      sensorBucket[2] = Soil::instance()->GetSoilTemperature();
      sensorBucket[3] = Soil::instance()->GetSoil();
      sensorBucket[4] = Par::instance()->GetPar();
      sensorBucket[5] = Par::instance()->GetParAcc();

      byte payload_sta_1[28];
      craftBytesArrayOfSensorPayload(1, sensorBucket, bucketSize, payload_sta_1);
      printBytes(payload_sta_1, 28);
      sensors.write(payload_sta_1, 28);

      byte payload_sta_2[28];
      craftBytesArrayOfSensorPayload(2, sensorBucket, bucketSize, payload_sta_2);
      printBytes(payload_sta_2, 28);
      sensors.write(payload_sta_2, 28);

      String stringPayload = craftStringOfSensorPayload(sensorBucket, bucketSize);
      Serial.println(stringPayload); Serial.println();
      sensors.println(stringPayload);
    }

    void craftBytesArrayOfSensorPayload(byte sta, float *sensorBucket, int bucketSize, byte* payload) {
      byte payloadSize = 28;
      payload[0] = 0xEE;
      payload[1] = sta;
      
      // payload[2-27] = sensor_payload
      int indexPayload = 2;
      for(int i = 0; i < bucketSize; i++) {
        byte value[4];
        float2Bytes(sensorBucket[i], value);
        memcpy(payload + indexPayload + sizeof(float) * i, value, 4); 
      }

      payload[27] = 0xEF;
      payload[26] = modsum(payload, payloadSize);
    }

    String craftStringOfSensorPayload(float *sensorBucket, int bucketSize) {
      String payload = "{";
      for(int i = 0; i < bucketSize; i++) {
        payload += String(sensorBucket[i]);
        payload = (i != bucketSize - 1) ? payload + "," : payload + "}";
      }
      
      return payload;
    }

    byte modsum(byte *payload, byte payloadSize) {
      int sum = 0;
      for(int i = 0; i < payloadSize; i++) {
        sum += payload[i];
      }

      return sum % 255;
    }
};
Sensors *Sensors::s_instance = NULL;