
/*******************************************************************************
&Description :
    Make Tx Data File (Function/Error/Sensor/Para/Part Data)
    MQTT Protocol (Message Queuing Telemetry Transport)
    JSON Format (JavaScript Object Notation)
    
&Histiry : 
    ShinHM 2021.2.22 update
    -. Organize
*******************************************************************************/


/***** Header *****************************************************************/
#include "WIFI_Common.h"
#include "WIFI_TxMake.h"
#include "WIFI_Status.h"
#include "WIFI_TxForm.h"
#include "WIFI_TxProtocol.h"

#include "WIFI_SetCommon.h"
#include "WIFI_SetErrorData.h"
#include "WIFI_SetFunctionData.h"
#include "WIFI_SetSensorData.h"
#include "WIFI_SetPartData.h"
#include "WIFI_SetInformationData.h"
#include "WIFI_SetAccumulateData.h"


/***** Define *****************************************************************/


/***** Variable ***************************************************************/


/***** Constant ***************************************************************/


/***** Static Function ********************************************************/
static U8 SelectTxType ( void );
static void MakeTxData ( U8 mu8TxPara );
static U16 MakeTagId ( I8 *pBuf, U16 mLen, U16 mId );
static U16 MakeTagData ( I8 *pBuf, U16 mLen, I8 *pData );
static U16 DeleteComma ( I8 *pBuf, U16 mLen );
static void* GetCmdList ( E_WIFI_TX_TYPE_T mType );
static U16 GetCmdSize ( E_WIFI_TX_TYPE_T mType );


/***** Table ******************************************************************/


/***** Global Function ********************************************************/
void WifiTxDataSelect ( void )
{
    WifiTxTypeList_T *pList = NULL;
    U8 i = 0U;
    U8 mSize = 0U;
    U8 mCmd = 0U;
    U32 mData = 0UL;

    /* Data Type Select */
    mCmd = SelectTxType();
    mData = GetWifiSendStatus(mCmd);
    
    /* Get Type List */
    pList = (WifiTxTypeList_T *)GetWifiTxTypeEventList(mCmd);
    mSize = (U8)GetWifiTxTypeListSize(mCmd);

    /* Make Tx Data */
    for ( i = 0 ; i < mSize; i++ )
    {
        U32 mId = 0UL;
    	U32 mCheck = 0UL;
        mId = (pList+i)->Id;
        mCheck = (mData&mId);
        if ( mCheck == mId )
        {
            if ( (pList+i)->mu8Seq == WIFI_TX_SEQ_SEND )
            {
                SetWifiTxStatus( STATUS_TX_STATE, SET );
            }
            SetWifiTxStatus( STATUS_TX_SEQ, (pList+i)->mu8Seq );
            MakeTxData( (pList+i)->mu8Para );
            ClearWifiSendStatus( mCmd, mId );
            SetWifiTxStatus( STATUS_TX_SEQ, WIFI_TX_SEQ_NULL );
            break;
        }
    }
}


/***** Local Function *********************************************************/
static U8 SelectTxType ( void )
{
    U8 mCmd = 0U;
    U32 mData = 0UL;
    
    mData = GetWifiSendStatus( TX_ATCMD );
    if ( mData == 0 )
    {
        mCmd = TX_DATA;
    }
    else
    {
        mCmd = TX_ATCMD;
    }
    return mCmd;
}

/* Make Tx Data */
static void MakeTxData ( U8 mu8TxPara )
{
    WifiTxCommandList_T *pCmdList = NULL;
    U16 mu16CmdSize = 0U;
    U8 i = 0U;
    E_WIFI_TX_TYPE_T mType = 0U;
    U8 mu8Target = 0U;
    U8 mu8RequestId = 0U;
    WifiTxFuncList_T *pList = NULL;
    U16 mu16Size = 0U;
    U16 mu16Len = 0U;    // 데이터 버퍼 길이
    static I8 mai8AtCmd[LENGTH_AT_CMD_MAX];
    static I8 mai8Api[LENGTH_API_NO+1];
    static I8 mai8GetDataBuf[LENGTH_WIFI_DATA_ONE];	// 하나의 데이터 값 버퍼
    static I8 mai8DataBuff[LENGTH_WIFI_DATA_MAX];   // 아스키로 변환된 최종데이터 버퍼

    _MEMSET_( (void __FAR*) mai8AtCmd, '\0', sizeof(mai8AtCmd) );
    _MEMSET_( (void __FAR*) mai8Api, '\0', sizeof(mai8Api) );
    _MEMSET_( (void __FAR*) mai8GetDataBuf, '\0', sizeof(mai8GetDataBuf) );
    _MEMSET_( (void __FAR*) mai8DataBuff, '\0', sizeof(mai8DataBuff) );
    
    /* Data Matching (Tx데이터 Parameter에 따라 Data table Matching) */
    pCmdList = (WifiTxCommandList_T *)GetWifiTxCmdEventList();
    mu16CmdSize = GetWifiTxCmdListSize();
    for ( i = 0 ; i < mu16CmdSize; i++ )
    {
        if ( mu8TxPara == (pCmdList+i)->Para )
    	{
    	    mType = (pCmdList+i)->Type;
            mu8Target = (pCmdList+i)->Target;
            mu8RequestId = (pCmdList+i)->ReId;
            far_sprintf_s( mai8AtCmd, 0, (pCmdList+i)->AtCmd );
            far_sprintf_s( mai8Api, 0, (pCmdList+i)->Api );
            break;
    	}
    }
    
    /* Get Data List */
    pList = GetCmdList( mType );
    mu16Size = GetCmdSize( mType );

    /* Make Tag List Id & Data */
    if ( pList != NULL )
    {
        for ( i = 0 ; i < mu16Size; i++ )
        {
            WifiTxFun_T pFun;
            pFun = (pList+i)->TxFunc;
            if ( pFun != NULL )
            {
                U16 mId = 0U;
                _MEMSET_( (void __FAR*) mai8GetDataBuf, '\0', sizeof(mai8GetDataBuf) );
                mId = (pList+i)->Id;
                pFun( mId, mai8GetDataBuf );
                mu16Len = MakeTagId( mai8DataBuff, mu16Len, mId );
                mu16Len = MakeTagData( mai8DataBuff, mu16Len, mai8GetDataBuf );
            }
        }
    }
    
    /* Delete Comma */
    mu16Len = DeleteComma( mai8DataBuff, mu16Len );
    
    /* Send Protocol ( Target / ReId Usage Status / DataBuf / AT Cmd / API ) */
    WifiSendProtocol( mu8Target, mu8RequestId, mai8DataBuff, mai8AtCmd, mai8Api );
}

/* Make Tag Id (4Byte + 1Byte) */
static U16 MakeTagId ( I8* pBuf, U16 mLen, U16 mId )
{
    pBuf[mLen++] = WIFI_HEX2ASCII((U8)((mId >> 12) & 0x000F));
    pBuf[mLen++] = WIFI_HEX2ASCII((U8)((mId >> 8) & 0x000F));
    pBuf[mLen++] = WIFI_HEX2ASCII((U8)((mId >> 4) & 0x000F));
    pBuf[mLen++] = WIFI_HEX2ASCII((U8)((mId) & 0x000F));
    pBuf[mLen++] = ASCII_COLON; // 0x3A :
    return mLen;
}

/* Make Tag Data (Max7Byte + 1Byte) */
static U16 MakeTagData ( I8 *pBuf, U16 mLen, I8 *pData )
{
    mLen += far_sprintf_s( &pBuf[mLen], mLen, pData );
    pBuf[mLen++] = ASCII_COMMA; // 0x2C ,
    return mLen;
}

/* Delete Comma */
static U16 DeleteComma ( I8 *pBuf, U16 mLen )
{
    if ( mLen == 0 )
    {
        return;
    }
    if ( pBuf[mLen-1] == ASCII_COMMA )
    {
        mLen = mLen-1;
        pBuf[mLen] = ASCII_NULL; // 'null'
    }
    return mLen;
}

static void* GetCmdList ( E_WIFI_TX_TYPE_T mType )
{
    WifiTxFuncList_T *pList = NULL;
    if ( mType == TYPE_FUNC_CMD )
    {
    	pList = (WifiTxFuncList_T *)GetWifiFuncEventList();
        //mu16Size = GetWifiFuncListSize();
    }
    else if ( mType == TYPE_SENSOR_CMD )
    {
    	pList = (WifiTxFuncList_T *)GetWifiSensorEventList();
        //mu16Size = GetWifiSensorListSize();
    }
    else if ( mType == TYPE_ERROR_CMD )
    {
    	pList = (WifiTxFuncList_T *)GetWifiErrorEventList();
        //mu16Size = GetWifiErrorListSize();
    }
    else if ( mType == TYPE_PART_CMD )
    {
    	pList = (WifiTxFuncList_T *)GetWifiPartEventList();
        //mu16Size = GetWifiPartListSize();
    }
    else if ( mType == TYPE_ACCUMULATE_CMD )
    {
        pList = (WifiTxFuncList_T *)GetWifiAccumulateEventList();
        //mu16Size = GetWifiAccumulateListSize();
    }
    else if ( mType == TYPE_INFO_RQST_CMD )
    {
        pList = (WifiTxFuncList_T *)GetWifiInformationEventList();
        //mu16Size = GetWifiInformationListSize();
    }
    else if ( mType == TYPE_INFO_RQST_USER_CMD )
    {
        pList = (WifiTxFuncList_T *)GetWifiInfoUserEventList();
    }
    else if ( mType == TYPE_INFO_RQST_DATA_CMD )
    {
        pList = (WifiTxFuncList_T *)GetWifiInfoDataEventList();
    }
    else
    {
        pList = NULL;
        //mu16Size = 0;
    }
    return pList;
}

static U16 GetCmdSize ( E_WIFI_TX_TYPE_T mType )
{
    U16 mSize = 0U;
    if ( mType == TYPE_FUNC_CMD )
    {
        mSize = GetWifiFuncListSize();
    }
    else if ( mType == TYPE_SENSOR_CMD )
    {
        mSize = GetWifiSensorListSize();
    }
    else if ( mType == TYPE_ERROR_CMD )
    {
        mSize = GetWifiErrorListSize();
    }
    else if ( mType == TYPE_PART_CMD )
    {
        mSize = GetWifiPartListSize();
    }
    else if ( mType == TYPE_ACCUMULATE_CMD )
    {
        mSize = GetWifiAccumulateListSize();
    }
    else if ( mType == TYPE_INFO_RQST_CMD )
    {
        mSize = GetWifiInformationListSize();
    }
    else if ( mType == TYPE_INFO_RQST_USER_CMD )
    {
        mSize = GetWifiInformationListSize();
    }
    else if ( mType == TYPE_INFO_RQST_DATA_CMD )
    {
        mSize = GetWifiInformationListSize();
    }
    else
    {
        mSize = 0U;
    }
    return mSize;
}


