/**
 * 배수( 자동 / 수동 ) 기능에 대한 모듈
 *
 */
#include "empty_cold_tank.h"
#include "valve.h"
#include "error.h"

enum
{
    STEP_EMPTY_READY,      // 배수 준비
    STEP_EMPTY,
    STEP_EMPTY_COLD,
    STEP_EMPTY_CLOSE,
    STEP_EMPTY_DONE,
};

// 냉수 탱크 800ml 추출 시간은 5분 30초
// 설정 시간은 5분 40초
#define FAUCET_TIMEOUT       3400UL //  5m 40sec * 60s * 10, @100ms

// 설정 시간은 12분
#define DRAIN_TIMEOUT       7200UL //  12m * 60s * 10, @100ms


EmptyColdTank_T     EmptyCold;


static U8 IsValidStartDrain(void);

void  InitEmptyColdTank(void)
{
    EmptyCold.Start  = FALSE;

}

void  GetEmptyColdTankData(EmptyColdTank_T *pData)
{
    MEMCPY( (void __FAR *)pData, (const void __FAR *)&EmptyCold, sizeof( EmptyColdTank_T ));
}


// 자동 배수 시작/중지
void StartEmptyColdTank(ColdEmptyType_T mu8Type)
{
    EmptyCold.Start = TRUE;

    EmptyCold.Type = mu8Type;
    EmptyCold.Step  = 0;
}


//void StartEmptyColdTank(U8 mu8Type)
//{
//    EmptyCold.Start = TRUE;
//}

void StopEmptyColdTank(void)
{
    if( EmptyCold.Start == TRUE )
    {
        if( EmptyCold.Step <= STEP_EMPTY_COLD )
        {
            EmptyCold.Step = STEP_EMPTY_CLOSE;
        }
    }
}

static void StopStatus(void)
{
    EmptyCold.Start = FALSE;
}

U8 IsStartEmptyColdTank(void)
{
    return EmptyCold.Start;
}

ColdEmptyType_T GetEmptyColdTankType(void)
{
    return EmptyCold.Type;
}

// 냉수 추출 파우셋 밸브로 비우기
static U8 DoEmptyColdFaucet(void)
{
    static U8 mu16WaitTime = 0;
    U8 mStatus = FALSE;


    switch(EmptyCold.Step)
    {
        case STEP_EMPTY_READY:
            CloseValve( VALVE_COLD_IN );
            CloseValve( VALVE_COLD_OUT );
            CloseValve( VALVE_COLD_DRAIN );

            OpenValve( VALVE_COLD_AIR );

            EmptyCold.ExitTime = FAUCET_TIMEOUT;
            mu16WaitTime = 2;   // 200ms..
            EmptyCold.Step++;
            break;

        case STEP_EMPTY:
            if( mu16WaitTime != 0 )
            {
                mu16WaitTime--;
            }
            else
            {
                OpenValve( VALVE_COLD_OUT );
                EmptyCold.Step++;
            }
            break;

        case STEP_EMPTY_COLD:
            // 냉수 탱크 비우기
            CloseValve( VALVE_COLD_IN );
            CloseValve( VALVE_COLD_DRAIN );

            OpenValve( VALVE_COLD_AIR );
            OpenValve( VALVE_COLD_OUT );

            // 최대 배수 시간 설정
            if( EmptyCold.ExitTime != 0 )
            {
                EmptyCold.ExitTime--;
            }
            else
            {
                EmptyCold.Step++;
            }
            break;

        case STEP_EMPTY_CLOSE:
            CloseValve( VALVE_COLD_IN );
            CloseValve( VALVE_COLD_OUT );

            OpenValve( VALVE_COLD_AIR );
            CloseValve( VALVE_COLD_DRAIN );

            EmptyCold.Step++;
            break;

        case STEP_EMPTY_DONE:
            mStatus = TRUE;
            break;

        default:
            EmptyCold.Step = 0;
            break;
    }


    return mStatus;
}

// 냉수 탱크 배수 밸브로 비우기
static U8 DoEmptyColdDrainValve(void)
{
    static U8 mu16WaitTime = 0;
    U8 mStatus = FALSE;


    switch(EmptyCold.Step)
    {
        case STEP_EMPTY_READY:
            CloseValve( VALVE_COLD_IN );
            CloseValve( VALVE_COLD_OUT );
            CloseValve( VALVE_COLD_DRAIN );

            OpenValve( VALVE_COLD_AIR );

            EmptyCold.ExitTime = DRAIN_TIMEOUT;
            mu16WaitTime = 2;   // 200ms..
            EmptyCold.Step++;
            break;

        case STEP_EMPTY:
            if( mu16WaitTime != 0 )
            {
                mu16WaitTime--;
            }
            else
            {
                OpenValve( VALVE_COLD_DRAIN );
                EmptyCold.Step++;
            }
            break;

        case STEP_EMPTY_COLD:
            // 냉수 탱크 비우기
            CloseValve( VALVE_COLD_IN );
            CloseValve( VALVE_COLD_DRAIN );

            OpenValve( VALVE_COLD_AIR );
            OpenValve( VALVE_COLD_DRAIN );

            // 최대 배수 시간 설정
            if( EmptyCold.ExitTime != 0 )
            {
                EmptyCold.ExitTime--;
            }
            else
            {
                EmptyCold.Step++;
            }
            break;

        case STEP_EMPTY_CLOSE:
            CloseValve( VALVE_COLD_IN );
            CloseValve( VALVE_COLD_OUT );

            OpenValve( VALVE_COLD_AIR );
            CloseValve( VALVE_COLD_DRAIN );

            EmptyCold.Step++;
            break;

        case STEP_EMPTY_DONE:
            mStatus = TRUE;
            break;

        default:
            EmptyCold.Step = 0;
            break;
    }


    return mStatus;
}

static U8 DoEmptyCold(void)
{
    if( EmptyCold.Type == TYPE_FAUCET )
    {
        return DoEmptyColdFaucet();
    }
    
    // if EMPTY_COLD_TYPE_DRAIN_VALVE
    return DoEmptyColdDrainValve();
}

void ControlEmptyColdTank(void)
{
    U8 mu8Ret = FALSE;
    

    if( IsStartEmptyColdTank() == FALSE )
    {
        return ;
    }

    // 에러 발생시 종료
    if( IsErrorType( ERR_TYPE_EMPTY_COLD ) == TRUE )
    {
        StopEmptyColdTank();
    }

    // 배수...
    if( DoEmptyCold() == TRUE )
    {
        StopStatus();
    }
}

