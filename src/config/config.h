#ifdef SG_SENSOR_V1
const int AIR_SENSOR_DATA_PIN = 2;
const int AIR_SENSOR_CLK_PIN = 3;

const int SOIL_SENSOR_DATA_PIN = 6;
const int SOIL_SENSOR_CLK_PIN = 7;

const int PAR_PIN = A0;

const int SG_SENSOR_RX = 8;
const int SG_SENSOR_TX = 9;
const String SG_VERSION = "v1";
#endif

#ifdef SG_SENSOR_V2
const int AIR_SENSOR_DATA_PIN = 2;
const int AIR_SENSOR_CLK_PIN = 3;

const int SOIL_SENSOR_DATA_PIN = 5;
const int SOIL_SENSOR_CLK_PIN = 6;

const int PAR_PIN = A3;

const int SG_SENSOR_RX = 9;
const int SG_SENSOR_TX = 7;
const String SG_VERSION = "v2";
#endif
