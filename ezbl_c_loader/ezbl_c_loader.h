#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <Windows.h>
#include <stdint.h>


#pragma pack(1) 
typedef struct BL2_FILE_FORMAT_HEADER
{
	uint8_t SyncData[16];
	uint32_t FILE_ID;
	uint32_t FILE_LEN;
	uint8_t  BOODID_HASH[16];
	uint32_t  APPID_VER_BUILD;
	uint16_t APPID_VER_MINOR;
	uint16_t  APPID_VER_MAJOR;
	uint8_t HMAC_SHA_256[16];
	uint32_t REC_LEN;
	uint32_t REC_ADDR;

} BL2_FILE_FORMAT_HEADER;

#pragma pack(1) 
typedef struct BL2_FILE_FORMAT_DATA
{
	uint8_t *Data;
	uint32_t dataSize;
	uint8_t FILE_HASH[32];
	uint32_t CRC32;

}  BL2_FILE_FORMAT_DATA;

#pragma pack(1) 
typedef struct BL2_FILE_FORMAT
{
	BL2_FILE_FORMAT_HEADER bl2H;
	BL2_FILE_FORMAT_DATA bl2D;

}BL2_FILE_FORMAT;


typedef enum EZBL_Errors
{
	EZBL_OK = 1,
	ERROR_OPEN_FILE,
	ERROR_NOT_BL2_FILE,
	ERROR_NO_DATA_EXIST,
	ERROR_INVALID_FILE_ID

} EZBL_Errors;


int UART_InitializeCOMPort(TCHAR *comFile, int comBaudRate,TCHAR *errorMsg,DWORD errorMsgSize);
void UART_Write(uint8_t *data, uint32_t size);
void UART_Read(uint8_t *data, uint32_t size);
EZBL_Errors BLOB_Read(char *fileName);