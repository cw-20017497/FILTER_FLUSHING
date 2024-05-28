#ifndef __HW__H__
#define __HW__H__

#include "mcu.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_wdt.h"
#include "r_cg_timer.h"
#include "r_cg_adc.h"
#include "r_cg_serial.h"
#include "r_cg_intc.h"
#include "r_cg_macrodriver.h"


/* HW
 *
 * TIMER12-13 : PWM 
 *  -. DRAIN PUMP OUTPUT
 *
 * TIMER0 
 *  -. BASE TIMER 1ms
 *
 *  TIMER1
 *  -. STEP MOTOR 1ms
 *
 *  TIMER2
 *  -. HEATER OUT ( TRIAC )
 *  -. 8.33ms ( 120hz @ 60hz )
 *
 * PIN INTERRUPT
 *  -. FLOW METER
 *  -. INTP11
 *
 */


/* Watchdog timer reset */
#define	RESET_WDT()					R_WDT_Restart()

/* COMP POWER */
#define  P_COMP_ONOFF               P4.5
#define  TURN_ON_COMP()             do{ P_COMP_ONOFF = 1; }while(0)
#define  TURN_OFF_COMP()            do{ P_COMP_ONOFF = 0; }while(0)
#define  GET_STATUS_COMP()          ( P_COMP_ONOFF )

/* ICE FEEDER CW */
#define  P_FEEDER_CW                P4.4
#define  TURN_ON_FEEDER_CW()        do{ P_FEEDER_CW = 1; }while(0)
#define  TURN_OFF_FEEDER_CW()       do{ P_FEEDER_CW = 0; }while(0)
#define  GET_STATUS_FEEDER_CW()     ( P_FEEDER_CW )

/* ICE FEEDER CCW */
#define  P_FEEDER_CCW                P4.3
#define  TURN_ON_FEEDER_CCW()        do{ P_FEEDER_CCW = 1; }while(0)
#define  TURN_OFF_FEEDER_CCW()       do{ P_FEEDER_CCW = 0; }while(0)
#define  GET_STATUS_FEEDER_CCW()     ( P_FEEDER_CCW )

/* ICE TARY CW */
#define  P_ICE_TRAY_CW                P4.1
#define  TURN_ON_ICE_TRAY_CW()        do{ P_ICE_TRAY_CW = 1; }while(0)
#define  TURN_OFF_ICE_TRAY_CW()       do{ P_ICE_TRAY_CW = 0; }while(0)
#define  GET_STATUS_ICE_TRAY_CW()     ( P_ICE_TRAY_CW )

/* ICE TRAY CCW */
#define  P_ICE_TRAY_CCW                P4.2
#define  TURN_ON_ICE_TRAY_CCW()        do{ P_ICE_TRAY_CCW = 1; }while(0)
#define  TURN_OFF_ICE_TRAY_CCW()       do{ P_ICE_TRAY_CCW = 0; }while(0)
#define  GET_STATUS_ICE_TRAY_CCW()     ( P_ICE_TRAY_CCW )

/* SMPS 12V ON/OFF */
#define  P_SMPS_12V                    P4.7
#define  TURN_ON_SMPS_12V()            do{ P_SMPS_12V = 0; }while(0)
#define  TURN_OFF_SMPS_12V()           do{ P_SMPS_12V = 1; }while(0)
#define  GET_STATUS_SMPS_12V()         ( P_SMPS_12V )

/* DRAIN TANK HIGH LEVEL */
#define  P_DRAIN_TANK_HIGH             P8.1
#define  GET_DRAIN_TANK_HIGH()         ( P_DRAIN_TANK_HIGH )

/* DRAIN TANK LOW LEVEL */
#define  P_DRAIN_TANK_LOW              P8.0
#define  GET_DRAIN_TANK_LOW()          ( P_DRAIN_TANK_LOW )


/* STEP  MOTOR - GAS VALVE */
// A,B,/A,/B
#define  P_STEP_MOTOR_GAS_SW_1         P7.0     // A
#define  P_STEP_MOTOR_GAS_SW_2         P0.5     // B
#define  P_STEP_MOTOR_GAS_SW_3         P7.1     // /A
#define  P_STEP_MOTOR_GAS_SW_4         P0.6     // /B

#define  TURN_ON_STEP_MOTOR_GAS_SW_1()     do { P_STEP_MOTOR_GAS_SW_1 = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_GAS_SW_2()     do { P_STEP_MOTOR_GAS_SW_2 = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_GAS_SW_3()     do { P_STEP_MOTOR_GAS_SW_3 = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_GAS_SW_4()     do { P_STEP_MOTOR_GAS_SW_4 = 1; }while(0)

#define  TURN_OFF_STEP_MOTOR_GAS_SW_1()     do { P_STEP_MOTOR_GAS_SW_1 = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_GAS_SW_2()     do { P_STEP_MOTOR_GAS_SW_2 = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_GAS_SW_3()     do { P_STEP_MOTOR_GAS_SW_3 = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_GAS_SW_4()     do { P_STEP_MOTOR_GAS_SW_4 = 0; }while(0)

#define  GET_STEP_MOTOR_GAS_SW_1()     (P_STEP_MOTOR_GAS_SW_1)
#define  GET_STEP_MOTOR_GAS_SW_2()     (P_STEP_MOTOR_GAS_SW_2)
#define  GET_STEP_MOTOR_GAS_SW_3()     (P_STEP_MOTOR_GAS_SW_3)
#define  GET_STEP_MOTOR_GAS_SW_4()     (P_STEP_MOTOR_GAS_SW_4)

/* STEP MOTOR - ICE DOOR  */
#define  P_STEP_MOTOR_ICE_DOOR_4       P5.5
#define  P_STEP_MOTOR_ICE_DOOR_3       P5.6
#define  P_STEP_MOTOR_ICE_DOOR_2       P5.7
#define  P_STEP_MOTOR_ICE_DOOR_1       P1.7

#define  TURN_ON_STEP_MOTOR_ICE_DOOR_1()     do { P_STEP_MOTOR_ICE_DOOR_1 = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_ICE_DOOR_2()     do { P_STEP_MOTOR_ICE_DOOR_2 = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_ICE_DOOR_3()     do { P_STEP_MOTOR_ICE_DOOR_3 = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_ICE_DOOR_4()     do { P_STEP_MOTOR_ICE_DOOR_4 = 1; }while(0)

#define  TURN_OFF_STEP_MOTOR_ICE_DOOR_1()     do { P_STEP_MOTOR_ICE_DOOR_1 = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_ICE_DOOR_2()     do { P_STEP_MOTOR_ICE_DOOR_2 = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_ICE_DOOR_3()     do { P_STEP_MOTOR_ICE_DOOR_3 = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_ICE_DOOR_4()     do { P_STEP_MOTOR_ICE_DOOR_4 = 0; }while(0)

#define  GET_STEP_MOTOR_ICE_DOOR_4()   (P_STEP_MOTOR_ICE_DOOR_4)
#define  GET_STEP_MOTOR_ICE_DOOR_3()   (P_STEP_MOTOR_ICE_DOOR_3)
#define  GET_STEP_MOTOR_ICE_DOOR_2()   (P_STEP_MOTOR_ICE_DOOR_2)
#define  GET_STEP_MOTOR_ICE_DOOR_1()   (P_STEP_MOTOR_ICE_DOOR_1)

/* STEP MOTOR - FLOW_VALVE ( HOT WATER OUT )  */
#define  P_STEP_MOTOR_FLOW_VALVE_4      P14.2
#define  P_STEP_MOTOR_FLOW_VALVE_3      P14.1
#define  P_STEP_MOTOR_FLOW_VALVE_2      P14.0
#define  P_STEP_MOTOR_FLOW_VALVE_1      P4.6

#define  TURN_ON_STEP_MOTOR_FLOW_VALVE_1()     do { P_STEP_MOTOR_FLOW_VALVE_1 = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_FLOW_VALVE_2()     do { P_STEP_MOTOR_FLOW_VALVE_2 = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_FLOW_VALVE_3()     do { P_STEP_MOTOR_FLOW_VALVE_3 = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_FLOW_VALVE_4()     do { P_STEP_MOTOR_FLOW_VALVE_4 = 1; }while(0)

#define  TURN_OFF_STEP_MOTOR_FLOW_VALVE_1()     do { P_STEP_MOTOR_FLOW_VALVE_1 = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_FLOW_VALVE_2()     do { P_STEP_MOTOR_FLOW_VALVE_2 = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_FLOW_VALVE_3()     do { P_STEP_MOTOR_FLOW_VALVE_3 = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_FLOW_VALVE_4()     do { P_STEP_MOTOR_FLOW_VALVE_4 = 0; }while(0)

#define  GET_STEP_MOTOR_FLOW_VALVE_4()  P_STEP_MOTOR_FLOW_VALVE_4
#define  GET_STEP_MOTOR_FLOW_VALVE_3()  P_STEP_MOTOR_FLOW_VALVE_3
#define  GET_STEP_MOTOR_FLOW_VALVE_2()  P_STEP_MOTOR_FLOW_VALVE_2
#define  GET_STEP_MOTOR_FLOW_VALVE_1()  P_STEP_MOTOR_FLOW_VALVE_1

/* DRAIN PUMP PWM OUTPUT */
#define  P_DRAIN_PUMP                  P6.7
#define  TURN_ON_DRAIN_PUMP()          do{ P_DRAIN_PUMP = 1; }while(0)
#define  TURN_OFF_DRAIN_PUMP()         do{ P_DRAIN_PUMP = 0; }while(0)

/* FILTER DETECT */
#define  P_FILTER_DETECT               P6.6
#define  GET_STATUS_FILTER_DETECT()    ( P_FILTER_DETECT )


/* SWING BAR */
#define  P_SWING_BAR                   P5.0
#define  TURN_ON_SWING_BAR()           do{ P_SWING_BAR = 1; }while(0)
#define  TURN_OFF_SWING_BAR()          do{ P_SWING_BAR = 0; }while(0)
#define  GET_STATUS_SWING_BAR()        ( P_SWING_BAR )

/* UV_ICE_TRAY ON/OFF (1)*/
#define  P_UV_ONOFF_ICE_TRAY           P1.5
#define  TURN_ON_UV_ICE_TRAY()         do{ P_UV_ONOFF_ICE_TRAY = 1; }while(0)
#define  TURN_OFF_UV_ICE_TRAY()        do{ P_UV_ONOFF_ICE_TRAY = 0; }while(0)
#define  GET_STATUS_UV_ICE_TRAY()      ( P_UV_ONOFF_ICE_TRAY )

/* UV_ICE TANK ON/OFF (2)*/
#define  P_UV_ONOFF_ICE_TANK            P1.6
#define  TURN_ON_UV_ICE_TANK()          do{ P_UV_ONOFF_ICE_TANK = 1; }while(0)
#define  TURN_OFF_UV_ICE_TANK()         do{ P_UV_ONOFF_ICE_TANK = 0; }while(0)
#define  GET_STATUS_UV_ICE_TANK()       ( P_UV_ONOFF_ICE_TANK )


/* WIFI */
#define P_WIFI_ONOFF                            P0.4
#define TURN_ON_WIFI_MODULE()                  do{ P_WIFI_ONOFF = 1; }while(0)
#define TURN_OFF_WIFI_MODULE()                 do{ P_WIFI_ONOFF = 0; }while(0)

#define P_WIFI_RESET                            P0.1
#define TURN_ON_RESET_WIFI_MODULE()            do{ P_WIFI_RESET = 1; }while(0)
#define TURN_OFF_RESET_WIFI_MODULE()           do{ P_WIFI_RESET = 0; }while(0)


/* ICE TRAY */
#define  P_MICRO_SW_ICE_TRAY_2          P11.1
#define  GET_MICRO_SW_ICE_TRAY_2()      ( P_MICRO_SW_ICE_TRAY_2 )

/* ICE TRAY */
#define  P_MICRO_SW_ICE_TRAY_1          P14.6
#define  GET_MICRO_SW_ICE_TRAY_1()      ( P_MICRO_SW_ICE_TRAY_1 )

/* ICE TANK COVER REED_1 */
#define  P_REED_SW_TANK_OPEN            P1.0
#define  GET_REED_SW_TANK_OPEN()        ( P_REED_SW_TANK_OPEN )

/* ICE TANK COVER REED_2 */
#define  P_REED_SW_TANK_OPEN_2          P10.1
#define  GET_REED_SW_TANK_OPEN_2()      ( P_REED_SW_TANK_OPEN_2 )

#if 0
/* COLD TANK OVERFLOW */
#define  P_COLD_TANK_LOW                P7.5
#define  GET_COLD_TANK_LOW()            ( P_COLD_TANK_LOW )

/* COLD TANK HIGH */
#define  P_COLD_TANK_HIGH               P7.5
#define  GET_COLD_TANK_HIGH()           ( P_COLD_TANK_HIGH )

/* COLD TANK OVERFLOW */
#define  P_COLD_TANK_OVERFLOW           P10.1
#define  GET_COLD_TANK_OVERFLOW()       ( P_COLD_TANK_OVERFLOW )
#endif

/* ICE - ICE FULL */
#define  P_ICE_INFRARED                P0.0
#define  TURN_ON_ICE_INFRARED()        do{ P_ICE_INFRARED = 1; }while(0)
#define  TURN_OFF_ICE_INFRARED()       do{ P_ICE_INFRARED = 0; }while(0)
#define  GET_STATUS_ICE_INFRARED()     ( P_ICE_INFRARED )


/* LEAK ON/OFF */
#define  P_LEAK_ONOFF                P13.0
#define  TURN_ON_LEAK_ONOFF()        do{ P_LEAK_ONOFF = 0; }while(0)
#define  TURN_OFF_LEAK_ONOFF()       do{ P_LEAK_ONOFF = 1; }while(0)
#define  GET_STATUS_LEAK_ONOFF()     ( P_LEAK_ONOFF )


/* Condensor DC FAN MOTOR - 1 */
#define  P_FAN_MOTOR_1                      P5.1
#define  TURN_ON_FAN_MOTOR_1()              do{ P_FAN_MOTOR_1 = 1; }while(0)
#define  TURN_OFF_FAN_MOTOR_1()             do{ P_FAN_MOTOR_1 = 0; }while(0)
#define  GET_STATUS_FAN_MOTOR_1_ONOFF()     (P_FAN_MOTOR_1)


/* COLD TEMP ADC ON/OFF - ROOM TEMPERTURE  */
#define  P_ADC_COLD_ONOFF           P6.2
#define  TURN_ON_ADC_COLD()         do{ P_ADC_COLD_ONOFF = 1; }while(0)
#define  TURN_OFF_ADC_COLD()        do{ P_ADC_COLD_ONOFF = 0; }while(0)


/* COLD TEMP-2 ADC ON/OFF - ROOM TEMPERTURE  */
#define  P_ADC_COLD_2_ONOFF         P7.6
#define  TURN_ON_ADC_COLD_2()       do{ P_ADC_COLD_2_ONOFF = 1; }while(0)
#define  TURN_OFF_ADC_COLD_2()      do{ P_ADC_COLD_2_ONOFF = 0; }while(0)


/* ROOM ADC ON/OFF - ROOM TEMPERTURE  */
#define  P_ADC_ROOM_ONOFF           P6.3
#define  TURN_ON_ADC_ROOM()         do{ P_ADC_ROOM_ONOFF = 1; }while(0)
#define  TURN_OFF_ADC_ROOM()        do{ P_ADC_ROOM_ONOFF = 0; }while(0)

/* CODY SERVICE S/W */
#define  P_SERVICE_SW               P14.5
#define  GET_STATUS_SERVICE()       ( P_SERVICE_SW )

/* EOL COMM LINE */
#define  PM_EOL_RX                  PM14.3
#define  P_EOL_RX                   P14.3

#define  PM_EOL_TX                  PM14.4
#define  P_EOL_TX                   P14.4

/* BLDC COMP COMM LINE */
#define  PM_COMP_RX                  PM1.1
#define  P_COMP_RX                   P1.1

#define  PM_COMP_TX                  PM1.2
#define  P_COMP_TX                   P1.2


#define P_FLOW_SENSOR               P7.7
#define  GET_FLOW_SENSOR()          ( P_FLOW_SENSOR )


// RELAY
#define P_HEATER_RELAY               P7.3
#define TURN_ON_HEATER_RELAY()       do{ P_HEATER_RELAY = 1; }while(0)
#define TURN_OFF_HEATER_RELAY()      do{ P_HEATER_RELAY = 0; }while(0)
#define GET_STATUS_HEATER_RELAY()    (P_HEATER_RELAY )

// TRIAC
#define P_HEATER_TRIAC                P7.2
#define TURN_ON_HEATER_TRIAC()        do{ P_HEATER_TRIAC = 1; }while(0)
#define TURN_OFF_HEATER_TRIAC()       do{ P_HEATER_TRIAC = 0; }while(0)
#define GET_STATUS_HEATER_TRIAC()     (P_HEATER_TRIAC )

#define P_HEATER_INPUT              P13.7
#define GET_STATUS_HEATER_INPUT()     (P_HEATER_INPUT )


/* VALVE - OUT POWER - DC 24V */
// ROOM OUT, HOT OUT, COLD OUT, COLD DRAIN
#define  P_VALVE_POWER_OUT                  P7.4
#define  OPEN_VALVE_POWER_OUT()             do{ P_VALVE_POWER_OUT = 1; }while(0)
#define  CLOSE_VALVE_POWER_OUT()            do{ P_VALVE_POWER_OUT = 0; }while(0)
#define  GET_STATUS_VALVE_POWER_OUT()       ( P_VALVE_POWER_OUT )


/* VALVE - NOS */
#define  P_VALVE_NOS                        P5.4
#define  OPEN_VALVE_NOS()                   do{ P_VALVE_NOS = 0; }while(0)
#define  CLOSE_VALVE_NOS()                  do{ P_VALVE_NOS = 1; }while(0)
#define  GET_STATUS_VALVE_NOS()             ( P_VALVE_NOS )


/* VALVE - ROOM OUT */
#define  P_VALVE_ROOM_OUT                   P8.3
#define  OPEN_VALVE_ROOM_OUT()              do{ P_VALVE_ROOM_OUT = 1; }while(0)
#define  CLOSE_VALVE_ROOM_OUT()             do{ P_VALVE_ROOM_OUT = 0; }while(0)
#define  GET_STATUS_VALVE_ROOM_OUT()        ( P_VALVE_ROOM_OUT )

/* VALVE - ICE TRAY */
#define  P_VALVE_ICE_TRAY_IN                P8.6
#define  OPEN_VALVE_ICE_TRAY_IN()           do{ P_VALVE_ICE_TRAY_IN = 1; }while(0)
#define  CLOSE_VALVE_ICE_TRAY_IN()          do{ P_VALVE_ICE_TRAY_IN = 0; }while(0)
#define  GET_STATUS_VALVE_ICE_TRAY_IN()     ( P_VALVE_ICE_TRAY_IN )


/* VALVE - COLD OUT */
#define  P_VALVE_COLD_OUT                   P10.2
#define  OPEN_VALVE_COLD_OUT()              do{ P_VALVE_COLD_OUT = 1; }while(0)
#define  CLOSE_VALVE_COLD_OUT()             do{ P_VALVE_COLD_OUT = 0; }while(0)
#define  GET_STATUS_VALVE_COLD_OUT()        ( P_VALVE_COLD_OUT )

/* VALVE - COLD IN */
#define  P_VALVE_COLD_IN                    P8.2
#define  OPEN_VALVE_COLD_IN()               do{ P_VALVE_COLD_IN = 1; }while(0)
#define  CLOSE_VALVE_COLD_IN()              do{ P_VALVE_COLD_IN = 0; }while(0)
#define  GET_STATUS_VALVE_COLD_IN()         ( P_VALVE_COLD_IN )

/* VALVE - COLD AIR ( NOS ) */
#define P_VALVE_COLD_AIR                    P8.7
#define OPEN_VALVE_COLD_AIR()               do{ P_VALVE_COLD_AIR = 0; }while(0)
#define CLOSE_VALVE_COLD_AIR()              do{ P_VALVE_COLD_AIR = 1; }while(0)
#define GET_STATUS_VALVE_COLD_AIR()         ( P_VALVE_COLD_AIR )

#define P_VALVE_COLD_DRAIN                  P3.0
#define OPEN_VALVE_COLD_DRAIN()             do{ P_VALVE_COLD_DRAIN = 0; }while(0)
#define CLOSE_VALVE_COLD_DRAIN()            do{ P_VALVE_COLD_DRAIN = 1; }while(0)
#define GET_STATUS_VALVE_COLD_DRAIN()       ( P_VALVE_COLD_DRAIN )

/* VALVE - COLD FLUSHING (FEED) */
#define  P_VALVE_COLD_FLUSH                 P5.2
#define  OPEN_VALVE_COLD_FLUSH()            do{ P_VALVE_COLD_FLUSH = 0; }while(0)
#define  CLOSE_VALVE_COLD_FLUSH()           do{ P_VALVE_COLD_FLUSH = 1; }while(0)
#define  GET_STATUS_VALVE_COLD_FLUSH()      (P_VALVE_COLD_FLUSH)

/* VALVE - HOT IN */
#define  P_VALVE_HOT_IN                     P5.3
#define  OPEN_VALVE_HOT_IN()                do{ P_VALVE_HOT_IN = 1; }while(0)
#define  CLOSE_VALVE_HOT_IN()               do{ P_VALVE_HOT_IN = 0; }while(0)
#define  GET_STATUS_VALVE_HOT_IN()          ( P_VALVE_HOT_IN )

#define P_VALVE_HOT_DRAIN                   P8.4
#define OPEN_VALVE_HOT_DRAIN()              do{ P_VALVE_HOT_DRAIN = 0; }while(0)
#define CLOSE_VALVE_HOT_DRAIN()             do{ P_VALVE_HOT_DRAIN = 1; }while(0)
#define  GET_STATUS_VALVE_HOT_DRAIN()       ( P_VALVE_HOT_DRAIN )

#define P_VALVE_HOT_OUT                     P8.5
#define OPEN_VALVE_HOT_OUT()                do{ P_VALVE_HOT_OUT = 1; }while(0)
#define CLOSE_VALVE_HOT_OUT()               do{ P_VALVE_HOT_OUT = 0; }while(0)
#define  GET_STATUS_VALVE_HOT_OUT()         ( P_VALVE_HOT_OUT )


#endif /* __HW__H__ */

