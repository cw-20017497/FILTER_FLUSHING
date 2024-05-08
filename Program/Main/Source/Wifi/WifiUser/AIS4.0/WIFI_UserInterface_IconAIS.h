
#ifndef _WIFI_UserInterface_ICON_AIS_H_
#define _WIFI_UserInterface_ICON_AIS_H_


typedef struct _wifi_status_
{
    U8 OnOff;
    U8 FirstParing;
    U8 FotaModule;
    U8 FotaMain;
} WIFI_Status_T;


void UserWriteEep ( U16 mu8Addr, U8 mu8Val );
U8 UserReadEep ( U16 mu8Addr );
void UserBuzzSound ( U8 mu8Buzz );

U8 GetUserSystemStatus ( U8 mu8Value );

U16 GetUserSystemError ( U16 mu16Err );
U16 GetUserSystemFunction ( U16 mu16Func );
I16 GetUserSystemSensor ( U16 mu16Sen );
I16 GetUserSystemPart ( U16 mu16Part );
U32 GetUserSystemAccumulate ( U16 mu16Acc );

void SetUserSystemFunction ( U16 mu16Func, U16 mData );

void ClearUserSystemData ( void );
void SetUserSystemDisplay ( U8 mu8Display );
void SetUserSystemTime ( U16 mYear, U16 mMonth, U16 mDate, U16 mHour, U16 mMin, U16 mSec, U16 mDay );

/* Status Progress */
void ProcessChangedStatus(void);

#endif

