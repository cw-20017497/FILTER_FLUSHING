
#ifndef _WIFI_SetCommon_H_
#define _WIFI_SetCommon_H_


U8 IsUserModelCHP ( void );

void ApplyFunctionData ( U16 mId, I8 *pBuf );

void ApplyInfoData ( U16 mId, I8 *pBuf);

void ConvertDataHexToAscii ( U8 mType, U16 mu16Id, I8 *pBuf );


#endif


