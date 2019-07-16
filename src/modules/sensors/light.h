#include <Task.h>
extern TaskManager taskManager;
#include "TSL2561.h"
TSL2561 tsl(TSL2561_ADDR_FLOAT);

class Light : public Task
{
public:
  static Light *s_instance;
  Light() : Task(MsToTaskTime(1000))
  {
    value = 0;
    tsl.setGain(TSL2561_GAIN_0X);
    tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);
  };
  static Light *instance()
  {
    if (!s_instance)
      s_instance = new Light;
    return s_instance;
  }
  
  float GetLight(){
    return value;
  }

private:
  float value;
  virtual bool OnStart()
  {
    value = 0;
    return true;
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir, full;
    ir = lum >> 16;
    full = lum & 0xFFFF;
    value = tsl.calculateLux(full, ir);
  }
};
Light *Light::s_instance = NULL;