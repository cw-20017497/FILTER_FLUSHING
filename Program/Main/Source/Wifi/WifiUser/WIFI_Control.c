
/*******************************************************************************
&Description :
  제품에서 사용하는 WIFI 함수 File
  
&Histiry : 
  ShinHM 2021.07.09 update
*******************************************************************************/


/***** Header *****************************************************************/
#include "WIFI_Common.h"
#include "WIFI_Fota.h"
#include "WIFI_DataCheck.h"
#include "WIFI_RxProcess.h"
#include "WIFI_TxProtocol.h"
#include "WIFI_RxServer.h"
#include "WIFI_Status.h"
#include "WIFI_UartInterrupt.h"
#include "WIFI_UserInterface.h"


/***** Define *****************************************************************/


/***** Variable ***************************************************************/


/***** Constant ***************************************************************/


/***** Static Function ********************************************************/
static void InitializeWifiAllValue ( void );
static void WifiDualApModeKey ( void );
static void WifiApModekey ( void );
static void WifiApBLEkey ( void );
static void WifiOnOffKey ( void );
static void WifiTestKey ( void );
static void WifiOnKey ( void );
static void WifiOffKey ( void );


/***** Table ******************************************************************/


/***** Global Function ********************************************************/
void WifiControlProcess ( E_WIFI_TIME_T mu8Time )
{
    if( mu8Time == WIFI_TIME_INI ) /* 초기화 (EEPROM 이전) */
    {
        InitializeWifiAllValue();
    }
    else if ( mu8Time == WIFI_TIME_SET ) /* 초기셋팅 (EEPROM 이후 While 이전) */
    {
        WifiOnOffSetting();
    }
    else if ( mu8Time == WIFI_TIME_FACTORY ) /* 공장모드 진입 시 eep 데이터 초기화 */
    {
        WifiFactorySetting();
    }
    else if ( mu8Time == WIFI_TIME_WHILE )
    {
        WifiRxDataProcess();
        ReceiveWifiUart();
    }
    else // ( ( mu8Time >= WIFI_TIME_100MS ) && ( mu8Time <= WIFI_TIME_1SEC ) )
    {
        WifiDataCheckProcess(mu8Time);
    }
}

void WifiKey ( E_WIFI_KEY_T mu8Key )
{
    if ( mu8Key == WIFI_KEY_AP )
    {
        WifiApModekey();
    }
    else if ( mu8Key == WIFI_KEY_BLE )
    {
        WifiApBLEkey();
    }
    else if ( mu8Key == WIFI_KEY_ON )
    {
        WifiOnKey();
    }
    else if ( mu8Key == WIFI_KEY_OFF )
    {
        WifiOffKey();
    }
    else if ( mu8Key == WIFI_KEY_ONOFF )
    {
        WifiOnOffKey();
    }
    else if ( mu8Key == WIFI_KEY_TEST )
    {
        WifiTestKey();
    }
    else
    {
        
    }
}

void WifiSendData ( E_WIFI_DATA_T mu8Data )
{
    if ( GetWifiServerStatus(STATUS_SVR_CONNECT) != SET )
    { /* 서버연결 이전에는 동작 안함 */
        return;
    }
    
    if ( mu8Data == WIFI_DATA_FUNCTION )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_FUNC);
    }
    else if ( mu8Data == WIFI_DATA_SENSOR )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_SENSOR);
    }
    else if ( mu8Data == WIFI_DATA_ERROR )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_ERR);
    }
    else if ( mu8Data == WIFI_DATA_PART )
    {
        if ( GetUserInterface( USER_SMART_SENSING_STATUS ) != TRUE )
        {
            SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_PART);
        }
    }
    else if ( mu8Data == WIFI_DATA_EXAMINE )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_EXCEPT_FUNC);
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_EXAMINE);
    }
    else if ( mu8Data == WIFI_DATA_INFO )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_RQST_INFO_USER);
    }
}

U8 GetWifiStatusValue ( E_WIFI_STATUS_T mType )
{
    U8 mValue = 0U;
    U8 mApStep = 0U;

    if ( mType == WIFI_STATUS_DISP ) /* WIFI Display */
    {
        mApStep = GetWifiIndicateStatus(STATUS_PARING);
        if ( GetWifiApStatus(STATUS_AP_WIFI) != CLEAR )
        {
            if ( mApStep == WIFI_AP_STEP_WAIT )
            {
                mValue = WIFI_DISP_AP_STEP0;
            }
            else if ( mApStep == WIFI_AP_STEP_START )
            {
                mValue = WIFI_DISP_AP_STEP1;
            }
            else if ( mApStep == WIFI_AP_STEP_MODEM )
            {
                mValue = WIFI_DISP_AP_STEP2;
            }
            else if ( mApStep == WIFI_AP_STEP_ROOTCA )
            {
                mValue = WIFI_DISP_AP_STEP3;
            }
            else
            {
                mValue = WIFI_DISP_AP_START;
            }
        }
        else if ( GetWifiApStatus(STATUS_AP_BLE) != CLEAR )
        {
            if ( mApStep == WIFI_AP_STEP_WAIT )
            {
                mValue = WIFI_DISP_BLE_STEP0;
            }
            else if ( mApStep == WIFI_AP_STEP_START )
            {
                mValue = WIFI_DISP_BLE_STEP1;
            }
            else if ( mApStep == WIFI_AP_STEP_MODEM )
            {
                mValue = WIFI_DISP_BLE_STEP2;
            }
            else if ( mApStep == WIFI_AP_STEP_ROOTCA )
            {
                mValue = WIFI_DISP_BLE_STEP3;
            }
            else
            {
                mValue = WIFI_DISP_BLE_START;
            }
        }
        else
        {
            mValue = GetWifiIndicateStatus(STATUS_DISPLAY);
        }
    }
    else if ( mType == WIFI_STATUS_POWER ) /* WIFI 전원 */
    {
        mValue = GetWifiControlStatus(STATUS_CON_POWER);
    }
    else if ( mType == WIFI_STATUS_TEST ) /* 테스트모드 공유기연결 확인 (DISPLAY) */
    {
        mValue = GetWifiIndicateStatus(STATUS_FACTORY);
    }
    else if ( mType == WIFI_STATUS_ERROR ) /* WIFI Error */
    {
        mValue = GetWifiIndicateStatus(STATUS_ERROR);
    }
    else if ( mType == WIFI_STATUS_AP_FIRST )
    {
        mValue = GetWifiApStatus(STATUS_AP_FIRST);
    }
    else if ( mType == WIFI_STATUS_AP_MODE ) /* AP MODE 확인 (DISPLAY) */
    {
        if ( GetWifiApStatus(STATUS_AP_WIFI) != CLEAR )
        {
            mValue = SET;
        }
    }
    else if ( mType == WIFI_STATUS_AP_BLE ) /* BLE AP MODE 확인 (DISPLAY) */
    {
        if ( GetWifiApStatus(STATUS_AP_BLE) != CLEAR )
        {
            mValue = SET;
        }
    }
    else if ( mType == WIFI_STATUS_AP_STEP ) /* 페어링 스텝 확인 (DISPLAY) */
    {
        mValue = GetWifiIndicateStatus(STATUS_PARING);
    }  
    else if ( mType == WIFI_STATUS_AP_ERROR ) /* WIFI AP Mode Error */
    {
        if ( GetWifiIndicateStatus(STATUS_PARING) >= WIFI_AP_STEP_START )
        {
            mValue = GetWifiIndicateStatus(STATUS_AP_ERROR);
        }
    }
    
    return mValue;
}

U16 GetWifiRequestValue ( E_WIFI_RQST_T mId )
{
    U16 mValue = 0;
    mValue = GetWifiInfoData(mId);
    return mValue;
}

void InterruptUartWifiTx ( void )
{
    IntWifiUartTx();
}

void InterruptUartWifiRx ( U8 mu8RxData )
{
    IntWifiUartRx(mu8RxData);
}

void InterruptUartWifiError ( void )
{
    U8 rx_data = 0;
    volatile U8 err_type;

    rx_data = R_RXD;
    err_type = (U8)(SSR03 & 0x0007U);
    SIR03 = (U16)err_type;
}


/***** Local Function *********************************************************/
static void InitializeWifiAllValue ( void )
{
    InitializeWifiFota();
    InitializeWifiStatus();
    InitializeWifiRxProcess();
    InitializeWifiRxServer();
    InitializeWifiTxProtocol();
    InitializeWifiUart();
    InitializeWifiUserInterface();
}

static void WifiDualApModeKey ( void )
{
    static U8 mu8Dual = 0;
    /* BLE -> Off -> WiFi -> Off -> BLE... repeat */
    if ( GetWifiControlStatus(STATUS_CON_POWER) != ON )
    {
        if ( mu8Dual  == CLEAR )
        {
            mu8Dual = SET;
        }
        else
        {
            mu8Dual = CLEAR;
        }
    }

    if ( mu8Dual == SET )
    {
        WifiApModekey();
    }
    else
    {
        WifiApBLEkey();
    }

}

// WIFI AP MODE KEY
static void WifiApModekey ( void )
{
    if ( ( GetWifiApStatus(STATUS_AP_WIFI) != CLEAR ) || GetWifiApStatus(STATUS_AP_BLE) != CLEAR )
    { // AP MODE 에서는 Off
        WifiOffKey();
        return;
    }

    if ( GetWifiControlStatus(STATUS_CON_POWER) != ON )
    {
        HAL_SetWifiOnOff(ON);
        SetWifiApStatus(STATUS_AP_WIFI,RESET);
    }
    else
    {
        if ( GetWifiServerStatus(STATUS_SVR_CONNECT) == SET )
        { // AP접속 시 서버연결되어 있으면 종료신호 전송 후 AP 진입
            SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_STOP);
            SetWifiApStatus(STATUS_AP_WIFI,WAIT);
        }
        else
        {
            SetWifiApStatus(STATUS_AP_WIFI,RESET);
        }
    }
    ClearRxStatus();
    SetWifiIndicateStatus(STATUS_AP_ERROR,WIFI_DISP_OFF);
    SetWifiBuzzSound(WIFI_BUZZER_AP_START);
}

static void WifiApBLEkey ( void )
{
    if ( ( GetWifiApStatus(STATUS_AP_WIFI) != CLEAR ) || GetWifiApStatus(STATUS_AP_BLE) != CLEAR )
    { // AP MODE 에서는 Off
        if ( GetWifiServerStatus(STATUS_SVR_CONNECT) == SET )
        {
            SetWifiSendStatus(TX_ATCMD,WIFI_AT_BLE_OFF);
            SetWifiIndicateStatus(STATUS_PARING,WIFI_AP_STEP_CLEAR);
            SetWifiBuzzSound(WIFI_BUZZER_BLE_OFF);
        }
        else
        {
            WifiOffKey();
        }
        return;
    }
    
    if ( GetWifiControlStatus(STATUS_CON_POWER) != ON )
    {
        HAL_SetWifiOnOff(ON);
    }
    else
    {
        //SetWifiSendStatus(TX_ATCMD,WIFI_AT_BLE_ADV);
    }
    SetWifiApStatus(STATUS_AP_BLE,WAIT);
    SetWifiBuzzSound(WIFI_BUZZER_AP_START);
}

// WIFI ON/OFF KEY
static void WifiOnOffKey ( void )
{
    if ( GetWifiControlStatus(STATUS_CON_POWER) != ON )
    {
        WifiOnKey();
    }
    else
    {
        WifiOffKey();
    }
}

// Test Key Operation
static void WifiTestKey ( void )
{
    HAL_SetWifiOnOff(ON);
    SetWifiBuzzSound(WIFI_BUZZER_SELECT);
}

// On Key Operation
static void WifiOnKey ( void )
{
    if ( GetWifiControlStatus(STATUS_CON_POWER) == ON )
    {
        SetWifiBuzzSound(WIFI_BUZZER_ERROR);
        return;
    }
    
    if ( ( GetUserInterface(USER_FACTORY_STATUS) == SET ) || ( GetWifiApStatus(STATUS_AP_FIRST) == SET ) )
    {
        HAL_SetWifiOnOff(ON);
        WifiWriteEep(EEPROM_ADDR_WIFI_POWER, ON);
        SetWifiBuzzSound(WIFI_BUZZER_SETUP);
    }
    else
    {
        SetWifiBuzzSound(WIFI_BUZZER_ERROR);
    }
    ClearRxStatus();
}

// Off Key Operation
static void WifiOffKey ( void )
{
    if ( GetWifiControlStatus(STATUS_CON_POWER) == OFF )
    {
        SetWifiBuzzSound(WIFI_BUZZER_ERROR);
        return;
    }
    
    if ( GetWifiServerStatus(STATUS_SVR_CONNECT) == SET )
    { // 종료신호 송신 이후 일정시간 지나면 종료동작
        SetWifiServerStatus(STATUS_SVR_END,SET);
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_STOP);
    }
    else
    {
        HAL_SetWifiOnOff(OFF);
    }
    SetWifiBuzzSound(WIFI_BUZZER_CANCEL);
    ClearRxStatus();
}

