
#include "WIFI_Common.h"
#include "WIFI_SetFunctionData.h"
#include "WIFI_SetSensorData.h"
#include "WIFI_SetErrorData.h"
#include "WIFI_SetPartData.h"
#include "WIFI_SetAccumulateData.h"
#include "WIFI_UserInterface_IconAIS.h"
#include "WIFI_Control.h"

/* USER */
#include "sound.h"
#include "front.h"
#include "eol.h"
#include "fct.h"
#include "rtc.h"
#include "error.h"
#include "filter.h"
#include "flush_water_out.h"
#include "cold_water.h"
#include "ice.h"
#include "process_display.h"
#include "hot_water.h"
#include "hot_water_out.h"
#include "water_out.h"
#include "eeprom.h"
#include "temp.h"
#include "power_saving.h"
#include "energy.h"
#include "user.h"
#include "ice_door.h"
#include "uv.h"
#include "voice.h"
#include "service.h"
#include "smart_check.h"
#include "diagnosis.h"

#ifdef MODEL_NO_1

#define PROGRESS_ID_UV_WATER_OUT    0
#define PROGRESS_ID_UV_ICE_OUT      1
#define PROGRESS_ID_UV_ICE_TRAY     2
#define PROGRESS_ID_UV_ICE_TANK     3
#define PROGRESS_ID_SILVER_CARE     4
#define PROGRESS_ID_DRAIN_WATER     5
#define MAX_PROGRESS_NUM            6
static U16 GetProgressVal(U8 id);


WIFI_Status_T Wifi;

void UserWriteEep ( U16 mu16Addr, U8 mu8Val )
{
    if( mu16Addr == EEPROM_ADDR_WIFI_POWER )
    {
        Wifi.OnOff  = mu8Val;
        SaveEepromId ( EEP_ID_WIFI_POWER );
    }
    else if( mu16Addr == EEPROM_ADDR_FIRST_PARING )
    {
        Wifi.FirstParing = mu8Val;
        SaveEepromId ( EEP_ID_WIFI_FIRST_PARING );
    }
    else if( mu16Addr == EEPROM_ADDR_FOTA_MODULE )
    {
        Wifi.FotaModule = mu8Val;
        SaveEepromId ( EEP_ID_WIFI_FOTA_MODULE );
    }
    else if( mu16Addr == EEPROM_ADDR_FOTA_MCU )
    {
        Wifi.FotaMain   = mu8Val;
        SaveEepromId ( EEP_ID_WIFI_FOTA_MCU );
    }
}

U8 UserReadEep ( U16 mu16Addr )
{
    if( mu16Addr == EEPROM_ADDR_WIFI_POWER )
    {
        return Wifi.OnOff;
    }
    else if( mu16Addr == EEPROM_ADDR_FIRST_PARING )
    {
        return Wifi.FirstParing;
    }
    else if( mu16Addr == EEPROM_ADDR_FOTA_MODULE )
    {
        return Wifi.FotaModule;
    }
    else if( mu16Addr == EEPROM_ADDR_FOTA_MCU )
    {
        return Wifi.FotaMain;
    }

    return 0;
}

void UserBuzzSound ( U8 mu8Buzz )
{
    U8 mu8Sound = SOUND_NONE;

    switch( mu8Buzz )
    {
        case WIFI_BUZZER_SELECT:
            mu8Sound = SOUND_SELECT;
            break;

        case WIFI_BUZZER_AP_CONNECT:
            mu8Sound = SOUND_WIFI_CONNECTING;
            break;

        case WIFI_BUZZER_SETUP:
            mu8Sound = SOUND_WIFI_CONNECTING;
            break;

        case WIFI_BUZZER_SERVER_CONNECT:
            mu8Sound = SOUND_WIFI_CONNECTED;
            break;

        case WIFI_BUZZER_AP_FAIL:
            mu8Sound = SOUND_WIFI_FAIL_CONNECTING;
            break;

        case WIFI_BUZZER_CANCEL:
            mu8Sound = SOUND_WIFI_DISCONNECTED;
            break;

        case WIFI_BUZZER_AP_START:
            mu8Sound = SOUND_SETUP;
            break;

        case WIFI_BUZZER_ERROR:
            mu8Sound = SOUND_ERROR;
            break;

        case WIFI_BUZZER_SERVER_FIRST:    
            mu8Sound = SOUND_WIFI_CONNECTED_FIRST;
            break;

        case WIFI_BUZZER_BLE_OFF:
            mu8Sound = SOUND_CANCEL;
            break;
    }

    Sound( mu8Sound );
}

U8 GetUserSystemStatus ( U8 mu8Value )
{
    /*
       mVal = USER_MODEL_TYPE;
0 : CHP
1 : CP
mVal = USER_FACTORY_STATUS;
0 CLEAR : Normal
1 SET : Factory/PCB Test mode
mVal = USER_RX_STATUS;
0 FALSE : App Control X (추출/플러싱/EW살균/설정모드/커버열림/필터탈착)
1 TRUE : App Control O
mVal = USER_SMART_POSSIBLE_STATUS;
0 FALSE : Smart Check X (추출/플러싱/EW살균/설정모드)
1 TRUE : Smart Check O
mVal = USER_ERROR_STATUS;
0 FALSE : Normal
1 TRUE : Error
mVal = USER_FOTA_STATUS;
0 FALSE : Fota X (추출/플러싱/EW살균)
1 TRUE : Fota O
mVal = USER_SMART_SENSING_STATUS;
0 FALSE : Normal
1 TRUE : Smart Checking
*/

    U8 mu8Return = 0;
    if ( mu8Value == USER_MODEL_TYPE )
    {
        if ( GetSystem() == SYSTEM_CP)
        {
            mu8Return = USER_MODEL_CP;
        }
        else
        {
            mu8Return = USER_MODEL_CHP;
        }
    }
    else if ( mu8Value == USER_FACTORY_STATUS ) 
    { 
        /* 공장테스트모드 확인 */
        if ( GetEolStatus() == TRUE 
                  || GetFctStatus() == TRUE 
           )
        {
            mu8Return = SET; // 공장모드
        }
        else
        {
            mu8Return = CLEAR; // 일반모드
        }
    }
    else if ( mu8Value == USER_RX_STATUS )
    { 
        // 추출, 필터 OPEN, 설정 변경 중
        if ( GetWaterOut() == TRUE 
                || IsOpenFilter( ID_FILTER_COVER ) == FILTER_OPEN 
                || IsOpenFilter( ID_FILTER  ) == FILTER_OPEN 
                || IsDoneFlushInitSetup() == FALSE
                || IsOpenIceDoor() == TRUE 
           )
        {   
            mu8Return = FALSE; // 어플동작금지
        }
        else
        {
            mu8Return = TRUE; // 어플동작가능
        }
    }
    else if ( mu8Value == USER_SMART_POSSIBLE_STATUS )
    { 
        // 추출 중,
        // 플러싱 완료 전
        if ( IsValidStartSmartCheck() == FALSE )
        {
            mu8Return = FALSE; // 정밀진단 불가
        }
        else
        {
            mu8Return = TRUE; // 정밀진단 가능
        }

    }
    else if ( mu8Value == USER_ERROR_STATUS )
    { /* 제품 에러확인 */
        if ( GetErrorId() != ERR_NONE )
        {
            mu8Return = TRUE;   // 에러
        }
        else
        {
            mu8Return = FALSE;  // 정상
        }
    }
    else if ( mu8Value == USER_FOTA_POSSIBLE_STATUS )
    { // 추출 / 플러싱 / EW살균
        if ( GetWaterOut() == TRUE 
                || IsDoneFlushInitSetup() == FALSE
           )
        {
            mu8Return = FALSE; // 포타 불가
        }
        else
        {
            mu8Return = TRUE; // 포타 가능
        }
    }
    else if ( mu8Value == USER_SMART_SENSING_STATUS )
    {
        if( IsStartDiagnosis() == TRUE )
        {
            mu8Return = TRUE ; // 정밀진단 중
        }
        else
        {
            mu8Return = FALSE; // 일반
        }
    }
    else
    {
        mu8Return = 0U;
    }
    return mu8Return;
}

/* User Data Clear */
// 커버 OPEN 시간
// 전력 데이터 
// 물 사용량
// 서버 미 연결 시 데이터 클리어 (누적되어 데이터 초과금지)
void ClearUserSystemData ( void ) 
{
    // TANK OPEN 시간
    ResetServiceOpenTime();

    // 소비전력 측정 데이터
    ClearAllEnergyWatt();

    // 필터 플러싱 완료 FLAG
    SetFilterChangedFlushStatus( FILTER_CHANGED_NONE );

    // 물 사용량
    ClearAllUsageWater();
}

/* User Display Setting */
// FOTA 진입 준비 상태
//  -. 부하 동작 중인 것 끄고...
//  -. WIFI LED를 3초 ON, 1초 OFF
void SetUserSystemDisplay ( U8 mu8Display )
{
    if ( mu8Display == USER_DISPLAY_FOTA ) // FOTA
    { // FOTA 진입 5초 전
        SetFotaDisp( TRUE );
    }
}

/* Time Setting */
void SetUserSystemTime ( U16 mYear, U16 mMonth, U16 mDate, U16 mHour, U16 mMin, U16 mSec, U16 mDay )
{
    TimeData_T mTime;

    mTime.Y2K     = 20;
    mTime.DayWeek = (U8)mDay;
    mTime.Month   = (U8)mMonth;
    mTime.Date    = (U8)mDate;
    mTime.Hour    = (U8)mHour;
    mTime.Min     = (U8)mMin;
    mTime.Sec     = (U8)mSec;

    SetRtcTime( &mTime );
    SetRtcConfigInit( TRUE );
}


/* User Error Data */
typedef struct _user_wifi_error_
{
    U16 API_Id;
    ErrorId_T ErrorId;
} WifiError_T;

const static WifiError_T ErrorTable[] = 
{
    { WIFI_ERROR_0006_HOTIN_TEMP,       ERR_TEMP_HOT_WATER_IN   },
    { WIFI_ERROR_0007_HOTOUT_TEMP,      ERR_TEMP_HOT_WATER      },
    { WIFI_ERROR_0008_COLD_TEMP,        ERR_TEMP_COLD_WATER     },
    { WIFI_ERROR_0009_NATURAL_TEMP,     ERR_TEMP_AMBIENT        },
    { WIFI_ERROR_000C_BLDC_COMP,        ERR_COMP_COMM           },
    { WIFI_ERROR_000E_DRAIN_PUMP,       ERR_DRAIN_PUMP          },
    { WIFI_ERROR_0019_STOP_WATER,       ERR_OUTAGE_WATER        },
    { WIFI_ERROR_001B_STOP_HOTWATER,    ERR_OUTAGE_HOT_WATER    },
    { WIFI_ERROR_001C_LEAK_FLOW,        ERR_LEAK_FLOW           },
    { WIFI_ERROR_001D_LEAK_SENSOR,      ERR_LEAK                },
    { WIFI_ERROR_001E_HOT_OVER,         ERR_HEATER_OVERHEAT     },
    { WIFI_ERROR_001F_HEATER_SENSOR,    ERR_TEMP_HEATER         },
    { WIFI_ERROR_0020_ROOM_TEMP,        ERR_TEMP_ROOM_WATER     },
    { WIFI_ERROR_0021_MICRO_SW_DETECT,  ERR_MICRO_SW_DETECT     },
    { WIFI_ERROR_0022_MICRO_SW_MOVE,    ERR_MICRO_SW_MOVE       },

    { WIFI_ERROR_0029_BLDC_STATUS,      ERR_COMP                }
};
#define SZ_ERROR_TABLE  (sizeof(ErrorTable)/sizeof(WifiError_T))
U16 GetUserSystemError ( U16 mu16Err )
{
    U8  i;
    U16 mu16Data = 0U;   // 0:Normal / 1:Error 

    for( i = 0; i < SZ_ERROR_TABLE; i++)
    {
        if( ErrorTable[ i ].API_Id == mu16Err )
        {
            mu16Data = GetErrorStatus( ErrorTable[ i ].ErrorId );
            break;
        }
    }

    return mu16Data;
}

static U16 ConvWifiTemp(TEMP_T mTemp)
{
    U16 mu16Val = 0U;


    /* ROUND UP */
    mTemp = mTemp + 0.5f;
    if( mTemp > 999.0f )
    {
        mTemp = 999.0f;
    }
    else if( mTemp < -999.0f )
    {
        mTemp = -999.0f;
    }

    mu16Val = (U16)mTemp;
    return mu16Val;
}

/* User Sensor Data */
I16 GetUserSystemSensor ( U16 mu16Sen )
{
    I16 mu16Data = 0;
    TEMP_T  mTemp;
    U8 mu8Month;
    U32 mu32Watt;

    switch ( mu16Sen )
    {
        case WIFI_SENSOR_0001_COLD_TEMP : // 0 ~ 99
            mTemp = GetTemp( TEMP_ID_COLD_WATER );
            mu16Data = ConvWifiTemp( mTemp );
            break;

        case WIFI_SENSOR_0003_WATER_TEMP : // 0 ~ 99
            mTemp = GetTemp( TEMP_ID_ROOM_WATER );
            mu16Data = ConvWifiTemp( mTemp );
            break;

        case WIFI_SENSOR_0004_OUT_TEMP : //  0 ~ 99
            mTemp = GetTemp( TEMP_ID_AMBIENT );
            mu16Data = ConvWifiTemp( mTemp );
            break;

        case WIFI_SENSOR_000E_WATT_DATA : // 0.01~250.00 (ex -> 1.78 = 178)

            // Watt hour
            mu32Watt = GetEnergyWattAll();
            mu32Watt /= 10;   // #.### -> #.##

            if( mu32Watt > 5000 )
            {
                mu32Watt = 5000;
            }
            else if( mu32Watt < 1 )
            {
                mu32Watt = 1;
            }

            mu16Data = (U16)mu32Watt;

            // 데이터 전달 후 초기화
            ClearAllEnergyWatt();
            break;

        case WIFI_SENSOR_000F_COMP_DATA : //  0 / 1
            // 목표 RPS와 현재 RPS가 0RPS 이상인 경우에만 압축기 운전 상태로 본다.
            // Note. BLDC COMP와 통신 에러가 발생되는 경우, 
            //       Current Rps가 반영안되는 경우가 있을 수 있음.
            //       그래서 반드시 Target과 Current 값을 함꼐 봐야함.
            if( GetCompBldcTargetRps() > 0 
                    && GetCompBldcCurrentRps() > 0 )
            {
                mu16Data = ON;
            }
            else
            {
                mu16Data = OFF;
            }
            break;

        default:
            mu16Data = 0U;
            break;
    }

    return mu16Data;
}

/* User Part Data */
// 고장 진단
I16 GetUserSystemPart ( U16 mu16Part )
{
    if( IsValidPartID( mu16Part ) == TRUE )
    {
        return GetSmartCheckDataId( mu16Part );
    }

    return 0;  // no parts
}


U32 GetUserSystemAccumulate ( U16 mu16Acc )
{
    U32 mu32Data = 0U;

    switch ( mu16Acc ) // 0 ~ 999999
    {
        case WIFI_ACCM_0000_MONTH_WATER_USER :
            mu32Data = 1000000;
            break;

        case WIFI_ACCM_0004_DAY_WATER_USER :
            mu32Data = 1010;
            break;

        default:
            mu32Data = 0U;
            break;
    }
    /* Max 999999 */
    if ( mu32Data  >= 999999U )
    {
        mu32Data = 999999U;
    }
    return mu32Data;
}

/* App Control -> User Function Data */
#define W_OFF           0
#define W_ON            1

#define W_CLOSE         0
#define W_OPEN          1

#define W_INSTALLED     0
#define W_REMOVED       1

#define W_HOT_UNLOCK    1
#define W_HOT_LOCK      2

#define W_UNLOCK        0
#define W_LOCK          1

#define W_FILTER_NONE   0
#define W_FILTER_RESET  1

#define W_PROGRESS_OFF      0
#define W_PROGRESS_ON       1
#define W_PROGRESS_GOING    2

//#define W_ICE_OFF           0
#define W_ICE_MODE_COLD     1
//#define W_ICE_MODE_BALANCE  2
#define W_ICE_MODE_ICE      3

#define W_COLD_OFF          0
#define W_COLD_MODE_1       1
#define W_COLD_MODE_2       2

#define W_WATER_ROOM        1
#define W_WATER_COLD        2
#define W_WATER_HOT         3

#define W_ICE_LARGE         0
#define W_ICE_SMALL         2

#define W_ICE_OFF           0
#define W_ICE_ON            1

#define W_MIN_SAVIN_HOUR    0
#define W_MAX_SAVIN_HOUR    23

#define W_MIN_SAVIN_MIN     0
#define W_MAX_SAVIN_MIN     59

#define W_SMART_CHECK_START 1
#define W_SMART_CHECK_STOP  0

#define W_SMART_CHECK_ERROR  1
#define W_SMART_CHECK_OK     0


U8 mu8IsSkipSendData = FALSE;
void SetUserSystemFunction ( U16 mu16Func, U16 mData )
{
    U8 mu8IsSendData = FALSE;


    switch ( mu16Func )
    {
        case WIFI_FUNC_0002_COLD_SEL :
            if(mData == W_ON )
            {
                SetColdWaterConfigMake( TRUE );
                Sound( SOUND_SETUP_COLD );
            }
            else if( mData == W_OFF )
            {
                SetColdWaterConfigMake( FALSE );
                Sound( SOUND_CANCEL_COLD );
            } 
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0003_HOT_LOCK_SEL :  
            if( GetSystem() == SYSTEM_CHP )
            {
                if (mData == W_HOT_UNLOCK)
                {
                    SetLockHot( UNLOCK );
                    Sound( SOUND_UNLOCK_HOT );
                }
                else if(mData == W_HOT_LOCK )
                {
                    SetLockHot( LOCK );
                    Sound( SOUND_LOCK_HOT );
                }   
                else
                {
                    mu8IsSendData = TRUE;
                }
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0005_LOCK_SEL :
            if ( mData == W_LOCK )
            {
                SetLockAll( LOCK );
                Sound( SOUND_LOCK_ALL );
            }
            else if( mData == W_UNLOCK )
            {
                SetLockAll( UNLOCK );
                Sound( SOUND_UNLOCK_ALL );
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0008_ICE_SEL:
            if( mData == W_ICE_MODE_ICE )
            {
                SetIceTurbo( TRUE );
                Sound( SOUND_SETUP );
            }
            else if( mData == W_ICE_MODE_COLD )
            {
                SetIceTurbo( FALSE );
                Sound( SOUND_SETUP );
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0031_VOICE_SEL :
            if( mData >= MIN_VOICE_LEVEL && mData <= MAX_VOICE_LEVEL )
            {
                U8 mu8Sound;

                SetVoiceLevel( (U8)mData );
                mu8Sound = SOUND_SETTING_MUTE + ( (SoundId_T)mData - 1 );
                Sound( mu8Sound );
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0032_ICELOCK_SEL :
            if ( mData == W_LOCK )
            {
                SetLockIce( LOCK );
                Sound( SOUND_LOCK_ICE );
            }
            else if( mData == W_UNLOCK )
            {
                SetLockIce( UNLOCK );
                Sound( SOUND_UNLOCK_ICE );
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0033_VOICE_VOL_SEL :
            if( mData >= MIN_VOICE_VOLUME && mData <= MAX_VOICE_VOLUME )
            {
                SetVoiceVolume( (U8)mData );
                Sound( SOUND_SETUP );
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

            // 고온수 사용
        case WIFI_FUNC_003B_HOT_TEMP_SEL :
            if( mData == W_OFF || mData == W_ON )
            {
                SetHotConfigUser( (U8)mData );
                if( mData == W_ON )
                {
                    Sound( SOUND_SETUP );
                }
                else
                {
                    Sound( SOUND_CANCEL );
                }
            }
            else
            {
                mu8IsSendData = TRUE;
            }

            break;

        case WIFI_FUNC_0040_SMART_CHECK_RQST :
            if( mData == W_SMART_CHECK_START )
            {
                StartSmartCheck();
                Sound( SOUND_SMART_DIAG_START );
            }
            else
            {
                StopSmartCheck();
            }
            break;

        case WIFI_FUNC_0047_WATER_QUANTITY_SEL :
            if( mData >= MIN_USER_AMOUNT_LITER 
                    && mData <= MAX_USER_AMOUNT_LITER )
            {
                SetWaterOutUserAmountLiter( mData );
                Sound( SOUND_SETUP_MY_AMOUNT );
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0049_OUT_BUTTON_LIGHT :
            if( mData == W_OFF || mData == W_ON )
            {
                SetConfigDeco( DECO_LED_BUTTON, (U8)mData );
                if( mData == W_OFF )
                {
                    Sound( SOUND_CANCEL );
                }
                else
                {
                    Sound( SOUND_SETUP );
                }
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_004A_WELCOME_LIGHT :
            if( mData == W_OFF || mData == W_ON )
            {
                SetConfigDeco( DECO_LED_WELCOME, (U8)mData );
                if( mData == W_OFF )
                {
                    Sound( SOUND_CANCEL );
                }
                else
                {
                    Sound( SOUND_SETUP );
                }
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0051_QUANTITY_USE_SEL :
            if( mData == W_OFF || mData == W_ON )
            {
                SetWaterOutConfigUser( (U8)mData );

                if( mData == W_OFF )
                {
                    Sound( SOUND_CANCEL );
                }
                else
                {
                    Sound( SOUND_SETUP );
                }
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0058_ICE_SIZE_SEL :
            if( mData == W_ICE_LARGE || mData == W_ICE_SMALL )
            {
                if( mData == W_ICE_LARGE )
                {
                    SetIceSize( ICE_LARGE );
                }
                else 
                {
                    SetIceSize( ICE_SMALL );
                }

                Sound( SOUND_SETUP );
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0059_ICE_ONOFF_SEL :
            if( mData == W_ICE_OFF || mData == W_ICE_ON )
            {
                if( mData == W_ICE_OFF )
                {
                    Sound( SOUND_CANCEL_ICE );
                    SetIceConfigMake( FALSE );
                }
                else
                {
                    Sound( SOUND_SETUP_ICE );
                    SetIceConfigMake( TRUE );
                }
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

            // 고장진단 결과
        case WIFI_FUNC_005C_SMART_CHECK_RESULT :
            if( mData == W_SMART_CHECK_OK )
            {
                Sound( SOUND_SMART_DIAG_NO_ERR_DONE );
            }
            else
            {
                Sound( SOUND_SMART_DIAG_ERR_DONE );
            }
            break;

        case WIFI_FUNC_005E_SLEEP_MODE_SEL :
            if( mData == W_OFF || mData == W_ON )
            {
                SetSavingConfig( (U8)mData );
                if( mData == W_OFF )
                {
                    Sound( SOUND_CANCEL_SLEEP );
                }
                else
                {
                    Sound( SOUND_SETUP_SLEEP );
                }
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_005F_SLEEP_START_HOUR :
            if( mData >= W_MIN_SAVIN_HOUR
                    && mData <= W_MAX_SAVIN_HOUR )
            {
                SetSavingAlarmHour( ALARM_ON, (U8)mData);
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0060_SLEEP_START_MIN :
            if( mData >= W_MIN_SAVIN_MIN
                    && mData <= W_MAX_SAVIN_MIN )
            {
                SetSavingAlarmMin( ALARM_ON, (U8)mData);
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0061_SLEEP_END_HOUR :
            if( mData >= W_MIN_SAVIN_HOUR
                    && mData <= W_MAX_SAVIN_HOUR )
            {
                SetSavingAlarmHour( ALARM_OFF, (U8)mData);
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;

        case WIFI_FUNC_0062_SLEEP_END_MIN :
            if( mData >= W_MIN_SAVIN_MIN
                    && mData <= W_MAX_SAVIN_MIN )
            {
                SetSavingAlarmMin( ALARM_OFF, (U8)mData);
                // SLEEP 데이터는 4개 데이터 한번에 전달되므로 마지막에 한번만 울림
                Sound( SOUND_SETUP );       
            }
            else
            {
                mu8IsSendData = TRUE;
            }
            break;
            
        default:
            break;
    }


    if( mu8IsSendData == TRUE )
    {
        WifiSendData( WIFI_DATA_FUNCTION );
    }
    else
    {
        mu8IsSkipSendData = TRUE;
    }
}


static U8 GetVal( U8 (*pFun)(void), U8 Val, U8 mTrue, U8 mFalse )
{
    if( pFun() == Val )
    {
        return mTrue;
    }

    return mFalse;
}


/* User Function Data */
U16 GetUserSystemFunction ( U16 mu16Func )
{
    U16 mu16Data = 0U;

    switch ( mu16Func )
    {        
        case WIFI_FUNC_0002_COLD_SEL :
            mu16Data = GetVal( GetColdWaterConfigMake, TRUE, W_ON, W_OFF );
            break;

        case WIFI_FUNC_0003_HOT_LOCK_SEL :
            mu16Data = GetVal( GetLockHot, LOCK, W_HOT_LOCK, W_HOT_UNLOCK );
            break;

        case WIFI_FUNC_0005_LOCK_SEL : 
            mu16Data = GetVal( GetLockAll, LOCK, W_LOCK, W_UNLOCK );
            break;

        case WIFI_FUNC_0008_ICE_SEL:
            if( GetIceTurbo() == TRUE )
            {
                mu16Data = W_ICE_MODE_ICE;
            }
            else
            {
                mu16Data = W_ICE_MODE_COLD;
            }
            break;

        case WIFI_FUNC_000A_COLD_TEMP : 
#if 0
            mu16Data = GetVal( GetColdWaterConfigMake, FALSE, W_COLD_OFF, W_COLD_MODE_2 );
#else
            if( GetColdWaterConfigMake() == FALSE )
            {
                mu16Data = W_COLD_OFF;
            }
            else
            {
                if( GetColdWaterTempStatus() == COLD_STATUS_GOOD )
                {
                    mu16Data = W_COLD_MODE_2;
                }
                else
                {
                    mu16Data = W_COLD_MODE_1;
                }

            }
#endif

            break;

        case WIFI_FUNC_000B_WATER_SEL : 
            {
                U8 mu8Sel;

                mu8Sel = GetWaterOutSelect();
                if( mu8Sel == SEL_WATER_COLD )
                {
                    mu16Data = W_WATER_COLD;
                }
                else if( mu8Sel == SEL_WATER_HOT )
                {
                    mu16Data = W_WATER_HOT;
                }
                else 
                {
                    mu16Data = W_WATER_ROOM;
                }
            }
            break;        

        case WIFI_FUNC_000D_WATER_OUTQUANTITY :
            mu16Data = GetUserUsageDailyWater( USER_NONE ) * 10U /* 10ml */; 
            ClearAllUsageWater();
            break;

            // 정수/냉수 추출 시에는 0으로 처리
        case WIFI_FUNC_001A_HOT_INTEMP :
            if( GetWaterOutLastedSelect() != SEL_WATER_HOT )
            {
                mu16Data = 0;
            }
            else
            {
                TEMP_T mTemp;
                mTemp = GetTemp( TEMP_ID_HOT_IN );
                mu16Data = ConvWifiTemp( mTemp );
            }
            break;

        case WIFI_FUNC_001B_HOT_MEANTEMP :
            if( GetWaterOutLastedSelect() != SEL_WATER_HOT )
            {
                mu16Data = 0;
            }
            else
            {
                TEMP_T mTemp;
                mTemp = GetHotOutAverageOutTemp();
                mu16Data = ConvWifiTemp( mTemp );
            }
            break;

        case WIFI_FUNC_001C_HOT_SELTEMP :
            if( GetWaterOutLastedSelect() != SEL_WATER_HOT )
            {
                mu16Data = 0;
            }
            else
            {
                U8 mu8SelHot;

                mu8SelHot = GetHotSelect();
                mu16Data = GetHotSelectTemp( mu8SelHot );
            }
            break;

        case WIFI_FUNC_001F_DRAIN_STATUS :
            mu16Data = GetProgressVal( PROGRESS_ID_DRAIN_WATER );
            break;

            // TANK OPEN 시간... (sec)
        case WIFI_FUNC_0024_HEART_TIME4 :
            mu16Data = GetServiceOpenTime();
            ResetServiceOpenTime();
            break;

        case WIFI_FUNC_002C_SILVER_CARE : 
            mu16Data = GetProgressVal( PROGRESS_ID_SILVER_CARE );
            break;

        case WIFI_FUNC_0030_ICE_MAKETIME:
            mu16Data = GetIceDoorOpeningTimeIot();
            SetIceDoorOpeningTimeIot( 0U );
            break;
            
        case WIFI_FUNC_0031_VOICE_SEL :
            mu16Data = GetVoiceLevel();
            break;

        case WIFI_FUNC_0032_ICELOCK_SEL:
            mu16Data = GetVal( GetLockIce, LOCK, W_LOCK, W_UNLOCK );
            break;

        case WIFI_FUNC_0033_VOICE_VOL_SEL :
            mu16Data = GetVoiceVolume();
            break;

        case WIFI_FUNC_0035_COVER1_OPEN :
            if( IsOpenFilter( ID_FILTER_COVER ) == FILTER_OPEN )
            {
                mu16Data = W_OPEN;
            }
            else
            {
                mu16Data = W_CLOSE;
            }
            break;
	case WIFI_FUNC_0036_COVER2_OPEN :
	    if( GetServiceCheckTankOpen() == W_OPEN )
            {
                mu16Data = W_OPEN;
            }
            else
            {
                mu16Data = W_CLOSE;
            }
	    break;

        case WIFI_FUNC_003B_HOT_TEMP_SEL :
            mu16Data = GetHotConfigUser();
            break;

        case WIFI_FUNC_0040_SMART_CHECK_RQST :
            mu16Data = GetSmartCheckStatusId(WIFI_FUNC_0040_SMART_CHECK_RQST);
            break;

        case WIFI_FUNC_0041_SMART_CHECK_STATUS :
            mu16Data = GetSmartCheckStatusId(WIFI_FUNC_0041_SMART_CHECK_STATUS);
            break;

        case WIFI_FUNC_0042_SMART_CHECK_PROGRESS :
            mu16Data = GetSmartCheckStatusId(WIFI_FUNC_0042_SMART_CHECK_PROGRESS);
            break;

        case WIFI_FUNC_0043_FILTER_STATUS1 :
            if (IsOpenFilter( ID_FILTER ) == FILTER_OPEN )
            {   
                mu16Data = W_REMOVED;
            }
            else
            {
                mu16Data = W_INSTALLED;
            }
            break;

        case WIFI_FUNC_0047_WATER_QUANTITY_SEL :
            mu16Data = GetWaterOutUserAmountLiter();
            break;

        case WIFI_FUNC_0049_OUT_BUTTON_LIGHT:
            if( GetConfigDeco( DECO_LED_BUTTON ) == TRUE )
            {
                mu16Data = W_ON;
            }
            else
            {
                mu16Data = W_OFF;
            }
            break;

        case WIFI_FUNC_004A_WELCOME_LIGHT :
            if( GetConfigDeco( DECO_LED_WELCOME ) == TRUE )
            {
                mu16Data = W_ON;
            }
            else
            {
                mu16Data = W_OFF;
            }
            break;

        case WIFI_FUNC_0050_UV_STERILIZE_STATUS :
            mu16Data = GetProgressVal( PROGRESS_ID_UV_WATER_OUT );
            break;

        case WIFI_FUNC_0051_QUANTITY_USE_SEL :
            mu16Data = GetVal( GetWaterOutConfigUser, TRUE, W_ON, W_OFF );
            break;

        case WIFI_FUNC_0053_FILTER_RESET1 :
            mu16Data = GetVal( GetFilterChangedFlushStatus, FILTER_CHANGED_FLUSH, W_FILTER_RESET, W_FILTER_NONE );
            if( mu16Data == W_FILTER_RESET )
            {
                // Reset Filter 
                ResetFilterUsage();
                SetFilterChangedFlushStatus( FILTER_CHANGED_NONE );
            }
            break;

        case WIFI_FUNC_0058_ICE_SIZE_SEL :
            if( GetIceSize() == ICE_LARGE )
            {
                mu16Data = W_ICE_LARGE;
            }
            else
            {
                mu16Data = W_ICE_SMALL;
            }
            break;

        case WIFI_FUNC_0059_ICE_ONOFF_SEL :
            mu16Data = (U16)GetIceConfigMake();
            break;
            
        case WIFI_FUNC_005A_UV_ICE_TANK_STATUS :
            mu16Data = GetProgressVal( PROGRESS_ID_UV_ICE_TANK );
            break;
            
        case WIFI_FUNC_005B_UV_ICE_TRAY_STATUS :
            mu16Data = GetProgressVal( PROGRESS_ID_UV_ICE_TRAY );
            break;
            
        case WIFI_FUNC_005C_SMART_CHECK_RESULT :
            // unused...
            break;
            
        case WIFI_FUNC_005D_UV_ICE_FAUCET_STATUS :
            mu16Data = GetProgressVal( PROGRESS_ID_UV_ICE_OUT );
            break;

        case WIFI_FUNC_005E_SLEEP_MODE_SEL :
            mu16Data = (U16)GetSavingConfig();
            break;
            
        case WIFI_FUNC_005F_SLEEP_START_HOUR :
            mu16Data = (U16)GetSavingAlarmHour( ALARM_ON );
            break;
            
        case WIFI_FUNC_0060_SLEEP_START_MIN :
            mu16Data = (U16)GetSavingAlarmMin( ALARM_ON );
            break;
            
        case WIFI_FUNC_0061_SLEEP_END_HOUR :
            mu16Data = (U16)GetSavingAlarmHour( ALARM_OFF );
            break;
            
        case WIFI_FUNC_0062_SLEEP_END_MIN :
            mu16Data = (U16)GetSavingAlarmMin( ALARM_OFF );
            break;

        default:
            mu16Data = 0U;
            break;
    }

    return mu16Data;
}

////////////////////////////////////////////////////////////////////////////////
// 동작 상태 변화 관리 
////////////////////////////////////////////////////////////////////////////////

typedef struct _progress_
{
    U8 Status;      // W_PROGRESS_OFF, ..GOING, ..ON

    U8 PrevVal;     // ON, OFF
    U8 CurVal;      // ON, OFF
    U8 (*pGetStatus)(void);
} Progress_T;

static U8 GetStatusUvWaterOut(void);
static U8 GetStatusUvIceOut(void);
static U8 GetStatusUvIceTray(void);
static U8 GetStatusUvIceTank(void);
static U8 GetStatusSilverCare(void);
static U8 GetStatusDrainWater(void);

Progress_T  ProgressList[ MAX_PROGRESS_NUM ] = 
{
    { W_PROGRESS_OFF,  0,   0, GetStatusUvWaterOut }, 
    { W_PROGRESS_OFF,  0,   0, GetStatusUvIceOut  }, 
    { W_PROGRESS_OFF,  0,   0, GetStatusUvIceTray }, 
    { W_PROGRESS_OFF,  0,   0, GetStatusUvIceTank }, 
    { W_PROGRESS_OFF,  0,   0, GetStatusSilverCare }, 
    { W_PROGRESS_OFF,  0,   0, GetStatusDrainWater }, 
};

static U8 GetProgressStatus(U8 id )
{
    return ProgressList[ id ].Status;
}

static void SetProgressStatus(U8 id, U8 mu8Status)
{
    ProgressList[ id ].Status = mu8Status;
}

static U16 GetProgressVal(U8 id)
{
    U16 mu16Data;

    mu16Data = GetProgressStatus( id );
    if( mu16Data == W_PROGRESS_ON )
    {
        SetProgressStatus( id, W_PROGRESS_GOING );
    }

    return mu16Data;
}

// 파우셋 UV 출력
static U8 GetStatusUvWaterOut(void)
{
    if( IsTurnOnUvId( UV_WATER_OUT ) == TRUE )
    {
        return ON;
    }

    return OFF;
}

static U8 GetStatusUvIceOut(void)
{
    if( IsTurnOnUvId( UV_ICE_DOOR ) == TRUE )
    {
        return ON;
    }

    return OFF;
}

static U8 GetStatusUvIceTray(void)
{
    if( IsTurnOnUvId( UV_ICE_TRAY ) == TRUE )
    {
        return ON;
    }

    return OFF;
}

static U8 GetStatusUvIceTank(void)
{
    if( IsTurnOnUvId( UV_ICE_TANK ) == TRUE )
    {
        return ON;
    }

    return OFF;
}

// 얼음 추출 또는 냉수/정수/온수 추출
static U8 GetStatusSilverCare(void)
{
    // 물 추출 미사용 시간이 48시간을 초과하면...
    if( GetSystemUnusedTime() >= SILVER_CARE_TIME_MIN )
    {
        return ON;  // 사용 없음
    }

    return OFF; // 사용 있음.
}

static U8 GetStatusDrainWater(void)
{
    return OFF;
}

// 상태 변화가 있고 진행 중인 경우..
static U8 CheckStateType3(void)
{
    U8 i;
    Progress_T *pList;
    U8 IsChangedStatus = FALSE;

    for( i = 0; i < MAX_PROGRESS_NUM; i++ )
    {
        pList = &ProgressList[ i ];

        pList->CurVal = pList->pGetStatus();
        if( pList->PrevVal != pList->CurVal )
        {
            pList->PrevVal = pList->CurVal;
            if( pList->CurVal == ON )
            {
                if( pList->Status == W_PROGRESS_OFF )
                {
                    pList->Status = W_PROGRESS_ON;
                    IsChangedStatus = TRUE;
                }
            }
            else if( pList->CurVal == OFF )
            {
                pList->Status = W_PROGRESS_OFF;
                IsChangedStatus = TRUE;
            }
        }
    }

    return IsChangedStatus;
}

/////////////////////////////////////////////////////////////////////////////
// 설정 상태 변경 이벤트 발생시 처리
//  1. 냉수 ON/OFF
//  2. 얼음 ON/OFF
//  ...
//  3. 음성...
//  4. 필터...
//  5. 추출 정지/시작
//  6. 필터 커버
//  ...
/////////////////////////////////////////////////////////////////////////////
typedef struct _status_chagend_event_
{
    U16 Count;
    U16 PrevVal;
    U16 CurVal;
    U8 (*pGetStatus)(void);

} StatusEvent_T;

static U8 GetDeco_WelcomLed(void);
static U8 GetDeco_ButtonLed(void);
static U8 GetFilterStatus_Filter(void);
static U8 GetFilterStatus_FilterCover(void);
static U8 GetWaterOutUserAmountLiterEx(void);

StatusEvent_T   StatusEventList[] = 
{
    {  0, 0,   0, GetColdWaterConfigMake }, 

    {  0, 0,   0, GetIceConfigMake     }, 
    {  0, 0,   0, GetIceTurbo          }, 
    {  0, 0,   0, GetIceSize           }, 
    {  0, 0,   0, GetLockIce           }, 
    {  0, 0,   0, IsOpenIceDoor        }, 

    {  0, 0,   0, GetHotConfigUser     }, 
    {  0, 0,   0, GetLockHot           }, 
    {  0, 0,   0, GetLockAll           }, 
    {  0, 0,   0, GetSavingConfig      }, 

    {  0, 0,   0, GetVoiceLevel        }, 
    {  0, 0,   0, GetVoiceVolume       }, 

    {  0, 0,   0, GetDeco_WelcomLed    },
    {  0, 0,   0, GetDeco_ButtonLed    },

    {  0, 0,   0, GetWaterOut          }, 

    {  0, 0,   0, GetFilterStatus_Filter       }, 
    {  0, 0,   0, GetFilterStatus_FilterCover  }, 
    {  0, 0,   0, GetFilterChangedFlushStatus  }, 

    {  0, 0,   0, GetWaterOutUserAmountLiterEx }, 
    {  0, 0,   0, GetWaterOutConfigUser },

    {  0, 0,   0, GetSmartCheckStatus   },
    {  0, 0,   0, GetSmartCheck         },
    {  0, 0,   0, GetDiagnosisProgress  },

    {  0, 0,   0, GetServiceCheckTankOpen  }
};
#define SZ_STATUS_EVENT_LIST    (sizeof(StatusEventList)/sizeof(StatusEvent_T))

static U8 GetDeco_WelcomLed(void)
{
    return GetConfigDeco( DECO_LED_WELCOME );
}

static U8 GetDeco_ButtonLed(void)
{
    return GetConfigDeco( DECO_LED_BUTTON );
}

static U8 GetFilterStatus_Filter(void)
{
    return IsOpenFilter( ID_FILTER );
}

static U8 GetFilterStatus_FilterCover(void)
{
    return IsOpenFilter( ID_FILTER_COVER );
}

#define DELAY_TIME  2   // 2sec @1sec
static U8 GetWaterOutUserAmountLiterEx(void)
{
    static U8 InitPowrOn = FALSE;
    static U8 IsChangedStatus = FALSE;
    static U8 mu8DelayTime = DELAY_TIME;
    static U32 mu32PreVal   = 0;
    U32 mu32CurVal = 0;

    // 초기 설정 값 반영
    if( InitPowrOn == FALSE )
    {
        InitPowrOn = TRUE;

        mu32PreVal = GetWaterOutUserAmount();
        return FALSE;
    }

    mu32CurVal = GetWaterOutUserAmount();
    if( mu32PreVal != mu32CurVal )
    {
        mu32PreVal = mu32CurVal;

        IsChangedStatus = TRUE;
        mu8DelayTime = DELAY_TIME;
    }

    // 지연 시간을 준다.
    // 버튼 조작을 통해 입력되는 경우...
    if( IsChangedStatus == TRUE )
    {
        if( mu8DelayTime != 0 )
        {
            mu8DelayTime--;
        }
        else
        {
            IsChangedStatus = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}


// 상태 변화가 있는 경우.. 
// 변화가 있거나 없거나...
static U8 CheckStateType2(void)
{
    U8 i;
    StatusEvent_T *pList;
    U8 IsChangedStatus = FALSE;

    for( i = 0; i < SZ_STATUS_EVENT_LIST; i++ )
    {
        pList = &StatusEventList[ i ];

        pList->CurVal = pList->pGetStatus();
        if( pList->PrevVal != pList->CurVal )
        {
            pList->PrevVal = pList->CurVal;

            pList->Count++;
            IsChangedStatus = TRUE;
        }
    }

    return IsChangedStatus;
}

U8 dbg_iot_fun = 0; // for dbg...
void ProcessChangedStatus(void)
{
    if( CheckStateType2() == TRUE || CheckStateType3() == TRUE )
    {
        dbg_iot_fun++;

        if( mu8IsSkipSendData == TRUE )
        {
            mu8IsSkipSendData = FALSE;
        }
        else
        {
            WifiSendData( WIFI_DATA_FUNCTION );
        }
    }
}

#endif //ifdef MODEL_NO_1

