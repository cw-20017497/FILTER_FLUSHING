#include "hot_water.h"
#include "error.h"
#include "water_out.h"
#include "flush_water_out.h"
#include "eeprom.h"
#include "hot_water_out.h"
#include "heater_out.h"
#include "relay.h"



// TEMP HOT SELECT ('C)
#define TEMP_HOT_USER       100
#define TEMP_HOT_COFFEE     85      
#define TEMP_HOT_TEA        70
#define TEMP_HOT_MILK       45
const static U16 gu16TempList[ SEL_HOT_NUM ] = 
{
    TEMP_HOT_MILK,
    TEMP_HOT_TEA,
    TEMP_HOT_COFFEE,
    TEMP_HOT_USER,
};


// 전원 RESET 후, 온수 자동 드레인 동작 전 대기 시간
//#define POWER_ON_TIME           20U  // 3sec @100ms

// 온수 자동 드레인 대기 시간..
//#define INIT_FULL_WAIT_TIME     30U  // 3sec @100ms

#define HEATER_ON_TIME          20U // 2sec @100ms

HotWater_T Hot;


void  InitHotWater(void)
{
    Hot.InitFull            = FALSE;
    Hot.Select              = SEL_HOT_USER;
    Hot.HeaterOnTime        = HEATER_ON_TIME;
    Hot.ConfUser            = TRUE;
    Hot.Cooling             = FALSE;
#if CONFIG_TEST_HOT_OUT
    Hot.Test                = TRUE;
#endif
}

void  GetHotWaterData(HotWater_T *pData)
{
    MEMCPY( (void __FAR *)pData, (const void __FAR *)&Hot, sizeof( HotWater_T ));
}

void  SetHotWaterInitFull(U8 mu8Val )
{
    Hot.InitFull = mu8Val;
}

U8    GetHotWaterInitFull(void)
{
    return Hot.InitFull;
}

void  SetHotWaterCooling(U8 mu8Val )
{
    Hot.Cooling = mu8Val;
}

U8    GetHotWaterCooling(void)
{
    return Hot.Cooling;
}

void SetHotSelect(U8 mu8Sel )
{
    Hot.Select = mu8Sel;
}

U8   GetHotSelect(void)
{
    return Hot.Select;
}

void ClearHotHeaterOnTime(void)
{
    Hot.HeaterOnTime = 0;
}

// 온수 사용자 물양 
void SetHotConfigUser(U8 mu8Conf )
{
    Hot.ConfUser = mu8Conf;
    SaveEepromId( EEP_ID_CONF_USER_HOT );
}

U8   GetHotConfigUser(void)
{
    return Hot.ConfUser;
}

U16  GetHotSelectTemp(U8 mu8Sel)
{
    if( mu8Sel > SEL_HOT_NUM )
    {
        mu8Sel = SEL_HOT_COFFEE;
    }

    return gu16TempList[ mu8Sel ];
}


void  SetHotConfigTest(U8 mu8Test)
{
    Hot.Test = mu8Test;
}

U16  GetHotConfigTest(void)
{
    return Hot.Test;
}


U8 IsExpiredHeaterOnTime(void)
{
    if( Hot.HeaterOnTime == 0 )
    {
        return TRUE;
    }

    return FALSE;
}

void CheckHotOutHeaterOnTime(void)
{
    // 온수 추출 중에 압축기 RELAY가 ON 되는 것을 막는다.
    // 온수 히터 소비 전류가 높은 상태에서 
    // 압축기 RELAY ON 시점에 발생되는 inrush current가 더해지면
    // over current 되는 문제를 방지 하기 위한 동작이다.
    if( IsHotOut() == TRUE 
            || GetHeaterOutput() != 0 )
    {
        if( IsTurnOnRelay( RELAY_COMP ) == FALSE )
        {
            Hot.HeaterOnTime = HEATER_ON_TIME;
        }
    }
    
    if( Hot.HeaterOnTime != 0 )
    {
        Hot.HeaterOnTime--;
    }
}
