#include <stdio.h>
#include <stdlib.h>
#include "ezbl_c_loader.h"
#include <Windows.h>




int main(int argc, char* argv[])
{
	TCHAR errorMsg[100];

	if (UART_InitializeCOMPort("\\\\.\\COM24",
		115200,
		errorMsg,
		sizeof(errorMsg)) == 0)
	{
		printf("Error open com port\n");
		return 1;
	}

	BL2_FILE_FORMAT blob;
	char fileName[200] = "D:\\TG\\merge\\ezbl-v2.04_PIC24HJ64GP510A\\mux_evb_bootloader\\dist\\uart\\production\\mux_evb_bootloader.production.bl2";
	uint16_t response;
	if (BLOB_Read(fileName) == 0)
		return 0;

	
	GetBlob(&blob);

	UART_Write((uint8_t *)&blob.bl2H, 64);

	/*
	Phase 2: Accept / Erase
	If the.bl2 file offering is deemed by the Bootloader to be valid for programming, the bootloader will respond with
	a 16 - bit, byte little - endian keep - alive flow control advertisement to signify that the host has touched a wanting
	recipient and should continue to transmit the file.The value of this special keep - alive message is 0xFFF0 (-16).
	*/

	UART_Read(&response, sizeof(response)); // 2 bytes
	if (response != 0xFFF0)
	{
		printf("error in header response\n");
		return 0;
	}

	// here they said just to read for ack because it is going to erase
	UART_Write((uint8_t *)&blob.bl2H, 64);

	UART_Write((uint8_t *)&blob.bl2H.REC_LEN, sizeof(blob.bl2H.REC_LEN));
	UART_Write((uint8_t *)&blob.bl2H.REC_ADDR, sizeof(blob.bl2H.REC_ADDR));
	
	for (int i = 0; i < blob.bl2D.dataSize; i++)
	{
		UART_Write((uint8_t *)&blob.bl2D.Data[i], 1);
	}
	UART_Write((uint8_t *)blob.bl2D.FILE_HASH, 32);
	UART_Write((uint8_t *)&blob.bl2D.CRC32, 4);
	
	UART_Read(&response, sizeof(response));  
	if (response != 0x4)
	{
		printf("loading error\n");
		return 0;
	}

	return 1;
}

