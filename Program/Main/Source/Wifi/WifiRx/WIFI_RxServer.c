
/*******************************************************************************
&Description :
    Analysis Server API
  
&Histiry : ShinHM
    2021.04.22 update
    2021.12.10 update
*******************************************************************************/


/******************************************************************************/
/***** Header *****************************************************************/
/******************************************************************************/
#include "WIFI_Common.h"
#include "WIFI_RxServer.h"
#include "WIFI_TxProtocol.h"
#include "WIFI_Initialize.h"
#include "WIFI_Status.h"
#include "WIFI_SetFunctionData.h"
#include "WIFI_SetInformationData.h"
#include "WIFI_UserInterface.h"


/***** Define *****************************************************************/
/* API No (Server -> MCU) */
const signed char API_RX_A1002[] = "A1002"; // Disconnect
const signed char API_RX_A1011[] = "A1011"; // Function Control
const signed char API_RX_A1013[] = "A1013"; // Function Request
const signed char API_RX_A1023[] = "A1023"; // Sensor Request
const signed char API_RX_A1033[] = "A1033"; // Error Request
const signed char API_RX_A1035[] = "A1035"; // Error Selfcheck Request
const signed char API_RX_A1051[] = "A1051"; // Parameter Control
const signed char API_RX_A1053[] = "A1053"; // Parameter Request
const signed char API_RX_A1083[] = "A1083"; // SmartDetect Period Request
const signed char API_RX_A2011[] = "A2011"; // Information Respond

/* Wifi Recv Cmd */
#define AT_CMD_RECV "*ICT*AWS_RECV:"

/* Length & Position */
#define LENGTH_HEADER 5 // STX + LENGTH
#define POSITION_STX 0 // 0 (1Byte)
#define POSITION_LENGTH 1 // 1~4 (4Byte)
#define POSITION_API 5 // 5~9 (5Byte)
#define POSITION_RQID 10 // 10~25 (16Byte)
#define POSITION_OPEN 26 // 26 (1Byte) '{'
#define POSITION_DATA 27 // 27~ (nByte)


/***** Variable ***************************************************************/
I8 ai8RxProtocol[LENGTH_WIFI_DATA_MAX]; // RX DATA (STX ~ ETX)
U16 au16InfoData[WIFI_INFO_NUM];


/***** Constant ***************************************************************/


/***** Static Function ********************************************************/
static void RunAnalysis ( void );
static U8 IsRxDataOk ( void );

static U8 RxA1011 ( void );
static U8 RxA2011 ( void );

static void DoFuncData ( I8* pBuf );
static void DoInfoData ( I8* pBuf );

static void MakeCheckId ( I8* pBuf, U16 mLen, U16 mId );


/***** Table ******************************************************************/
typedef U8 (*WifiRxApiFun_T)( void );
typedef struct _wifi_rxapi_list_
{
    I8* Api;
    U32 Tx;
    WifiRxApiFun_T Func;
} WifiRxApiList_T;

/* Wifi Rx Table */
static const WifiRxApiList_T WifiRxApiList[] =
{   /*  API             Tx                      Function */
    {   API_RX_A1002,   WIFI_TX_NULL,           NULL    },
    {   API_RX_A1011,   WIFI_TX_RESP_RECV,      RxA1011 },
    {   API_RX_A1013,   WIFI_TX_RESP_FUNC,      NULL    },
    {   API_RX_A1023,   WIFI_TX_RESP_SENSOR,    NULL    },
    {   API_RX_A1033,   WIFI_TX_RESP_ERR,       NULL    },
    {   API_RX_A1035,   WIFI_TX_SELF_ERR,       NULL    },
    {   API_RX_A1051,   WIFI_TX_SEND_PARA,      NULL    },
    {   API_RX_A1053,   WIFI_TX_RESP_PARA,      NULL    },
    {   API_RX_A1083,   WIFI_TX_RESP_PART,      NULL    },
    {   API_RX_A2011,   WIFI_TX_NULL,           RxA2011 }
};
#define SZ_RX_API_LIST  ( sizeof(WifiRxApiList) / sizeof(WifiRxApiList_T) )


/***** Global Function ********************************************************/
void InitializeWifiRxServer ( void )
{
    _MEMSET_( (void __FAR*) ai8RxProtocol, '\0', sizeof(ai8RxProtocol) );
    _MEMSET_( (void __FAR*) au16InfoData, 0, sizeof(au16InfoData) );
}

void WifiInsertRxAPI ( I8* mai8Recv )
{
    U16 mu16Len = 0U;
    U16 mu16Position = 0U;
    U16 i = 0U;

    _MEMSET_( (void __FAR*) ai8RxProtocol, '\0', sizeof(ai8RxProtocol) );

    /* Check Max Length */
    mu16Len = _STRLEN_( (const char __FAR*) mai8Recv );
    mu16Position = _STRLEN_( (const char __FAR*) AT_CMD_RECV );
    if( mu16Len > LENGTH_WIFI_ALL_MAX )
    {
        return;
    }
  	
    /* Insert Data */
    for ( i = 0 ; i < mu16Len ; i++ )
    {   /* STX ~ ETX */
        ai8RxProtocol[i] = mai8Recv[mu16Position + i];
        if ( ai8RxProtocol[i] == ASCII_ETX )
        {
            break;
        }
    }
    
    /* Analysis */
    RunAnalysis();
}

U16 GetWifiInfoData ( U16 mId )
{
    return au16InfoData[mId];
}

void SetWifiInfoData ( U16 mId, U16 mData )
{
    au16InfoData[mId] = mData;
    if ( mId == WIFI_INFO_0006_DAY )
    {
        SetUserTimeSetting( &au16InfoData[0] );
    }
    if ( ( mId == WIFI_INFO_0007_PART_PERIOD ) && ( GetWifiIniStatus( STATUS_INI_INFO ) != SET ) )
    {
        SetWifiIniStatus( STATUS_INI_INFO, SET );
    }
}


/***** Local Function *********************************************************/
// <-9v 5%@LEM :P<.
static void RunAnalysis ( void )
{
    WifiRxApiList_T *pList = NULL;
    U8 i = 0U;
    static I8 mai8RxAPINo[LENGTH_API_NO+1];
    static I8 mai8RqId[LENGTH_REQUEST_ID+1];
    
    _MEMSET_( (void __FAR*) mai8RxAPINo, '\0', sizeof(mai8RxAPINo));
    _MEMSET_( (void __FAR*) mai8RqId, '\0', sizeof(mai8RqId));
    
    /* Validity Check */
    if( IsRxDataOk() != TRUE )
    {
        return;
    }
    
    /* API & RequestID */
    for ( i = 0; i < LENGTH_API_NO ; i++ )
    {
        mai8RxAPINo[i] = ai8RxProtocol[POSITION_API + i];
    }
    for ( i = 0; i < LENGTH_REQUEST_ID ; i++ )
    {
        mai8RqId[i] = ai8RxProtocol[POSITION_RQID + i];
    }
    SetRequestId( mai8RqId );
    
    /* Run */
    pList = WifiRxApiList;
    for ( i = 0; i < SZ_RX_API_LIST; i++ )
    {
        if ( _STRNCMP_( (const char __FAR*) (pList+i)->Api, (const char __FAR*) mai8RxAPINo, LENGTH_API_NO ) == N_TRUE )
        {   /* API NO :P<.HD GX4g 5?@[ C38. */
            WifiRxApiFun_T pFun;
            U8 mAck = TRUE;

            pFun = (pList+i)->Func;
            
            if ( pFun != NULL )
            {
                mAck = pFun();
            }
            if ( ( mAck == TRUE ) && ( (pList+i)->Tx != WIFI_TX_NULL ) )
            {
                SetWifiSendStatus( TX_DATA, (pList+i)->Tx );
            }
            break;
        }
    }
}

// @/H?<: 0K;g
static U8 IsRxDataOk ( void )
{
    U16 mu16i = 0U;
    U16 mu16DataLength = 0U;
    U32 mu32CheckSumBuf = 0UL;
    U8 mu8CheckSum = 0U;
    U8 mu8RxCheckSum = 0U;

    /* Check STX */
    if ( ai8RxProtocol[POSITION_STX] != ASCII_STX )
    {
        return FALSE;
    }
    
    /* Check ETX */
    if ( ai8RxProtocol[ _STRLEN_( (const char __FAR*) ai8RxProtocol ) - 1 ] != ASCII_ETX )
    {
        return FALSE;
    }
    
    /* Check Length */
    mu16DataLength = (WIFI_ASCII2HEX(ai8RxProtocol[POSITION_LENGTH]) << 12) & 0xF000;
    mu16DataLength += (WIFI_ASCII2HEX(ai8RxProtocol[POSITION_LENGTH+1]) << 8) & 0x0F00;
    mu16DataLength += (WIFI_ASCII2HEX(ai8RxProtocol[POSITION_LENGTH+2]) << 4) & 0x00F0;
    mu16DataLength += WIFI_ASCII2HEX(ai8RxProtocol[POSITION_LENGTH+3]) & 0x000F;
    
    if ( ( mu16DataLength + LENGTH_HEADER ) != _STRLEN_( (const char __FAR*) ai8RxProtocol ) )
    {
        return FALSE;
    }
    
    /* Check CHECKSUM */
    for ( mu16i = 0 ; mu16i < ( _STRLEN_( (const char __FAR*) ai8RxProtocol ) - 3 ) ; mu16i++ )
    { // CHECKSUM0z ETX8& ;)0m 8p5N
        if ( mu16i > LENGTH_WIFI_ALL_MAX )
        {
            break;
        }
        else
        {
            mu32CheckSumBuf += ai8RxProtocol[mu16i];
        }
    }
    mu8CheckSum = (U8)mu32CheckSumBuf;
    mu8RxCheckSum = (U8)((WIFI_ASCII2HEX(ai8RxProtocol[_STRLEN_( (const char __FAR*) ai8RxProtocol)-3]) << 4) & 0xF0);
    mu8RxCheckSum += (U8)(WIFI_ASCII2HEX(ai8RxProtocol[_STRLEN_( (const char __FAR*) ai8RxProtocol)-2]) & 0x0F);
    if ( mu8RxCheckSum != mu8CheckSum )
    {
        return FALSE;
    }
    return TRUE;
}

static U8 IsRecvPossible ( void )
{
    U16 mu16ExceptId = 0U;

    mu16ExceptId = (WIFI_ASCII2HEX(ai8RxProtocol[POSITION_DATA]) << 12) & 0xF000;
    mu16ExceptId += (WIFI_ASCII2HEX(ai8RxProtocol[POSITION_DATA+1]) << 8) & 0x0F00;
    mu16ExceptId += (WIFI_ASCII2HEX(ai8RxProtocol[POSITION_DATA+2]) << 4) & 0x00F0;
    mu16ExceptId += WIFI_ASCII2HEX(ai8RxProtocol[POSITION_DATA+3]) & 0x000F;

    if ( GetUserInterface( USER_SMART_SENSING_STATUS ) == TRUE )
    { // Gv@g A$9PAx4\ AxG` A_
        if ( IsExceptId( mu16ExceptId ) == TRUE )
        { // A$9PAx4\ =C@[/A$Av ?dC;88 Gc0!
            return TRUE;
        }
    }
    
    if ( GetUserInterface( USER_RX_STATUS ) != TRUE ) // A&>n:R0!;sEB
    {
        if ( IsExceptId( mu16ExceptId ) == TRUE ) // A$9PAx4\8m7I
        {
            if ( GetUserInterface( USER_SMART_POSSIBLE_STATUS ) == TRUE ) // A$9PAx4\0!4I
            {
                return TRUE;
            }
        }
    }
    else // A&>n0!4I;sEB
    {
        if ( IsExceptId( mu16ExceptId ) == TRUE ) // A$9PAx4\8m7I
        {
            if ( GetUserInterface( USER_SMART_POSSIBLE_STATUS ) != TRUE ) // A$9PAx4\:R0!
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}


// 1b4I <3A$ :/0f ?dC; <v=E
static U8 RxA1011 ( void )
{
    U16 i = 0U;
    static I8 pBuf[LENGTH_WIFI_DATA_ID];

    _MEMSET_( (void __FAR*) pBuf, '\0', sizeof(pBuf) );    

    /* Validity Check */
    if ( IsRecvPossible() != TRUE )
    {
        SetWifiBuzzSound( WIFI_BUZZER_ERROR );
        return TRUE;
    }

    /* Run */
    for ( i = 0 ; i < LENGTH_WIFI_DATA_ID ; i++ )
    {
        pBuf[i] = ai8RxProtocol[POSITION_DATA + i];
        if ( pBuf[i] == ASCII_CLOSE )
        {
            break;
        }
    }
    DoFuncData( pBuf );
#ifdef USER_USE_ACK
    return TRUE;
#endif
    return FALSE;
}

static U8 RxA2011 ( void )
{
    U16 i = 0U;
    static I8 pBuf[LENGTH_WIFI_INFO_MAX];
    
    _MEMSET_( (void __FAR*) pBuf, '\0', sizeof(pBuf) );

    WifiCheckInitializeInfoAck();
    
	/* Run */
    for ( i = 0 ; i < LENGTH_WIFI_INFO_MAX ; i++ )
    {
        pBuf[i] = ai8RxProtocol[POSITION_DATA + i]; // { ~ }
        if ( pBuf[i] == ASCII_CLOSE )
        {
            break;
        }
    }
    DoInfoData( pBuf );
    return FALSE;
}

static void DoFuncData ( I8* pBuf )
{
    WifiTxFuncList_T *pList = NULL;
    U16 i = 0U;
    U16 mu16Size = 0U;
    I8 mi8Data[LENGTH_WIFI_DATA_ID];
	  
    _MEMSET_( (void __FAR*) mi8Data, '\0', sizeof(mi8Data) );

    /* Table Matching */
    pList = (WifiTxFuncList_T *)GetWifiFuncEventList(); // EW@L:m 8.=:F.
    mu16Size = GetWifiFuncListSize(); // EW@L:m ;g@LAn
    
    /* Control */
    for ( i = 0 ; i < mu16Size; i++ )
    {
        WifiRxFun_T pFun;
        I8 mi8Id[LENGTH_WIFI_DATA_ONE];
        U8 mu8i = 0U;

        _MEMSET_( (void __FAR*) mi8Id, '\0', sizeof(mi8Id) );
        
        pFun = (pList+i)->RxFunc;
        MakeCheckId( mi8Id, 0, (pList+i)->Id );
        if ( _STRNCMP_( (const char __FAR*) mi8Id, (const char __FAR*) pBuf, LENGTH_WIFI_ID_ONE ) == N_TRUE ) // ID+:(5Byte) check
        {
            for ( mu8i = 0 ; mu8i < LENGTH_WIFI_DATA_ONE ; mu8i++ )
            {
                if ( ( pBuf[LENGTH_WIFI_ID_ONE+mu8i] == ASCII_COMMA ) || ( pBuf[LENGTH_WIFI_ID_ONE+mu8i] == ASCII_CLOSE ) )
                {
                    break;
                }
                else
                {
                    mi8Data[mu8i] = pBuf[LENGTH_WIFI_ID_ONE+mu8i];
                }
            }
            if ( pFun != NULL )
            {
                pFun( (pList+i)->Id, mi8Data );
                if ( ( IsExceptId( (pList+i)->Id ) == TRUE ) && ( mi8Data[0] == ASCII_NO_1 ) )
                {
                    SetRequestIdExcept();
                }
            }
        }
    }
}


// 1b4I <3A$ :/0f @{?k
static void DoInfoData ( I8* pBuf )
{
    WifiTxFuncList_T *pList = NULL;
    U16 i = 0U;
    U16 mu16Size = 0U;
    U16 mu16InfoCount = 0U;
    U8 mi = 0U;
	  
    //_MEMSET_( (void __FAR*) mi8Data, '\0', sizeof(mi8Data) );

    pList = (WifiTxFuncList_T *)GetWifiInformationEventList(); // EW@L:m 8.=:F.
    mu16Size = GetWifiInformationListSize(); // EW@L:m ;g@LAn
    
    
    for ( i = 0 ; i < mu16Size; i++ )
    {
        U8 mu8Info = 0U;
        WifiRxFun_T pFun;
        I8 mi8Id[LENGTH_WIFI_ID_ONE+1];
        I8 mId[LENGTH_WIFI_ID_ONE+1]; // Ref
        I8 mi8Info[LENGTH_WIFI_DATA_ONE];

        _MEMSET_( (void __FAR*) mId, '\0', sizeof(mId) );
        _MEMSET_( (void __FAR*) mi8Id, '\0', sizeof(mi8Id) );
        _MEMSET_( (void __FAR*) mi8Info, '\0', sizeof(mi8Info) );

        for ( mu8Info = 0 ; mu8Info < LENGTH_WIFI_ID_ONE ; mu8Info++ )
        {
            mi8Id[mu8Info] = pBuf[mu16InfoCount]; // { ~ }
            mu16InfoCount++;
            if ( mi8Id[mu8Info] == ASCII_COLON )
            {
                break;
            }
        }

        for ( mu8Info = 0 ; mu8Info < LENGTH_WIFI_DATA_ONE ; mu8Info++ )
        {
            mi8Info[mu8Info] = pBuf[mu16InfoCount]; // { ~ }
            mu16InfoCount++;
            if ( mi8Info[mu8Info] == ASCII_COMMA || mi8Info[mu8Info] == ASCII_CLOSE )
            {
                mi8Info[mu8Info] = ASCII_NULL;
                break;
            }
        }

        for ( mi = 0 ; mi < mu16Size; mi++ )
        {
            pFun = (pList+mi)->RxFunc;
            MakeCheckId( mId, 0, (pList+mi)->Id );        
        
            if ( _STRNCMP_( (const char __FAR*) mId, (const char __FAR*) mi8Id, LENGTH_WIFI_ID_ONE ) == N_TRUE ) // ID+:(5Byte) check
            {
                if ( pFun != NULL )
                {
                    pFun( (pList+mi)->Id, mi8Info );
                    break;
                }
            }
        }
    }
}

// Make Id (4Byte + 1Byte)
static void MakeCheckId ( I8* pBuf, U16 mLen, U16 mId )
{
    pBuf[mLen++] = WIFI_HEX2ASCII((U8)((mId >> 12) & 0x000F));
    pBuf[mLen++] = WIFI_HEX2ASCII((U8)((mId >> 8) & 0x000F));
    pBuf[mLen++] = WIFI_HEX2ASCII((U8)((mId >> 4) & 0x000F));
    pBuf[mLen++] = WIFI_HEX2ASCII((U8)((mId) & 0x000F));
    pBuf[mLen++] = ASCII_COLON; // 0x3A :
}

