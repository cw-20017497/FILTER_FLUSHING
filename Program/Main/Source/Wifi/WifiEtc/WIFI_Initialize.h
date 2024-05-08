
#ifndef _WIFI_Initialize_H_
#define _WIFI_Initialize_H_

/*******************************************************************************
FUNCTION NAME: WifiStartProcess

DESCRIPTION: WIFI 전원 ON 이후 서버연결 완료 시 초기 데이터 서버 전송

PARAMETERS: X

RETURN VALUE: X

NOTES: 100ms ( DataCheck )
*******************************************************************************/
void WifiStartProcess ( void );

/*******************************************************************************
FUNCTION NAME: WifiCheckInitializeAck

DESCRIPTION: 초기데이터 전송 ACK 확인

PARAMETERS: X

RETURN VALUE: X

NOTES: 호출 ( ProcessRx )
*******************************************************************************/
void WifiCheckInitializeAck ( void );

void WifiCheckInitializeInfoAck ( void );
#endif

