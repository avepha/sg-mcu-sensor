#include <Task.h>
#include <K30_I2C.h>

K30_I2C k30_i2c = K30_I2C(0x34);
extern TaskManager taskManager;

class CO2 : public Task
{
  public:
    static CO2 *s_instance;
    CO2() : Task(MsToTaskTime(1000)){
        _ppmCO2 = 0;
        tempco2 = 0;
    };
    static CO2 *instance()
    {
      if (!s_instance)
      s_instance = new CO2;
      return s_instance;
    }

    float GetCO2(){
      return _ppmCO2;
    }

  private:
    int _ppmCO2, tempco2;
    int rc;
    virtual bool OnStart()
    {
        return true;
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        rc = k30_i2c.readCO2(_ppmCO2);
        if (rc == 0)
        {
            tempco2 = _ppmCO2;
        }
        else
        {
            _ppmCO2 = tempco2;
        }
    }
};
CO2 *CO2::s_instance = NULL;