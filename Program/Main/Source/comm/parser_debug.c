/***********************************************************************************************
 * INCLUDE 
 */
#include "hw.h"
#include "level.h"
#include "relay.h"
#include "valve.h"
#include "cold_water.h"
#include "comp.h"
#include "ice.h"
#include "ice_full.h" 
#include "ice_maker.h"
#include "temp.h"
#include "drain_pump.h"
#include "hal_adc.h"
#include "gas_switch_valve.h"
#include "hal_step_motor.h"
#include "ice_tray.h"
#include "power_saving.h"
#include "error.h"
#include "rtc.h"
#include "hal_input.h"
#include "filter.h"

#include "service.h"

#include "parser_debug.h"
#include "util.h"
#include "hal_step_motor.h"
#include "user.h"
#include "key_event_lib.h"
#include "hal_uv.h"
#include "uv.h"
#include "energy.h"
#include "flow_meter.h"
#include "front.h"
#include "hot_water.h"
#include "hot_water_out.h"
#include "hot_water_out_preheat.h"
#include "hot_water_heater.h"
#include "hot_water_flow.h"
#include "heater_out.h"
#include "flow_valve.h"
#include "flush_water_out.h"
#include "reverse_flush_water_out.h"
#include "err_comp_bldc.h"
#include "eeprom.h"
#include "hal_pump.h"


#include <stdio.h>
#include <stdlib.h>

/***********************************************************************************************
 * DEFINITION 
 */
#define MIN_PKT_SZ      6           /* HEADER + TAIL */


#define DBG_REVERSE_SAVE                       'S'

#define DBG_REVERSE_RELEASE_AIR_TIME_UP        '1'
#define DBG_REVERSE_RELEASE_AIR_TIME_DOWN      '2'
#define DBG_REVERSE_IN_AIR_TIME_UP             '3'
#define DBG_REVERSE_IN_AIR_TIME_DOWN           '4'
#define DBG_REVERSE_PRESSURE_AIR_TIME_UP       '5'
#define DBG_REVERSE_PRESSURE_AIR_TIME_DOWN     '6'
#define DBG_REVERSE_BREAK_TIME_UP              '7'
#define DBG_REVERSE_BREAK_TIME_DOWN            '8'
#define DBG_REVERSE_FLUSHING_TIME_UP           '9'
#define DBG_REVERSE_FLUSHING_TIME_DOWN         'A'
#define DBG_REVERSE_FEED_OUT_TIME_UP           'B'
#define DBG_REVERSE_FEED_OUT_TIME_DOWN         'C'
#define DBG_REVERSE_SKIP                       'D'
#define DBG_REVERSE_RESET                      'E'


extern WaterOut_T   ReverseOut;
void ParserDebugControl( U8 *buf, I16 len)
{
    U8 mu8MsgType;

    mu8MsgType = buf[0];

    if( mu8MsgType == DBG_REVERSE_SAVE )
    {
        SaveEepromId( EEP_ID_REVERSE_1 );
        SaveEepromId( EEP_ID_REVERSE_2 );
    }
    else if( mu8MsgType == DBG_REVERSE_RELEASE_AIR_TIME_UP )
    {
        REVERSE_RELEASE_AIR_TIME += 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_RELEASE_AIR_TIME_DOWN )
    {
        REVERSE_RELEASE_AIR_TIME -= 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_IN_AIR_TIME_UP      )
    {
        REVERSE_IN_AIR_TIME += 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_IN_AIR_TIME_DOWN    )
    {
        REVERSE_IN_AIR_TIME -= 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_PRESSURE_AIR_TIME_UP )
    {
        REVERSE_PRESSURE_AIR_TIME += 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_PRESSURE_AIR_TIME_DOWN )
    {
        REVERSE_PRESSURE_AIR_TIME -= 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_BREAK_TIME_UP       )
    {
        REVERSE_BREAK_TIME += 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_BREAK_TIME_DOWN     )
    {
        REVERSE_BREAK_TIME -= 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_FLUSHING_TIME_UP    )
    {
        REVERSE_FLUSHING_TIME += 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_FLUSHING_TIME_DOWN  )
    {
        REVERSE_FLUSHING_TIME -= 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_FEED_OUT_TIME_UP    )
    {
        REVERSE_FEED_OUT_TIME += 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_FEED_OUT_TIME_DOWN  )
    {
        REVERSE_FEED_OUT_TIME -= 100UL;
    }
    else if( mu8MsgType == DBG_REVERSE_SKIP  )
    {
        ReverseOut.WaitTime = 0;
    }
    else if( mu8MsgType == DBG_REVERSE_RESET  )
    {
        dbg_reverse_repeat = 0;
        SaveEepromId( EEP_ID_REVERSE_REPEAT );
    }
}

#if 0
void ParserDebugStepMotor( U8 *buf, I16 len)
{
    sm_step_t the_step;

    the_step = (sm_step_t)(buf[1] - '0') * 100;
    the_step += (sm_step_t)(buf[2] - '0') * 10;
    the_step += (sm_step_t)(buf[3] - '0') * 1;
    HAL_SetHoldStep( SM_ID_0, 0 );
    HAL_SetTargetStep( SM_ID_0, the_step );
}
#endif

#if 0
void ParserDebugTestRtc( U8 *buf, I16 len)
{
    TimeData_T  mTime;

    if( buf[1] == '4' )     //  RTC +1 year
    {
        GetRtcTime( &mTime );
        mTime.Year += 1;
        SetRtcTime( &mTime );
    }
    else if( buf[1] == '5' )    // RTC +1 Month 
    {
        GetRtcTime( &mTime );
        mTime.Month += 1;
        if( mTime.Month > 12 )
        {
            mTime.Month = 1;
        }
        SetRtcTime( &mTime );
    }
    else if( buf[1] == '6' )    // RTC +1 date
    {
        I8 mLastedDay;

        GetRtcTime( &mTime );
        mLastedDay =  GetLastDateByMonth( mTime.Year, mTime.Month );

        mTime.Date += 1;
        if( mTime.Date > mLastedDay )
        {
            mTime.Date = 1;
        }
        SetRtcTime( &mTime );
    }
}
#endif

#if 0
void ParserDebugSettingClock( U8 *buf, I16 len)
{
    I8 timeBuf[20];
    TimeData_T mTime;

    // year
    MEMSET( (void __FAR *)&timeBuf[0], '\0', sizeof( timeBuf) );
    MEMCPY( (void __FAR *)&timeBuf[0], (const void __FAR *)&buf[1], 4 );
    mTime.Y2K = 1;
    mTime.Year = (U8)( ATOI( (const void __FAR *)timeBuf ) - 2000 );

    // month
    MEMSET( (void __FAR *)&timeBuf[0], '\0', sizeof( timeBuf) );
    MEMCPY( (void __FAR *)&timeBuf[0], (const void __FAR *)&buf[6], 2 );
    mTime.Month = (U8)ATOI( (const void __FAR *)timeBuf );

    // date
    MEMSET( (void __FAR *)&timeBuf[0], '\0', sizeof( timeBuf) );
    MEMCPY( (void __FAR *)&timeBuf[0], (const void __FAR *)&buf[9], 2 );
    mTime.Date = (U8)ATOI( (const void __FAR *)timeBuf );

    // hour
    MEMSET( (void __FAR *)&timeBuf[0], '\0', sizeof( timeBuf) );
    MEMCPY( (void __FAR *)&timeBuf[0], (const void __FAR *)&buf[12], 2 );
    mTime.Hour = (U8)ATOI( (const void __FAR *)timeBuf );

    // min
    MEMSET( (void __FAR *)&timeBuf[0], '\0', sizeof( timeBuf) );
    MEMCPY( (void __FAR *)&timeBuf[0], (const void __FAR *)&buf[15], 2 );
    mTime.Min = (U8)ATOI( (const void __FAR *)timeBuf );

    // sec
    MEMSET( (void __FAR *)&timeBuf[0], '\0', sizeof( timeBuf) );
    MEMCPY( (void __FAR *)&timeBuf[0], (const void __FAR *)&buf[18], 2 );
    mTime.Sec = (U8)ATOI( (const void __FAR *)timeBuf );
    if( mTime.Sec != 59 )
    {
        mTime.Sec += 1;
    }

    mTime.DayWeek = 0;  // UNUSED

    SetRtcTime( &mTime );
}
#endif

I16 MakePkt_Debug_Reverse_1( U8 *buf, U16 mu16PktType )
{
    I16 len = 0;

   /* PACKET TYPE */
   len = SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"TYPE_1=");

   /* VALVE, PUMP */
   len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d@",
           IsOpenValve( VALVE_FILTER_FEED ),
           IsOpenValve( VALVE_FILTER_FLUSHING ),
           IsOpenValve( VALVE_FILTER_OUT ),
           HAL_IsTurnOnDrainPump(),
            HAL_GetAdcValue( ANI_PUMP_DRAIN_FB )
           );

   /* TIME VARAIBLE */
   len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%lu:%lu:%lu:%lu:%lu:%lu:%lu:%u:%d@",
           REVERSE_RELEASE_AIR_TIME / 100UL,
           REVERSE_IN_AIR_TIME / 100UL,
           REVERSE_PRESSURE_AIR_TIME /100UL,
           REVERSE_BREAK_TIME / 100UL,
           REVERSE_FLUSHING_TIME / 100UL,
           REVERSE_FEED_OUT_TIME / 100UL,
           ReverseOut.WaitTime / 100UL,
           dbg_reverse_step,
           dbg_reverse_repeat
           );


   /* END */
   len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)" \r\n" );

    return len;
}

I16 MakePkt_Debug_1( U8 *buf, U16 mu16PktType )
{
    I16 len = 0;

    FlowMeter_T mFlow;


    GetFlowMeterData( &mFlow );

   /* PACKET TYPE */
   len = SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"TYPE_1=");

   /* INIT */
   len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d:%d:%d:%d:%d@", 
           VERSION_MAJOR,
           GetSystem(),
           GetFrontSystem(),
           GetCustomerType(),
           GetColdWaterInitFull(),
           GetHotWaterInitFull(),
           GetFlushSetupInit(),
           GetFlushStatus(),
           GetFlushPowerOn()
           );

   /* VALVE */
   len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d@", 
           !GET_STATUS_VALVE_NOS(),
           GET_STATUS_VALVE_ROOM_OUT(),

           !GET_STATUS_VALVE_COLD_AIR(),
           GET_STATUS_VALVE_COLD_IN(),
           GET_STATUS_VALVE_COLD_OUT(),
           GET_STATUS_VALVE_COLD_DRAIN(),

           GET_STATUS_VALVE_HOT_IN(),
           GET_STATUS_VALVE_HOT_OUT(),
           !GET_STATUS_VALVE_HOT_DRAIN(),

           GET_STATUS_VALVE_POWER_OUT(),
           GET_STATUS_VALVE_ICE_TRAY_IN(),
           GET_STATUS_VALVE_COLD_FLUSH()
           );

   /* RELAY */
   len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d@", 
           IsTurnOnRelay( RELAY_COMP ),
           IsTurnOnRelay( RELAY_FEEDER_CW ),
           IsTurnOnRelay( RELAY_FEEDER_CCW ),
           
           IsTurnOnRelay( RELAY_ICE_TRAY_CW ),
           IsTurnOnRelay( RELAY_ICE_TRAY_CCW )
           );

   /* FLOW */
   len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%.2f:%d@",
           mFlow.Hz,
           mFlow.LPM,
           mFlow.Outage
           );

   /* INPUT */
   len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d@",
           HAL_GetInputValue( IN_TANK_OPEN ),
           GetFilterStatus( ID_FILTER_COVER ),
           GetFilterStatus( ID_FILTER )
           );


   /* END */
   len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)" \r\n" );

    return len;
}

static U8 CheckIceMakeMode( void )
{
    if( IsSetIceMakeMode( ICE_MODE_DUMMY_DEICING ) == TRUE )
    {
        return 1;
    }
    if( IsSetIceMakeMode( ICE_MODE_INIT ) == TRUE )
    {
        return 2;
    }
    if( IsSetIceMakeMode( ICE_MODE_READY ) == TRUE )
    {
        return 3;
    }
    if( IsSetIceMakeMode( ICE_MODE_ICING ) == TRUE )
    {
        return 4;
    }
    if( IsSetIceMakeMode( ICE_MODE_DEICING ) == TRUE )
    {
        return 5;
    }

    return 0x00;
}

I16 MakePkt_Debug_2( U8 *buf, U16 mu16PktType )
{
    U8 mu8Mode;
    I16 len = 0;
    
    Ice_T         mIceData;
    IceFull_T     mIceFullData;
    DoInit_T      mInitData;
    DoReady_T     mReadyData;
    DoIcing_T     mIcingData;
    DoDeIcing_T   mDeIcingData;
    IceTray_T     mIceTray;


    /* PACKET TYPE */
    len = SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"TYPE_2=");

    /* ICE */
    GetIceData( &mIceData );
    GetIceFullData( &mIceFullData );
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:", 
            mIceData.ConfigMake,
            mIceData.Make,
            mIceData.Turbo,
            CheckIceMakeMode()
            );

    /* ICE - 2 */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d:%d:%d:", 
            mIceFullData.Full,         // 만빙
            mIceFullData.Start,
            GET_STATUS_ICE_INFRARED(), // ICE FULL TURN ON 
            HAL_GetAdcValue( ANI_SENSOR_INFRARED ),   // ICE FULL 
            mIceFullData.CheckCount,
            mIceFullData.CheckTime,
            mIceData.FullCheckTimer   // 만빙 검??주기 ??간
            );

    /* ICE - 3 */
    GetIceMakerInitData( &mInitData );
    GetIceMakerReadyData( &mReadyData );
    GetIceMakerIcingData( &mIcingData );
    GetIceMakerDeIcingData( &mDeIcingData );
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%.1f:%.1f:%lu:%d:%d:", 
            mInitData.Step,
            mReadyData.Step,
            mIcingData.Step,
            GetTemp( TEMP_ID_ROOM_WATER ),
            GetTemp( TEMP_ID_AMBIENT ),
            (mReadyData.TrayFeedAmount/10UL),   
            (mIcingData.IcingTime/10U),  
            GET_STATUS_SWING_BAR()
            );

    /* ICE - 4 */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d:%d:%d@", 
            mDeIcingData.Step,
            mDeIcingData.Region,       // ??빙 ??도 ????
            mDeIcingData.RepeatNum,    // 목표 ????
            mDeIcingData.Count,        // ????????
            mDeIcingData.OnOffCmd,     // ??빙 ????ON/OFF
            (mDeIcingData.OnOffTime/10),     // ??빙 ??????????간
            mIceData.Size
            );

    /* ICE TRAY */
    GetIceTrayData( &mIceTray );
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%lu:%lu:",
            mIceTray.Posi_DeIcing,
            mIceTray.Posi_Icing,
            mIceTray.StartTime,
            mIceTray.MovingTime / 100UL
            );

    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%lu:%d:%d:%lu:%lu:%d:%d@", 
            mIceTray.RR_Count,
            mIceTray.RR_WaitTime / 100UL,
            mIceTray.RCF_Cmd,
            mIceTray.RCF_CmdCount,
            mIceTray.RCF_Time / 100UL,
            mIceTray.RFD_WaitTime / 100UL,
            mIceTray.ErrorIcingCount,
            mIceTray.ErrorDeIcingCount
            );

    /* END */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)" \r\n" );

    return len;
}

// DC FAN
I16 MakePkt_Debug_3( U8 *buf, U16 mu16PktType )
{
    I16 len = 0;
    
    ColdWater_T mColdData;
    Compressor_T  mCompData;
    DrainPump_T   mDrainData;
    GasSwitchValve_T mGasSwitch;

    /* PACKET TYPE */
    len = SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *) "TYPE_3=");

    /* COLD WATER */
    GetColdWaterData( &mColdData );
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%.1f:%.1f:%.1f:%d:%u@", 
            mColdData.ConfigMake,
            mColdData.Make,
            mColdData.TempTargetOn,
            mColdData.TempTargetOff,
            mColdData.TempCurrent,
            mColdData.ExtraMake,
            mColdData.ExtraMakeTime
            );

    /* COMP & DC FAN */
    GetCompData( &mCompData );
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%lu:%lu:%d:%d:%d:%d:%d:%d@", 
            GetCompOnOff(),          // COMP
            mCompData.ProtectOffTime,
            mCompData.OnTime,
            mCompData.OffTime,
            GET_STATUS_FAN_MOTOR_1_ONOFF(),   
            0, 
            0, //gu16FanMotorExtraTime,
            GetCompBldcTargetRps(),
            GetCompBldcCurrentRps(),
            GetCompBldcErrorCode()
            );

    /* DRAIN PUMP */
    GetDrainData( &mDrainData );
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d:%d:%d:%d@", 
            mDrainData.Cmd,
            mDrainData.RetryCount,
            mDrainData.RetryCmd,
            mDrainData.OnOffTime,
            mDrainData.Error,
            GET_DRAIN_TANK_LOW(),
            GET_DRAIN_TANK_HIGH(),
            GetTankLevel( LEVEL_ID_DRAIN )
            );

    /* GAS SWITCH VALVE */
    GetGasSwitchData( &mGasSwitch );
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d@", 
            mGasSwitch.Status,
            mGasSwitch.Mode,
            HAL_GetTargetStep( SM_ID_0 ),
            HAL_GetCurrentStep( SM_ID_0 )
            );
    /* END */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)" \r\n" );

    return len;

}

I16 MakePkt_Debug_4( U8 *buf, U16 mu16PktType )
{
    I16 len = 0;
    TimeData_T mTime;
    
    /* PACKET TYPE */
    len = SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"TYPE_4=");

    /* ADC */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%u:%u:%u:%u:%u:", 
            HAL_GetAdcValue( ANI_TEMP_HOT_IN ),  
            HAL_GetAdcValue( ANI_TEMP_HOT_OUT ),     
            HAL_GetAdcValue( ANI_TEMP_HEATER ),     
            HAL_GetAdcValue( ANI_TEMP_ROOM_WATER ), 
            HAL_GetAdcValue( ANI_TEMP_COLD_WATER )
            );

    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%u:%u:%u:%u:", 
            HAL_GetAdcValue( ANI_TEMP_AMBIENT ),
            HAL_GetAdcValue( ANI_TEMP_EVA_1 ),      
            HAL_GetAdcValue( ANI_SENSOR_LEAK ),
            HAL_GetAdcValue( ANI_DC_FAN )
            );

    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%u:%u:%u:%u:%u:", 
            HAL_GetAdcValue( ANI_SENSOR_INFRARED ),
            HAL_GetAdcValue( ANI_PUMP_DRAIN_FB ),
            HAL_GetAdcValue( ANI_UV_ICE_TANK ),         // UV TRAY
            HAL_GetAdcValue( ANI_UV_ICE_TRAY ),         // UV ICE TANK
            HAL_GetAdcValue( ANI_UV_WATER_OUT )         // UV FAUCET
            );

    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%u:%u:%u:%u:", 
            HAL_GetAdcValue( ANI_UV_ICE_DOOR ),         // UV ICE DOOR
            HAL_GetAdcValue( ANI_HEATER_FB ),     
            HAL_GetAdcValue( ANI_24V_FB ),     
            HAL_GetAdcValue( ANI_12V_FB )
            );

    /* TEMPERTURE */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f@", 
            GetTemp( TEMP_ID_HOT_IN ),
            GetTemp( TEMP_ID_HOT_OUT ),
            GetTemp( TEMP_ID_HEATER ),
            GetTemp( TEMP_ID_ROOM_WATER ),
            GetTemp( TEMP_ID_COLD_WATER ),
            GetTemp( TEMP_ID_AMBIENT ),
            GetTemp( TEMP_ID_EVA_1 )
            );

    /* RTC */
    GetRtcTime( &mTime );
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%02d:%02d:%02d:%02d:%02d:%02x@", 
            mTime.Year + 2000,
            mTime.Month,
            mTime.Date,
            mTime.Hour,
            mTime.Min,
            mTime.Sec,
            GetRtcRegStatus()
            );


    /* END */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)" \r\n" );

    return len;

}

I16 MakePkt_Debug_5( U8 *buf, U16 mu16PktType )
{
    I16 len = 0;
    PowerSaving_T mSavingData;
    Service_T   mServiceData;


    GetSavingData( &mSavingData );
    
    /* PACKET TYPE */
    len = SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"TYPE_5=");

    /* POWER SAVING */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d@",
            mSavingData.Config,
            mSavingData.Status
            );
#if 0
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:",
            mSavingData.Config,
            mSavingData.Status,
            0,//mSavingData.Dark2Light,
            0//mSavingData.Light2Dark
            );

    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d:%d@",
            0, //mSavingData.IsDark,
            0, //HAL_GetAdcValue( ANI_SAVING_PHOTO ),
            0, //mSavingData.AdcLimitSleep,
            0, //mSavingData.AdcLimitWakeUp,
            0, //mSavingData.TimerSleep,
            0 //mSavingData.TimerWakeUp
            );
#endif

    /* ERROR */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d:",
            IsError( ERR_TEMP_COLD_WATER ),
            IsError( ERR_TEMP_ROOM_WATER ),
            IsError( ERR_TEMP_AMBIENT ),
            IsError( ERR_TEMP_HOT_WATER_IN ),
            IsError( ERR_TEMP_HOT_WATER )
            );

    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:",
            IsError( ERR_TEMP_HEATER ),
            IsError( ERR_TEMP_EVA_1 ),
            IsError( ERR_LEAK ),
            IsError( ERR_MICRO_SW_DETECT )
            );

    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d:%d:%d:%d@",
            IsError( ERR_MICRO_SW_MOVE ),
            IsError( ERR_DRAIN_PUMP ),
            IsError( ERR_COMP ),
            IsError( ERR_COMP_COMM ),
            IsError( ERR_OUTAGE_WATER ),
            IsError( ERR_OUTAGE_HOT_WATER ),
            IsError( ERR_HEATER_OVERHEAT ),
            IsError( ERR_LEAK_FLOW )
            );

    /* ERROR - COMP ERR */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d:%d:%d:%d:%d:",
            GetCompBldcErrorCode(),
            GetCompBldcLastedErrorCode(),
            GetErrCompBldcCount( ERR_COMP_PRT, ERR_BLDC_STARTING_FAIL ),
            GetErrCompBldcCount( ERR_COMP_PRT, ERR_BLDC_CURRENT_SENSING_ERROR ),
            GetErrCompBldcCount( ERR_COMP_PRT, ERR_BLDC_OVER_CURRENT ),
            GetErrCompBldcCount( ERR_COMP_PRT, ERR_BLDC_ABNORMAL_VOLTAGE ),
            GetErrCompBldcCount( ERR_COMP_PRT, ERR_BLDC_OVERHEAT_CONTROLLER ),
            GetErrCompBldcCount( ERR_COMP_PRT, ERR_BLDC_DISCONNECTION_COMM ),
            GetErrCompBldcCount( ERR_COMP_PRT, ERR_BLDC_OUT_OF_RANGE )
            );

    /* ERROR - COMP PRT */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%d:%d:%d@",
            GetErrCompBldcCount( ERR_COMP_ERR, ERR_BLDC_STARTING_FAIL ),
            GetErrCompBldcCount( ERR_COMP_ERR, ERR_BLDC_CURRENT_SENSING_ERROR ),
            GetErrCompBldcCount( ERR_COMP_ERR, ERR_BLDC_OVER_CURRENT ),
            GetErrCompBldcCount( ERR_COMP_ERR, ERR_BLDC_ABNORMAL_VOLTAGE ),
            GetErrCompBldcCount( ERR_COMP_ERR, ERR_BLDC_OVERHEAT_CONTROLLER ),
            GetErrCompBldcCount( ERR_COMP_ERR, ERR_BLDC_DISCONNECTION_COMM ),
            GetErrCompBldcCount( ERR_COMP_ERR, ERR_BLDC_OUT_OF_RANGE )
            );

    /* Service */
    GetServiceCheckData( &mServiceData );
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%lu:%lu@",
            mServiceData.Status,
            mServiceData.TankOpen,
            mServiceData.Time,
            mServiceData.SaveTime
            );


    /* END */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)" \r\n" );


    return len;
}

extern PreHeat_T PreHeat;   // hot out preheating...
extern AverageAdc_T HeaterAdc;
extern HeaterOffset_T  HeaterOffset;
extern HotWaterFlow_T HeaterFlow;
#if 1
I16 MakePkt_Debug_6( U8 *buf, U16 mu16PktType )
{
    I16 len = 0;
    HotOut_T mHotOut;
    HeaterOut_T mHeaterOut;
    FlowValve_T mFlowValve;
    HotWaterHeater_T mHeater;


    GetHotOutData( &mHotOut );
    GetHeaterOutData( &mHeaterOut );
    GetFlowValveData( &mFlowValve );
    GetHotWaterHeaterData( &mHeater );

    /* PACKET TYPE */
    len = SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"TYPE_6=");

    // HOT OUT
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%.1f:%.1f@",
            GetLockHot(),
            GetHotWaterInitFull(),
            mHotOut.TargetOutTemp,
            mHotOut.TargetPreHeatTemp
            );


    // HEATER
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d@",
            mHeaterOut.Output,
            GET_STATUS_HEATER_RELAY(),
            GET_STATUS_HEATER_TRIAC(),
            HAL_GetCurrentStep( SM_ID_2 )
            );


    // UV
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d@",
            HAL_GetUvOnOffStatus( HAL_UV_ICE_TANK ),
            HAL_GetUvOnOffStatus( HAL_UV_ICE_TRAY ),
            HAL_GetUvOnOffStatus( HAL_UV_ICE_DOOR ),
            HAL_GetUvOnOffStatus( HAL_UV_WATER_OUT )
            );

    // TEMP SENSOR TIME
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d@",
            GetTempSensorTime( TEMP_ID_COLD_WATER ),
            GetTempSensorTime( TEMP_ID_ROOM_WATER )
            );

    // HEATER
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%d:%d:%.2f@",
            mHeater.Class,
            mHeater.RawWatt,
            mHeater.Watt,
            mHeater.TargetWatt,
            HeaterFlow.TargetLPM
            );

    /* END */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)" \r\n" );

    return len;
}
#endif

// 플러싱 
#if 0
I16 MakePkt_Debug_8( U8 *buf, U16 mu16PktType )
{
    I16 len = 0;


    /* PACKET TYPE */
    len = SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"TYPE_8=");

    // FLUSH OUT
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%d:%.1f:%.1f:%d:%d@",
            Flush.Status,
            Flush.TargetAmount,
            Flush.FilterTargetAmount,
            Flush.CurrentAmount,
            Flush.FilterTimeOut,
            Flush.ColdTimeOut,
            Flush.DoneWaitTime,

            );



    /* END */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)" \r\n" );

}
#endif

#if 0
I16 MakePkt_Debug_7( U8 *buf, U16 mu16PktType )
{
    I16 len = 0;
    UserInfo_T mUser;


    GetUserData( &mUser );
    
    /* PACKET TYPE */
    len = SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"TYPE_7=");

    // ENERGY
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)"%d:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu:%lu@",
            GetEnergyMonth(),
            GetEnergyWattMonth( 1 ) / 1000000UL , GetEnergyRateMonth( 1 ),
            GetEnergyWattMonth( 2 ) / 1000000UL , GetEnergyRateMonth( 2 ),
            GetEnergyWattMonth( 3 ) / 1000000UL , GetEnergyRateMonth( 3 ),
            GetEnergyWattMonth( 4 ) / 1000000UL , GetEnergyRateMonth( 4 ),
            GetEnergyWattMonth( 5 ) / 1000000UL , GetEnergyRateMonth( 5 ),
            GetEnergyWattMonth( 6 ) / 1000000UL , GetEnergyRateMonth( 6 ),
            GetEnergyWattMonth( 7 ) / 1000000UL , GetEnergyRateMonth( 7 ),
            GetEnergyWattMonth( 8 ) / 1000000UL , GetEnergyRateMonth( 8 ),
            GetEnergyWattMonth( 9 ) / 1000000UL , GetEnergyRateMonth( 9 ),
            GetEnergyWattMonth( 10 ) / 1000000UL, GetEnergyRateMonth( 10 ),
            GetEnergyWattMonth( 11 ) / 1000000UL, GetEnergyRateMonth( 11 ),
            GetEnergyWattMonth( 12 ) / 1000000UL, GetEnergyRateMonth( 12 )
            );


    /* END */
    len += SPRINTF( (char __FAR *)&buf[ len ], (const char __FAR *)" \r\n" );

    return len;
}
#endif
