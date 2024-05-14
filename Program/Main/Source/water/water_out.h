#ifndef __WATER_OUT_H__
#define __WATER_OUT_H__

#include "prj_type.h"



void  InitWaterOut(void);


/* 선택 */
#define  SEL_WATER_ROOM    0
#define  SEL_WATER_COLD    1
#define  SEL_WATER_HOT     2
#define  SEL_WATER_SODA    3
#define  SEL_WATER_FLUSH   4
#define  SEL_WATER_CODY    5
#define  SEL_WATER_REVERSE 6
#define  MAX_WATER_ID      7
void  SetWaterOutSelect(U8 mu8Id );
U8    GetWaterOutSelect(void);
U8    GetWaterOutLastedSelect(void);

void  UpdateWaterOutReturnTime(void);
void ResetWaterOutReturnTime(void);
//void  UpdateWaterOutReturnTimeUser(void);

U16 GetWaterOutReturnTime(void);
U16 GetWaterOutReturnTimeAmount(void);
//U16 GetWaterOutReturnTimeUser(void);

/* 추출/중지 */
U8 StartWaterOutFlush(U8 mu8Sound);
void StartWaterOutFlushPowerOn(void);

void  StartWaterOut(void);
void  StopWaterOut(void);
U8 GetWaterOut(void);

// 재추출 금지 시간
U8 IsExpiredOutProtectTime(void);

/* 연속 추출 */
void  SetWaterOutContinue(U8 mu8Mode );
U8    GetWaterOutContinue(void);

/* 연속 추출 - 버튼으로 길게 누를 때 */
void SetWaterOutInfinityLongKey(void);
void ClearWaterOutInfinityLongKey(void);


#define  FIXED_AMOUNT_HALF        0  /* 120mL */
#define  FIXED_AMOUNT_ONE         1  /* 250mL */
#define  FIXED_AMOUNT_TWO         2  /* 500mL */
#define  FIXED_AMOUNT_INFINITY    3  /* INFINITY */
#define  FIXED_AMOUNT_USER        4  /* USER */
#define  FIXED_AMOUNT_TANK_CLEAN  5  /* TANK CLEAN MODE */
#define  MAX_FIXED_AMOUNT_NUM     6  
void  SetWaterOutFixedAmountType(U8 mType );
U8 GetWaterOutFixedAmoutType(void);
U8    GetWaterOutAmoutType(void);
void  ClearWaterOutInfinityAmout(void);

U8 GetWaterOutConfFixedAmountType(void);
void SetWaterOutConfFixedAmountType(U8 mu8Type);

#define  INFINITY_3MIN_TIMEOUT            18000UL   // 3min @10ms
#define  INFINITY_10MIN_TIMEOUT           60000UL   // 60min @10ms
#define  INFINITY_120MIN_TIMEOUT         720000UL   // 120min @10ms
void SetWaterOutConfInfinityTimeOut(U32 mu32TimeOut);


/* USER AMOUNT */
void SetWaterOutConfigUser(U8 mu8Config);
U8 GetWaterOutConfigUser(void);

// mLiter 단위 get/set
#define MIN_USER_AMOUNT_LITER   130U     // 130ml
#define MAX_USER_AMOUNT_LITER   1000U    // 1000ml
U16 GetWaterOutUserAmountLiter(void);
void SetWaterOutUserAmountLiter(U16 mu16Liter);

U32 GetWaterOutUserAmount(void);
void SetWaterOutUserAmount(U32 mu32UserAmount);

void IncrementWaterOutAmount(void);
void IncrementWaterOutAmountLimit(void);

/* 추출 시간 (마지막, 일일, 누적) */
#define LASTED_TIME_ID    0 /* 마지막 추출 시간  */
#define DAILY_TIME_ID     1 /* 일일 단위 추출 시간 */
#define TOTAL_TIME_ID     2 /* 누적 시간 */
#define MAX_TIME_ID       3
void  SetWaterOutTime(U8 mu8Type, U8 mu8Id, U32 mu32Time );
U32  GetWaterOutTime(U8 mu8Type, U8 mu8Id );


// 추출 시간을 컵 사이즈로 반환
//U8 GetWaterOutCupNum(U8 mu8Type, U8 mu8Id);


U8  GetWaterOutPercent(void);

U16  GetWaterOutAmount(void);

// 추출 밸브 제어 
void  ControlWaterOut(void);

#endif /* __WATER_OUT_H__ */
