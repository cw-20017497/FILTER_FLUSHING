#include "hw.h"
#include "reverse_flush_water_out.h"
#include "eeprom.h"
#include "valve.h"
#include "flow_meter.h"
//#include "front.h"
#include "water_out_common.h"
#include "water_out.h"
#include "sound.h"
//#include "drain_pump.h"
//#include "cold_water.h"
#include "error.h"
//#include "filter.h"
#include "hal_pump.h"


// 저유량 조건 상관 없음.
// 플러싱은 3분
//


U8  dbg_reverse_step = 0;
U16  dbg_reverse_repeat = 0;
WaterOut_T   ReverseOut;


static void InitOutEventList(void);
static void UpdateReverseFlushCurrentAmount(void);
static U32 GetFindOutTargetTime( WaterOutType_T mType);


#define VALVE_DELAY_TIME            10U

// 잠깐 완료 대기 
#define MAX_DONE_WAIT_TIME          ( 1U * 100U )      // 1sec * 100ms 


#if 0
#define REVERSE_RELEASE_AIR_TIME    ( 3U * 100U )
#define REVERSE_IN_AIR_TIME         ( 5U * 100U )
#define REVERSE_PRESSURE_AIR_TIME   ( 10U * 100U )
#define REVERSE_BREAK_TIME          ( 1U * 100U )
#define REVERSE_FLUSHING_TIME       ( 20U * 100U )
#define REVERSE_FEED_OUT_TIME       ( 60U * 60U * 100U )
#else
U32 REVERSE_INIT_OUT_TIME       = (2L * 60U * 100UL );
U32 REVERSE_RELEASE_AIR_TIME    = (3L * 100UL );
U32 REVERSE_IN_AIR_TIME         = (5L * 100UL );
U32 REVERSE_PRESSURE_AIR_TIME   = (10L * 100UL );
U32 REVERSE_BREAK_TIME          = (1UL * 100UL );
U32 REVERSE_FLUSHING_TIME       = (20UL * 100UL );
U32 REVERSE_FEED_OUT_TIME       = (60UL * 60UL * 100UL );
U8  REVERSE_REPEAT_COUNT        = 3;
#endif

const static WaterOutTable_T ReverseFlushTableList[] = 
{
    { OUT_AMOUNT_REVERSE_RELEASE_AIR,   3  },
    { OUT_AMOUNT_REVERSE_IN_AIR,        5  },
    { OUT_AMOUNT_REVERSE_PRESSURE_AIR,  10 },
    { OUT_AMOUNT_REVERSE_DOING,         20 },
    { OUT_AMOUNT_REVERSE_OUT,           60 }
};

#define SZ_FLUSH_TABLE   (sizeof(ReverseFlushTableList)/sizeof(WaterOutTable_T))
#define DEFAULT_AMOUNT  0UL

// 용량별 추출 시간을 반환한다.
static U32 GetFindOutTargetTime( WaterOutType_T mType)
{
    U8 i;

    for( i = 0; i < SZ_FLUSH_TABLE ; i++ )
    {
        if( ReverseFlushTableList[ i ].Type == mType )
        {
            return ReverseFlushTableList[ i ].Amount * 60U * 10U ;
        }
    }

    return DEFAULT_AMOUNT;
}


void InitReverseFlushOut(void)
{
    ReverseOut.Out       = FALSE;
    ReverseOut.PrevOut   = FALSE;

    ReverseOut.TargetAmount  = 0UL;
    ReverseOut.CurrentAmount = 0UL;

    ReverseOut.Mode = 0U;
}


U32 GetReverseFlushCurrentAmount(void)
{
    return ReverseOut.CurrentAmount;
}

void SetReverseFlushCurrentAmount(U32 mu32Amount)
{
    ReverseOut.CurrentAmount = mu32Amount;
}


// 상세 제어 모드 
void SetReverseFlushOutMode( U16 mu16Mode )
{
    ReverseOut.Mode |= mu16Mode;
}

void ClearReverseFlushOutMode( U16 mu16Mode )
{
    ReverseOut.Mode &= ~mu16Mode;
}

U8 IsSetReverseFlushOutMode(U16 mu8Mode )
{
    if( (ReverseOut.Mode & mu8Mode ) == mu8Mode )
    {
        return TRUE;
    }

    return FALSE;
}


U16 GetReverseFlushOutMode(void )
{
    return ReverseOut.Mode;
}


void TurnOnReverseFlushOut(WaterOutType_T mType)
{
    (void)mType;

    ReverseOut.Out          = TRUE;

    ReverseOut.TargetAmount = GetFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
}


U8 IsReverseFlushOut(void)
{
    return ReverseOut.Out;
}


void TurnOffReverseFlushOut(void)
{
    ReverseOut.Out = FALSE;
}


static U8 IsChangedOutStatus(void)
{
    if( ReverseOut.Out != ReverseOut.PrevOut )
    {
        ReverseOut.PrevOut = ReverseOut.Out;

        return TRUE;
    }

    return FALSE;
}

static void StartReverseFlushOut(void)
{
    SetReverseFlushOutMode( REVERSE_FLUSH_OUT_CHP );
}


static void StopReverseFlushOut(void)
{
    ClearReverseFlushOutMode( REVERSE_FLUSH_OUT_CANCEL );
    SetReverseFlushOutMode( REVERSE_FLUSH_DONE );
}



typedef U8 (*DoAction_T)(U8 *pStep);
typedef struct _reverse_flush_out_event_
{
    U16 Mode;               // 출수 동작 상세 제어 모드
    U8  Step;               // 이벤트 헨들러 스텝 변수
    DoAction_T DoAction;    // 제어 모드에 대한 이벤트 핸들러
} ReverseFlushOutEvent_T;

static U8 DoReverseInit(U8 *pStep);
static U8 DoReverseFlushing(U8 *pStep);
static U8 DoDone(U8 *pStep);
static ReverseFlushOutEvent_T OutEventList[] = 
{
    { REVERSE_FLUSH_INIT,           0U, DoReverseInit     },     
    { REVERSE_FLUSH_DOING,          0U, DoReverseFlushing },     
    { REVERSE_FLUSH_DONE,           0U, DoDone            }
};
#define SZ_OUT_EVENT_LIST  (sizeof(OutEventList)/sizeof(ReverseFlushOutEvent_T))

static void InitOutEventList(void)
{
    U8 i;

    for( i = 0; i < SZ_OUT_EVENT_LIST ; i++ )
    {
        OutEventList[i].Step = 0U;
    }
}

static void Outflow(void)
{
    U8 i;
    U8 mu8Ret = FALSE;
    U8 mu8Step;
    U16 mu16Mode;
    ReverseFlushOutEvent_T *pList = NULL;

    for( i = 0; i < SZ_OUT_EVENT_LIST ; i++ )
    {
        pList = &OutEventList[ i ];

        if( IsSetReverseFlushOutMode( pList->Mode ) == TRUE )
        {
            if( pList->DoAction != NULL )
            {
                mu8Ret = pList->DoAction( &pList->Step );
                if( mu8Ret == TRUE )
                {
                    ClearReverseFlushOutMode( pList->Mode );
                }
                break;
            }
        }
    }
}

enum
{
    DBG_STEP_POWER_ON,
    DBG_STEP_RELEASE_AIR,
    DBG_STEP_IN_AIR,
    DBG_STEP_PRESSURE_AIR,
    DBG_STEP_BREAK,

    DBG_STEP_FLUSHING,
    DBG_STEP_OUT,
    DBG_STEP_DONE,
};

static U8 DoReverseInit(U8 *pStep)
{
    switch( *pStep )
    {
        case 0:
            dbg_reverse_step = DBG_STEP_POWER_ON;

            OpenValve( VALVE_FILTER_FEED );
            CloseValve( VALVE_FILTER_FLUSHING );
            OpenValve( VALVE_FILTER_OUT );

            ReverseOut.WaitTime = REVERSE_INIT_OUT_TIME;
            HAL_TurnOffPump( HAL_PUMP_DRAIN );

            (*pStep)++;
            break;

        case 1:
            // WAIT RELEASE AIR
            if( ReverseOut.WaitTime != 0 )
            {
                ReverseOut.WaitTime--;
            }
            else
            {
                CloseValve( VALVE_FILTER_FEED );
                CloseValve( VALVE_FILTER_FLUSHING );
                CloseValve( VALVE_FILTER_OUT );

                return TRUE;
                (*pStep)++;
            }
            break;

        default:
            (*pStep) = 0;

            break;
    }

    return FALSE;
}

enum
{
    REVERSE_INIT,
    REVERSE_RELEASE_AIR,
    REVERSE_RELEASE_AIR_WAIT,
    REVERSE_IN_AIR_WAIT,
    REVERSE_PRESSURE_AIR,
    REVERSE_PRESSURE_AIR_WAIT,
    REVERSE_PRESSURE_BREAK_TIME,
    REVERSE_FLUSHING_WAIT,
    REVERSE_FEED_OUT,
    REVERSE_FEED_OUT_DONE
};


U8  ReverseRepeat = 0;
static U8 DoReverseFlushing(U8 *pStep)
{
    static U16 mu16Time = 0U; // @10ms


    switch( *pStep )
    {
        case REVERSE_INIT:
            CloseValve( VALVE_FILTER_FEED );
            CloseValve( VALVE_FILTER_FLUSHING );
            CloseValve( VALVE_FILTER_OUT );

            mu16Time = VALVE_DELAY_TIME;
            (*pStep)++;
            break;

        case REVERSE_RELEASE_AIR:
            if( mu16Time != 0 )
            {
                mu16Time--;
            }
            else
            {
                ReverseRepeat   = REVERSE_REPEAT_COUNT;
                dbg_reverse_step = DBG_STEP_RELEASE_AIR;

                ReverseOut.WaitTime = REVERSE_RELEASE_AIR_TIME;
                OpenValve( VALVE_FILTER_FLUSHING );
                (*pStep)++;
            }
            break;

        case REVERSE_RELEASE_AIR_WAIT:
            // WAIT RELEASE AIR
            if( ReverseOut.WaitTime != 0 )
            {
                ReverseOut.WaitTime--;
            }
            else
            {
                dbg_reverse_step = DBG_STEP_IN_AIR;
                // TURN ON PUMP FOR IN AIR
                ReverseOut.WaitTime = REVERSE_IN_AIR_TIME;
                HAL_TurnOnPump( HAL_PUMP_DRAIN, 24.0f );
                (*pStep)++;
            }
            break;

        case REVERSE_IN_AIR_WAIT:
            // WAIT RELEASE AIR
            if( ReverseOut.WaitTime != 0 )
            {
                ReverseOut.WaitTime--;
            }
            else
            {
                // TURN OFF PUMP 
                mu16Time = VALVE_DELAY_TIME;

                CloseValve( VALVE_FILTER_FLUSHING );
                HAL_TurnOffPump( HAL_PUMP_DRAIN );
                (*pStep)++;
            }
            break;


        case REVERSE_PRESSURE_AIR:
            if( mu16Time != 0 )
            {
                mu16Time--;
            }
            else
            {
                dbg_reverse_step = DBG_STEP_PRESSURE_AIR;
                ReverseOut.WaitTime = REVERSE_PRESSURE_AIR_TIME;
                OpenValve( VALVE_FILTER_FEED );
                (*pStep)++;
            }
            break;

        case REVERSE_PRESSURE_AIR_WAIT:
            // WAIT RELEASE AIR
            if( ReverseOut.WaitTime != 0 )
            {
                ReverseOut.WaitTime--;
            }
            else
            {
                // CLOSE FEED VALVE 
                dbg_reverse_step = DBG_STEP_BREAK;
                ReverseOut.WaitTime = REVERSE_BREAK_TIME;
                CloseValve( VALVE_FILTER_FEED );
                (*pStep)++;
            }
            break;


        case REVERSE_PRESSURE_BREAK_TIME:
            // WAIT RELEASE AIR
            if( ReverseOut.WaitTime != 0 )
            {
                ReverseOut.WaitTime--;
            }
            else
            {
                // OPEN FEED VALVE - REVERSE FLUSHING
                dbg_reverse_step = DBG_STEP_FLUSHING;
                ReverseOut.WaitTime = REVERSE_FLUSHING_TIME;
                OpenValve( VALVE_FILTER_FLUSHING );
                (*pStep)++;
            }
            break;

        case REVERSE_FLUSHING_WAIT:
            // WAIT RELEASE AIR
            if( ReverseOut.WaitTime != 0 )
            {
                ReverseOut.WaitTime--;
            }
            else
            {
                // 3회 반복
                if( ReverseRepeat != 0)
                {
                    ReverseRepeat--;
                }

                if( ReverseRepeat != 0 )
                {
                    (*pStep) = REVERSE_RELEASE_AIR_WAIT;
                }
                else
                {
                    mu16Time = VALVE_DELAY_TIME;
                    CloseValve( VALVE_FILTER_FLUSHING );
                    (*pStep)++;
                }
            }
            break;

        case REVERSE_FEED_OUT:
            if( mu16Time != 0 )
            {
                mu16Time--;
            }
            else
            {
                dbg_reverse_step = DBG_STEP_OUT;
                ReverseOut.WaitTime = REVERSE_FEED_OUT_TIME;
                OpenValve( VALVE_FILTER_FEED );
                OpenValve( VALVE_FILTER_OUT );
                (*pStep)++;
            }
            break;


        case REVERSE_FEED_OUT_DONE:
            // WAIT RELEASE AIR
            if( ReverseOut.WaitTime != 0 )
            {
                ReverseOut.WaitTime--;
            }
            else
            {
                // RESTART AGAING!!!
                CloseValve( VALVE_FILTER_FEED );
                CloseValve( VALVE_FILTER_OUT );
                (*pStep) = REVERSE_INIT;

                dbg_reverse_step = DBG_STEP_DONE;
                dbg_reverse_repeat++;
                SaveEepromId( EEP_ID_REVERSE_REPEAT );
            }
            break;

        default:
            (*pStep) = 0;

            break;
    }

    return FALSE;
}


static U8 DoDone(U8 *pStep)
{
    // Close All Valvaes..
    CloseValve( VALVE_FILTER_FEED );
    CloseValve( VALVE_FILTER_FLUSHING );
    CloseValve( VALVE_FILTER_OUT );

    HAL_TurnOffPump( HAL_PUMP_DRAIN );


    // Unreigster flow meter..
    RegisterFlowMeterId( FLOW_ID_WATER, NULL );


    Sound( SOUND_EFFLUENT_END );
    SetWaterOutSelect( SEL_WATER_ROOM );
    return TRUE;
}


// 추출량 반환 ( percent )
U8 GetReverseFlushOutPercent(void)
{
    return CalcPercent( ReverseOut.TargetAmount, ReverseOut.CurrentAmount );
}

// 추출량 ml 단위 반환
U16 GetReverseFlushOutAmount(void)
{
    return CalcLiter( ReverseOut.CurrentAmount );
}


static void UpdateReverseFlushCurrentAmount(void)
{
    ReverseOut.CurrentAmount++;     
}

static U8 IsDoneReverseFlushOut(void)
{
    // 단수 조건이면 종료한다.
    if( IsErrorType( ERR_TYPE_ROOM_WATER_OUT ) == TRUE )
    {
        return TRUE;
    }

    return FALSE;
}

// 추출 제어
// RETURN 0x01 : 추출 완료
// 0x00 추출 중...
U8 ControlWaterReverseFlushOut(void)
{
    // 추출 상태 변경
    if( IsChangedOutStatus() == TRUE )
    {
        // 정지 상태이면, 추출 시작
        if( ReverseOut.Out == TRUE )
        {
            // 추출 시작
            StartReverseFlushOut();
            InitOutEventList();
        }
        else
        {
            // 추출 중이였으면, 추출 중지
            StopReverseFlushOut();

            StopCheckOutage();
        }
    }


    // 목표 추출량 만큼만 추출하고 종료한다.
    if( ReverseOut.Out == TRUE )
    {
        if( IsDoneReverseFlushOut() == TRUE )
        {
            SetReverseFlushCurrentAmount( 0UL );
            TurnOffReverseFlushOut();
            return WATER_OUT_DONE;
        }
        else if( IsSetReverseFlushOutMode( REVERSE_FLUSH_DONE ) == TRUE )
        {
            TurnOffReverseFlushOut();
            return WATER_OUT_DONE;
        }
    }

    Outflow();
    return WATER_OUT_GOING;
}
