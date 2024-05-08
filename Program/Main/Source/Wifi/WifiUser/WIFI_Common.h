
#ifndef _WIFI_COMMON_H_
#define _WIFI_COMMON_H_

#define ICON_AIS
//#define HIDDEN_BUILTIN
//#define HIDDEN_PRO
//#define HIDDEN_WIDTH  // 세로
//#define HIDDEN_LENGTH // 가로

#ifdef ICON_AIS
#include "WIFI_UserCommon_IconAIS.h"
#endif

#ifdef HIDDEN_BUILTIN
#include "WIFI_UserCommon_BuiltIn.h"
#endif

#ifdef HIDDEN_PRO
#include "WIFI_UserCommon_Pro.h"
#endif

#ifdef HIDDEN_WIDTH
#include "WIFI_UserCommon_Hidden.h"
#endif

#ifdef HIDDEN_LENGTH
#include "WIFI_UserCommon_Hidden.h"
#endif

//#include "WIFI_DefUser.h"

// Test AT CMD : at *ict*sconn="id" "pw"
// IoT_3Lv_LG2_2G / @iot13-forever@
// B1 OTA TEST : at *ict*sconn="IQ0F1CA0264"
// Factory : coway_iot_01 / 12345678

//#define ICON_MAX        MODEL_NO_1
//#define HIDDEN_BUILTIN  MODEL_NO_2
//#define HIDDEN_PRO      MODEL_NO_3
//#define HIDDEN_WIDTH    MODEL_NO_4
//#define HIDDEN_LENGTH   MODEL_NO_5

/***** USER Setting Start *****************************************************/
/* USER Test Config */
#define TEST_B1_OTA     0 // 0:Normal / 1:OTAtest
#define TEST_WIFI_PJT   0 // 0:Normal / 1:Technology Project
#define TEST_MODEM_SSID "ssinissi"//"IQ0F1CA0264"
#define TEST_MODEM_PW   " 1234567890"

/***** Define *****************************************************************/
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef N_TRUE
#define N_TRUE 0
#endif

#ifndef N_FALSE
#define N_FALSE 1
#endif

#ifndef SET
#define SET 1
#endif

#ifndef CLEAR
#define CLEAR 0
#endif

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

#define W_COUNT_WIFI_POWER    100 // 100ms*100 전원 인가 후 안정화

#define CONCAT(a, b) a##b

#define TURN_ON_WIFI_POW()  do{ P_WIFI_ON = ON; }while(0)
#define TURN_OFF_WIFI_POW() do{ P_WIFI_ON = OFF; }while(0)
#define TURN_ON_WIFI_RST()  do{ P_WIFI_RSTN = ON; }while(0)
#define TURN_OFF_WIFI_RST() do{ P_WIFI_RSTN = OFF; }while(0)

/* Data Length */
#define LENGTH_MODEL_NAME       13
#define LENGTH_MODEL_TYPE       6
#define LENGTH_API_NO           5
#define LENGTH_AT_CMD_MAX       50
#define LENGTH_REQUEST_ID       16  // Request ID Byte
#define LENGTH_WIFI_ID_ONE      5	// OneID+: Max Byte
#define LENGTH_WIFI_DATA_ONE    7	// OneData Max Byte
#define LENGTH_WIFI_DATA_ID     13	// ID+OneData Total Max Byte
#define LENGTH_WIFI_INFO_MAX    (USER_INFO_MAX_NUM*LENGTH_WIFI_DATA_ID)	// ID Num * One ID Max 13byte
#define LENGTH_WIFI_DATA_MAX    (USER_DATA_MAX_NUM*LENGTH_WIFI_DATA_ID)	// ID Num * One ID Max 13byte
#define LENGTH_WIFI_ALL_MAX     (LENGTH_WIFI_DATA_MAX+100)	// All Data Max Byte (include of At Cmd, Len, ...)

/* ASCII */
#define ASCII_STX       0x02
#define ASCII_ETX       0x03
#define ASCII_STAR      '*'
#define ASCII_QUOTE     '"'     // 0x22 // "
#define ASCII_COLON     ':'     // 0x3A // :
#define ASCII_COMMA     ','     // 0x2C // ,
#define ASCII_OPEN      '{'     // 0x7B	// {
#define ASCII_CLOSE     '}'     // 0x7D	// }
#define ASCII_DOT       '.'     // 0x2E	// .
#define ASCII_MINUS     '-'     // 0x2D // -
#define ASCII_NULL      '\0'    // 0x00 // null
#define ASCII_DONE      0x0D    // <0x0D> Data Done
#define ASCII_ATDONE    0x0A    // <0x0A> AT CMD Done
#define ASCII_SPACE     ' '     // 0x20
#define ASCII_UNDERBAR  '_'     //
#define ASCII_NO_1      '1'     // 0x31

/* Etc */
#define COWAY_SSID      "Coway_"
#define WIFI_NULL       0xFF


enum
{
    //CLEAR = 0x00,
    //SET,
    RESET = 0x02,
    WAIT,
    START,
    RETRY
};

enum
{
    UNUSED = 0x00,
    USED,
    REUSED
};

typedef enum
{
    USER_MODEL_CHP = 0x00,
    USER_MODEL_CP,
    USER_MODEL_CHP_1,
    USER_MODEL_CP_1
} E_USER_MODEL_T;


typedef enum
{
    USER_MODEL_TYPE = 0x00,
    USER_FACTORY_STATUS,
    USER_RX_STATUS,
    USER_SMART_POSSIBLE_STATUS,
    USER_ERROR_STATUS,
    USER_FOTA_POSSIBLE_STATUS,
    USER_SMART_SENSING_STATUS
} E_USER_VALUE_T;

typedef enum
{
    USER_DISPLAY_FOTA = 0x00,
    USER_DISPLAY_SELFCHECK,
    USER_DISPLAY_PARTCHECK
} E_USER_DISPLAY_T;

typedef enum
{
    TO_SERVER = 0x00,
    TO_MODULE,
    TO_APMODE,
    TO_VERSION,
    TO_SCONN,
    TO_BLEADV
} E_TARGET_EXCEPT_T;


typedef void (*WifiTxFun_T)(U16 mId, I8 *pBuf);
typedef void (*WifiRxFun_T)(U16 mId, I8 *pBuf);
typedef struct _wifi_tx_list_
{
    U16 Id;
    WifiTxFun_T TxFunc;
    WifiRxFun_T RxFunc;
} WifiTxFuncList_T;
typedef void (*WifiExFun_T)( U8 mExcept, I8* pBuf, U16 mLen );

#if ( FOTA == 1 )
#ifndef __FAR
#define __FAR       __far
#endif

#ifndef _MEMSET_
#define _MEMSET_    memset_f    //memset_f( void __far *a, int b, size_t n );
#endif

#ifndef _MEMCPY_
#define _MEMCPY_    memcpy_f    //memcpy_f( void __far *a, const void __far *b, size_t n );
#endif

#ifndef _MEMMOVE_
#define _MEMMOVE_   memmove_f   //memmove_f( void __far *a, const void __far *b, size_t n );
#endif

#ifndef _MEMCMP_
#define _MEMCMP_    memcmp_f    //memcmp_f( const void __far *a, const void __far *b, size_t n );
#endif

#ifndef _STRCPY_
#define _STRCPY_    strcpy_f    //strcpy_f( char __far *a, const char __far *b );
#endif

#ifndef _STRNCPY_
#define _STRNCPY_   strncpy_f   //strncpy_f( char __far *a, const char __far *b, size_t n );
#endif

#ifndef _STRCMP_
#define _STRCMP_    strcmp_f    //strcmp_f( const char __far *a, const char __far *b );
#endif

#ifndef _STRNCMP_
#define _STRNCMP_   strncmp_f   //strncmp_f( const char __far *a, const char __far *b, size_t n );
#endif

#ifndef _STRLEN_
#define _STRLEN_    strlen_f    //strlen_f( const char __far *a );
#endif

#ifndef _SPRINTF_
#define _SPRINTF_   sprintf_f   //sprintf_f( char __far *a, const char __far *format, char __far *b );
#endif

#ifndef _ATOI_
#define _ATOI_      atoi_f      //atoi_f( const char __far *a );
#endif

#else
#define __FAR       
#define _MEMSET_    memset      //memset( void *a, int b, size_t n );
#define _MEMCPY_    memcpy      //memcpy( void *a, const void *b, size_t n );
#define _MEMMOVE_   memmove     //memmove( void *a, const void *b, size_t n );
#define _MEMCMP_    memcmp      //memcmp( const void *a, const void *b, size_t n );
#define _STRCPY_    strcpy      //strcpy( char *a, const char *b );
#define _STRNCPY_   strncpy     //strncpy( char *a, const char *b, size_t n );
#define _STRCMP_    strcmp      //strcmp( const char *a, const char *b );
#define _STRNCMP_   strncmp     //strncmp( const char *a, const char *b, size_t n );
#define _STRLEN_    strlen      //strlen( const char *a );
#define _SPRINTF_   sprintf     //sprintf( char *a, const char *f, char *b );
#define _ATOI_      atoi        //atoi( const char *a );

#endif

/***** Function ***************************************************************/
U8 WIFI_HEX2DEC ( U8 mu8HexData );
U8 WIFI_DEC2HEX ( U8 mu8DecData );
U16 WIFI_Bit16HEX2DEC ( U16 mu16HexData );
U16 WIFI_DEC2Bit16HEX ( U16 mu16DecData );
U8 WIFI_HEX2ASCII ( U8 mu8HexData );
U8 WIFI_ASCII2HEX ( U8 mu8AsciiData );
U16 far_sprintf_s ( I8* pBuf, U16 mLen, I8* pData );

#endif
