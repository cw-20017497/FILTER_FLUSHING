
#ifndef _WIFI_SetSensorData_H_
#define _WIFI_SetSensorData_H_

typedef enum
{
    WIFI_SENSOR_0001_COLD_TEMP = 0x0001,
    WIFI_SENSOR_0002_HOT_TEMP,
    WIFI_SENSOR_0003_WATER_TEMP,
    WIFI_SENSOR_0004_OUT_TEMP,
    WIFI_SENSOR_0005_LUX_DATA,
    WIFI_SENSOR_0006_FILTER1_DATA,
    WIFI_SENSOR_0007_FILTER2_DATA,
    WIFI_SENSOR_0008_FILTER3_DATA,
    WIFI_SENSOR_0009_FILTER4_DATA,
    WIFI_SENSOR_000A_VOLTAGE_DATA,
    WIFI_SENSOR_000B_CURRENT_DATA,
    WIFI_SENSOR_000C_FACTOR_DATA,
    WIFI_SENSOR_000D_RSSI_DATA,
    WIFI_SENSOR_000E_WATT_DATA,
    WIFI_SENSOR_000F_COMP_DATA,
    WIFI_SENSOR_0010_TEM_TEMP,
    WIFI_SENSOR_0011_WATT_KRW,
    WIFI_SENSOR_0012_COLD_UP_TEMP,
    WIFI_SENSOR_0013_COLD_DOWN_TEMP,
    
    WIFI_SENSOR_NUM
}E_API_A1020_T;

void* GetWifiSensorEventList ( void );
U16 GetWifiSensorListSize ( void );

U8 IsPointData ( U8 mType, U16 mId );

#endif


