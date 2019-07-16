#include <Task.h>
extern TaskManager taskManager;
class Par : public Task
{
  public:
    static Par *s_instance;
    Par() : Task(MsToTaskTime(100)){};
    static Par *instance()
    {
      if (!s_instance)
      s_instance = new Par;
      return s_instance;
    }

    float GetPar(){
      return par;
    }

  private:
    float par;
    virtual bool OnStart()
    {
        return true;
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
      /*
        float raw = analogRead(0);
        //some equation...
      */
    int raw = analogRead(A0);
		float k = 2.514/1023; // convert to volt (Volt/ADC)
		float j = 1000; // sensitivity = 1000(Watt/Volt)
		par = (float)raw*j*k; // watt/sqm
    }
};
Par *Par::s_instance = NULL;