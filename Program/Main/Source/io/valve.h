#ifndef __VALVE_H__
#define __VALVE_H__

#include "prj_type.h"


#define VALVE_NOS               0x0001      // NOS
#define VALVE_ROOM_OUT          0x0002
#define VALVE_HOT_IN            0x0004
#define VALVE_HOT_OUT           0x0008
#define VALVE_HOT_DRAIN         0x0010      // NOS
#define VALVE_COLD_AIR          0x0020      // NOS
#define VALVE_COLD_OUT          0x0040
#define VALVE_COLD_IN           0x0080
#define VALVE_COLD_DRAIN        0x0100
#define VALVE_COLD_FLUSH        0x0200
#define VALVE_ICE_TRAY_IN       0x0400

// FITLER REVERSING-FLUSING SYSTEM
#define VALVE_FILTER_FEED       VALVE_COLD_OUT //VALVE_ICE_TRAY_IN
#define VALVE_FILTER_FLUSHING   VALVE_COLD_FLUSH
#define VALVE_FILTER_OUT        VALVE_COLD_DRAIN

#define VALVE_ALL ( \
        VALVE_NOS \
        | VALVE_COLD_AIR \
        | VALVE_COLD_DRAIN \
        | VALVE_HOT_IN \
        | VALVE_ROOM_OUT \
        | VALVE_HOT_OUT \
        | VALVE_COLD_OUT \
        | VALVE_ICE_TRAY_IN \
        | VALVE_COLD_IN \
        | VALVE_COLD_FLUSH \
        | VALVE_HOT_DRAIN \
        )

#define NORMAL_OPEN_VALVES  ( \
        VALVE_NOS \
        | VALVE_HOT_DRAIN \
        | VALVE_COLD_AIR )

void InitValve(void);

void OpenValve(U16 mu16Valve);

void CloseValve(U16 mu16Valve);

U16 GetValveStatus(void);

U8 IsOpenValve(U16 mu16Valve );

void ControlValve(void);

#endif /* __VALVE_H__ */
