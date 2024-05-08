#include "hot_water_temp_table.h"

#include "hot_water.h"
#include "hot_water_out.h"
#include "hot_water_heater.h"

/*
 *     [ PRE-HEAT,   TARGET-HEAT ]
 *        xxx            xxx        [ CLASS LOW ]
 *        xxx            xxx        [ CLASS MID ]
 *        xxx            xxx        [ CLASS HIGH ]
 *
 *  MILK    : 45'c
 *  TEA     : 70'c
 *  COFFE   : 85'c
 *  USER    : 95'c
 */

typedef struct _hot_temp_
{
    TEMP_T  tPreHeat;
    TEMP_T  tOut;
} TargetTemp_T;


static TargetTemp_T HotOutTempList_Low[ SEL_HOT_NUM ][ HEATER_CLASS_NUM ] = 
{
    // MILK ( 45'C)
    {
        /* PRE-HEAT,   TARGET */
        { 35.0f,       45.0f },     // heater class low     
        { 35.0f,       45.0f },     // heater class mid
        { 35.0f,       45.0f }      // heater class high
    },

    // TEA ( 70'C)
    {
        /* PRE-HEAT,   TARGET */
        { 65.0f,       73.0f },     // heater class low
        { 60.0f,       73.0f },     // heater class mid
        { 55.0f,       73.0f }      // heater class high
    },


    // COFFEE ( 85'C)
    {
        /* PRE-HEAT,   TARGET */
        { 78.0f,       89.5f },     // heater class low
        { 75.0f,       89.5f },     // heater class mid
        { 70.0f,       89.5f }      // heater class high
    },

    // User
    {
        /* PRE-HEAT,   TARGET */
        { 78.0f,       98.0f },     // heater class low
        { 75.0f,       98.0f },     // heater class mid
        { 70.0f,       97.0f }      // heater class high
    }
};

static TargetTemp_T HotOutTempList_Mid[ SEL_HOT_NUM ][ HEATER_CLASS_NUM ] = 
{
    // MILK ( 45'C)
    {
        /* PRE-HEAT,   TARGET */
        { 35.0f,       45.0f },     // heater class low     
        { 35.0f,       45.0f },     // heater class mid
        { 35.0f,       45.0f }      // heater class high
    },

    // TEA ( 70'C)
    {
        /* PRE-HEAT,   TARGET */
        { 60.0f,       73.0f },     // heater class low
        { 64.0f,       73.0f },     // heater class mid
        { 58.0f,       73.0f }      // heater class high
    },


    // COFFEE ( 85'C)
    {
        /* PRE-HEAT,   TARGET */
        { 75.0f,       88.5f },     // heater class low
        { 75.0f,       88.5f },     // heater class mid
        { 65.0f,       88.5f }      // heater class high
    },

    // User
    {
        /* PRE-HEAT,   TARGET */
        { 73.0f,       98.0f },     // heater class low
        { 72.0f,       98.0f },     // heater class mid
        { 65.0f,       97.0f }      // heater class high
    }
};

static TargetTemp_T HotOutTempList_High[ SEL_HOT_NUM ][ HEATER_CLASS_NUM ] = 
{
    // MILK ( 45'C)
    {
        /* PRE-HEAT,   TARGET */
        { 35.0f,       45.0f },     // heater class low     
        { 35.0f,       45.0f },     // heater class mid
        { 35.0f,       45.0f }      // heater class high
    },

    // TEA ( 70'C)
    {
        /* PRE-HEAT,   TARGET */
        { 65.0f,       73.0f },     // heater class low
        { 55.0f,       73.0f },     // heater class mid
        { 55.0f,       73.0f }      // heater class high
    },


    // COFFEE ( 85'C)
    {
        /* PRE-HEAT,   TARGET */
        { 75.0f,       88.5f },     // heater class low
        { 75.0f,       88.5f },     // heater class mid
        { 65.0f,       88.5f }      // heater class high
    },

    // User
    {
        /* PRE-HEAT,   TARGET */
        { 75.0f,       98.0f },     // heater class low
        { 75.0f,       98.0f },     // heater class mid
        { 65.0f,       97.0f }      // heater class high
    }
};


// 희망 온도에 따른 히터의 목표 프리히팅 온도를 반환
TEMP_T GetHotTargetPreHeatTemp( U8 mu8Sel, U8 mu8Class, U8 mu8TempRegion )
{
    if( mu8TempRegion == REGION_TEMP_LOW )
    {
        return HotOutTempList_Low[ mu8Sel ][ mu8Class ].tPreHeat;
    }
    else if( mu8TempRegion == REGION_TEMP_MID )
    {
        return HotOutTempList_Mid[ mu8Sel ][ mu8Class ].tPreHeat;
    }

    return HotOutTempList_High[ mu8Sel ][ mu8Class ].tPreHeat;
}


// 희망 온도에 따른 히터의 목표 프리히팅 온도를 반환
TEMP_T GetHotTargetOutTemp( U8 mu8Sel, U8 mu8Class, U8 mu8TempRegion )
{
    if( mu8TempRegion == REGION_TEMP_LOW )
    {
        return HotOutTempList_Low[ mu8Sel ][ mu8Class ].tOut;
    }
    else if( mu8TempRegion == REGION_TEMP_MID )
    {
        return HotOutTempList_Mid[ mu8Sel ][ mu8Class ].tOut;
    }

    return HotOutTempList_High[ mu8Sel ][ mu8Class ].tOut;
}




