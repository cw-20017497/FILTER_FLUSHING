#include "hw.h"
#include "key_handler.h"
#include "key.h"
#include "key_common_handler.h"
#include "front.h"
#include "hot_water.h"
#include "cold_water.h"
#include "water_out.h"
#include "tank_clean.h"
#include "ice.h"
#include "ice_maker.h"
#include "ice_door.h"
#include "ice_tray.h"
#include "power_saving.h"
#include "sound.h"
#include "eeprom.h"
#include "error.h"
#include "display_lib.h"
#include "display_clock.h"
#include "process_display.h"
#include "rtc.h"
#include "eol.h"
#include "fct.h"
#include "time_short.h"
#include "timer.h"
#include "timer_id.h"
#include "drain.h"
#include "health.h"
#include "drain_water.h"
#include "drain_pump.h"
#include "flush_water_out.h"
#include "leak.h"
#include "filter.h"
#include "flow_meter.h"
#include "voice.h"
#include "cody_kit.h"
#include "empty_cold_tank.h"
#include "flush_water_out.h"
#include "service.h"
#include "ice_tray_test.h"
#include "key_event_lib.h"
#include "WIFI_Control.h"

#if CONFIG_TEST
#include "comp.h"
#endif

static void DispOnOff(U8 mu8OnOff);

static U8 SelSound(void);

/* 온수 버튼 */
static U8 SelHot(void);

/* 냉수 버튼 */
static U8 SelCold(void);
static U8 OnColdMake(void);
static U8 OffColdMake(void);
static U8 OnOffColdMake(void);
static U8 OnOffEmptyColdFaucet(void);
static U8 OnOffEmptyColdDrain(void);


/* 냉수 버튼 */
static U8 SelRoom(void);
#if 0   // unsued
static U8 OnDecoLed(void);
static U8 OffDecoLed(void);
#endif
static U8 ClearMemento(void);

/* 연속 추출 */
static U8 SelContyInfinity(void);

/* 설정 버튼 */
static U8 SelSetting(void);
static U8 OnSetting(void);
#if CONFIG_GAS_FILLING
static U8 SelGasFiling(void);
#endif

/* 물양 선택 */
static U8 SelAmount(void);
#if 0
static U8 SelAmountEx(void);
#endif
static U8 ConfigAmount(void);


/* 제빙 설정/해제 */
static U8 OnIceMake(void);
static U8 OffIceMake(void);
static U8 OnOffIceMake(void);

static U8 SelIce(void);

/* 제빙 우선 */
static U8 OnOffIceTurbo(void);

/* 얼음 잠금 버튼 */
static U8 OnOffIceLock(void);

static U8 SelIceSize(void);

/* 온수 잠금 버튼 */
static U8 OnOffHotLock(void);

static U8 OnOffHotUser(void);
static U8 OnOffHotTest(void);

/* 절전 버튼 */
static U8 OnOffSave(void);

static U8 SettingSound(void);
static U8 SettingCustomer(void);
static U8 SettingClock(void);
static U8 SettingClockSave(void);
static U8 SelCancelFlush(void);
static U8 CancelFlush(void);

/* 사용자 물양 설정 */
//static U8 OnOffUserAmount(void);

//static U8 OnVersion(void);
static U8 OnOffMemento(void);

static U8 OnLockAll(void);
static U8 OffLockAll(void);
//static U8 OnOffLockAll(void);


//static U8 OnOffMute(void);
static U8 OnOffCodyKit(void);
static U8 OnTestIceTray(void);

/* 온수 미사용 절전 */
static U8 OnOffUnusedSave(void);

/* 강제 탈빙 */
static U8 OnDeIcing(void);
static U8 OnManualDrain(void);

/* EEPROM 초기화 */
static U8 OnEraseEeprom(void);

/* WIFI */
static U8 OnWifiAp(void);
static U8 OnWifiBle(void);
static U8 OnWifiOnOff(void);

/* EOL */
static U8 OnEolStart(void);
static U8 OnFrontStart(void);
static U8 OnFctStart(void);
static U8 OnTimeShort(void);

static U8 OnTestShortCompTime(void);

/* UV TEST CODE */
static U8 OnOffUvIceTank(void);
static U8 OnOffUvIceTray(void);
static U8 OnOffUvIceOut(void);
static U8 OnOffUvWaterOut(void);

static U8 KeyContDownEx(void);
static U8 KeyContUpEx(void);
static U8 SelHotTest(void);

// 일반 모드 리스트
KeyEventList_T KeyEventList[] =
{
    /* KEY,            Short,            2sec,           3sec,               5sec,           Pop,           TS */
    /* SINGLE KEY */
#if CONFIG_UV_MANUAL
    { K_HOT,            SelHot,          NULL,           NULL,           NULL,          NULL,           OnOffUvIceOut    },
    { K_ROOM,           SelRoom,         NULL,           NULL,           NULL,          NULL,           OnOffUvIceTank   },
    { K_COLD,           SelCold,         NULL,           NULL,           NULL,          NULL,           OnOffUvIceTray   },
    { K_AMOUNT,         SelAmount,       NULL,           ConfigAmount,   NULL,          NULL,           OnOffUvWaterOut  },
#else
    { K_HOT,            SelHot,          NULL,           OnOffHotLock,   NULL,          KeyStopCont,    KeyContDownEx },
    { K_ROOM,           SelRoom,         NULL,           OnWifiBle,      NULL,          NULL,           NULL },
    { K_COLD,           SelCold,         NULL,           OnColdMake,     OffColdMake,   KeyStopCont,    KeyContUpEx },
    { K_AMOUNT,         SelAmount,       NULL,           ConfigAmount,   NULL,          NULL,           NULL },
#endif  /* CONFIG_UV_MANUAL */

#if CONFIG_GAS_FILLING
    { K_SETTING,        SelSetting,      SelGasFiling,   NULL,               NULL,           NULL,          NULL },
#elif CONFIG_TEST_HOT_OUT
    { K_SETTING,        SelHotTest,      NULL,          NULL,               NULL,           NULL,          NULL },
#else
    { K_SETTING,        SelSetting,      NULL,            OnSetting,         NULL,           NULL,          NULL },
#endif  /* CONFIG_GAS_FILLING */

    { K_LOCK_ALL,       OnLockAll,       NULL,             OffLockAll,       NULL,           NULL,          NULL },
    { K_ICE_LOCK,       OnOffIceLock,    NULL,             NULL,             NULL,           NULL,          NULL },
    { K_ICE_MAKE,       OnOffIceMake,    NULL,             NULL,             NULL,           NULL,          NULL },
    { K_HOT_LOCK,       OnOffHotLock,    NULL,             NULL,             NULL,           NULL,          NULL },
    { K_SAVE,           OnOffSave,       NULL,             NULL,             NULL,           NULL,          NULL },
    { K_COLD_MAKE,      OnOffColdMake,   NULL,             NULL,             NULL,           NULL,          NULL },
    
    { K_ICE,            SelIce,          NULL,             OnIceMake,        OffIceMake,     NULL,          NULL },
#if CONFIG_UV_MANUAL
    { K_CP_ICE_TURBO,   OnOffIceTurbo,   NULL,             NULL,             NULL,           NULL,          OnOffUvIceOut },
#else
    { K_CP_ICE_TURBO,   OnOffIceTurbo,   NULL,             NULL,             NULL,           KeyStopCont,   KeyContDownEx },
#endif
    { K_CP_ICE_SIZE,    SelIceSize,      NULL,             OnIceMake,        OffIceMake,     NULL,          NULL },
    { K_CP_ICE_MAKE,    OnOffIceMake,    NULL,             NULL,             NULL,           NULL,          NULL },

    /* MULTI KEY */
    { K_HOT_USER,           SelSound,    NULL,             OnOffHotUser,     NULL,           NULL,          NULL },
#if  CONFIG_TEST_HOT_OUT 
    { K_HOT_TEST,           SelSound,    NULL,             OnOffHotTest,     NULL,           NULL,          NULL },
#endif
    { K_SETTING_SOUND,      SelSound,    NULL,             SettingSound,     NULL,           NULL,          NULL },
    { K_SETTING_CUSTOMER,   SelSound,    NULL,             SettingCustomer,  NULL,           NULL,          NULL },
    { K_SETTING_CLOCK,      SelSound,    NULL,             SettingClock,     NULL,           NULL,          NULL },
    { K_SETTING_CLOCK_CP,   SelSound,    NULL,             SettingClock,     NULL,           NULL,          NULL },
    { K_CANCEL_FLUSH,       SelCancelFlush,    NULL,       CancelFlush,      NULL,           NULL,          NULL },
    { K_CANCEL_FLUSH_CP,    SelCancelFlush,    NULL,       CancelFlush,      NULL,           NULL,          NULL },

    { K_EMPTY_COLD_DRAIN,    SelSound,    NULL,             OnOffEmptyColdDrain,   NULL,    NULL,          NULL },
    { K_EMPTY_COLD_DRAIN_CP, SelSound,    NULL,             OnOffEmptyColdDrain,   NULL,    NULL,          NULL },
    { K_EMPTY_COLD,          SelSound,    NULL,             OnOffEmptyColdFaucet,  NULL,    NULL,          NULL },
    { K_EMPTY_COLD_CP,       SelSound,    NULL,             OnOffEmptyColdFaucet,  NULL,    NULL,          NULL },
    { K_DRAIN_COLD,          SelSound,    NULL,             OnManualDrain,   NULL,           NULL,          NULL },
    { K_DRAIN_COLD_CP,       SelSound,    NULL,             OnManualDrain,   NULL,           NULL,          NULL },
    { K_DEICING,             SelSound,    OnDeIcing,        NULL,             NULL,           NULL,          NULL },
    //{ K_MANUAL_DRAIN,       SelSound,    OnManualDrain,    NULL,             NULL,           NULL,          NULL },

    { K_WIFI_AP,            SelSound,    NULL,             OnWifiAp,         NULL,           NULL,          NULL },
    { K_WIFI_ONOFF,         SelSound,    NULL,             OnWifiOnOff,      NULL,           NULL,          NULL },

    { K_CODY_KIT,           SelSound,    OnOffCodyKit,     NULL,             NULL,           NULL,          NULL },
    { K_CODY_KIT_CP,        SelSound,    OnOffCodyKit,     NULL,             NULL,           NULL,          NULL },

    { K_TEST_ICE_TRAY,      SelSound,    OnTestIceTray,    NULL,             NULL,           NULL,          NULL },
    { K_TEST_ICE_TRAY_CP,   SelSound,    OnTestIceTray,    NULL,             NULL,           NULL,          NULL },

    /* TEST */
#if CONFIG_TEST
    { K_TEST_COMP,      SelSound,     OnTestShortCompTime,        NULL, NULL, NULL, NULL },
#endif
    { K_TEST_TS,        SelSound,      OnTimeShort,               NULL, NULL, NULL, NULL },
    { K_TEST_FCT,       SelSound,      OnFctStart,                NULL, NULL, NULL, NULL },
    { K_TEST_FRONT,     SelSound,      OnFrontStart,              NULL, NULL, NULL, NULL },
    { K_TEST_EOL,       SelSound,      OnEolStart,                NULL, NULL, NULL, NULL },
    { K_TEST_EOL_CP,    SelSound,      OnEolStart,                NULL, NULL, NULL, NULL }
};

// 테스트 모드(TIME SHORT) 리스트
KeyEventList_T KeyEventTimeShortList[] =
{
    /* KEY,            Short,            2sec,  3sec,  5sec,  Pop,  TS */
    /* SINGLE KEY */
    { K_HOT,            SelHot,          NULL,  NULL,  NULL,  NULL, NULL },
    { K_ROOM,           SelRoom,         NULL,  NULL,  NULL,  NULL, OnWifiBle },
    { K_COLD,           SelCold,         NULL,  NULL,  NULL,  NULL, OnOffColdMake },
    { K_AMOUNT,         SelAmount,       NULL,  NULL,  NULL,  NULL, ConfigAmount },
    { K_LOCK_ALL,       OnLockAll,       NULL,  OffLockAll,  NULL,  NULL, NULL },

    { K_SETTING,        SelSetting,      NULL,  NULL,  NULL,  NULL, OnSetting },
    { K_ICE_LOCK,       OnOffIceLock,    NULL,  NULL,  NULL,  NULL, NULL },
    { K_ICE_MAKE,       OnOffIceMake,    NULL,  NULL,  NULL,  NULL, NULL },
    { K_HOT_LOCK,       OnOffHotLock,    NULL,  NULL,  NULL,  NULL, NULL },
    { K_SAVE,           OnOffSave,       NULL,  NULL,  NULL,  NULL, NULL },
    { K_COLD_MAKE,      OnOffColdMake,   NULL,  NULL,  NULL,  NULL, NULL },

    { K_ICE,            SelIce,          NULL,  NULL,  NULL,  NULL, NULL },
    { K_CP_ICE_TURBO,   OnOffIceTurbo,   NULL,  NULL,  NULL,  NULL, NULL },
    { K_CP_ICE_SIZE,    SelIceSize,      NULL,  NULL,  NULL,  NULL, NULL },
    { K_CP_ICE_MAKE,    OnOffIceMake,    NULL,  NULL,  NULL,  NULL, NULL },

    /* MULTI KEY */
    { K_SETTING_SOUND,      SelSound,    NULL,  NULL,  NULL,  NULL, SettingSound     },
    { K_SETTING_CUSTOMER,   SelSound,    NULL,  NULL,  NULL,  NULL, SettingCustomer  },

    { K_DEICING,            SelSound,    NULL,  NULL,  NULL,  NULL, OnDeIcing      },
    //{ K_MANUAL_DRAIN,       SelSound,    NULL,  NULL,  NULL,  NULL, OnManualDrain  },

    { K_WIFI_AP,            SelSound,    NULL,  NULL,  NULL,  NULL, OnWifiAp       },
    { K_WIFI_ONOFF,         SelSound,    NULL,  NULL,  NULL,  NULL, OnWifiOnOff    },

    { K_TEST_COMP,          SelSound,    NULL,  NULL,  NULL,  NULL, OnTestShortCompTime },

    //{ K_VERSION,            SelSound,    NULL,  NULL,  NULL,  NULL, OnVersion     },
    { K_MEMENTO,            SelSound,    NULL,  NULL,  NULL,  NULL, OnOffMemento  },
};

// 테스트 모드(TIME SHORT - 메멘토 ) 리스트
KeyEventList_T KeyEventMementoList[] =
{
    /* KEY,            Short,            2sec,  3sec,  5sec,  Pop,  TS */
    /* SINGLE KEY */
    { K_SETTING,       NULL,         NULL,  NULL,  NULL,  NULL, OnOffMemento },
    { K_ROOM,          NULL,         NULL,  NULL,  NULL,  NULL, ClearMemento },
};

static U8 IsValidSoundMenu(U32 mu32Key)
{
    if( GetSoundDisp() == FALSE )
    {
        return TRUE;
    }

    if( GetSystem() == SYSTEM_CHP )
    {
        if( mu32Key != K_COLD
                && mu32Key != K_HOT
                && mu32Key != K_SETTING_SOUND 
          )
        {
            return FALSE;
        }
    }
    else
    {
        if( mu32Key != K_COLD
                && mu32Key != K_CP_ICE_TURBO
                && mu32Key != K_SETTING_SOUND 
          )
        {
            return FALSE;
        }
    }

    return TRUE;
}

static U8 IsValidClockMenu(U32 mu32Key)
{
    /* 시간 설정 메뉴 */
    if( GetClockDisp() == TRUE )
    {
        if( GetSystem() == SYSTEM_CHP )
        {
            if( mu32Key != K_SETTING 
                    && mu32Key != K_HOT 
                    && mu32Key != K_COLD 
              )
            {
                return FALSE;
            }
        }
        else
        {
            if( mu32Key != K_SETTING 
                    && mu32Key != K_CP_ICE_TURBO
                    && mu32Key != K_COLD 
              )
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

U8 IsValidNormalKeyCondition(U32 mu32Key)
{
    /* 배수 중지 */
    if( ResetDrainWater() == TRUE )
    {
        Sound( SOUND_CANCEL );
        return FALSE;
    }

    /* 시간 설정 */
    if( IsValidClockMenu( mu32Key ) == FALSE )
    {
        return FALSE;
    }

    /* 냉수 배수 - FAUCET */
    if( IsStartEmptyColdTank() == TRUE 
        && GetEmptyColdTankType() == TYPE_FAUCET )
    {
        if( mu32Key != K_EMPTY_COLD 
                && mu32Key != K_EMPTY_COLD_CP )
        {
            return FALSE;
        }
    }

    /* 냉수 배수 - DRAIN */
    if( IsStartEmptyColdTank() == TRUE 
        && GetEmptyColdTankType() == TYPE_DRAIN )
    {
        if( mu32Key != K_EMPTY_COLD_DRAIN 
                && mu32Key != K_EMPTY_COLD_DRAIN_CP )
        {
            return FALSE;
        }
    }


    // 사운드 설정 메뉴
    if( IsValidSoundMenu( mu32Key ) == FALSE )
    {
        return FALSE;
    }

    // 관리 모드 변경
    if( GetCustomerDisp() == TRUE )
    {
        if( mu32Key != K_SETTING_CUSTOMER ) 
        {
            return FALSE;
        }
    }

    /* 필터 커버 open 시 */
    if( GetFilterDisp() == TRUE )
    {
        if( mu32Key != K_CODY_KIT 
                && mu32Key != K_CODY_KIT_CP )
        {
            return FALSE;
        }
    }

    /* 전체 잠금 상태 */
    if( GetLockAll() == LOCK )
    {
        if( mu32Key != K_LOCK_ALL )
        {
            return FALSE;
        }
    }

    /* 초기 플러싱미 완료 상태이면 테스트만 허용*/
    if( IsDoneFlushInitSetup() == FALSE )
    {
        if( mu32Key != K_TEST_FCT 
                && mu32Key != K_TEST_FRONT
                && mu32Key != K_TEST_EOL
                && mu32Key != K_TEST_EOL_CP
                && mu32Key != K_CANCEL_FLUSH
                && mu32Key != K_CANCEL_FLUSH_CP
                && mu32Key != K_COLD
                && mu32Key != K_LOCK_ALL 
                && mu32Key != K_ROOM 
                && mu32Key != K_WIFI_AP 
                && mu32Key != K_WIFI_ONOFF 
          )
        {
            return FALSE;
        }
    }

    /* SETTING MENU 에서만 허용 */
    if( GetSettingDisp() == TRUE )
    {
        if(  mu32Key != K_ICE_LOCK
                && mu32Key != K_HOT_LOCK
                && mu32Key != K_COLD_MAKE
                && mu32Key != K_ICE_MAKE
                && mu32Key != K_SAVE
                && mu32Key != K_SETTING
                && mu32Key != K_LOCK_ALL 
                && mu32Key != K_CP_ICE_MAKE
          )
        {
            return FALSE;
        }
        
        // UPDATE SETTING MENU TIMER..
        StartDispTimerId( DISP_TIMER_SETTING_MENU );
    }
    else
    {
        /* SETTING MENU - 설정 상태가 아닐 때 SKip */
        if( mu32Key == K_ICE_LOCK
                || mu32Key == K_HOT_LOCK
                || ( mu32Key == K_LOCK_ALL && GetLockAll() == UNLOCK )
                || mu32Key == K_COLD_MAKE
                || mu32Key == K_ICE_MAKE
                || mu32Key == K_SAVE
                || mu32Key == K_CP_ICE_MAKE
          )
        {
            return FALSE;
        }
    }

    /* MEMENTO DISPLAY */
    if( GetMementoDisp() == TRUE )
    {
        if( mu32Key != K_SETTING 
                && mu32Key != K_ROOM )
        {
            return FALSE;
        }
    }

#if 0
    /* 전체 잠금 - 해제 버튼만 허용 */
    if( GetLockAll() == LOCK )
    {
        // CHP 모델은 설정-전체잠금
        if( mu32Key != K_LOCK_ALL 
                && mu32Key != K_SETTING )
        {
            StartDispTimerId( DISP_TIMER_ALARM_SETTING );
            Sound( SOUND_ALARM_LOCK_ALL ); 
            return FALSE;
        }
    }
#endif

    /* 물 추출 중 */
    if( GetWaterOut() == TRUE 
            && GetWaterOutSelect() == SEL_WATER_CODY )
    {
        if( mu32Key != K_CODY_KIT 
            || mu32Key != K_CODY_KIT_CP )
        {
            Sound( SOUND_ERROR );
            return FALSE;
        }
    }

#if 0
    if( GetWaterOut() == TRUE 
            && GetWaterOutSelect() != SEL_WATER_CODY )
    {
        if( GetWaterOutSelect() != SEL_WATER_FLUSH )
        {
            Sound( SOUND_ERROR );
        }
        return FALSE;
    }
#else
    if( GetWaterOut() == TRUE 
            && GetWaterOutSelect() != SEL_WATER_CODY )
    {
        if( GetWaterOutSelect() != SEL_WATER_FLUSH )
        {
            // 냉온정 추출... 에러부저음
            Sound( SOUND_ERROR );   
        }
        else
        {
            // 플러싱 추출 중..
            if( mu32Key == K_WIFI_AP 
                    || mu32Key == K_WIFI_ONOFF 
                    || mu32Key == K_ROOM )
            {
                return TRUE;    // wifi 페어링 입력은 허용
            }
        }

        return FALSE;   // 일반 추출이면 FALSE
    }
#endif

    /* 얼음 추출 중 */
    if( GetIceDoorStatus() == ICE_DOOR_OPEN 
            || GetIceDoorStatus() == ICE_DOOR_OPENING )
    {
        if( mu32Key != K_AMOUNT
                && mu32Key != K_ROOM
                && mu32Key != K_COLD
                && mu32Key != K_HOT )
        {
            return FALSE;
        }
    }

    return TRUE;
}

// 모드에 따라 메멘토 타임쇼트, 일반 3가지를 반환
void* GetNormalKeyEventList(void)
{
    if( GetMementoDisp() == TRUE )
    {
        return KeyEventMementoList;
    }
    else if( GetTimeShortStatus() == TRUE )
    {
        return KeyEventTimeShortList;
    }

    return KeyEventList;
}

// 모드에 따라 메멘토 타임쇼트, 일반 3가지를 반환
U8 GetNormalKeyEventListSize(void)
{
    if( GetMementoDisp() == TRUE )
    {
        return ( sizeof(KeyEventMementoList) / sizeof(KeyEventList_T) );
    }
    else if( GetTimeShortStatus() == TRUE )
    {
        return ( sizeof(KeyEventTimeShortList) / sizeof(KeyEventList_T) );
    }

    return ( sizeof(KeyEventList) / sizeof(KeyEventList_T) );
}



static U8 SetSelectWater( U8 mu8Id )
{
//    // 추출 중에는, 물 선택을 할 수 없다.
//    if( GetWaterOut() == TRUE )
//    {
//        return FALSE;   // error..
//    }

    SetWaterOutSelect( mu8Id );
    return TRUE;
}

static void DispOnOff(U8 mu8OnOff)
{
    if( mu8OnOff == ON )
    {
        StartDispTimerId( DISP_TIMER_ON );
        StopDispTimerId( DISP_TIMER_OFF );
    }
    else
    {
        StartDispTimerId( DISP_TIMER_OFF );
        StopDispTimerId( DISP_TIMER_ON );
    }
}

static U8 SelSound(void)
{
    return SOUND_SELECT;
}

// 시간 설정- 항목 변경
static U8 OnClockItem(void)
{
    if( GetClockDisp() == TRUE )
    {
        NextClock();
        StartDispTimerId( DISP_TIMER_NEXT );
        StartDispTimerId( DISP_TIMER_CLOCK_MENU );
        return TRUE;
    }

    return FALSE;
}

// 시간 설정- 증가
static U8 OnClockUp(void)
{
    if( GetClockDisp() == TRUE )
    {
        UpClock();
        StartDispTimerId( DISP_TIMER_UP );
        StartDispTimerId( DISP_TIMER_CLOCK_MENU );
        return TRUE;
    }

    return FALSE;
}

// 시간 설정- 감소
static U8 OnClockDown(void)
{
    // 사운드 볼륨 조정
    if( GetClockDisp() == TRUE )
    {
        DownClock();

        StartDispTimerId( DISP_TIMER_DOWN );
        StartDispTimerId( DISP_TIMER_CLOCK_MENU );
        return TRUE;
    }

    return FALSE;
}

// 시간 설정 메뉴 진입
static U8 SettingClock(void)
{
    if( GetClockDisp() == FALSE )
    {
        if( GetRtcConfigInit() == FALSE )
        {
            InitClock( TRUE );
        }
        else
        {
            InitClock( FALSE );
        }

        InitContKey( DEFAULT_CONT_TICK ); 
        SetSettingDisp( FALSE );    // 설정 진입은 해제 
        SetClockDisp( TRUE );
    }

    StartDispTimerId( DISP_TIMER_CLOCK_MENU );
    return SOUND_SETUP;
}

static U8 SettingClockSave(void)
{
    SaveClock();
    SetClockDisp( FALSE );

    StartDispTimerId( DISP_TIMER_SETTING_CLOCK );
    return SOUND_SETUP;
}

void ContySettingClockKey(void)
{
    ControlContKey( OnClockDown, OnClockUp );
}

static U8 SelCancelFlush(void)
{
    if( IsExpiredTestKeyLimitTimer() == TRUE )
    {
        return SOUND_NONE;
    }

    return SOUND_SELECT;
}

// 플러싱 강제 취소
static U8 CancelFlush(void)
{
    if( IsExpiredTestKeyLimitTimer() == TRUE )
    {
        return SOUND_NONE;
    }


    SetFlushSetupInit( FLUSH_STATUS_DONE );
    SetFlushStatus( FLUSH_STATUS_DONE );
    SetFlushPowerOn( FLUSH_STATUS_DONE );
    SaveEepromId( EEP_ID_FLUSH_STATUS );
    SaveEepromId( EEP_ID_FLUSH_SETUP_INIT );

    SetFilterChangedStatus( FILTER_MOUNTED );
    SaveEepromId( EEP_ID_FILTER_CHANGED );

    SetColdWaterInitFull( TRUE );
    SetFotaReboot( FALSE );
    SetWaterOutSelect( SEL_WATER_ROOM );

    return SOUND_SETUP;
}

static U8 OnVolumeUp(void)
{
    if( GetSoundDisp() == TRUE )
    {
        if( GetVoiceLevel() != VOICE_LEVEL_OFF )
        {
            StartDispTimerId( DISP_TIMER_UP );
            StartDispTimerId( DISP_TIMER_DOWN );
            VolumeUp();
        }

        StartDispTimerId( DISP_TIMER_SOUND_MENU );
        return TRUE;
    }

    return FALSE;
}

static U8 OnVolumeDown(void)
{
    // 사운드 볼륨 조정
    if( GetSoundDisp() == TRUE )
    {
        if( GetVoiceLevel() != VOICE_LEVEL_OFF )
        {
            StartDispTimerId( DISP_TIMER_UP );
            StartDispTimerId( DISP_TIMER_DOWN );
            VolumeDown();
        }

        StartDispTimerId( DISP_TIMER_SOUND_MENU );
        return TRUE;
    }

    return FALSE;
}

/* 온수 버튼 */
static U8 SelHot(void)
{
    U8 mu8SelHot;


    if( OnVolumeDown() == TRUE )
    {
        return SOUND_SELECT;
    }

    if( OnClockDown() == TRUE )
    {
        return SOUND_SELECT;
    }
    

    if( GetLockHot() == LOCK )
    {
        StartDispTimerId( DISP_TIMER_ALARM_LOCK_HOT );
        return SOUND_ALARM_LOCK_HOT;
    }

    // 온수 단수 에러 발생되면 자동 해제될 때까지 온수 추출 금지
    if( GetOutageStatus( TYPE_HOT_WATER ) == FLOW_OUTAGE )
    {
        StartDispTimerId( DISP_TIMER_ERROR );
        return SOUND_ERROR_OUTAGE;
    }
    
    // 온수 에러이면 선택 금지. 에러 표시
    if( IsErrorType( ERR_TYPE_HOT_WATER_OUT ) == TRUE )
    {
        StartDispTimerId( DISP_TIMER_ERROR );
        return SOUND_ERROR;
    }

    if( GetWaterOutSelect() != SEL_WATER_HOT )
    {
        if( GetHotConfigUser() == TRUE )
        {
            SetHotSelect( SEL_HOT_USER );
        }
        else
        {
            SetHotSelect( SEL_HOT_COFFEE );
        }
    }
    else if( IsExpiredDispTimer( DISP_TIMER_KEY_HOT_TEMP ) == FALSE )
    {
        mu8SelHot = GetHotSelect();
        if( mu8SelHot == SEL_HOT_COFFEE )
        {
            mu8SelHot = SEL_HOT_TEA;
        }
        else if( mu8SelHot == SEL_HOT_TEA )
        {
            mu8SelHot = SEL_HOT_MILK;
        }
        else if( mu8SelHot == SEL_HOT_MILK )
        {
            if( GetHotConfigUser() == TRUE )
            {
                mu8SelHot = SEL_HOT_USER;
            }
            else
            {
                mu8SelHot = SEL_HOT_COFFEE;
            }
        }
        else
        {
            mu8SelHot = SEL_HOT_COFFEE;
        }
        SetHotSelect( mu8SelHot ); 
    }

    /* 온수 선택 */
    if( SetSelectWater( SEL_WATER_HOT ) == TRUE )
    {
        //StartDispTimerId( DISP_TIMER_KEY_AMOUNT );
        StartDispTimerId( DISP_TIMER_KEY_HOT_TEMP );
        StartDispTimerId( DISP_TIMER_KEY_HOT );

        return SOUND_SELECT;
    }

    return SOUND_ERROR;
}

/* 냉수 버튼 */
static U8 SelCold(void)
{
    U8 mu8Sound = SOUND_SELECT;

    // 필터 플러싱 단계인 경우 필터 플러싱 취소 
    if( GetFlushSetupInit() == FLUSH_STATUS_DONE
            && GetFlushStatus() == FLUSH_STATUS_INIT )
    {
        // 필터 플러싱 진입 취소
        SetFlushStatus( FLUSH_STATUS_DONE );
        SetFlushPowerOn( FLUSH_STATUS_DONE );
        SaveEepromId( EEP_ID_FLUSH_STATUS );

        SetFilterChangedStatus( FILTER_MOUNTED );
        SaveEepromId( EEP_ID_FILTER_CHANGED );

        SetWaterOutSelect( SEL_WATER_ROOM );

        return SOUND_SELECT;
    }
    
    if( GetFlushSetupInit() == FLUSH_STATUS_INIT 
            || GetFlushPowerOn() == FLUSH_STATUS_INIT )
    {
        return SOUND_NONE;
    }

    // 사운드 볼륨 조정
    if( OnVolumeUp() == TRUE )
    {
        return SOUND_SELECT;
    }

    // 시간 증가 
    if( OnClockUp() == TRUE )
    {
        return SOUND_SELECT;
    }

    // 단수 에러 해제
    ClearOutageError();

    if( IsErrorType( ERR_TYPE_COLD_WATER_OUT ) == TRUE )
    {
        StartDispTimerId( DISP_TIMER_ERROR );
        return SOUND_ERROR;
    }

    if( SetSelectWater( SEL_WATER_COLD ) == TRUE )
    {
        //StartDispTimerId( DISP_TIMER_KEY_AMOUNT );
        StopDispTimerId( DISP_TIMER_KEY_HOT );
        StopDispTimerId( DISP_TIMER_KEY_HOT_TEMP );
    }

    if( GetColdWaterConfigMake() == FALSE )
    {
        mu8Sound = SOUND_ALARM_COLD_OFF;
        StartDispTimerId( DISP_TIMER_ALARM_MAKE_OFF_COLD );
    }

    return mu8Sound;
}

static U8 OnOffEmptyColdFaucet(void)
{
    if( IsStartEmptyColdTank() == FALSE )
    {
        StartEmptyColdTank( TYPE_FAUCET );
    }
    else
    {
        StopEmptyColdTank();
    }

    return SOUND_SELECT;
}

static U8 OnOffEmptyColdDrain(void)
{
    if( IsStartEmptyColdTank() == FALSE )
    {
        StartEmptyColdTank( TYPE_DRAIN );
    }
    else
    {
        StopEmptyColdTank();
    }

    return SOUND_SELECT;
}

/* 냉수 생성 On */
U8 gu8ColdOff = FALSE;
static U8 OnColdMake(void)
{
    if( GetClockDisp() == TRUE
            || GetCustomerDisp() == TRUE 
            || GetSoundDisp() == TRUE 
            || GetWaterOut() == TRUE )
    {
        return SOUND_NONE;
    }

    if( GetColdWaterConfigMake() == TRUE )
    {
        return SOUND_NONE;
    }

    gu8ColdOff = TRUE;
    SetColdWaterConfigMake( TRUE );
    UpdateWaterOutReturnTime();
    return SOUND_SETUP_COLD;
}

static U8 OffColdMake(void)
{
    if( GetClockDisp() == TRUE
            || GetCustomerDisp() == TRUE 
            || GetSoundDisp() == TRUE 
            || GetWaterOut() == TRUE )
    {
        return SOUND_NONE;
    }

    if( GetColdWaterConfigMake() == FALSE )
    {
        return SOUND_NONE;
    }

    if( gu8ColdOff == TRUE )
    {
        return SOUND_NONE;
    }

    SetColdWaterConfigMake( FALSE );
    ResetWaterOutReturnTime();
    //UpdateWaterOutReturnTime();
    return SOUND_CANCEL_COLD;
}

static U8 OnOffColdMake(void)
{
    U8 mu8Sound = SOUND_NONE;

    if( GetColdWaterConfigMake() == FALSE )
    {
        SetColdWaterConfigMake( TRUE );
        mu8Sound = SOUND_SETUP_COLD;
        DispOnOff( ON );
    }
    else
    {
        SetColdWaterConfigMake( FALSE );
        mu8Sound = SOUND_CANCEL_COLD;
        DispOnOff( OFF );
    }

    return mu8Sound;
}


/* 정수 버튼 */
static U8 SelRoom(void)
{
    if( GetWaterOut() == TRUE 
            && GetWaterOutSelect() == SEL_WATER_FLUSH )
    {
        return SOUND_NONE;
    }
    // 현재 시간 설정 중이면 버튼 Skip
    if( GetClockDisp() == TRUE )
    {
        return SOUND_NONE;
    }

    // 메멘토 
    if( GetMementoDisp() == TRUE )
    {
        return SOUND_NONE;
    }

    // 단수 에러 해제
    ClearOutageError();

    if( IsErrorType( ERR_TYPE_ROOM_WATER_OUT ) == TRUE )
    {
        StartDispTimerId( DISP_TIMER_ERROR );
        return SOUND_ERROR;
    }

    if( SetSelectWater( SEL_WATER_ROOM ) == TRUE )
    {
        //StartDispTimerId( DISP_TIMER_KEY_AMOUNT );
        StopDispTimerId( DISP_TIMER_KEY_HOT );
        StopDispTimerId( DISP_TIMER_KEY_HOT_TEMP );
        return SOUND_SELECT;
    }
    return SOUND_ERROR;
}

#if 0 // unused
static U8 OnDecoLed(void)
{
    if( GetConfigDeco( DECO_LED_BUTTON ) == TRUE )
    {
        return SOUND_NONE;
    }

    SetConfigDeco( DECO_LED_BUTTON, TRUE );
    return SOUND_SETUP;
}

static U8 OffDecoLed(void)
{
    /* DECO LED OFF */
    if( GetConfigDeco( DECO_LED_BUTTON ) == TRUE )
    {
        SetConfigDeco( DECO_LED_BUTTON, FALSE );
        return SOUND_CANCEL;
    }

    return SOUND_NONE;
}
#endif

static U8 ClearMemento(void)
{
    if( GetMementoDisp() == TRUE )
    {
        /* MEMENTO CLEAR */
        ClearMementoError();
        
        return SOUND_SELECT;
    }

    return SOUND_NONE;
}

// 추출 중?
static U8 IsWaterOutConty(U8 mu8Out, U8 mu8Conty)
{
    // 추출 중인데,
    // 연속은 연속이고,
    // 레버 입력이 없는 상태
    if( mu8Out == TRUE      
            && mu8Conty == TRUE  
            && HAL_GetLeverWater() == 0   
      )
    {
        return TRUE;    // 추출 중
    }

    return FALSE;   // 추출 없음
}

// 일반 추출 중?
static U8 IsWaterOut(U8 mu8Out, U8 mu8Conty)
{
    // 추출 중인데,
    // 연속은 아니고,
    // 레버 입력 중인 경우
    if( mu8Out == TRUE      
            && mu8Conty == FALSE  
            && HAL_GetLeverWater() != 0   
      )
    {
        return TRUE;    // 추출 중
    }

    return FALSE;   // 추출 없음
}


static U8 ChangeAmount(U8 mu8Amount)
{
    if( mu8Amount == FIXED_AMOUNT_HALF )
    {
        mu8Amount = FIXED_AMOUNT_ONE;
    }
    else if( mu8Amount == FIXED_AMOUNT_ONE )
    {
        mu8Amount = FIXED_AMOUNT_TWO;
    }
    else if( mu8Amount == FIXED_AMOUNT_TWO )
    {
        mu8Amount = FIXED_AMOUNT_INFINITY;
    }
    else if( mu8Amount == FIXED_AMOUNT_INFINITY )
    {
        // 사용자 물양 설정시에만 선택..
        if( GetWaterOutConfigUser() == TRUE )
        {
            mu8Amount = FIXED_AMOUNT_USER;
        }
        else
        {
            mu8Amount = FIXED_AMOUNT_HALF;
        }
    }
    else if( mu8Amount == FIXED_AMOUNT_USER )
    {
        mu8Amount = FIXED_AMOUNT_HALF;
    }
    else 
    {
        mu8Amount = FIXED_AMOUNT_HALF;
    }

    return mu8Amount;
}

// 정량
static U8 SelAmount(void)
{
    U8 mu8Sound = SOUND_SELECT;
    U8 mu8Amount;


    if( GetWaterOut() == TRUE )
    {
        return SOUND_ERROR;
    }


    //if( IsExpiredDispTimer( DISP_TIMER_KEY_AMOUNT ) == FALSE )
    {
        mu8Amount = GetWaterOutAmoutType();
        mu8Amount = ChangeAmount( mu8Amount );
        SetWaterOutFixedAmountType( mu8Amount );
    }

    // 물양 표시 시간 갱신
    StartDispTimerId( DISP_TIMER_KEY_AMOUNT );

    // 온수 온도 표시 유지 
    if( GetWaterOutSelect() == SEL_WATER_HOT )
    {
        StartDispTimerId( DISP_TIMER_KEY_HOT );
    }
    StopDispTimerId( DISP_TIMER_KEY_HOT_TEMP );

    return mu8Sound;
}

static U8 AmountUser(void)
{
    IncrementWaterOutAmount();

    return SOUND_SELECT;
}

#if 0
static U8 SelAmountEx(void)
{
    if( GetSettingDisp() == TRUE 
            && GetWaterOutConfigUser() == TRUE )
    {
        IncrementWaterOutAmount();
    //    UpdateDispKeyAmount();
        StartDispTimerId( DISP_TIMER_KEY_AMOUNT );

        return SOUND_SELECT;
    }

    return SelAmount();
}
#endif

static U8 ConfigAmount(void)
{
    U8 mu8Amount;

    if( GetWaterOut() == TRUE )
    {
        return SOUND_NONE;
    }

    mu8Amount = GetWaterOutAmoutType();
    if( mu8Amount != FIXED_AMOUNT_INFINITY )
    {
        SetWaterOutConfFixedAmountType( mu8Amount );
        StartDispTimerId( DISP_TIMER_CONFIG_AMOUNT );
        return SOUND_SETUP;
    }

    return SOUND_NONE;
}

/* 설정 메뉴 표시 */
// 시간 설정 중 : (AM,시간,분)
// 설정 화면 상태 : 설정 상태 종료
static U8 SelSetting(void)
{
    if( GetWaterOut() == TRUE 
            || IsOpenIceDoor() == TRUE )
    {
        return SOUND_NONE;
    }

    // 시간 설정 항목 변경
    if( GetClockDisp() == TRUE )
    {
        if( OnClockItem() == TRUE )
        {
            return SOUND_SELECT;
        }
    }

    // 설정 화면 EXIT 
    if( GetSettingDisp() == TRUE )
    {
        SetSettingDisp( FALSE );
        StopDispTimerId( DISP_TIMER_KEY_AMOUNT );
        return SOUND_CANCEL;
    }

    return SOUND_SETTING_MENU;

}


// 설정 메뉴 진입.
// 시간 설정 중 : 시간 저장 기능.
static U8 OnSetting(void)
{
    if( GetWaterOut() == TRUE 
            || IsOpenIceDoor() == TRUE )
    {
        return SOUND_NONE;
    }

    // 현재 시간 설정 중이면 버튼 Skip
    if( GetClockDisp() == TRUE )
    {
        return SettingClockSave();
    }


    // 설정 화면 표시
    SetSettingDisp( TRUE );

    StartDispTimerId( DISP_TIMER_SETTING_MENU );
    StopDispTimerId( DISP_TIMER_KEY_AMOUNT );
    StopDispTimerId( DISP_TIMER_KEY_HOT );
    StopDispTimerId( DISP_TIMER_KEY_HOT_TEMP );

    return SOUND_NONE;
}

#if CONFIG_GAS_FILLING
extern U8 filling = 0;
#include "gas_switch_valve.h"
static U8 SelGasFiling(void)
{
    filling = !filling;

    if( filling == 1 )
    {
        SetColdWaterConfigMake( FALSE );
        SetIceConfigMake( FALSE );

        GasSwitchCold();
        SetCompMode( COMP_MODE_FORCED );
        TurnOnForcedComp();

        SetCompProtectOffTime( 0 );
        SetCompBldcTargetRps( 60 );
    }
    else
    {
        SetCompMode( COMP_MODE_NONE );
        SetColdWaterConfigMake( TRUE );
        SetIceConfigMake( TRUE );
    }

    return SOUND_SETUP;
}
#endif

// 대 - 소 - 빠른/대 - 빠른/소

static U8 SelIce(void)
{
    U8 mu8Turbo;
    U8 mu8Size;


    if( GetIceConfigMake() == FALSE )
    {
        StartDispTimerId( DISP_TIMER_ALARM_MAKE_OFF_ICE );
        return SOUND_ALARM_ICE_OFF;
    }

    // 활성화
    if( IsExpiredDispTimer( DISP_TIMER_KEY_ICE ) == TRUE )
    {
        StartDispTimerId( DISP_TIMER_KEY_ICE );
        return SOUND_SELECT;
    }

    // 설정 변경
    mu8Turbo = GetIceTurbo();
    mu8Size = GetIceSize();
    if( mu8Turbo == FALSE && mu8Size == ICE_LARGE )
    {
        SetIceSize( ICE_SMALL );
    }
    else if( mu8Turbo == FALSE && mu8Size == ICE_SMALL )
    {
        SetIceTurbo( TRUE );
        SetIceSize( ICE_LARGE );
    }
    else if( mu8Turbo == TRUE && mu8Size == ICE_LARGE )
    {
        SetIceSize( ICE_SMALL );
    }
    else
    {
        SetIceTurbo( FALSE );
        SetIceSize( ICE_LARGE );
    }

    StartDispTimerId( DISP_TIMER_KEY_ICE );

    return SOUND_SELECT;
}


/* 제빙 쾌속 설정 */
static U8 OnOffIceTurbo(void)
{
    if( OnVolumeDown() == TRUE )
    {
        return SOUND_SELECT;
    }

    if( OnClockDown() == TRUE )
    {
        return SOUND_SELECT;
    }

    // 얼음 off, 버튼 입력 불가
    if( GetIceConfigMake() == FALSE )
    {
        StartDispTimerId( DISP_TIMER_ALARM_MAKE_OFF_ICE );
        return SOUND_ALARM_ICE_OFF;
    }
    
    if( GetIceTurbo() == TRUE )
    {
        SetIceTurbo( FALSE );
        return SOUND_CANCEL;
    }

    SetIceTurbo( TRUE );
    return SOUND_SETUP;
}

/* 제빙 기능 설정 */
U8 gu8IceOff = FALSE;
static U8 OnIceMake(void)
{
    if( GetIceConfigMake() == TRUE )
    {
        return SOUND_NONE;
    }

    gu8IceOff = TRUE;
    SetIceConfigMake( TRUE );
    StopTimer( TIMER_ID_DRAIN_PUMP_24H );
    return SOUND_SETUP_ICE;
}

static U8 OffIceMake(void)
{
    if( GetIceConfigMake() == FALSE )
    {
        return SOUND_NONE;
    }


    if( gu8IceOff == TRUE )
    {
        return SOUND_NONE;
    }
    SetIceConfigMake( FALSE );
    // 강제 도어 닫기
    ForcedCloseIceDoor();
    // 드레인 탱크 배수 
    //TurnOnDrainPump();
    StartDrainPumpOnLevel();
    // 24시간 후, 드레인 탱크 배수
    StartTimer( TIMER_ID_DRAIN_PUMP_24H, SEC(86400) );

    return SOUND_CANCEL_ICE;
}

static U8 OnOffIceMake(void)
{
    if( GetIceConfigMake() == TRUE )
    {
        DispOnOff( OFF );
        return OffIceMake();
    }

    DispOnOff( ON );
    return OnIceMake();
}


/* 얼음 잠금 설정/해제 */
static U8 OnOffIceLock(void)
{
    if( GetLockIce() == LOCK )
    {
        SetLockIce( UNLOCK );
        StopDispTimerId( DISP_TIMER_ALARM_SETTING );
        return SOUND_UNLOCK_ICE;
    }

    SetLockIce( LOCK );
    return SOUND_LOCK_ICE;
}

/* 얼음 크기 변경 */
static U8 SelIceSize(void)
{
    U8 mu8Size;

    // 얼음 off이면, 버튼 입력 금지
    if( GetIceConfigMake() == FALSE )
    {
        StartDispTimerId( DISP_TIMER_ALARM_MAKE_OFF_ICE );
        return SOUND_ALARM_ICE_OFF;
    }

    // 활성화
    if( IsExpiredDispTimer( DISP_TIMER_KEY_ICE ) == TRUE )
    {
        StartDispTimerId( DISP_TIMER_KEY_ICE );
        return SOUND_SELECT;
    }

    mu8Size = GetIceSize();
    if( mu8Size == ICE_LARGE )
    {
        mu8Size = ICE_SMALL;
    }
    else
    {
        mu8Size = ICE_LARGE;
    }

    SetIceSize( mu8Size );

    StartDispTimerId( DISP_TIMER_KEY_ICE );
    return SOUND_SELECT;
}


/* 온수 잠금 버튼 */
static U8 OnOffHotLock(void)
{
    if( GetClockDisp() == TRUE
            || GetCustomerDisp() == TRUE 
            || GetSoundDisp() == TRUE 
            || GetWaterOut() == TRUE )
    {
        return SOUND_NONE;
    }

    // 볼륨 업다운 또는 시간 설정시에는 skip
    //if( OnVolumeDown() == TRUE 
    //        || OnClockDown() == TRUE )
    //{
    //    return SOUND_NONE;
    //}

    /* 온수 잠금 / 해제 */
    if( GetLockHot() == UNLOCK )
    {
        // 온수 선택 중이면, 강제로 정수 선택으로 이동 시킨다.
        if( GetWaterOutSelect() == SEL_WATER_HOT )
        {
            SetWaterOutSelect( SEL_WATER_ROOM );
        }
        SetLockHot( LOCK );
        return SOUND_LOCK_HOT;
    }

    SetLockHot( UNLOCK );
    StopDispTimerId( DISP_TIMER_ALARM_SETTING );
    return SOUND_UNLOCK_HOT;
}

static U8 OnOffHotUser(void)
{

    if( GetHotConfigUser() == TRUE )
    {
        SetHotConfigUser( FALSE );
        return SOUND_CANCEL;
    }

    SetHotConfigUser( TRUE );
    return SOUND_SETUP;
}

U8 dbg_hot_out_temp = 0;
static U8 OnOffHotTest(void)
{

    if( GetHotConfigTest() == TRUE )
    {
        SetHotConfigTest( FALSE );
        return SOUND_CANCEL;
    }

    SetHotConfigTest( TRUE );
    return SOUND_SETUP;
}


#if !CONFIG_TEST_8585
static U8 OnOffSave(void)
{
    // RTC 미설정 상태는 불허
    if( GetRtcConfigInit() == FALSE )
    {
        SetSavingConfig( FALSE );
        return SOUND_ALARM_SLEEP_TIME;
    }

    if( GetSavingConfig() == TRUE )
    {
        SetSavingConfig( FALSE );

        return SOUND_CANCEL_SLEEP;
    }

    SetSavingConfig( TRUE );

    return SOUND_SETUP_SLEEP;
}

#else
/* 절전 설정/해제  */
extern U8 the_8585_step;
extern U8 the_8585_step_2;
static U8 OnOffSave(void)
{
    if( GetSavingConfig() == TRUE )
    {
        SetSavingConfig( FALSE );

#if CONFIG_TEST_8585
        DisableTimer( TIMER_ID_TEST_8585 );
        DisableTimer( TIMER_ID_TEST_8585_2 );
        the_8585_step = 0;
#endif

#if CONFIG_TEST_8585_ICE
        DisableTimer( TIMER_ID_TEST_8585_ICE );
        the_8585_step_2 = 0;
#endif
        return SOUND_CANCEL;
    }

    SetSavingConfig( TRUE );

#if CONFIG_TEST_8585
    StartTimer( TIMER_ID_TEST_8585, SEC(20) );
    StartTimer( TIMER_ID_TEST_8585_2, SEC(20) );
#endif

#if CONFIG_TEST_8585_ICE
    StartTimer( TIMER_ID_TEST_8585_ICE, SEC(30) );
#endif
    return SOUND_SETUP;
}
#endif

/* 음성 모드 설정 */
static U8 SettingSound(void)
{
    U8 mu8Level;
    U8 mu8Sound = SOUND_SELECT;


    mu8Level = GetVoiceLevel();

    if( GetSoundDisp() == FALSE )
    {
        // 설정 화면 활성화
        SetSoundDisp( TRUE );
    }
    else
    {
        if( mu8Level == VOICE_LEVEL_OFF )
        {
            mu8Level = VOICE_LEVEL_VOICE_BEEP;
        }
        else if( mu8Level == VOICE_LEVEL_BEEP )
        {
            mu8Level = VOICE_LEVEL_OFF;
        }
        else 
        {
            mu8Level = VOICE_LEVEL_BEEP;
        }

        SetVoiceLevel( mu8Level );
    }


    // sound 출력 종류 결정
    if( mu8Level == VOICE_LEVEL_OFF )
    {
        mu8Sound = SOUND_SETTING_MUTE;
    }
    else if( mu8Level == VOICE_LEVEL_BEEP )
    {
        mu8Sound = SOUND_SETTING_EFFECT;
    }
    else 
    {
        mu8Sound = SOUND_SETTING_VOICE;
    }

    StartDispTimerId( DISP_TIMER_SOUND_MENU );

    return mu8Sound;
}

static U8 SettingCustomer(void)
{
    U8 mu8Type;
    U8 mu8Sound = SOUND_SETUP;


    if( GetCustomerDisp() == FALSE )
    {
        // 설정 화면 활성화
        SetCustomerDisp( TRUE );
    }
    else
    {
        mu8Type = GetCustomerType();
        if( mu8Type == CUSTOMER_SELF )
        {
            mu8Type = CUSTOMER_VISIT;
        }
        else 
        {
            mu8Type = CUSTOMER_SELF;
        }

        SetCustomerType( mu8Type );
    }

    StartDispTimerId( DISP_TIMER_CUSTOMER_MENU );

    return SOUND_SETUP;
}


/* 사용자 물양 설정 */
#if 0
static U8 OnOffUserAmount(void)
{
    U8 mu8Sound = SOUND_NONE;

    if( GetWaterOutConfigUser() == TRUE )
    {
        SetWaterOutConfigUser( FALSE );
        mu8Sound = SOUND_CANCEL;
    }
    else
    {
        SetWaterOutConfigUser( TRUE );
        mu8Sound = SOUND_SETUP;
    }

    return mu8Sound;
}
#endif

/* 버전 표시 */
#if 0
static U8 OnVersion(void)
{
    SetVersionDisp(0);

    return SOUND_SELECT;
}
#endif

/* 메멘토 표시 */
static U8 OnOffMemento(void)
{
    if( GetMementoDisp() == FALSE )
    {
        SetMementoDisp( TRUE );
    }
    else
    {
        SetMementoDisp( FALSE );
    }

    return SOUND_SELECT;
}


/* 전체 잠금 설정 */
static U8 OnLockAll(void)
{
    if( GetLockAll() == UNLOCK )
    {
        SetLockAll( LOCK );
        return SOUND_LOCK_ALL;
    }

    StartDispTimerId( DISP_TIMER_ALARM_LOCK_ALL );
    return SOUND_ALARM_LOCK_ALL;
}

/* 전체 잠금 해제 */
static U8 OffLockAll(void)
{
    if( GetLockAll() == LOCK )
    {
        SetLockAll( UNLOCK );
        return SOUND_UNLOCK_ALL;
    }

    return SOUND_NONE;
}


#if 0
static U8 OnOffLockAll(void)
{
    U8 mu8Sound = SOUND_LOCK_ALL;


    if( GetLockAll() == LOCK )
    {
        SetLockAll( UNLOCK );
        mu8Sound = SOUND_UNLOCK_ALL;
    }
    else
    {
        SetLockAll( LOCK );
    }

    return mu8Sound;
}
#endif

#if 0
static U8 OnOffMute(void)
{
    U8 mu8Sound = SOUND_SETTING_VOICE;

    if( IsSetSoundMute() == FALSE )
    {
        SetSoundMute();
    }
    else
    {
        ClearSoundMute();
    }

    return SOUND_SETTING_MUTE;
}
#endif


static U8 OnOffCodyKit(void)
{
    if( GetCodyKitStatus() == TRUE )
    {
        StopWaterOut();
    }
    else
    {
        SetSelectWater( SEL_WATER_CODY );
        StartWaterOut();
    }

    return SOUND_SETUP;
}

static U8 OnTestIceTray(void)
{
    if( GetServiceCheckTankOpen() == TRUE )
    {
        if( GetIceTrayTestStatus() == FALSE )
        {
            StopDummyDeIcing();
            StopIceMake();
            StartIceTrayTest();
            return SOUND_SETUP;
        }
    }

    return SOUND_NONE;
}


static U8 OnDeIcing(void)
{
    // 제빙 중일 때, 강제 탈빙을 동작 시킨다. 
    if( StopIceMake() == TRUE )
    {
        return SOUND_SETUP;
    }

    return SOUND_SELECT;
}


static U8 OnManualDrain(void)       /*Manual Drain*/
{
    if( IsValidHealth() == FALSE )
    {
        return SOUND_ERROR;
    }

    if( StartDrainWater( FULL_DRAIN ) == FALSE )
    {
        return SOUND_ERROR;
    }

    return SOUND_EFFLUENT;
}


static U8 OnEraseEeprom(void)
{
    if( IsExpiredConfigTime() == FALSE )
    {
        EraseEeprom();
        return SOUND_SELECT;
    }

    return SOUND_NONE;
}

static U8 OnWifiAp(void)
{
    WifiKey(WIFI_KEY_AP);

    // 이전 AP 접속 에러가 있는 경우 표시
    if( GetWifiStatusValue(WIFI_STATUS_ERROR) != 0 )
    {
        StartDispTimerId( DISP_TIMER_WIFI_AP_ERROR );
    }
    return SOUND_NONE;
}

static U8 OnWifiBle(void)
{
    if( GetWaterOut() == TRUE 
            && GetWaterOutSelect() != SEL_WATER_FLUSH )
    {
        return SOUND_NONE;
    }

    WifiKey(WIFI_KEY_BLE);

    if( GetWifiStatusValue(WIFI_STATUS_ERROR) != 0 )
    {
        StartDispTimerId( DISP_TIMER_WIFI_AP_ERROR );
    }

    return SOUND_NONE;
}

static U8 OnWifiOnOff(void)
{
    WifiKey(WIFI_KEY_ONOFF);

    return SOUND_NONE;
}


// EOL 검사 시작
static U8 OnEolStart(void)
{
    if( StartEolLoad() == TRUE )
    {
        return SOUND_EOL;
    }

    return SOUND_NONE;
#if 0
    // EOL 테스트 진입 시간이 만료되면,
    // 부저음 없이 Pass
    if( IsExpiredEolLimitTimer() == TRUE )
    {
        return SOUND_NONE;
    }

    // 현재 EOL 테스트 중이면, Pass
    if( GetEolStatus() == TRUE )
    {
        return SOUND_NONE;
    }

    // EOL 시작
    StartEol( EOL_TYPE_LOAD );
    StartDisplayInit();
    SetVersionDisp( 0 );

    // WIFI 
    WifiKey(WIFI_KEY_TEST);

    return SOUND_EOL;
#endif
}

// FRONT 검사 시작
static U8 OnFrontStart(void)
{
    // EOL 테스트 진입 시간이 만료되면,
    // 부저음 없이 Pass
    if( IsExpiredEolLimitTimer() == TRUE )
    {
        return SOUND_NONE;
    }

    // 현재 EOL 테스트 중이면, Pass
    if( GetEolStatus() == TRUE )
    {
        return SOUND_NONE;
    }

    // 누수 감지
    if( GetLeakStatus() == TRUE 
            || IsLeakSemi() == TRUE )
    {
        return SOUND_ERROR;
    }

    // 수위 센서 불량
    if( GetDrainLevel() != LEVEL_LOW )
    {
        return SOUND_ERROR;
    }

    // 필터 리드스위치 미감지 
    if( GetFilterStatus( ID_FILTER ) == FILTER_OPEN )
    {
        return SOUND_ERROR;
    }

    // MATCH FRONT와 MAIN CHP TYPE 
    if( GetSystem() != GetFrontSystem() )
    {
        return SOUND_ERROR;
    }

    // EOL 시작
    StartEol( EOL_TYPE_FRONT );

    TurnOffDrainPumpEx();
    MoveIceTrayIcingEol();
    StartDisplayInit();
    SetVersionDisp( 0 );

    // WIFI 
    WifiKey(WIFI_KEY_TEST);

    return SOUND_EOL;
}


// TIME SHORT 테스트 모드
static U8 OnTimeShort(void)
{
    // 부저음 없이 Pass
    if( IsExpiredTimeShortLimitTimer() == TRUE )
    {
        return SOUND_NONE;
    }

    // TIME SHORT 시작
    StartTimeShort();
    return SOUND_EOL;
}


// FCT 검사 시작
static U8 OnFctStart(void)
{
    // EOL 테스트 진입 시간이 만료되면,
    // 부저음 없이 Pass
    if( IsExpiredFctLimitTimer() == TRUE )
    {
        return SOUND_NONE;
    }


    // FCT 시작
    StartFct();

    // WIFI 
    WifiKey(WIFI_KEY_TEST);
    return SOUND_SELECT;
}

static U8 OnTestShortCompTime(void)
{
    // 압축기 구속 시간 0으로 초기화
#if CONFIG_TEST
    SetCompProtectOffTime(0);
#endif
    return SOUND_SELECT;
}


static U8 KeyContDownEx(void)
{
    if( GetClockDisp() == TRUE )
    {
        return KeyContDown();
    }

    return SOUND_NONE;
}

static U8 KeyContUpEx(void)
{
    if( GetClockDisp() == TRUE )
    {
        return KeyContUp();
    }

    return SOUND_NONE;
}


#if CONFIG_TEST_HOT_OUT
static U8 SelHotTest(void)
{
    if( GET_STATUS_VALVE_HOT_IN() == 0 )
    {
        OPEN_VALVE_HOT_IN();
    }
    else
    {
        CLOSE_VALVE_HOT_IN();
    }

    return SOUND_SELECT;
}
#endif

/* UV TEST CODE */
#if CONFIG_UV_MANUAL
extern U8 dbg_uv_ice_door;
extern U8 dbg_uv_water_out;
extern U8 dbg_uv_ice_tray;
extern U8 dbg_uv_ice_tank;
static U8 OnOffUvIceTank(void)
{
    dbg_uv_ice_tank = !dbg_uv_ice_tank;
    return SOUND_SETUP;
}

static U8 OnOffUvIceTray(void)
{
    dbg_uv_ice_tray = !dbg_uv_ice_tray;
    return SOUND_SETUP;
}

static U8 OnOffUvIceOut(void)
{
    dbg_uv_ice_door = !dbg_uv_ice_door;
    return SOUND_SETUP;
}

static U8 OnOffUvWaterOut(void)
{
    dbg_uv_water_out = !dbg_uv_water_out;

    return SOUND_SETUP;
}
#endif
