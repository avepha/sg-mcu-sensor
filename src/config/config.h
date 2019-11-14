#define AIR_SENSOR_DATA_PIN 2
#define AIR_SENSOR_CLK_PIN 3

#define SOIL_SENSOR_DATA_PIN 5
#define SOIL_SENSOR_CLK_PIN 6

#define PAR_PIN A3

#define SG_SENSOR_RX 9
#define SG_SENSOR_TX 7

#ifdef SG_SENSOR_V1
#include "./v1_conf.h"
#endif

#ifdef SG_SENSOR_V2
#include "./v2_conf.h"
#endif
