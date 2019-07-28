#include <Arduino.h>

void printBytes(byte *values, byte size)
{
    for (int i = 0; i < size; i++)
    {
        Serial.print(values[i], HEX);
        Serial.print(' ');
    }
    Serial.println();
}

void float2Bytes(float val, byte *bytes_array)
{
    union {
        float float_variable;
        byte temp_array[4];
    } u;
    u.float_variable = val;
    memcpy(bytes_array, u.temp_array, 4);
}