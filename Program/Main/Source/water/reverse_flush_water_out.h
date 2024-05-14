#ifndef __REVERSE_FLUSH_WATER_OUT_H__
#define __REVERSE_FLUSH_WATER_OUT_H__

#include "prj_type.h"
#include "water_out_type.h"


// 상세 제어 모드
#define REVERSE_FLUSH_INIT          0x0001     
#define REVERSE_FLUSH_DOING         0x0002     
#define REVERSE_FLUSH_DONE          0x0004     


// OUT SETUP - CHPI
#define REVERSE_FLUSH_OUT_CHP  (\
        REVERSE_FLUSH_INIT          | \
        REVERSE_FLUSH_DOING     \
        )


// CANCEL 
#define REVERSE_FLUSH_OUT_CANCEL  (\
        REVERSE_FLUSH_INIT |\
        REVERSE_FLUSH_DOING \
        )


extern U32 REVERSE_RELEASE_AIR_TIME;
extern U32 REVERSE_IN_AIR_TIME;
extern U32 REVERSE_PRESSURE_AIR_TIME;
extern U32 REVERSE_BREAK_TIME;
extern U32 REVERSE_FLUSHING_TIME;
extern U32 REVERSE_FEED_OUT_TIME;

extern U8  dbg_reverse_step;
extern U16  dbg_reverse_repeat;


void InitReverseFlushOut(void);

// Get/Set Flush Current Amount
U32 GetReverseFlushCurrentAmount(void);
void SetReverseFlushCurrentAmount(U32 mu32Amount);


// 추출 시작 명령
// mType : 추출 타입 - unused...
void TurnOnReverseFlushOut(WaterOutType_T mType);

// 추출 상태
// TRUE : 추출, FALSE, 정지
U8 IsReverseFlushOut(void);

// 추출 중지 명령
void TurnOffReverseFlushOut(void);

U8 GetReverseFlushOutPercent(void);

// 추출량 ml 단위 반환
U16 GetReverseFlushOutAmount(void);

// 추출 제어
U8 ControlWaterReverseFlushOut(void);

// 제어 모드 
void SetReverseFlushOutMode( U16 mu16Mode );
U16 GetReverseFlushOutMode(void );
void ClearReverseFlushOutMode( U16 mu16Mode );
U8 IsSetReverseFlushOutMode(U16 mu8Mode );

#endif /* __REVERSE_FLUSH_WATER_OUT_H__ */
