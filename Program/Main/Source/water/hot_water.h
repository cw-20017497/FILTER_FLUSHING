#ifndef __HOT_WATER_H__
#define __HOT_WATER_H__

#include "prj_type.h"

typedef struct _hot_water_
{
    U8 InitFull;    // TRUE : 초기 배수 완료
    U8 Select;      // 온수 추출 레벨 (MILK, TEA, COFFE)

    //U16 PowerOnWaitTime;    // 전원 RESET 후 대기 시간..
    //U16 InitFullWaitTime;   // 자동 초기 배수 동작 대기 시간

    U16 HeaterOnTime;
    U8  ConfUser;           // 사용자 온도 ( 95도 )

    U8 Cooling;                // 온수 추출 테스트 기능
    U8 Test;                // 온수 추출 테스트 기능
} HotWater_T;

// 초기화
void  InitHotWater(void);
void  GetHotWaterData(HotWater_T *pData);

// 최초 만수위( 직수에서는 초기 탱크 채우기 )
void  SetHotWaterInitFull(U8 mu8Val );
U8    GetHotWaterInitFull(void);

// COOLING 
void  SetHotWaterCooling(U8 mu8Val );
U8    GetHotWaterCooling(void);

// 온수 온도
#define SEL_HOT_MILK    0U       // MILK
#define SEL_HOT_TEA     1U       // TEA
#define SEL_HOT_COFFEE  2U       // COFFEE
#define SEL_HOT_USER    3U       // COFFEE
#define SEL_HOT_NUM     4U
void SetHotSelect(U8 mu8Sel );
U8   GetHotSelect(void);

void ClearHotHeaterOnTime(void);

void SetHotConfigUser(U8 mu8Conf );
U8   GetHotConfigUser(void);

U16  GetHotSelectTemp(U8 mu8Sel);

void  SetHotConfigTest(U8 mu8Test);
U16  GetHotConfigTest(void);

U8 IsExpiredHeaterOnTime(void);
void CheckHotOutHeaterOnTime(void);

#endif /* __HOT_WATER_H__ */
