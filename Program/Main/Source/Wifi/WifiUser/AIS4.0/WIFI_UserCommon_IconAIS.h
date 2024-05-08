
#ifndef _WIFI_COMMON_BUILTIN_H_
#define _WIFI_COMMON_BUILTIN_H_

#ifdef ICON_AIS
/***** User headers ****************************************************************/
#include "mcu.h"
#include "r_cg_serial.h"
#include "WIFI_DefUser.h"


/***** Type Define *****************************************************************/
#if 0
typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned int        U16;
typedef signed int          I16;
typedef const signed char   CI8;
typedef const signed int    CI16;
typedef unsigned long int   U32;
typedef float               F32;
typedef double              D64;
#endif

/***** library headers ********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

/********** HIDDEN_BUILTIN **********/
#define MODEL_NO_1
#define WIFI_VERSION    "V1.0.0.12"
#define MODEL_NAME_CHP  "CHPI-7400N"
#define MODEL_NAME_CP   "CPI-7400N"

// USER WIFI Port & Register
#define R_TXD           TXD1
#define R_RXD           RXD1
#define R_STIF          STIF1
#define P_WIFI_ON       P0.4
#define P_WIFI_RSTN     P0.1

// USER Define (Number of User Data)
#define USER_DATA_MAX_NUM   38  // ����ϴ� ������ �� ����
#define USER_INFO_MAX_NUM   15  // ����ϴ� ���� �� ����
//#define USER_USE_ACCUMULATE   // ���������� ��뿩��
#define USER_USE_WEATHER      // ���������� ��뿩��
#define USER_USE_ACK

// USER Define (RomSize/ProductNum)
#define FOTA            1           // 0:Unused / 1:Used Fota
#define FOTA_SIZE       "1025"       // FotaSize = Rom Size * 4 + 1
#define FOTA_TYPE_CHP   "113315"    // Product Code Number
#define FOTA_TYPE_CP    "113316"    // Product Code Number

// USER Define (EEPROM Address)
#define EEPROM_ADDR_WIFI_POWER      0x005E   // WIFI On/Off Status
#define EEPROM_ADDR_FIRST_PARING    0x005F   // WIFI First Paring Status
#define EEPROM_ADDR_FOTA_MODULE     0x0060   // WIFI Module Fota Status
#define EEPROM_ADDR_FOTA_MCU        0x0061   // WIFI MCU Fota Status

// USER Define (Buzzer)
#define	WIFI_BUZZER_SELECT          0x01  // �� (������ "��")
#define	WIFI_BUZZER_AP_CONNECT      0x02  // AP���� (������ "�츮��" / �������� ������ �����մϴ�)
#define	WIFI_BUZZER_SETUP           0x03  // WIFI ON (������ "�򵿢�" / �������� ������ �����մϴ�)
#define	WIFI_BUZZER_SERVER_CONNECT  0x04  // �������� (������ "�츮����" / �������� ������ �Ϸ�Ǿ����ϴ�)
#define	WIFI_BUZZER_AP_FAIL         0x05  // ��30�� ��� (������ "����" / �������� ������ ���еǾ����ϴ�. ���ͳ� �����⸦ Ȯ�����ּ���. ������ �ݺ��Ǹ� �ݼ��Ϳ� �����ּ���.)
#define	WIFI_BUZZER_CANCEL          0x06  // WIFI OFF (������ "����" / �������� ������ �����Ǿ����ϴ�. �ٽ� �����Ͻ÷��� ��ǰ ���� ��忡�� �������̸� ���ּ���.)
#define WIFI_BUZZER_AP_START        0x07  // AP������ (������ "�� ��")
#define	WIFI_BUZZER_ERROR           0x08  // �Ұ��� (������ "������")
#define WIFI_BUZZER_SERVER_FIRST    0x09  // �������� (������ "�츮����" / �������� ������ �Ϸ�Ǿ����ϴ�. �������� ���̿��ɾ� ���� ���� ����Ʈ�� ������ ������ �� �ֽ��ϴ�.)
#define	WIFI_BUZZER_BLE_OFF         0x0A  // ������� �о ������(����)
#endif

#endif

