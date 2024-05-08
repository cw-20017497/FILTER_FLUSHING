
#ifndef _WIFI_RxProcess_H_
#define _WIFI_RxProcess_H_

/*******************************************************************************
FUNCTION NAME: InitializeWifiProcessRx

DESCRIPTION: Initialize Rx Value

PARAMETERS: X

RETURN VALUE: X

NOTES: $)CCJ1b 1H8 ( Control )
*******************************************************************************/
void InitializeWifiRxProcess ( void );

/*******************************************************************************
FUNCTION NAME: GetWifiUartRxData

DESCRIPTION: Get uart Rx Data

PARAMETERS: mai8UartData

RETURN VALUE: X

NOTES: Int $)CH#Cb ( UartInterrupt )
*******************************************************************************/
void GetWifiUartRxData ( I8* mai8UartData );

/*******************************************************************************
FUNCTION NAME: WifiRxDataProcess

DESCRIPTION: Rx Data Process

PARAMETERS: X

RETURN VALUE: X

NOTES: While ( Control )
*******************************************************************************/
void WifiRxDataProcess ( void );

/*******************************************************************************
FUNCTION NAME: WifiRxAckTimer

DESCRIPTION: Module Connect Check

PARAMETERS: X

RETURN VALUE: X

NOTES: 100ms ( DataCheck )
*******************************************************************************/
void WifiRxAckTimer ( void );

void ClearRxStatus ( void );

#endif

