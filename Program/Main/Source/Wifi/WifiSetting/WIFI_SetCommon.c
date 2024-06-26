
/*******************************************************************************
&Description :
    Convert Data
    Apply Data
  
&Histiry : 
    ShinHM 2021.04.14 update
*******************************************************************************/


/******************************************************************************/
/***** Header *****************************************************************/
/******************************************************************************/
#include "WIFI_Common.h"
#include "WIFI_SetCommon.h"
#include "WIFI_TxForm.h"
#include "WIFI_UserInterface.h"
#include "WIFI_SetFunctionData.h"
#include "WIFI_SetInformationData.h"
#include "WIFI_SetSensorData.h"
#include "WIFI_RxServer.h"


static U32 GetData ( U8 mType, U16 mId );
static I16 GetNegativeData ( U8 mType, U16 mId );
static void SetUserInfoDataI8 ( U16 mId, I8 mInfoData );
static void SetUserInfoDataU16 ( U16 mId, U16 mData );
static void MakeNegativeData ( I16 mNegaData );
static void MakePointData ( I16 mPointData );
static void MakeData ( U32 mData );


I8 maiDataBuf[LENGTH_WIFI_DATA_ONE];



/******************************************************************************/
/***** Global Function ********************************************************/
/******************************************************************************/
// Check Model
U8 IsUserModelCHP ( void )
{
    if ( GetUserInterface( USER_MODEL_TYPE ) == USER_MODEL_CHP )
    {
        return TRUE; // CHP
    }
    return FALSE; // CP
}

// Apply App Control
void ApplyFunctionData ( U16 mId, I8 *pBuf )
{
    U8 mu8Size = 0U;
    U8 i = 0U;
    U8 mu8Buf = 0U;
    U8 mu8Data = 0U;    
    U16 mData = 0U;

    mu8Size = (U8)_STRLEN_( (const char __FAR*) pBuf );

    for ( i = 0 ; i < mu8Size ; i++ )
    {
        mu8Buf = pBuf[i];
        mu8Buf = (U8)(mu8Buf & 0x0F);
        mu8Data = WIFI_ASCII2HEX(mu8Buf);
        mData = (mData*10);
        mData = mData + ((U16)(mu8Data));
    }
    SetUserFunctionData( mId, mData );
}

// Apply Server Data (Time/Weather...)
void ApplyInfoData ( U16 mId, I8 *pBuf)
{
    U16 mu16InfoData = 0U;
    I8 mi8InfoData = 0;
    U8 mu8Size = 0U;
    U8 mCheck = 0U;
    U8 i = 0U;

    // Size $)CH.@N & >g<vorA$<v H.@N
    mu8Size = (U8)_STRLEN_( (const char __FAR*) pBuf );
    mCheck = IsPositiveData( mId );
    
    if ( mCheck == TRUE ) // positive
    {
        U8 mu8Data = 0;
        
        for ( i = 0 ; i < mu8Size ; i++ )
        {
            mu8Data = pBuf[i];
            mu8Data = WIFI_ASCII2HEX((U8)(mu8Data & 0x0F));
            mu16InfoData = (mu16InfoData*10);
            mu16InfoData = mu16InfoData + ((U16)(mu8Data));
        }
        SetUserInfoDataU16( mId, mu16InfoData );
    }
    else // negative
    {
        I8 mi8Data = 0;
        I8 mi8Info = 0;

        if ( pBuf[0] == ASCII_MINUS )
        {
            for ( i = 1 ; i < mu8Size ; i++ )
            {
                mi8Data = pBuf[i];
                mi8Data = WIFI_ASCII2HEX((U8)(mi8Data & 0x0F));
                mi8Info = (I8)(mi8InfoData*10);
                mi8InfoData = (I8)(mi8Info + mi8Data);
            }
            mi8InfoData = (I8)((mi8InfoData^0xFF)+1); // 2$)C@G :8<v
        }
        else
        {
            for ( i = 0 ; i < mu8Size ; i++ )
            {
                mi8Data = pBuf[i];
                mi8Data = WIFI_ASCII2HEX((U8)(mi8Data & 0x0F));
                mi8Info = (I8)(mi8InfoData*10);
                mi8InfoData = (I8)(mi8Info + mi8Data);
            }
        }
        SetUserInfoDataI8( mId, mi8InfoData );
    }
}

void ConvertDataHexToAscii ( U8 mType, U16 mId, I8 *pBuf )
{
    U32 mData = 0UL;
    I16 miData = 0;
    U8 mu8Data = 0U;
    U8 mCheck = 0U;
    //static I8 maiDataBuf[LENGTH_WIFI_DATA_ONE]; // $)C0mA$ 6@Z8. + "null"

    _MEMSET_( (void __FAR*) maiDataBuf, '\0', sizeof(maiDataBuf) );

    // Data $)C@T7B & <R<vorA$<v H.@N
    if ( mType == TYPE_SENSOR_CMD || mType == TYPE_PART_CMD )
    {
        mCheck = IsPointData( mType, mId );
        miData = GetNegativeData( mType, mId );
        if ( mCheck  == TRUE )
        { // $)C<R<v
            MakePointData( miData );
        }
        else
        { // $)CA$<v
            if ( miData >= 0 )
            { // 0 or $)C>g<v
                mData = (U32)(miData);
                MakeData( mData );
            }
            else
            { // $)C@=<v
                MakeNegativeData( miData );
            }
        }
    }
    else
    { // 0 or $)C>g<v
        mData = GetData( mType, mId );
        MakeData( mData );
    }

    // Data $)C@|4^
    far_sprintf_s( pBuf, 0, maiDataBuf );
    _MEMSET_( (void __FAR*) maiDataBuf, '\0', sizeof(maiDataBuf) );
}


/******************************************************************************/
/***** Local Function *********************************************************/
/******************************************************************************/
static U32 GetData ( U8 mType, U16 mId )
{
    U32 mData = 0UL;
    
    if ( mType == TYPE_FUNC_CMD )
    {
        mData = (U32)GetUserFunctionData( mId );
    }
    /*else if ( mType == TYPE_SENSOR_CMD )
    {
        mData = (U32)GetUserSensorData( mId );
    }*/
    else if ( mType == TYPE_ERROR_CMD )
    {
        mData = (U32)GetUserErrorData( mId );
    }
    else if ( mType == TYPE_PARA_CMD )
    {
        mData = 0UL;
    }
    else if ( mType == TYPE_PART_CMD )
    {
        //mData = (U32)GetUserPartData( mId );
    }
    else if ( mType == TYPE_ACCUMULATE_CMD )
    {
        mData = (U32)GetUserAccumulateData( mId );
    }
    else // ( mType == TYPE_INFO_RQST_CMD )
    {
        mData = 0UL;
    }
    
    return mData;
}

static I16 GetNegativeData ( U8 mType, U16 mId )
{
    I16 mData = 0;
    
    if ( mType == TYPE_SENSOR_CMD )
    {
        mData = (I16)GetUserSensorData( mId );
    }
    else if ( mType == TYPE_PART_CMD )
    {
        mData = (I16)GetUserPartData( mId );
    }
    else
    {
        mData = 0;
    }
    return mData;
}

static void SetUserInfoDataU16 ( U16 mId, U16 mInfoData )
{
    SetWifiInfoData(mId,mInfoData);
}

static void SetUserInfoDataI8 ( U16 mId, I8 mInfoData )
{
    U8 u8Temp = 0U;
    U16 u16Temp = 0U;
    
    u8Temp = (U8)mInfoData;
    u16Temp = (U16)u8Temp;
    SetWifiInfoData(mId,u16Temp);
}

static void MakeNegativeData ( I16 mNegaData )
{
    U8 mu8Data = 0U;
    U16 mData = 0U;

    mData = (U16)(mNegaData-1);
    mData = (U16)(mData^0xFFFF);

    maiDataBuf[0] = ASCII_MINUS;
    if ( mData >= 10000 ) // 10000 ~ 99999
    {
        mu8Data = (U8)(mData/10000);
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%10000);
                    
        mu8Data = (U8)(mData/1000);
        maiDataBuf[2] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%1000);
                    
        mu8Data = (U8)(mData/100);
        maiDataBuf[3] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%100);
                    
        mu8Data = (U8)(mData/10);
        maiDataBuf[4] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
                    
        maiDataBuf[5] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else if ( mData >= 1000 ) // 1000 ~ 9999
    {
        mu8Data = (U8)(mData/1000);
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%1000);
                    
        mu8Data = (U8)(mData/100);
        maiDataBuf[2] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%100);
                    
        mu8Data = (U8)(mData/10);      
        maiDataBuf[3] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
                    
        maiDataBuf[4] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else if ( mData >= 100 )
    {
        mu8Data = (U8)(mData/100);
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%100);
                    
        mu8Data = (U8)(mData/10);
        maiDataBuf[2] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
                    
        maiDataBuf[3] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else if ( mData >= 10 )
    {
        mu8Data = (U8)(mData/10);
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
                    
        maiDataBuf[2] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else
    {
        mu8Data = (U8)(mData);
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
}

static void MakePointData ( I16 mPointData )
{
    U8 mu8Data = 0U;
    U16 mData = 0U;
    
    if ( mPointData >= 9999U )
    {
        mData = 9999U;
    }
    else
    {
        mData = (U16)mPointData;
    }
    
    if ( mData >= 1000U )
    {
        mu8Data = (U8)(mData/1000);
        maiDataBuf[0] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%1000);
        mu8Data = (U8)(mData/100);
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        maiDataBuf[2] = '.';
        mData = (mData%100);
        mu8Data = (U8)(mData/10);
        maiDataBuf[3] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
        maiDataBuf[4] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else if ( mData >= 100U )
    {
        mu8Data = (U8)(mData/100);
        maiDataBuf[0] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        maiDataBuf[1] = '.';
        mData = (mData%100);
        mu8Data = (U8)(mData/10);
        maiDataBuf[2] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
        maiDataBuf[3] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else if ( mData >= 10U )
    {
        maiDataBuf[0] = '0';
        maiDataBuf[1] = '.';
        mu8Data = (U8)(mData/10);
        maiDataBuf[2] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
        maiDataBuf[3] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else
    {
        maiDataBuf[0] = '0';
        maiDataBuf[1] = '.';
        maiDataBuf[2] = '0';
        maiDataBuf[3] = WIFI_HEX2ASCII((U8)(mData & 0x0F));
    }
}

// Water Product
#define W_DEFAULT_MCU_DATA 9999U
#define W_DEFAULT_SVR_DATA 65535U
static void MakeData ( U32 mPosiData )
{
    U8 mu8Data = 0U;
    U32 mData = 0UL;

    if ( mPosiData == W_DEFAULT_MCU_DATA )
    {
        mData = W_DEFAULT_SVR_DATA;
    }
    else
    {
    mData = mPosiData;
    }
    
    if ( mData >= 100000U ) // 100000 ~ 999999
    {
        mu8Data = (U8)(mData/100000);
        maiDataBuf[0] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%100000);

        mu8Data = (U8)(mData/10000);
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%10000);
                    
        mu8Data = (U8)(mData/1000);
        maiDataBuf[2] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%1000);
                    
        mu8Data = (U8)(mData/100);
        maiDataBuf[3] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%100);
                    
        mu8Data = (U8)(mData/10);
        maiDataBuf[4] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
                    
        maiDataBuf[5] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else if ( mData >= 10000U ) // 10000 ~ 99999
    {
        mu8Data = (U8)(mData/10000);
        maiDataBuf[0] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%10000);
                    
        mu8Data = (U8)(mData/1000);
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%1000);
                    
        mu8Data = (U8)(mData/100);
        maiDataBuf[2] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%100);
                    
        mu8Data = (U8)(mData/10);
        maiDataBuf[3] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
                    
        maiDataBuf[4] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else if ( mData >= 1000U ) // 1000 ~ 9999
    {
        mu8Data = (U8)(mData/1000);
        maiDataBuf[0] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%1000);
                    
        mu8Data = (U8)(mData/100);
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%100);
                    
        mu8Data = (U8)(mData/10);      
        maiDataBuf[2] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
                    
        maiDataBuf[3] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else if ( mData >= 100U )
    {
        mu8Data = (U8)(mData/100);
        maiDataBuf[0] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mData = (mData%100);
                    
        mu8Data = (U8)(mData/10);
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
                    
        maiDataBuf[2] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else if ( mData >= 10U )
    {
        mu8Data = (U8)(mData/10);
        maiDataBuf[0] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
        mu8Data = (U8)(mData%10);
                    
        maiDataBuf[1] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
    else
    {
        mu8Data = (U8)(mData);
        maiDataBuf[0] = WIFI_HEX2ASCII((U8)(mu8Data & 0x0F));
    }
}


