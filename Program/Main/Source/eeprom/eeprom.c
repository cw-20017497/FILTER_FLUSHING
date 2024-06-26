#include "eeprom.h"
#include "util.h"
#include "hal_eeprom.h"
#include "rtc.h"
#include "time_short.h" 
#include "process_display.h" 
#include "crc16.h" 
#include "sound.h"

#include "cold_water.h"
#include "water_out.h"
#include "flush_water_out.h"
#include "reverse_flush_water_out.h"
#include "ice.h"
#include "power_saving.h"
#include "error.h"
#include "front.h"
#include "service.h"
#include "user.h"
#include "uv.h"
#include "energy.h"
#include "filter.h"
#include "voice.h"
#include "hot_water.h"

#include "WIFI_UserInterface_IconAIS.h"
#include "WIFI_Control.h"


#define EEP_ADDR_OFFSET   0x0040

// TOTAL 32 blocks
// 16 bytes per block
typedef enum
{
    EEP_BLOCK_NONE,
    EEP_BLOCK_0,
    EEP_BLOCK_1,
    EEP_BLOCK_2,
    EEP_BLOCK_3,
    EEP_BLOCK_4,
    EEP_BLOCK_5,
    EEP_BLOCK_6,
    EEP_BLOCK_7,

    EEP_BLOCK_8,    // 1~2    
    EEP_BLOCK_9,    // 2~4
#if 0
    EEP_BLOCK_10,   // 5~6
    EEP_BLOCK_11,   // 7~8
    EEP_BLOCK_12,   // 9~10

    EEP_BLOCK_13,   // 11 ~ 12 
    EEP_BLOCK_14,   // 13 ~ 14
    EEP_BLOCK_15,   // 15 ~ 16
    EEP_BLOCK_16,   // 17 ~ 18
    EEP_BLOCK_17,   // 19 ~ 20

    EEP_BLOCK_18,   // 21 ~ 22
    EEP_BLOCK_19,   // 23 ~ 24
    EEP_BLOCK_20,   // 25 ~ 26
    EEP_BLOCK_21,   // 27 ~ 28
    EEP_BLOCK_22,   // 29 ~ 30
#endif

    NUM_EEP_BLOCK
} EepromBlock_T;

typedef struct _eeprom_control_
{
    /* Init / Write  */
    U8 Init;
    U8 Write;
    U8 Erase;
    U8 FactoryReset;        // 공장 초기화 되면, TRUE 그렇지 않으면 FALSE
    U8 VersionMajor;
    U8 VersionMinor;

    U16 DelayTime;
    U8 ReadData[ EEP_PAGE_SIZE ];
    U8 WriteData[ EEP_PAGE_SIZE ];
} EEP_Control_T;


EEP_Control_T  Eep;


static U8 ReadDataBlocks(void);
static U8 WriteDataBlocks(void);
static void InitBlockTableAllSet(void);

void InitEeprom(void)
{
    U8 mu8Reset   = FALSE;

    Eep.Init      = FALSE;
    Eep.Erase     = FALSE;
    Eep.FactoryReset = FALSE;
    Eep.VersionMajor = VERSION_MAJOR;
    Eep.VersionMinor = VERSION_MINOR;

    if( ReadDataBlocks() == TRUE )
    {
        // 읽기 성공
        Eep.Init = TRUE; 

        if( Eep.VersionMinor != VERSION_MINOR )
        {
            if( HAL_RTC_EEPROM_AllErase() == TRUE )
            {
                Reset();
            }
        }
    }
    else
    {
        mu8Reset = TRUE;
    }

    if( mu8Reset == TRUE )
    {
        // 초기화 진행
        Eep.FactoryReset = TRUE;

        InitBlockTableAllSet();
        WriteDataBlocks();
    }

    // Write하고 Delay Time은 ReadData() 이후에 실행되어야 한다.
    // ReadData 동작 중에, Set함수들에 대해서 Eep.Write가 TRUE가되기 때문이다.
    // Eep.Write가 TRUE가되면 불필요한 eeprom write 동작이 한 번 실행되는 것을 막기 위해서이다.
    Eep.Write     = FALSE;
    Eep.DelayTime = 0;
}

typedef struct _eeprom_block_map_
{
    EepromId_T      id;
    EepromBlock_T   blockId;
} EepromMap_T;

const static EepromMap_T EepromMappingTable[] = 
{
    { EEP_ID_CONF_MAKE_COLD,            EEP_BLOCK_0 },      
    { EEP_ID_CONF_MAKE_ICE,             EEP_BLOCK_0 },      
    { EEP_ID_CONF_TURBO,                EEP_BLOCK_0 },      
    { EEP_ID_CONF_POWER_SAVING,         EEP_BLOCK_0 },      
    { EEP_ID_CONF_HEALTH,               EEP_BLOCK_0 },      
    { EEP_ID_CONF_DECO_LED,             EEP_BLOCK_0 },      
    { EEP_ID_CONF_FIXED_AMOUNT,         EEP_BLOCK_0 },      
    { EEP_ID_CONF_MUTE,                 EEP_BLOCK_0 },      
    { EEP_ID_CONF_UNUSED_SAVE,          EEP_BLOCK_0 },      
    { EEP_ID_SEL_WATER,                 EEP_BLOCK_0 },      
    { EEP_ID_LOCK_ALL,                  EEP_BLOCK_0 },      
    { EEP_ID_LOCK_ICE,                  EEP_BLOCK_0 },      
    { EEP_ID_SERVICE_CHECK_DAY,         EEP_BLOCK_0 },      
    { EEP_ID_ICE_SIZE,                  EEP_BLOCK_0 },      

    { EEP_ID_LOCK_HOT,                  EEP_BLOCK_1 },      
    { EEP_ID_CONF_UV_MODE,              EEP_BLOCK_1 },      
    { EEP_ID_FLUSH_SETUP_INIT,          EEP_BLOCK_1 },      
    { EEP_ID_FLUSH_STATUS,              EEP_BLOCK_1 },      
    //{ EEP_ID_FLUSH_PROGRESS,            EEP_BLOCK_1 },      
    { EEP_ID_CONF_USER_AMOUNT,          EEP_BLOCK_1 },      
    { EEP_ID_CONF_USER,                 EEP_BLOCK_1 },      
    { EEP_ID_CONF_USER_HOT,             EEP_BLOCK_1 },      
    { EEP_ID_TIME,                      EEP_BLOCK_1 },      

    { EEP_ID_MEMENTO_1,                 EEP_BLOCK_2 },      
    { EEP_ID_MEMENTO_2,                 EEP_BLOCK_2 },      
    { EEP_ID_MEMENTO_3,                 EEP_BLOCK_3 },      
    { EEP_ID_MEMENTO_4,                 EEP_BLOCK_3 },      
    { EEP_ID_MEMENTO_5,                 EEP_BLOCK_4 },      


    //{ EEP_ID_ENERGY,                    EEP_BLOCK_7 },      

    { EEP_ID_WIFI_POWER,                EEP_BLOCK_8 },
    { EEP_ID_WIFI_FIRST_PARING,         EEP_BLOCK_8 },
    { EEP_ID_WIFI_FOTA_MODULE,          EEP_BLOCK_8 },
    { EEP_ID_WIFI_FOTA_MCU,             EEP_BLOCK_8 },

    { EEP_ID_FACTORY_SETUP,             EEP_BLOCK_9 },      
    { EEP_ID_FILTER_CHANGED,            EEP_BLOCK_9 },      
    { EEP_ID_FILTER_USAGE_TIME,         EEP_BLOCK_9 },      
    { EEP_ID_VOICE_LEVEL,               EEP_BLOCK_9 },      
    { EEP_ID_VOICE_VOLUME,              EEP_BLOCK_9 },      

    { EEP_ID_CUSTOMER,                  EEP_BLOCK_9 },
    { EEP_ID_POWER_SAVING_TIME,         EEP_BLOCK_9 },

    { EEP_ID_REVERSE_1,                 EEP_BLOCK_5 },
    { EEP_ID_REVERSE_2,                 EEP_BLOCK_6 },
    { EEP_ID_REVERSE_REPEAT,            EEP_BLOCK_7 },

#if 0
    //{ EEP_ID_USER_A,                    EEP_BLOCK_5 },      
    //{ EEP_ID_USER_B,                    EEP_BLOCK_5 },      
    //{ EEP_ID_USER_C,                    EEP_BLOCK_6 },      
    //{ EEP_ID_USER_NONE,                 EEP_BLOCK_6 },      

    { EEP_ID_SODA_USAGE_0,              EEP_BLOCK_8  },
    { EEP_ID_SODA_USAGE_1,              EEP_BLOCK_8  },

    { EEP_ID_SODA_USAGE_2,              EEP_BLOCK_9  },
    { EEP_ID_SODA_USAGE_3,              EEP_BLOCK_9  },

    { EEP_ID_SODA_USAGE_4,              EEP_BLOCK_10 },
    { EEP_ID_SODA_USAGE_5,              EEP_BLOCK_10 },

    { EEP_ID_SODA_USAGE_6,              EEP_BLOCK_11 },
    { EEP_ID_SODA_USAGE_7,              EEP_BLOCK_11 },

    { EEP_ID_SODA_USAGE_8,              EEP_BLOCK_12 },
    { EEP_ID_SODA_USAGE_9,              EEP_BLOCK_12 },

    { EEP_ID_SODA_USAGE_10,             EEP_BLOCK_13 }, 
    { EEP_ID_SODA_USAGE_11,             EEP_BLOCK_13 }, 

    { EEP_ID_SODA_USAGE_12,             EEP_BLOCK_14 }, 
    { EEP_ID_SODA_USAGE_13,             EEP_BLOCK_14 }, 

    { EEP_ID_SODA_USAGE_14,             EEP_BLOCK_15 }, 
    { EEP_ID_SODA_USAGE_15,             EEP_BLOCK_15 }, 

    { EEP_ID_SODA_USAGE_16,             EEP_BLOCK_16 }, 
    { EEP_ID_SODA_USAGE_17,             EEP_BLOCK_16 }, 

    { EEP_ID_SODA_USAGE_18,             EEP_BLOCK_17 }, 
    { EEP_ID_SODA_USAGE_19,             EEP_BLOCK_17 }, 

    { EEP_ID_SODA_USAGE_20,             EEP_BLOCK_18 }, 
    { EEP_ID_SODA_USAGE_21,             EEP_BLOCK_18 }, 

    { EEP_ID_SODA_USAGE_22,             EEP_BLOCK_19 }, 
    { EEP_ID_SODA_USAGE_23,             EEP_BLOCK_19 }, 

    { EEP_ID_SODA_USAGE_24,             EEP_BLOCK_20 }, 
    { EEP_ID_SODA_USAGE_25,             EEP_BLOCK_20 }, 

    { EEP_ID_SODA_USAGE_26,             EEP_BLOCK_21 }, 
    { EEP_ID_SODA_USAGE_27,             EEP_BLOCK_21 }, 

    { EEP_ID_SODA_USAGE_28,             EEP_BLOCK_22 }, 
    { EEP_ID_SODA_USAGE_29,             EEP_BLOCK_22 }, 
#endif

    //{ EEP_ID_CONF_MAKE_HOT,             EEP_BLOCK_0 },    
    //{ EEP_ID_CONF_CONTY,                EEP_BLOCK_0 },      
    //{ EEP_ID_HOT_ALTITUDE,              EEP_BLOCK_0 },    
    //{ EEP_ID_DO_ICING,                  EEP_BLOCK_0 },      
    //{ EEP_ID_LOCK_HOT,                  EEP_BLOCK_0 },    
    //{ EEP_ID_MAKING_ICE,                EEP_BLOCK_1 },    

    //{ EEP_ID_STER_DAY_PASS,             EEP_BLOCK_2 },    
    //{ EEP_ID_STER_ADJUST_MODE,          EEP_BLOCK_2 },    
    //{ EEP_ID_STER_PERIOD,               EEP_BLOCK_2 },    
    //{ EEP_ID_STER_PERIOD_AUTO_CHANGE,   EEP_BLOCK_2 },    
    //{ EEP_ID_STER_RESERVATION_HOUR,     EEP_BLOCK_2 },    
    //{ EEP_ID_STER_RESERVATION_MIN,      EEP_BLOCK_2 },    
    //{ EEP_ID_STER_ERR_WATER_OUT,        EEP_BLOCK_2 },    
    //{ EEP_ID_STER_DOING,                EEP_BLOCK_2 },    
    //{ EEP_ID_NFC_ERROR_HIS_COUNT,       EEP_BLOCK_2 },    
    //{ EEP_ID_NFC_USER_HIS_COUNT,        EEP_BLOCK_2 },    
    //{ EEP_ID_NFC_STER_HIS_COUNT,        EEP_BLOCK_2 },    
    //{ EEP_ID_NFC_WATER_HIS_COUNT,       EEP_BLOCK_2 },    
};
#define SZ_EEPROM_MAPPING_TABLE     (sizeof(EepromMappingTable)/sizeof(EepromMap_T))

// eeprom id에 해당하는 block id를 반환
static EepromBlock_T GetBlockId(EepromId_T mu8Id )
{
    U8 i;

    for( i = 0; i < SZ_EEPROM_MAPPING_TABLE; i++ )
    {
        if( mu8Id == EepromMappingTable[ i ].id )
        {
            return EepromMappingTable[ i ].blockId;
        }
    }

    return EEP_BLOCK_NONE;  // 해당 ID 없음.
}


typedef U8 (*Action_T)(EepromBlock_T);
typedef struct _eeprom_block_table_
{
    EepromBlock_T   blockId;
    U8 write;                   // TRUE이면 Write 동작 수행
    Action_T    pWriteAction;   
    Action_T    pReadAction;   
}EepBlockTable_T;

static U8 WriteBlock0(EepromBlock_T mBlockId);
static U8 ReadBlock0(EepromBlock_T mBlockId);
static U8 WriteBlock1(EepromBlock_T mBlockId);
static U8 ReadBlock1(EepromBlock_T mBlockId);
static U8 WriteBlock2(EepromBlock_T mBlockId);
static U8 ReadBlock2(EepromBlock_T mBlockId);
static U8 WriteBlock3(EepromBlock_T mBlockId);
static U8 ReadBlock3(EepromBlock_T mBlockId);
static U8 WriteBlock4(EepromBlock_T mBlockId);
static U8 ReadBlock4(EepromBlock_T mBlockId);

static U8 WriteBlock5(EepromBlock_T mBlockId);
static U8 ReadBlock5(EepromBlock_T mBlockId);
static U8 WriteBlock6(EepromBlock_T mBlockId);
static U8 ReadBlock6(EepromBlock_T mBlockId);
static U8 WriteBlock7(EepromBlock_T mBlockId);
static U8 ReadBlock7(EepromBlock_T mBlockId);

static U8 WriteBlock8(EepromBlock_T mBlockId);
static U8 ReadBlock8(EepromBlock_T mBlockId);

static U8 WriteBlock9(EepromBlock_T mBlockId);
static U8 ReadBlock9(EepromBlock_T mBlockId);

//static U8 WriteSodaUsage( EepromBlock_T mBlockId );
//static U8 ReadSodaUsage( EepromBlock_T mBlockId );

// 블럭 ID에 해당하는 함수 호출
static EepBlockTable_T    EepromBlockTableList[] = 
{
    { EEP_BLOCK_0,      FALSE,   WriteBlock0,   ReadBlock0 },
    { EEP_BLOCK_1,      FALSE,   WriteBlock1,   ReadBlock1 },
    { EEP_BLOCK_2,      FALSE,   WriteBlock2,   ReadBlock2 },
    { EEP_BLOCK_3,      FALSE,   WriteBlock3,   ReadBlock3 },
    { EEP_BLOCK_4,      FALSE,   WriteBlock4,   ReadBlock4 },
    { EEP_BLOCK_5,      FALSE,   WriteBlock5,   ReadBlock5 },
    { EEP_BLOCK_6,      FALSE,   WriteBlock6,   ReadBlock6 },
    { EEP_BLOCK_7,      FALSE,   WriteBlock7,   ReadBlock7 },
    { EEP_BLOCK_8,      FALSE,   WriteBlock8,   ReadBlock8 },
    { EEP_BLOCK_9,      FALSE,   WriteBlock9,   ReadBlock9 },
};
#define SZ_BLOCK_TABLE     (sizeof(EepromBlockTableList)/sizeof(EepBlockTable_T))

static void InitBlockTableAllSet(void)
{
    U8 i;

    for( i = 0; i < SZ_BLOCK_TABLE; i++ )
    {
        EepromBlockTableList[ i ].write = TRUE;
    }
}

static void SetBlockWrite( EepromBlock_T muBlockId )
{
    U8 i;

    for( i = 0; i < SZ_BLOCK_TABLE; i++ )
    {
        if( muBlockId == EepromBlockTableList[ i ].blockId )
        {
            EepromBlockTableList[ i ].write = TRUE;
        }
    }
}


void SaveEepromId( EepromId_T mu8Id )
{
    EepromBlock_T   muBlockId;

    if( mu8Id > EEP_ID_NUM )
    {
        return ;
    }

    Eep.DelayTime = 1;  // 1sec..
    Eep.Write = TRUE;  

    muBlockId = GetBlockId( mu8Id );
    if( muBlockId != EEP_BLOCK_NONE )
    {
        SetBlockWrite( muBlockId );
    }
}

void EraseEeprom(void)
{
    Eep.Write = TRUE;
    Eep.Erase = TRUE;

    WifiControlProcess(WIFI_TIME_FACTORY);
}


static U8 EepromPageWrite( U16 mu16Addr, U8 *pData )
{
    if( IsRtcBatBackUpMode() == TRUE )
    {
        return FALSE;
    }

    mu16Addr += EEP_ADDR_OFFSET;
    return HAL_RTC_PageWrite( DEV_ADDR_EEP, mu16Addr, pData, EEP_PAGE_SIZE);
}

static U8 EepromSeqRead( U16 mu16Addr, U8 *pData, U8 mu16Length )
{
    if( IsRtcBatBackUpMode() == TRUE )
    {
        return FALSE;
    }

    mu16Addr += EEP_ADDR_OFFSET;
    return HAL_RTC_SeqRead( DEV_ADDR_EEP, mu16Addr, pData, mu16Length );
}


static U8 WriteBlock0(EepromBlock_T mBlockId)
{
    U16 mu16CheckSum = 0U;
    U8 mu8SelWater = SEL_WATER_ROOM;
    U8 mu8PrevSelWater = SEL_WATER_ROOM;


    // 탄산수 선택은 EEPROM에 저장하지 않는다.
    // 이 경우 이전 선택된 것을 저장한다.
    // 이전 선택된 물이 소다인 경우에는 정수를 기본으로 한다.
    mu8SelWater = GetWaterOutSelect();
    if( mu8SelWater == SEL_WATER_HOT 
            || mu8SelWater == SEL_WATER_FLUSH )
    {
        mu8SelWater = SEL_WATER_ROOM;
    }
    else if( mu8SelWater == SEL_WATER_COLD && GetColdWaterConfigMake() == FALSE )
    {
        mu8SelWater = SEL_WATER_ROOM;
    }

    Eep.WriteData[ 0 ]  = GetColdWaterConfigMake();    
    Eep.WriteData[ 1 ]  = 0;
    Eep.WriteData[ 2 ]  = GetIceConfigMake();
    Eep.WriteData[ 3 ]  = GetIceTurbo();
    Eep.WriteData[ 4 ]  = GetSavingConfig();
    Eep.WriteData[ 5 ]  = GetConfigDeco( DECO_LED_WELCOME );
    Eep.WriteData[ 6 ]  = GetConfigDeco( DECO_LED_BUTTON );
    Eep.WriteData[ 7 ]  = GetIceSize();
    Eep.WriteData[ 8 ]  = GetWaterOutConfFixedAmountType(); 
    Eep.WriteData[ 9 ]  = GetSoundMute();       
    Eep.WriteData[ 10 ] = GetLockIce();
    Eep.WriteData[ 11 ] = GetServiceCheckDay();
    Eep.WriteData[ 12 ] = mu8SelWater;
    Eep.WriteData[ 13 ] = GetLockAll();
    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = GET_LOW_BYTE( mu16CheckSum );


    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}

static U8 ReadBlockId(U8 mu8BlockId, U8 *pBuf)
{
    U8 mu8Ret;
    U16 mu16SrcChksum = 0U;
    U16 mu16CalcChksum = 0U;


    // Read 
    mu8Ret = EepromSeqRead( ((U16)mu8BlockId * EEP_PAGE_SIZE ), pBuf, EEP_PAGE_SIZE );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Checksum
    mu16SrcChksum  = GET_UINT_WORD( pBuf[EEP_PAGE_SIZE - 1], pBuf[EEP_PAGE_SIZE - 2] ); 
    mu16CalcChksum = crc16_cal( pBuf, (EEP_PAGE_SIZE - 2) );
    if( mu16SrcChksum != mu16CalcChksum )
    {
        return FALSE;
    }

    return TRUE;
}

static U8 ReadBlock0(EepromBlock_T mBlockId)
{
    U8 mu8Ret;

    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Load Data
    SetColdWaterConfigMake( Eep.ReadData[0] );
    // Eep.ReadData[1];
    SetIceConfigMake( Eep.ReadData[2] );
    SetIceTurbo( Eep.ReadData[3] );
    SetSavingConfig( Eep.ReadData[4] );

    SetConfigDeco( DECO_LED_WELCOME, Eep.ReadData[5] );
    SetConfigDeco( DECO_LED_BUTTON, Eep.ReadData[6] );
    SetIceSize( Eep.ReadData[7] );
    SetWaterOutConfFixedAmountType( Eep.ReadData[8] );
    SetWaterOutFixedAmountType( Eep.ReadData[8] );
    if( Eep.ReadData[9] != 0 )
    {
        SetSoundMute();
    }
    SetLockIce( Eep.ReadData[10] );
    SetServiceCheckDay( Eep.ReadData[11] );
    SetWaterOutSelect( Eep.ReadData[ 12 ] );
    SetLockAll( Eep.ReadData[13] );

    return TRUE;
}

static U8 WriteBlock1(EepromBlock_T mBlockId)
{
    U16 mu16CheckSum = 0U;
    U32 mu32Val;


    Eep.WriteData[ 0 ] = GetFlushSetupInit(); 
    Eep.WriteData[ 1 ] = GetFlushStatus();

    Eep.WriteData[ 2 ] = 0;
    Eep.WriteData[ 3 ] = 0;
    Eep.WriteData[ 4 ] = Eep.VersionMajor;
    Eep.WriteData[ 5 ] = Eep.VersionMinor;

    Eep.WriteData[ 6 ] = GetWaterOutConfigUser();

    mu32Val = GetWaterOutUserAmount();
    Eep.WriteData[ 7 ] = GET_32_BYTE_32( mu32Val );
    Eep.WriteData[ 8 ] = GET_32_BYTE_24( mu32Val );
    Eep.WriteData[ 9 ] = GET_32_BYTE_16( mu32Val );
    Eep.WriteData[ 10 ] = GET_32_BYTE_8( mu32Val );

    Eep.WriteData[ 11 ]  = GetLockHot();
    Eep.WriteData[ 12 ]  = GetHotConfigUser();
    Eep.WriteData[ 13 ]  = GetRtcConfigInit();
    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = GET_LOW_BYTE( mu16CheckSum );


    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}

static U8 ReadBlock1(EepromBlock_T mBlockId)
{
    U8 mu8Ret;
    U32 mu32Val;


    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Load Data

    /* FLUSHING - 최초 설치 */
    SetFlushSetupInit( Eep.ReadData[0] );      
    
    /* FLUSHING - 필터 교체 */
    SetFlushStatus( Eep.ReadData[1] );      // FLUSHING STATUS

    // Eep.ReadData[2]; 
    // Eep.ReadData[3]; 

    /* VERSION MAJOR & MINOR */
     Eep.VersionMajor = Eep.ReadData[4]; 
     Eep.VersionMinor = Eep.ReadData[5];

    /* USER AMOUNT */
    SetWaterOutConfigUser( Eep.ReadData[6] );
    mu32Val = GET_UINT_WORD_32( 
            Eep.ReadData[7], 
            Eep.ReadData[8], 
            Eep.ReadData[9], 
            Eep.ReadData[10]);
    SetWaterOutUserAmount( mu32Val );       // USER AMOUNT 

    /* HOT - LOCK */
    SetLockHot( Eep.ReadData[11] );

    /* HOT - CONFIG USER TEMP */
    SetHotConfigUser( Eep.ReadData[12] );

    /* RTC - INIT */
    SetRtcConfigInit( Eep.ReadData[13] );

    return TRUE;
}

static U8 WriteBlock2(EepromBlock_T mBlockId)
{
    U16 mu16CheckSum = 0U;

    Memento_T   mMemento;


    // Memento 0
    GetMementoError( 0, &mMemento );
    Eep.WriteData[ 0 ] = mMemento.Error;
    Eep.WriteData[ 1 ] = (U8)GET_HIGH_BYTE(mMemento.Date.u16Year);
    Eep.WriteData[ 2 ] = (U8)GET_LOW_BYTE(mMemento.Date.u16Year);
    Eep.WriteData[ 3 ] = mMemento.Date.u8Month;
    Eep.WriteData[ 4 ] = mMemento.Date.u8Date;

    // Memento 1
    GetMementoError( 1, &mMemento );
    Eep.WriteData[ 5 ] = mMemento.Error;
    Eep.WriteData[ 6 ] = (U8)GET_HIGH_BYTE(mMemento.Date.u16Year);
    Eep.WriteData[ 7 ] = (U8)GET_LOW_BYTE(mMemento.Date.u16Year);
    Eep.WriteData[ 8 ] = mMemento.Date.u8Month;
    Eep.WriteData[ 9 ] = mMemento.Date.u8Date;

    Eep.WriteData[ 10 ]  = 0;
    Eep.WriteData[ 11 ]  = 0;
    Eep.WriteData[ 12 ]  = 0;
    Eep.WriteData[ 13 ]  = 0;                          
    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = GET_LOW_BYTE( mu16CheckSum );


    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}

static U8 ReadBlock2(EepromBlock_T mBlockId)
{
    U8 mu8Ret;
    Memento_T   mMemento;


    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Load Data

    // Memento 0 
    mMemento.Error = Eep.ReadData[0];
    mMemento.Date.u16Year = GET_UINT_WORD( Eep.ReadData[2], Eep.ReadData[1] );
    mMemento.Date.u8Month = Eep.ReadData[3];
    mMemento.Date.u8Date  = Eep.ReadData[4];
    SetMementoError( 0, mMemento.Error, &mMemento.Date );

    // Memento 1
    mMemento.Error = Eep.ReadData[5];
    mMemento.Date.u16Year = GET_UINT_WORD( Eep.ReadData[7], Eep.ReadData[6] );
    mMemento.Date.u8Month = Eep.ReadData[8];
    mMemento.Date.u8Date  = Eep.ReadData[9];
    SetMementoError( 1, mMemento.Error, &mMemento.Date );

    // Eep.ReadData[10];
    // Eep.ReadData[11];
    // Eep.ReadData[12];
    // Eep.ReadData[13];

    return TRUE;
}

static U8 WriteBlock3(EepromBlock_T mBlockId)
{
    U16 mu16CheckSum = 0U;

    Memento_T   mMemento;


    // Memento 2
    GetMementoError( 2, &mMemento );
    Eep.WriteData[ 0 ] = mMemento.Error;
    Eep.WriteData[ 1 ] = (U8)GET_HIGH_BYTE(mMemento.Date.u16Year);
    Eep.WriteData[ 2 ] = (U8)GET_LOW_BYTE(mMemento.Date.u16Year);
    Eep.WriteData[ 3 ] = mMemento.Date.u8Month;
    Eep.WriteData[ 4 ] = mMemento.Date.u8Date;

    // Memento 3
    GetMementoError( 3, &mMemento );
    Eep.WriteData[ 5 ] = mMemento.Error;
    Eep.WriteData[ 6 ] = (U8)GET_HIGH_BYTE(mMemento.Date.u16Year);
    Eep.WriteData[ 7 ] = (U8)GET_LOW_BYTE(mMemento.Date.u16Year);
    Eep.WriteData[ 8 ] = mMemento.Date.u8Month;
    Eep.WriteData[ 9 ] = mMemento.Date.u8Date;

    Eep.WriteData[ 10 ]  = 0;
    Eep.WriteData[ 11 ]  = 0;
    Eep.WriteData[ 12 ]  = 0;
    Eep.WriteData[ 13 ]  = 0;                          
    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = GET_LOW_BYTE( mu16CheckSum );


    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}

static U8 ReadBlock3(EepromBlock_T mBlockId)
{
    U8 mu8Ret;
    Memento_T   mMemento;


    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Load Data

    // Memento 2 
    mMemento.Error = Eep.ReadData[0];
    mMemento.Date.u16Year = GET_UINT_WORD( Eep.ReadData[2], Eep.ReadData[1] );
    mMemento.Date.u8Month = Eep.ReadData[3];
    mMemento.Date.u8Date  = Eep.ReadData[4];
    SetMementoError( 2, mMemento.Error, &mMemento.Date );

    // Memento 3
    mMemento.Error = Eep.ReadData[5];
    mMemento.Date.u16Year = GET_UINT_WORD( Eep.ReadData[7], Eep.ReadData[6] );
    mMemento.Date.u8Month = Eep.ReadData[8];
    mMemento.Date.u8Date  = Eep.ReadData[9];
    SetMementoError( 3, mMemento.Error, &mMemento.Date );

    // Eep.ReadData[10];
    // Eep.ReadData[11];
    // Eep.ReadData[12];
    // Eep.ReadData[13];

    return TRUE;
}


static U8 WriteBlock4(EepromBlock_T mBlockId)
{
    U16 mu16CheckSum = 0U;

    Memento_T   mMemento;


    // Memento 4
    GetMementoError( 4, &mMemento );
    Eep.WriteData[ 0 ] = mMemento.Error;
    Eep.WriteData[ 1 ] = (U8)GET_HIGH_BYTE(mMemento.Date.u16Year);
    Eep.WriteData[ 2 ] = (U8)GET_LOW_BYTE(mMemento.Date.u16Year);
    Eep.WriteData[ 3 ] = mMemento.Date.u8Month;
    Eep.WriteData[ 4 ] = mMemento.Date.u8Date;

    Eep.WriteData[ 5 ] = 0;
    Eep.WriteData[ 6 ] = 0;
    Eep.WriteData[ 7 ] = 0;
    Eep.WriteData[ 8 ] = 0;
    Eep.WriteData[ 9 ] = 0;

    Eep.WriteData[ 10 ]  = 0;
    Eep.WriteData[ 11 ]  = 0;
    Eep.WriteData[ 12 ]  = 0;
    Eep.WriteData[ 13 ]  = 0;                          
    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = GET_LOW_BYTE( mu16CheckSum );


    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}


static U8 ReadBlock4(EepromBlock_T mBlockId)
{
    U8 mu8Ret;
    Memento_T   mMemento;


    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Load Data

    // Memento 4 
    mMemento.Error = Eep.ReadData[0];
    mMemento.Date.u16Year = GET_UINT_WORD( Eep.ReadData[2], Eep.ReadData[1] );
    mMemento.Date.u8Month = Eep.ReadData[3];
    mMemento.Date.u8Date  = Eep.ReadData[4];
    SetMementoError( 4, mMemento.Error, &mMemento.Date );

    // Eep.ReadData[5];
    // Eep.ReadData[6];
    // Eep.ReadData[7];
    // Eep.ReadData[8];
    // Eep.ReadData[9];
    // Eep.ReadData[10];
    // Eep.ReadData[11];
    // Eep.ReadData[12];
    // Eep.ReadData[13];

    return TRUE;
}



static U8 WriteBlock5(EepromBlock_T mBlockId)
{
    U16 mu16CheckSum = 0U;


    Eep.WriteData[ 0 ] = GET_32_BYTE_8( REVERSE_RELEASE_AIR_TIME );
    Eep.WriteData[ 1 ] = GET_32_BYTE_16( REVERSE_RELEASE_AIR_TIME );
    Eep.WriteData[ 2 ] = GET_32_BYTE_24( REVERSE_RELEASE_AIR_TIME );
    Eep.WriteData[ 3 ] = GET_32_BYTE_32( REVERSE_RELEASE_AIR_TIME );

    Eep.WriteData[ 4 ] = GET_32_BYTE_8( REVERSE_IN_AIR_TIME );
    Eep.WriteData[ 5 ] = GET_32_BYTE_16( REVERSE_IN_AIR_TIME );
    Eep.WriteData[ 6 ] = GET_32_BYTE_24( REVERSE_IN_AIR_TIME );
    Eep.WriteData[ 7 ] = GET_32_BYTE_32( REVERSE_IN_AIR_TIME );

    Eep.WriteData[ 8 ] = GET_32_BYTE_8( REVERSE_PRESSURE_AIR_TIME );
    Eep.WriteData[ 9 ] = GET_32_BYTE_16( REVERSE_PRESSURE_AIR_TIME );
    Eep.WriteData[ 10 ] = GET_32_BYTE_24( REVERSE_PRESSURE_AIR_TIME );
    Eep.WriteData[ 11 ] = GET_32_BYTE_32( REVERSE_PRESSURE_AIR_TIME );

    Eep.WriteData[ 12 ] = 0;
    Eep.WriteData[ 13 ] = 0;


    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = GET_LOW_BYTE( mu16CheckSum );


    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}


static U8 ReadBlock5(EepromBlock_T mBlockId)
{
    U8 mu8Ret;
    Liter_T mLiterDaily;
    Liter_T mLiterMonthly_1;
    Liter_T mLiterMonthly_2;


    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Load Data
    REVERSE_RELEASE_AIR_TIME = GET_UINT_WORD_32( 
            Eep.ReadData[3],
            Eep.ReadData[2],
            Eep.ReadData[1],
            Eep.ReadData[0] 
            );

    REVERSE_IN_AIR_TIME = GET_UINT_WORD_32( 
            Eep.ReadData[7],
            Eep.ReadData[6],
            Eep.ReadData[5],
            Eep.ReadData[4] 
            );

    REVERSE_PRESSURE_AIR_TIME = GET_UINT_WORD_32( 
            Eep.ReadData[11],
            Eep.ReadData[10],
            Eep.ReadData[9],
            Eep.ReadData[8] 
            );

    return TRUE;
}


static U8 WriteBlock6(EepromBlock_T mBlockId)
{
    U16 mu16CheckSum = 0U;

    Eep.WriteData[ 0 ] = GET_32_BYTE_8( REVERSE_BREAK_TIME );
    Eep.WriteData[ 1 ] = GET_32_BYTE_16( REVERSE_BREAK_TIME );
    Eep.WriteData[ 2 ] = GET_32_BYTE_24( REVERSE_BREAK_TIME );
    Eep.WriteData[ 3 ] = GET_32_BYTE_32( REVERSE_BREAK_TIME );

    Eep.WriteData[ 4 ] = GET_32_BYTE_8( REVERSE_FLUSHING_TIME );
    Eep.WriteData[ 5 ] = GET_32_BYTE_16( REVERSE_FLUSHING_TIME );
    Eep.WriteData[ 6 ] = GET_32_BYTE_24( REVERSE_FLUSHING_TIME );
    Eep.WriteData[ 7 ] = GET_32_BYTE_32( REVERSE_FLUSHING_TIME );

    Eep.WriteData[ 8 ] = GET_32_BYTE_8( REVERSE_FEED_OUT_TIME );
    Eep.WriteData[ 9 ] = GET_32_BYTE_16( REVERSE_FEED_OUT_TIME );
    Eep.WriteData[ 10 ] = GET_32_BYTE_24( REVERSE_FEED_OUT_TIME );
    Eep.WriteData[ 11 ] = GET_32_BYTE_32( REVERSE_FEED_OUT_TIME );

    Eep.WriteData[ 12 ] = 0;
    Eep.WriteData[ 13 ] = 0;

    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = GET_LOW_BYTE( mu16CheckSum );


    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}


// USER A 
static U8 ReadBlock6(EepromBlock_T mBlockId)
{
    U8 mu8Ret;
    Liter_T mLiterDaily;
    Liter_T mLiterMonthly_1;
    Liter_T mLiterMonthly_2;


    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Load Data
    REVERSE_BREAK_TIME = GET_UINT_WORD_32( 
            Eep.ReadData[3],
            Eep.ReadData[2],
            Eep.ReadData[1],
            Eep.ReadData[0] 
            );

    REVERSE_FLUSHING_TIME = GET_UINT_WORD_32( 
            Eep.ReadData[7],
            Eep.ReadData[6],
            Eep.ReadData[5],
            Eep.ReadData[4] 
            );

    REVERSE_FEED_OUT_TIME = GET_UINT_WORD_32( 
            Eep.ReadData[11],
            Eep.ReadData[10],
            Eep.ReadData[9],
            Eep.ReadData[8] 
            );


    return TRUE;
}



static U8 WriteBlock7(EepromBlock_T mBlockId)
{
    U16 mu16CheckSum = 0U;

    Eep.WriteData[ 0 ] = GET_HIGH_BYTE( dbg_reverse_repeat);
    Eep.WriteData[ 1 ] = GET_LOW_BYTE( dbg_reverse_repeat);
    Eep.WriteData[ 2 ] = 0;
    Eep.WriteData[ 3 ] = 0;
    Eep.WriteData[ 4 ] = 0;

    Eep.WriteData[ 5 ] = 0;
    Eep.WriteData[ 6 ] = 0;
    Eep.WriteData[ 7 ] = 0;
    Eep.WriteData[ 8 ] = 0;
    Eep.WriteData[ 9 ] = 0;

    Eep.WriteData[ 10 ]  = 0;
    Eep.WriteData[ 11 ]  = 0;
    Eep.WriteData[ 12 ]  = 0;
    Eep.WriteData[ 13 ]  = 0;                          

    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = (U8)GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = (U8)GET_LOW_BYTE( mu16CheckSum );


    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}



static U8 ReadBlock7(EepromBlock_T mBlockId)
{
    U8 mu8Ret;
    U8 mu8Current;
    U8 mu8Previous;
    U32 mu32Val;

    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Load Data
    dbg_reverse_repeat = GET_UINT_WORD( Eep.ReadData[1], Eep.ReadData[0] );

    return TRUE;
}


extern WIFI_Status_T Wifi;
static U8 WriteBlock8(EepromBlock_T mBlockId)
{
    U16 mu16CheckSum = 0U;


    Eep.WriteData[ 0 ] = Wifi.OnOff;
    Eep.WriteData[ 1 ] = Wifi.FirstParing;
    Eep.WriteData[ 2 ] = Wifi.FotaModule;
    Eep.WriteData[ 3 ] = Wifi.FotaMain;
    Eep.WriteData[ 4 ]  = 0;
    Eep.WriteData[ 5 ]  = 0;
    Eep.WriteData[ 6 ]  = 0;
    Eep.WriteData[ 7 ]  = 0;
    Eep.WriteData[ 8 ]  = 0;
    Eep.WriteData[ 9 ]  = 0;
    Eep.WriteData[ 10 ]  = 0;
    Eep.WriteData[ 11 ]  = 0;
    Eep.WriteData[ 12 ]  = 0;
    Eep.WriteData[ 13 ]  = 0;                          
    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = GET_LOW_BYTE( mu16CheckSum );


    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}



// WIFI 
static U8 ReadBlock8(EepromBlock_T mBlockId)
{
    U8 mu8Ret;


    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Load Data
    Wifi.OnOff          = Eep.ReadData[0];
    Wifi.FirstParing    = Eep.ReadData[1];
    Wifi.FotaModule     = Eep.ReadData[2];
    Wifi.FotaMain       = Eep.ReadData[3];

    return TRUE;
}

static U8 WriteBlock9(EepromBlock_T mBlockId)
{
    U16 mu16CheckSum = 0U;
    U32 mu32Time;


    Eep.WriteData[ 0 ] = GetFilterChangedStatus();
    Eep.WriteData[ 1 ] = GetFactorySetup();

    // VOCICE
    Eep.WriteData[ 2 ] = GetVoiceLevel();
    Eep.WriteData[ 3 ] = GetVoiceVolume();

    // FILTER USAGE
    mu32Time = GetFilterUsageTime();
    Eep.WriteData[ 4 ] = GET_32_BYTE_32( mu32Time );
    Eep.WriteData[ 5 ] = GET_32_BYTE_24( mu32Time );
    Eep.WriteData[ 6 ] = GET_32_BYTE_16( mu32Time );
    Eep.WriteData[ 7 ] = GET_32_BYTE_8( mu32Time );

    // Customer Type
    Eep.WriteData[ 8 ] = GetCustomerType();
    
    // PowerSaving On/OffTIme
    Eep.WriteData[ 9 ]  = GetSavingAlarmHour( ALARM_ON );
    Eep.WriteData[ 10 ]  = GetSavingAlarmMin( ALARM_ON );
    Eep.WriteData[ 11 ]  = GetSavingAlarmHour( ALARM_OFF );
    Eep.WriteData[ 12 ]  = GetSavingAlarmMin( ALARM_OFF );

    // unused
    Eep.WriteData[ 13 ]  = 0;                          
    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = GET_LOW_BYTE( mu16CheckSum );


    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}


static U8 ReadBlock9(EepromBlock_T mBlockId)
{
    U8 mu8Ret;
    U32 mu32Val;


    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    // Load Data
    SetFilterChangedStatus( Eep.ReadData[0] );
    SetFactroySetup( Eep.ReadData[1] );

    // Voice
    SetVoiceLevel( Eep.ReadData[2] );
    SetVoiceVolume( Eep.ReadData[3] );
    
    // Filter Usage Time
    mu32Val = GET_UINT_WORD_32( 
            Eep.ReadData[4], 
            Eep.ReadData[5], 
            Eep.ReadData[6], 
            Eep.ReadData[7] );
    SetFilterUsageTime( mu32Val );

    // Customer Type
    SetCustomerType( Eep.ReadData[ 8 ] );

    // Sleep On/OffTIme
    SetSavingAlarmHour( ALARM_ON, Eep.ReadData[ 9 ] );
    SetSavingAlarmMin( ALARM_ON, Eep.ReadData[ 10 ] );

    SetSavingAlarmHour( ALARM_OFF, Eep.ReadData[ 11 ] );
    SetSavingAlarmMin( ALARM_OFF, Eep.ReadData[ 12 ] );
    return TRUE;
}


#if 0
static U8 WriteSodaUsage( EepromBlock_T mBlockId )
{
    U8 mu8Index;
    U16 mu16CheckSum = 0U;
    SodaUsage_T mDay;


    mu8Index = (mBlockId - EEP_BLOCK_8 ) * 2;
    GetSodaUsageDayData( mu8Index, &mDay );

    Eep.WriteData[ 0 ] = mDay.Year;
    Eep.WriteData[ 1 ] = mDay.Month;
    Eep.WriteData[ 2 ] = mDay.Date;
    Eep.WriteData[ 3 ] = GET_HIGH_BYTE( mDay.Usage );
    Eep.WriteData[ 4 ] = GET_LOW_BYTE( mDay.Usage );

    GetSodaUsageDayData( mu8Index + 1, &mDay );
    Eep.WriteData[ 5 ] = mDay.Year;
    Eep.WriteData[ 6 ] = mDay.Month;
    Eep.WriteData[ 7 ] = mDay.Date;
    Eep.WriteData[ 8 ] = GET_HIGH_BYTE( mDay.Usage );
    Eep.WriteData[ 9 ] = GET_LOW_BYTE( mDay.Usage );

    Eep.WriteData[ 10 ]  = 0;
    Eep.WriteData[ 11 ]  = 0;
    Eep.WriteData[ 12 ]  = 0;
    Eep.WriteData[ 13 ]  = 0;                          
    mu16CheckSum = crc16_cal( &Eep.WriteData[0], (EEP_PAGE_SIZE - 2) );

    Eep.WriteData[EEP_PAGE_SIZE - 2] = GET_HIGH_BYTE( mu16CheckSum );
    Eep.WriteData[EEP_PAGE_SIZE - 1] = GET_LOW_BYTE( mu16CheckSum );

    // Write
    return EepromPageWrite( ((U16)mBlockId * EEP_PAGE_SIZE) , &Eep.WriteData[0] );
}
#endif

#if 0
static U8 ReadSodaUsage( EepromBlock_T mBlockId )
{
    U8 mu8Ret;
    U8 mu8Index;
    Liter_T mLiterDaily;


    mu8Ret = ReadBlockId( mBlockId, &Eep.ReadData[0] );
    if( mu8Ret == FALSE )
    {
        return FALSE;
    }

    mu8Index = (mBlockId - EEP_BLOCK_8) * 2;

    // Load Data
    mLiterDaily     = GET_UINT_WORD( Eep.ReadData[4], Eep.ReadData[3] );
    SetSodaUsageWater( mu8Index, Eep.ReadData[0], Eep.ReadData[1], Eep.ReadData[2], mLiterDaily );

    mLiterDaily     = GET_UINT_WORD( Eep.ReadData[9], Eep.ReadData[8] );
    SetSodaUsageWater( mu8Index + 1, Eep.ReadData[5], Eep.ReadData[6], Eep.ReadData[7], mLiterDaily );

    // Eep.ReadData[10]
    // Eep.ReadData[11]
    // Eep.ReadData[12]
    // Eep.ReadData[13]

    return TRUE;
}
#endif


static U8 WriteDataBlocks(void)
{
    U8 i = 0;
    volatile U8 mu8Ret = TRUE;
    EepromBlock_T mBlockId;

    for( i = 0; i < SZ_BLOCK_TABLE ; i++ )
    {
        if( EepromBlockTableList[ i ].write == TRUE )
        {
            if( EepromBlockTableList[ i ].pWriteAction != NULL ) 
            {
                mBlockId = EepromBlockTableList[ i ].blockId;
                mu8Ret = EepromBlockTableList[ i ].pWriteAction( mBlockId );
                if( mu8Ret == TRUE )
                {
                    EepromBlockTableList[ i ].write = FALSE;
                }
            }
            else
            {
                EepromBlockTableList[ i ].write = FALSE;
            }
        }
    }

    return TRUE;
}

static U8 ReadDataBlocks(void)
{
    U8 i = 0;
    volatile U8 mu8Ret = TRUE;
    EepromBlock_T mBlockId;


    for( i = 0; i < SZ_BLOCK_TABLE ; i++ )
    {
        if( EepromBlockTableList[ i ].pReadAction != NULL ) 
        {
            mBlockId = EepromBlockTableList[ i ].blockId;
            mu8Ret = EepromBlockTableList[ i ].pReadAction( mBlockId );
            if( mu8Ret == FALSE )
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}


void ProcessEeprom(void)
{
#if 0
    // TimeShort 모드에서는 eeprom 기능 비활성화
    // 단, 메멘토 모드인 경우에는 허용한다.
    if( GetTimeShortStatus() == TRUE 
            && GetMementoDisp() == FALSE )
    {
        Eep.Write = FALSE;
        return ;
    }

    // 추출 중에는 중지
    // 플러싱 중에서는 허용
    if( GetWaterOut() == TRUE )
    {
        if( GetWaterOutSelect() != SEL_WATER_FLUSH )
        {
            return ;
        }
    }
#endif

    // Write 명령 조건 검사
    if( Eep.Write == FALSE )
    {
        return ;
    }

    // Write 시간 지연 확인
    if( Eep.DelayTime != 0 )
    {
        Eep.DelayTime--;
        return ;
    }

    // Write Data
    if( IsRtcBatBackUpMode() == FALSE )
    {
        if( Eep.Erase == TRUE )
        {
            // eeprom 초기화는 초기화 후, 시스템 RESET
            if( HAL_RTC_EEPROM_AllErase() == TRUE )
            {
                Eep.Write = FALSE;
                Eep.Erase = FALSE;
                Reset();
            }
        }
        else
        {
            WriteDataBlocks();
            Eep.Write = FALSE;
        }
    }

}

U8 TestEeprom(void)
{
    volatile U8 mu8Ret = 0;
    volatile U8 mu8Read = 0;
    U16 mu16Addr;

    // 0x00 번지에 0x12값을 라이팅 하고 다시 읽어봄으로써 
    // eeprom 정상동작이 되는지 확인함...
    if( IsRtcBatBackUpMode() == FALSE )
    {
        mu16Addr = (EEP_BLOCK_0 * EEP_PAGE_SIZE) + EEP_ADDR_OFFSET;
        HAL_RTC_ByteWrite( DEV_ADDR_EEP, mu16Addr, 0x12 );
        HAL_RTC_ByteRead( DEV_ADDR_EEP, mu16Addr, &mu8Read );
    }

    if( mu8Read == 0x12 )
    {
        return TRUE;
    }

    return FALSE;
}

