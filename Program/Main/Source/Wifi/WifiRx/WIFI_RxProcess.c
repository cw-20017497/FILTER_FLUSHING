
/*******************************************************************************
&Description :
    WIFI Module AT Cmd Check (MODULE->MCU)
  
&Histiry : ShinHM
    2021.04.21 update
    2021.12.09 Logic update
*******************************************************************************/


/***** Header *****************************************************************/
#include "WIFI_Common.h"
#include "WIFI_RxProcess.h"
#include "WIFI_Initialize.h"
#include "WIFI_RxServer.h"
#include "WIFI_Fota.h"
#include "WIFI_Status.h"
#include "WIFI_UserInterface.h"


/***** Define *****************************************************************/
#define COUNT_CONNECT_MODULE 3000 // 100ms*3000 = 300s


/***** Variable ***************************************************************/
/* WifiRx Value */
typedef struct _wifirx_
{
    U8  u8State;        // Rx$)C:P<. (0:Rx0!4I / 1::P<.A_)
    U8	u8Wait;         // $)C4k1bA_@N 5%@LEM (0:>x@= / 1: @V@=)
    U8  u8CheckModem;   // $)C0x@/1b C9 ?,0aH.@N
    U8  u8CheckServer;  // $)C<-9v C9 ?,0aH.@N
    U16 u16ConnectCheckCount;  // $)C8p5b0z@G ?,0aH.@N (5:PD+?nF.)
} WifiRx_T;
WifiRx_T WifiRx;

I8 ai8WifiRxData[LENGTH_WIFI_ALL_MAX]; // ALL RX DATA Buffer (AT CMD + STX ~ ETX)
I8 ai8WifiRxBuff[LENGTH_WIFI_ALL_MAX]; // $)CC38.5G1b @|?! 5%@LEM0! 5i>n?@4B0f?l 9vF[


/***** Constant ***************************************************************/
/* Rx Protocol (MODULE -> MCU) */
const signed char AT_RX_DEVICE_READY[] = "*ICT*DEVICEREADY";
const signed char AT_RX_AWS_SET[] = "*ICT*AWS_SET:OK";
const signed char AT_RX_AUTO_CON[] = "*ICT*AUCONMODE:OK";
const signed char AT_RX_BLE_ADV[] = "*ICT*BLE_ADV:OK";
const signed char AT_RX_AP_START[] = "*ICT*AP_STARTED:OK";
const signed char AT_RX_AP_BLE[] = "*ICT*BLE_STATE:2";
const signed char AT_RX_BLE_OFF[] = "*ICT*BLE_STATE:0";

const signed char AT_RX_PAIRING[] = "*ICT*PAIRING";
const signed char AT_RX_AUTHENTICATE[] = "*ICT*AWS_IND:AUTHENTICATE OK";
const signed char AT_RX_CERTIFICATE[] = "*ICT*AWS_IND:CERTIFICATE OK";
const signed char AT_RX_PRIVATE[] = "*ICT*AWS_IND:PRIVATE_KEY OK";

const signed char AT_RX_ASSOCIATED0[] = "*ICT*ASSOCIATED:0";
const signed char AT_RX_IPALLOCATED[] = "*ICT*IPALLOCATED";
const signed char AT_RX_ASSOCIATED1[] = "*ICT*ASSOCIATED:1";
const signed char AT_RX_ASSOCIATED2[] = "*ICT*ASSOCIATED:2";
const signed char AT_RX_ASSOCIATED3[] = "*ICT*ASSOCIATED:3";
const signed char AT_RX_ASSOCIATED4[] = "*ICT*ASSOCIATED:4";

const signed char AT_RX_DISASSOCIATED[] = "*ICT*DISASSOCIATED";
const signed char AT_RX_DISCONNECTED[] = "*ICT*AWS_IND:DISCONNECTED";
const signed char AT_RX_SERVER_ERROR[]= "*ICT*AWS_IND:MQTT ERROR 201";

const signed char AT_RX_CONNECTED[] = "*ICT*AWS_IND:CONNECT OK";
const signed char AT_RX_SEND[] = "*ICT*AWS_IND:SEND OK";
const signed char AT_RX_RECV[] = "*ICT*AWS_RECV";

const signed char AT_RX_OTA_START[] = "*ICT*OTA_SEND_START=";
const signed char AT_RX_OTA_DATA[] = "*ICT*OTA_SEND_DATA=0000";
const signed char AT_RX_FOTA_RESET[] = "*ICT*WIFI_FOTA_RESET";


/***** Static Function ********************************************************/
/* Rx Protocol Function */
static U8 RxReady ( void );
static U8 RxAutoOn ( void );
static U8 RxAutoAck ( void );
static U8 RxBleAdvAck ( void );
static U8 RxApErrClear ( void );

static U8 RxModemOk ( void );
static U8 RxFactory ( void );
static U8 RxDiscnt ( void );
static U8 RxCnt ( void );

static U8 RxSend ( void );
static U8 RxRecv ( void );

static U8 RxFotaSize ( void );
static U8 RxFotaType ( void );
static U8 RxFotaModule ( void );


/* Rx Function */
static U8 IsRxPossible ( void );
static void DoConnect ( U8 mVal );


/***** Table ******************************************************************/
typedef U8 (*WifiRxAtFun_T)(void);
typedef struct _wifi_rxat_list_
{
    I8 *Id;
    U8  Disp;
    U8  Err;
    U8  ApWifi;
    U8  ApBle;
    U8  ApStep;
    U8  Connect;
    U8  Sound;
    WifiRxAtFun_T Func;
} WifiRxAtList_T;

static void RunRxData ( WifiRxAtList_T *pList, U8 mSoundSet );

/* Rx Protocol Table */
static const WifiRxAtList_T WifiRxAtList[] = 
{   /*  ID                  Display             Error           ApWiFi      ApBLE       ApStep                  Server      Sound                       Function    */
    {   AT_RX_DEVICE_READY, WIFI_DISP_TRY,      WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_AP_STEP_CLEAR,     WIFI_NULL,  WIFI_NULL,                  RxReady     },  // Module On
    {   AT_RX_AWS_SET,      WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              WIFI_NULL,  WIFI_NULL,                  RxAutoOn    },  // Version Ack
    {   AT_RX_AUTO_CON,     WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              WIFI_NULL,  WIFI_NULL,                  RxAutoAck   },  // AutoCon Ack
    {   AT_RX_BLE_ADV,      WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              WIFI_NULL,  WIFI_NULL,                  RxBleAdvAck },  // BLE ADV Ack        
    {   AT_RX_AP_START,     WIFI_DISP_APMODE,   WIFI_NULL,      SET,        WIFI_NULL,  WIFI_AP_STEP_WAIT,      WIFI_NULL,  WIFI_BUZZER_AP_CONNECT,     RxApErrClear},  // AP Mode Start
    {   AT_RX_AP_BLE,       WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  SET,        WIFI_AP_STEP_WAIT,      WIFI_NULL,  WIFI_BUZZER_AP_CONNECT,     RxApErrClear},  // AP BLE Mode Start
    {   AT_RX_BLE_OFF,      WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  CLEAR,      WIFI_AP_STEP_CLEAR,     WIFI_NULL,  WIFI_NULL,                  NULL        },  // Ble Off

    {   AT_RX_PAIRING,      WIFI_DISP_APMODE,   WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_AP_STEP_START,     WIFI_NULL,  WIFI_BUZZER_SELECT,         NULL        },  // PAIRING Step 1
    {   AT_RX_AUTHENTICATE, WIFI_DISP_APMODE,   WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_AP_STEP_ROOTCA,    WIFI_NULL,  WIFI_BUZZER_SELECT,         NULL        },  // PAIRING Step 3
    {   AT_RX_CERTIFICATE,  WIFI_DISP_APMODE,   WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_AP_STEP_ROOTCA,    WIFI_NULL,  WIFI_BUZZER_SELECT,         NULL        },  // PAIRING Step 4
    {   AT_RX_PRIVATE,      WIFI_DISP_APMODE,   WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_AP_STEP_ROOTCA,    WIFI_NULL,  WIFI_BUZZER_SELECT,         NULL        },  // PAIRING Step 5 
    
    {   AT_RX_ASSOCIATED0,  WIFI_DISP_TRY,      WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              CLEAR,      WIFI_BUZZER_SELECT,         RxModemOk   },  // $)C0x@/1b ?,0a?O7a (PAIRING Step 2)
    {   AT_RX_IPALLOCATED,  WIFI_DISP_TRY,      WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              WIFI_NULL,  WIFI_BUZZER_SERVER_CONNECT, RxFactory   },  // $)C0x@/1b ?,0a HD ipA$:8 @|<[?O7a
    {   AT_RX_ASSOCIATED1,  WIFI_DISP_AP1,      WIFI_DISP_AP1,  WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              CLEAR,      WIFI_NULL,                  NULL        },  // AP$)C0! :RF/A$ @L@/7N A"<S 2w@= (RESULT_FAIL)
    {   AT_RX_ASSOCIATED2,  WIFI_DISP_AP2,      WIFI_DISP_AP2,  WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              CLEAR,      WIFI_NULL,                  NULL        },  // $)CAvA$5H AP0! SCAN 5GAv >J@= (RESULT_NOT_FOUND)
    {   AT_RX_ASSOCIATED3,  WIFI_DISP_AP3,      WIFI_DISP_AP3,  WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              CLEAR,      WIFI_NULL,                  NULL        },  // $)CA"<S ?dC;?! 4kGQ @@4d@; =C0#3;?! 8x9^@= (RESULT_TIMEOUT)
    {   AT_RX_ASSOCIATED4,  WIFI_DISP_AP4,      WIFI_DISP_AP4,  WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              CLEAR,      WIFI_NULL,                  NULL        },  // Security $)CA$:80! 8BAv >J@= (RESULT_RESTRICTED)

    {   AT_RX_DISASSOCIATED,WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              CLEAR,      WIFI_NULL,                  NULL        },  // $)C8p5b0z 0x@/1b0! ?,0a@L 2w>nA|
    {   AT_RX_DISCONNECTED, WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              CLEAR,      WIFI_NULL,                  RxDiscnt    },  // $)C<-9v ?,0aA>7a (:qA$;s@{@N A>7aFG4\ >H5J)
    {   AT_RX_SERVER_ERROR, WIFI_DISP_AP5,      WIFI_DISP_AP5,  WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              CLEAR,      WIFI_NULL,                  NULL        },  // $)C<-9v A"<S?!7/

    {   AT_RX_CONNECTED,    WIFI_DISP_CONNECT,  WIFI_DISP_OFF,  WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              SET,        WIFI_BUZZER_SERVER_CONNECT, RxCnt       },  // Server Connect
    {   AT_RX_SEND,         WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              WIFI_NULL,  WIFI_NULL,                  RxSend      },  // Send Ack
    {   AT_RX_RECV,         WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              WIFI_NULL,  WIFI_NULL,                  RxRecv      },  // Recv Server Data
    
    {   AT_RX_OTA_START,    WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              WIFI_NULL,  WIFI_NULL,                  RxFotaSize  },  // OTA $)C?dC;
    {   AT_RX_OTA_DATA,     WIFI_NULL,          WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              WIFI_NULL,  WIFI_NULL,                  RxFotaType  },  // OTA $)CCJ1b 5%@LEM H.@N
    {   AT_RX_FOTA_RESET,   WIFI_DISP_TRY,      WIFI_NULL,      WIFI_NULL,  WIFI_NULL,  WIFI_NULL,              CLEAR,      WIFI_NULL,                  RxFotaModule}   // $)C8p5bFwE8 =C@[
};
#define SZ_RX_CMD_LIST  ( sizeof(WifiRxAtList) / sizeof(WifiRxAtList_T) )


/***** Global Function ********************************************************/
void InitializeWifiRxProcess ( void )
{
    _MEMSET_( (void __FAR*) &WifiRx, 0, sizeof( WifiRx_T ) );
    _MEMSET_( (void __FAR*) ai8WifiRxData, '\0', sizeof( ai8WifiRxData ) );
    _MEMSET_( (void __FAR*)ai8WifiRxBuff, '\0', sizeof( ai8WifiRxBuff ) );
}

void ClearRxStatus ( void )
{
    WifiRx.u8CheckModem = CLEAR;
    WifiRx.u8CheckServer = CLEAR;
    WifiRx.u16ConnectCheckCount = CLEAR;
}

void GetWifiUartRxData ( I8* mai8UartData )
{
    if ( WifiRx.u8State == SET )
    { /* $)C5%@LEM0! C38.5G1b @|?! 5%@LEM0! ?@4B 0f?l 9vF[?! @z@e */
        _STRCPY_( (char __FAR*) ai8WifiRxBuff, (const char __FAR*) mai8UartData );
        WifiRx.u8Wait = SET;
    }
    else
    { /* $)CGv@g 5%@LEM0! >x4B0f?l :P<.=C@[ */
        _STRCPY_( (char __FAR*) ai8WifiRxData, (const char __FAR*) mai8UartData );
        WifiRx.u8State = SET;
    }
}

void WifiRxDataProcess ( void )
{
    WifiRxAtList_T *pList = NULL;
    U8 i = 0U;
    U8 mSound = TRUE;

    /* Validity Check */
    if ( IsRxPossible() != TRUE )
    {
         return;
    }

    /* Rx AT Cmd Analysis */
    pList = WifiRxAtList;
    for ( i = 0 ; i < SZ_RX_CMD_LIST; i++ )
    {
        if ( _STRNCMP_( (const char __FAR*) (pList+i)->Id, (const char __FAR*) ai8WifiRxData, _STRLEN_( (const char __FAR*) (pList+i)->Id) ) == N_TRUE )
        {
            WifiRxAtFun_T pFun;
            
            pFun = (pList+i)->Func;
            
            if ( pFun != NULL )
            {
               mSound = pFun();
            }
            RunRxData( pList+i, mSound );
            break;
        }
    }
    _MEMSET_( (void __FAR*) ai8WifiRxData, '\0', sizeof( ai8WifiRxData ) );
   
    WifiRx.u16ConnectCheckCount = CLEAR;
    WifiRx.u8State = CLEAR;
    
}

void WifiRxAckTimer ( void )
{
    WifiRx.u16ConnectCheckCount++;
    if ( ( GetWifiApStatus( STATUS_AP_WIFI ) != CLEAR ) || ( GetWifiApStatus( STATUS_AP_BLE ) != CLEAR ) )
    {
        WifiRx.u16ConnectCheckCount = 0;
    }
    if ( WifiRx.u16ConnectCheckCount >= COUNT_CONNECT_MODULE )
    {
        _MEMSET_( (void __FAR*) &WifiRx, 0, sizeof( WifiRx_T ) );
        HAL_SetWifiOnOff( RESET );
    }
}


/***** Local Function *********************************************************/
static U8 IsRxPossible ( void )
{
    if ( WifiRx.u8State == CLEAR )
    {
        if ( WifiRx.u8Wait == SET )
        {
            _STRCPY_( (char __FAR*) ai8WifiRxData, (const char __FAR*) ai8WifiRxBuff );
            _MEMSET_( (void __FAR*)  ai8WifiRxBuff, '\0', sizeof( ai8WifiRxBuff ) );
            WifiRx.u8Wait = CLEAR;
            WifiRx.u8State = SET;
        }
        else
        {
            return FALSE;
        }
    }
    return TRUE;
}

#define W_DISPLAY_CONFIRM_COUNT 2
static void RunRxData ( WifiRxAtList_T *pList, U8 mSoundSet )
{
    static U8 mu8DisplayCount = 0U;
    
    if ( pList->Disp != WIFI_NULL )
    {
        if ( ( pList->Disp >= WIFI_DISP_AP1 ) && ( pList->Disp <= WIFI_DISP_AP5 ) )
        {
            mu8DisplayCount++;
            if ( mu8DisplayCount > W_DISPLAY_CONFIRM_COUNT )
            {
                mu8DisplayCount = 0U;
                SetWifiIndicateStatus( STATUS_DISPLAY, pList->Disp );
            }
        }
        else
        {
            SetWifiIndicateStatus( STATUS_DISPLAY, pList->Disp );
        }
    }
    if ( pList->Err != WIFI_NULL )
    {
        SetWifiIndicateStatus( STATUS_ERROR, pList->Err );
        if ( GetWifiIndicateStatus( STATUS_PARING ) > WIFI_AP_STEP_WAIT )
        {
            SetWifiIndicateStatus( STATUS_AP_ERROR, pList->Err );
        }
    }
    if ( pList->ApWifi != WIFI_NULL )
    {
        SetWifiApStatus( STATUS_AP_WIFI, pList->ApWifi );
    }
    if ( pList->ApBle != WIFI_NULL )
    {
        SetWifiApStatus( STATUS_AP_BLE, pList->ApBle );
    }
    if ( pList->ApStep != WIFI_NULL )
    {
        SetWifiIndicateStatus( STATUS_PARING, pList->ApStep );
    }
    if ( pList->Connect != WIFI_NULL )
    {
        DoConnect( pList->Connect );
    }
    if ( ( pList->Sound != WIFI_NULL ) && ( mSoundSet == TRUE ) )
    {
        SetWifiBuzzSound( pList->Sound );
    }
}

static void DoConnect ( U8 mVal )
{
    /*
    WIFI_DISP_AP5, $)CFd>n85A_ 8.<B>HGO4B C38. H.@N
    */
    if ( mVal == SET )
    {
        SetWifiIniStatus( STATUS_INI_SEQ, INI_SEQ_SERVER_CONNECT );
        SetWifiServerStatus( STATUS_SVR_CONNECT, SET );
    }
    else
    {
        SetWifiServerStatus( STATUS_SVR_CONNECT, CLEAR );
        SetWifiIniStatus( STATUS_INI_FINISH, CLEAR );
        SetWifiIniStatus( STATUS_INI_SEQ, INI_SEQ_NULL );
        SetWifiIniStatus( STATUS_INI_INFO, CLEAR );
    }
}

static U8 RxReady ( void )
{    
    /* Status : Module On Ready Ok */
    SetWifiControlStatus( STATUS_CON_READY, ON );
    /* Fota : Clear Module Fota */
    if ( GetWifiFotaStatus( STATUS_FOTA_MODULE ) == SET )
    {
        SetWifiFotaStatus( STATUS_FOTA_MODULE, CLEAR );
        WifiWriteEep( EEPROM_ADDR_FOTA_MODULE, CLEAR );
    }
    /* AT Cmd : Autocon On & Version */
    if ( GetUserInterface( USER_FACTORY_STATUS ) == SET )
    {
        SetWifiSendStatus( TX_ATCMD, WIFI_AT_TESTMODE );
    }
    else
    {
        SetWifiSendStatus( TX_ATCMD, WIFI_AT_VERSION );
    }
#if ( TEST_WIFI_PJT == 1 )
    SetWifiSendStatus( TX_ATCMD, WIFI_AT_SCONN );
#endif
    return FALSE;
}

static U8 RxAutoOn ( void )
{
    SetWifiSendStatus( TX_ATCMD, WIFI_AT_AUTO_ON );
    return FALSE;
}


static U8 RxAutoAck ( void )
{
    /* $)CA&G0FOTA AxG` =C ?@EdD\ ACK <v=E HD FOTA AxG` */
    if ( GetWifiFotaStatus( STATUS_FOTA_SEQ ) == FOTA_WAIT )
    {
        SetWifiFotaStatus( STATUS_FOTA_SEQ, FOTA_START );
    }
    else
    {
        SetWifiSendStatus( TX_ATCMD, WIFI_AT_BLE_ADV );
    }
    return FALSE;
}

static U8 RxBleAdvAck ( void )
{
    if ( GetWifiApStatus( STATUS_AP_BLE ) == START )
    {
        SetWifiSendStatus( TX_ATCMD, WIFI_AT_BLE_AP );
    }
    return FALSE;
}

static U8 RxApErrClear ( void )
{
    SetWifiIndicateStatus( STATUS_AP_ERROR, WIFI_DISP_OFF );
    return TRUE;
}

static U8 RxModemOk ( void )
{
    /* Status : Connecting & AP MODE Clear */
    if ( GetWifiIndicateStatus( STATUS_PARING ) >= WIFI_AP_STEP_START )
    {
        WifiWriteEep( EEPROM_ADDR_WIFI_POWER, SET );
        SetWifiIndicateStatus( STATUS_PARING, WIFI_AP_STEP_MODEM );
        WifiRx.u8CheckModem = CLEAR;
    }
    
    if ( WifiRx.u8CheckModem != SET )
    {
        WifiRx.u8CheckModem = SET;
        if ( GetWifiFotaStatus( STATUS_FOTA_MCU ) == CLEAR )
        {
            return TRUE;
        }
    }
    return FALSE;
}

static U8 RxFactory ( void )
{
    /* Display : Factory Ok */
    SetWifiIndicateStatus( STATUS_FACTORY, SET );
    if ( GetUserInterface( USER_FACTORY_STATUS ) == SET )
    {
        return TRUE;
    }
    return FALSE;
}

static U8 RxDiscnt ( void )
{
    if ( GetWifiServerStatus( STATUS_SVR_END ) == SET )
    {
        HAL_SetWifiOnOff( OFF );
    }
    if ( GetWifiApStatus( STATUS_AP_WIFI ) == WAIT )
    {
        SetWifiIndicateStatus( STATUS_DISPLAY, WIFI_DISP_TRY );
    	SetWifiApStatus( STATUS_AP_WIFI, RESET );
    }
    /* Fota : $)C<-9v?,0a A>7a HD Fota4k1b */
    if ( GetWifiFotaStatus( STATUS_FOTA_SEQ ) == FOTA_DISCONNECT )
    {
        SetWifiFotaStatus( STATUS_FOTA_SEQ, FOTA_WAIT );
        SetWifiSendStatus( TX_ATCMD, WIFI_AT_AUTO_OFF );
    }
    /* AT Cmd : X */
    /* Etc : X */
    return FALSE;
}

static U8 RxCnt(void)
{
    U8 mu8ApCheck = 0U;
    
    if ( GetWifiApStatus( STATUS_AP_FIRST ) == CLEAR ) // $)CCVCJ Fd>n85 @z@e
    {
        SetWifiApStatus( STATUS_AP_FIRST, SET );
        WifiWriteEep( EEPROM_ADDR_FIRST_PARING, SET );
    }
    if ( GetWifiIndicateStatus( STATUS_PARING ) == WIFI_AP_STEP_ROOTCA )
    {
        SetWifiIndicateStatus( STATUS_PARING, WIFI_AP_STEP_CLEAR );
        SetWifiApStatus( STATUS_AP_BLE, CLEAR );
        SetWifiApStatus( STATUS_AP_WIFI, CLEAR );
        WifiRx.u8CheckServer = CLEAR;
        mu8ApCheck = SET;
    }
    
    if ( WifiRx.u8CheckServer == CLEAR )
    {
        WifiRx.u8CheckServer = SET;
        if ( GetWifiFotaStatus( STATUS_FOTA_MCU ) == SET )
        {
            SetWifiFotaStatus( STATUS_FOTA_MCU, CLEAR );
            WifiWriteEep( EEPROM_ADDR_FOTA_MCU, CLEAR );
        }
        else
        {
            if ( mu8ApCheck == SET )
            {
                SetWifiBuzzSound( WIFI_BUZZER_SERVER_FIRST );
            }
            else
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static U8 RxSend ( void )
{
    SetWifiTxStatus( STATUS_TX_STATE, CLEAR );
    WifiCheckInitializeAck();
    return FALSE;
}

static U8 RxRecv(void)
{
    WifiInsertRxAPI( ai8WifiRxData );
    return FALSE;
}

static U8 RxFotaSize ( void )
{
    WifiCheckFotaSize( ai8WifiRxData );
    return FALSE;
}

static U8 RxFotaType ( void )
{
    WifiCheckFotaType( ai8WifiRxData );
    return FALSE;
}

static U8 RxFotaModule ( void )
{   /* Status : Module Fota */
    SetWifiFotaStatus( STATUS_FOTA_MODULE, SET );
    WifiWriteEep( EEPROM_ADDR_FOTA_MODULE, SET );
    SetWifiControlStatus( STATUS_CON_READY, OFF );
    return FALSE;
}


