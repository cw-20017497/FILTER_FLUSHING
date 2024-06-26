#ifndef __VOICE_H__
#define __VOICE_H__

#include "prj_type.h"

/*
   0	?? ??? ?????.
   1	?? ??? ???? ? ????. 
   2	?? ??? ?????.
   3	?? ??? ???? ? ????
   4	????? ??????. ?? ??? 3? ???? ??? ?? ???? ? ????. 
   5	??? ??????. ?? ???  3? ???? ???  ?? ???? ? ????. 
   6	?? ??? ??????. ?? ?? ??? 3? ???? ?? ???  ?? ???? ? ????. 
   7	??? ?? ????. ? ?? ??? ?? ? ????. 
   8	??? ???? ?????
   9	???? ??? ?????.
   10	???? ??? ???????. 
   11	???? ??? ?? ??????. 
   12	???? ??? ???????. 
   13	 ??? ???? ???? ???? ?????. 
   14	 ??? ???? ???? ???? ???????.  ???? ??? ??? ?????. 
   15	??? ?? ??? ??????. ??? ?????.
   16	??? ?? ??? ?????. ??? ? ??? ?????.
   17	????? ??? ???? ??? ??? ?? ? ????. ????? ?????.
   18	????? ??? ???? ??? ?? ?? ??? ?????. ????? ?????.
   19	??? ? ??? ??? ???????. ??????? ?????? ????? ?????.
   20	??? ???????. ??????? ?????? ????? ?????.
   21	???? ?????. 
   22	??? ?????. 
   23	?? ?? ???.
   24	?? ??? ???????. 
   25	??? ??? ???????.
   26	????? ?????.
   27	17_Meoldy
   28	76_Meoldy
   29	77_Meoldy
   30	87_Meoldy
   31	91_Meoldy
   32	92_Meoldy
   33	93_Meoldy
   34	94_Meoldy
*/

/* ID LIST */
typedef enum
{
#if 0
    ID_LOCK_HOT,
    ID_UNLOCK_HOT,

    ID_LOCK_ALL,
    ID_UNLOCK_ALL,

    ID_ALARM_LOCK_HOT,
    ID_ALARM_COLD_OFF,
    ID_ALARM_LOCK_ALL,

    ID_PREHEAT_HOT,
    ID_CAREFUL_HOT,

    ID_WIFI_CONNECTING,
    ID_WIFI_CONNECTED,
    ID_WIFI_FAIL_CONNECTING,
    ID_WIFI_DISCONNECTED,

    ID_FLUSHING,
    ID_FLSUHED,

    ID_OPEN_TOP_COVER,
    ID_CLOSE_WATER_VALVE,

    ID_ERROR_COLD,
    ID_ERROR_HOT,
    ID_ERROR_FEED,
    ID_ERROR_LEAK,

    ID_SOUND_VOICE,
    ID_SOUND_EFFECT,
    ID_SOUND_MUTE,

    ID_TEST_MODE,
    ID_ERROR,
    ID_CALL_SERVICE,
    
    ID_MELODY_17,
    ID_MELODY_76,
    ID_MELODY_77,
    ID_MELODY_87,
    ID_MELODY_91,
    ID_MELODY_92,
    ID_MELODY_93,
    ID_MELODY_94,
#endif

    ID_NONE = 255

} VoiceId_T;

void InitVoice(void);

void SetVoiceLevel(U8 mu8Level);

U8 GetVoiceLevel(void);

void PlayVoice(VoiceId_T id);

void StopVoice(void);

void SetVoiceVolume(U8 mu8Vol);

U8 GetVoiceVolume(void);


#endif /* __VOICE_H__ */
