
/*******************************************************************************
&Description :
    WIFI Data Check Operation

&Histiry : ShinHM
    2021.05.17 update
    2021.12.22 update
*******************************************************************************/


/***** Header *****************************************************************/
#include "WIFI_Common.h"
#include "WIFI_DataCheck.h"
#include "WIFI_Initialize.h"
#include "WIFI_RxProcess.h"
#include "WIFI_TxMake.h"
#include "WIFI_Fota.h"
#include "WIFI_Status.h"
#include "WIFI_UartInterrupt.h"
#include "WIFI_UserInterface.h"


/***** Define *****************************************************************/
typedef U8 (*WifiCheckFun_T)(void);
typedef void (*WifiDoFun_T)(void);

typedef enum
{
    CHK_MODULE_ONOFF = 0x00,    // HW Module
    CHK_ALWAYS_100MS,           // Always

    CHK_BOOT_MODULE,            // Check Booting
    CHK_BOOT_FOTA,              // Check Booting, After Fota

    CHK_APMODE_START,           // Wifi Apmode
    CHK_AP_BLE_START,           // Ble Apmode
    CHK_APMODE_FAIL,            // Check Fail

    CHK_SERVER_CNT,             // Check Server Connect
    CHK_SERVER_DCNT,            // Check Server Disconnect

    CHK_FIRST_DATA,             // First Data matching, After Connect
    CHK_FIRST_FAIL,             // Check Fail 

    CHK_SEND_DATA,              // Check Send Data
    CHK_SEND_ACK,               // Check ACK
    CHK_SEND_1MIN,              // Period 1m 
    CHK_RQST_20MIN,             // Period 20m
    CHK_RQST_24HOUR,            // Period 24hr

    CHK_FOTA_START,             // Check Fota

    CHK_NUM
} E_WIFI_CHECK_FUN_T;


/***** Variable ***************************************************************/
typedef struct _wificheck_
{
    U16  u16Count;
} WifiCheck_T;
WifiCheck_T WifiCheck[CHK_NUM];


/***** Constant ***************************************************************/


/***** Static Function ********************************************************/
static U8 CheckModuleOnOff ( void );
static void DoModuleOnOff ( void );
static U8 CheckAlways ( void );
static void DoAlways100ms ( void );

static U8 CheckBootModule ( void );
static U8 CheckBootFota ( void );

static U8 CheckApModeStart ( void );
static U8 CheckApBleStart ( void );
static U8 CheckApModeFail ( void );

static U8 CheckServerConnect ( void );
static U8 CheckServerDisconnect ( void );

static U8 CheckFirstData ( void );
static U8 CheckFirstFail ( void );

static U8 CheckSendData ( void );
static U8 CheckSendAck ( void );
static U8 CheckConnect ( void );

static U8 CheckFotaStart ( void );

static void DoReset ( void );
static void DoOff ( void );

static void DoApModeStart ( void );
static void DoApBleStart ( void );
static void DoApModeFail ( void );

static void DoFirstData ( void );
static void DoFirstFail ( void );

static void DoSendData ( void );
static void DoSendOk ( void );
static void DoSend1min ( void );
static void DoRqst20min ( void );
static void DoRqst24Hour ( void );

static void DoFotaStart ( void );


/***** Table ******************************************************************/
typedef struct _wifi_datacheck_list_
{
    E_WIFI_CHECK_FUN_T	Id;
    U8  u8TypeTime;
    U16 u16Count;
    WifiCheckFun_T CheckFunc;
    WifiDoFun_T DoFunc;
} WifiDataCheckList_T;

static void RunChkData ( WifiDataCheckList_T *pList, U8 i );

// Check Count Table
static const WifiDataCheckList_T WifiDataCheckList[] =
{   //  ID                  DefineTime          Count   CheckFunction           DoFunction
    {   CHK_MODULE_ONOFF,   WIFI_TIME_100MS,    5,      CheckModuleOnOff,       DoModuleOnOff   },
    {   CHK_ALWAYS_100MS,   WIFI_TIME_100MS,    1,      CheckAlways,            DoAlways100ms   },

    {   CHK_BOOT_MODULE,    WIFI_TIME_1SEC,     60,     CheckBootModule,        DoReset         },
    {   CHK_BOOT_FOTA,      WIFI_TIME_1SEC,     90,     CheckBootFota,          DoReset         },

    {   CHK_APMODE_START,   WIFI_TIME_1SEC,     2,      CheckApModeStart,       DoApModeStart   },
    {   CHK_AP_BLE_START,   WIFI_TIME_1SEC,     2,      CheckApBleStart,        DoApBleStart    },
    {   CHK_APMODE_FAIL,    WIFI_TIME_1SEC,     1800,   CheckApModeFail,        DoApModeFail    },

    {   CHK_SERVER_CNT,     WIFI_TIME_1SEC,     180,    CheckServerConnect,     DoReset         },
    {   CHK_SERVER_DCNT,    WIFI_TIME_100MS,    30,     CheckServerDisconnect,  DoOff           },

    {   CHK_FIRST_DATA,     WIFI_TIME_100MS,    10,     CheckFirstData,         DoFirstData     },
    {   CHK_FIRST_FAIL,     WIFI_TIME_1SEC,     60,     CheckFirstFail,         DoFirstFail     },

    {   CHK_SEND_DATA,      WIFI_TIME_100MS,    1,      CheckSendData,          DoSendData      },
    {   CHK_SEND_ACK,       WIFI_TIME_1SEC,     180,    CheckSendAck,           DoReset         },
    {   CHK_SEND_1MIN,      WIFI_TIME_1SEC,     60,     CheckConnect,           DoSend1min      },
    {   CHK_RQST_20MIN,     WIFI_TIME_1SEC,     1201,   CheckConnect,           DoRqst20min     },
    {   CHK_RQST_24HOUR,    WIFI_TIME_1MIN,     1440,   CheckConnect,           DoRqst24Hour    },

    {   CHK_FOTA_START,     WIFI_TIME_100MS,    10,     CheckFotaStart,         DoFotaStart     }
};

#define SZ_CK_LIST  ( sizeof(WifiDataCheckList) / sizeof(WifiDataCheckList_T) )


/***** Global Function ********************************************************/
void WifiDataCheckProcess ( U8 mu8Type )
{
    WifiDataCheckList_T *pList = NULL;
    U8 i = 0U;

    pList = WifiDataCheckList;

    for ( i = 0 ; i < SZ_CK_LIST; i++ )
    {
        if ( mu8Type == (pList+i)->u8TypeTime )
        {
            RunChkData( pList+i, i );
        }
    }
}

/***** Local Function *********************************************************/
static void RunChkData ( WifiDataCheckList_T *pList, U8 i )
{
    WifiCheckFun_T pChFun;
    WifiDoFun_T pDoFun;

    pChFun = pList->CheckFunc;
    pDoFun = pList->DoFunc;
    if ( pChFun() == TRUE )
    {
        WifiCheck[i].u16Count++;
        if ( WifiCheck[i].u16Count >= pList->u16Count )
        {
            pDoFun();
            WifiCheck[i].u16Count = 0;
        }
    }
    else
    {
        WifiCheck[i].u16Count = 0;
    }
}

static U8 CheckModuleOnOff ( void )
{
    static U16 mu16PowerCount = 0U;

    mu16PowerCount++;
    if ( mu16PowerCount < W_COUNT_WIFI_POWER )
    {
        if ( GetUserInterface( USER_FACTORY_STATUS ) == SET )
        {
            mu16PowerCount = W_COUNT_WIFI_POWER;
        }
        return FALSE;
    }
    else
    {
        mu16PowerCount = W_COUNT_WIFI_POWER;
    }

    if ( GetWifiModuleStatus( STATUS_MODULE_POWER ) == OFF )
    {
        SetWifiModuleStatus( STATUS_MODULE_SEQ, MODULE_NULL );
        return FALSE;
    }
    return TRUE;
}
static void DoModuleOnOff ( void )
{
    WifiModuleOnOffSeq();
}

static U8 CheckAlways ( void )
{
    if ( GetWifiControlStatus( STATUS_CON_POWER ) == ON )
    {
        return TRUE;
    }
    return FALSE;
}
static void DoAlways100ms ( void )
{
    WifiUartTxTimer();
    WifiUartRxTimer();
    WifiRxAckTimer();
}

static U8 CheckBootModule ( void )
{
    if ( ( GetWifiControlStatus( STATUS_CON_POWER ) == ON )
        && ( GetWifiControlStatus( STATUS_CON_READY ) == OFF )
        && ( GetWifiApStatus( STATUS_AP_WIFI ) == CLEAR )
        && ( GetWifiApStatus( STATUS_AP_BLE ) == CLEAR )
        && ( GetWifiFotaStatus( STATUS_FOTA_MODULE ) == CLEAR ) )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 CheckBootFota ( void )
{
    if ( ( GetWifiControlStatus( STATUS_CON_POWER ) == ON )
        && ( GetWifiControlStatus( STATUS_CON_READY ) == OFF )
        && ( GetWifiApStatus( STATUS_AP_WIFI ) == CLEAR )
        && ( GetWifiApStatus( STATUS_AP_BLE ) == CLEAR )
        && ( GetWifiFotaStatus( STATUS_FOTA_MODULE ) == SET ) )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 CheckApModeStart ( void )
{
    if ( ( GetWifiControlStatus( STATUS_CON_READY ) == ON )
        && ( GetWifiApStatus( STATUS_AP_WIFI ) != CLEAR ) )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 CheckApBleStart ( void )
{
    if ( ( GetWifiControlStatus( STATUS_CON_READY ) == ON )
        && ( GetWifiApStatus( STATUS_AP_BLE ) != CLEAR ) )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 CheckApModeFail ( void )
{
    if ( ( GetWifiApStatus( STATUS_AP_WIFI ) != CLEAR )
        || ( GetWifiApStatus( STATUS_AP_BLE ) != CLEAR ) )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 CheckServerConnect ( void )
{
    if ( ( GetWifiControlStatus( STATUS_CON_READY ) == ON )
        && ( GetWifiApStatus( STATUS_AP_WIFI ) == CLEAR )
        && ( GetWifiApStatus( STATUS_AP_BLE ) == CLEAR )
        && ( GetWifiServerStatus( STATUS_SVR_CONNECT ) == CLEAR ) )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 CheckServerDisconnect ( void )
{
    if ( GetWifiServerStatus( STATUS_SVR_END ) == SET )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 CheckFirstData ( void )
{
    if ( ( GetWifiControlStatus( STATUS_CON_READY ) == ON )
        && ( GetWifiIniStatus( STATUS_INI_FINISH ) != SET ) )
    {
        return TRUE;
    }
    SetWifiIniStatus( STATUS_INI_SEQ, INI_SEQ_NULL );
    return FALSE;
}

static U8 CheckFirstFail ( void )
{
    if ( ( GetWifiControlStatus( STATUS_CON_READY ) == ON )
        && ( GetWifiApStatus( STATUS_AP_WIFI ) == CLEAR )
        && ( GetWifiApStatus( STATUS_AP_BLE ) == CLEAR )
        && ( GetWifiIniStatus( STATUS_INI_FINISH ) != SET )
        && ( GetWifiServerStatus( STATUS_SVR_CONNECT ) == SET ) )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 CheckSendData ( void )
{
    if ( ( GetWifiControlStatus( STATUS_CON_READY ) == ON )
        && ( GetWifiTxStatus( STATUS_TX_SEQ ) == WIFI_TX_SEQ_NULL ) )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 CheckSendAck ( void )
{
    if ( ( GetWifiServerStatus( STATUS_SVR_CONNECT ) == SET )
        && ( GetWifiTxStatus( STATUS_TX_STATE ) == SET ) )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 CheckConnect ( void )
{
    if ( GetWifiServerStatus( STATUS_SVR_CONNECT ) == SET )
    {
        return TRUE;
    }
    ClearUserData();
    return FALSE;
}

static U8 CheckFotaStart ( void )
{
    if ( ( GetWifiFotaStatus( STATUS_FOTA_SEQ ) != FOTA_NULL )
        && ( GetUserInterface( USER_FOTA_POSSIBLE_STATUS ) == TRUE ) )
    {
        return TRUE;
    }
    return FALSE;
}

static void DoReset ( void )
{
    HAL_SetWifiOnOff( RESET );
}
static void DoOff ( void )
{
    HAL_SetWifiOnOff( OFF );
}

static void DoApModeStart ( void )
{
    if ( GetWifiApStatus( STATUS_AP_WIFI ) == WAIT )
    {
        SetWifiApStatus( STATUS_AP_WIFI, RESET );
    }
    else if ( GetWifiApStatus( STATUS_AP_WIFI ) == RESET )
    {
        ClearWifiAllStatus();
        SetWifiApStatus( STATUS_AP_WIFI, START );
    	SetWifiControlStatus( STATUS_CON_POWER, ON );
        SetWifiControlStatus( STATUS_CON_READY, ON );
        SetWifiIndicateStatus( STATUS_DISPLAY, WIFI_DISP_TRY );
        SetWifiSendStatus( TX_ATCMD, WIFI_AT_APMODE );
    }
    else if ( GetWifiApStatus( STATUS_AP_WIFI ) == START )
    {
        SetWifiApStatus( STATUS_AP_WIFI, RETRY );
    }
    else
    {

    }
}

static void DoApBleStart ( void )
{
    if ( GetWifiApStatus( STATUS_AP_BLE ) == WAIT )
    {
        SetWifiApStatus( STATUS_AP_BLE, START );
        SetWifiSendStatus( TX_ATCMD, WIFI_AT_BLE_ADV );
    }
}

static void DoApModeFail ( void )
{
    if ( GetWifiServerStatus( STATUS_SVR_CONNECT ) == SET )
    {
        SetWifiSendStatus( TX_ATCMD, WIFI_AT_BLE_OFF );
        SetWifiIndicateStatus( STATUS_PARING, WIFI_AP_STEP_CLEAR );
        SetWifiBuzzSound( WIFI_BUZZER_BLE_OFF );
    }
    else
    {
        HAL_SetWifiOnOff( OFF );
        SetWifiBuzzSound( WIFI_BUZZER_AP_FAIL );
    }
}

static void DoFirstData ( void )
{
    WifiStartProcess();
}

static void DoFirstFail ( void )
{
    SetWifiIniStatus( STATUS_INI_SEQ, INI_SEQ_SERVER_CONNECT );
}

static void DoSendData ( void )
{
    WifiTxDataSelect();
}

static void DoSend1min ( void )
{
    SetWifiSendStatus( TX_DATA, WIFI_TX_SEND_SENSOR );
}

static void DoRqst20min ( void )
{
#ifdef USER_USE_WEATHER
    if ( GetWifiIniStatus( STATUS_INI_INFO ) == SET )
    {
        SetWifiSendStatus( TX_DATA, WIFI_TX_RQST_INFO_DATA );
    }
    else
    {
        SetWifiSendStatus( TX_DATA, WIFI_TX_RQST_INFO );
    }
#endif
}

static void DoRqst24Hour ( void )
{
    SetWifiSendStatus( TX_DATA, WIFI_TX_RQST_INFO );
}

static void DoFotaStart ( void )
{
    WifiFotaProcess();
}


