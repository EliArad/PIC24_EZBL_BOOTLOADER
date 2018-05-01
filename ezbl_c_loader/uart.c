#include "ezbl_c_loader.h"


static HANDLE hComm;
OVERLAPPED comEvent;	// Event object for use by WaitCommEvent() or ReadFile()/WriteFile() on hComm UART

int UART_InitializeCOMPort(TCHAR *comFile, 
							  int comBaudRate,
							  TCHAR *errorMsg, 
							  DWORD errorMsgSize)
{
	
	DWORD			dwSize;
	COMMCONFIG		comConfig;
	COMMTIMEOUTS	comTimeouts;
	DWORD			errorMsgGrowth;

	if ((errorMsg != NULL) && errorMsgSize)	// Null terminate for successful return/no error message written
	{
		errorMsg[0] = L'\0';
	}

	hComm = CreateFile(comFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (hComm == INVALID_HANDLE_VALUE)
	{
		printf(L"Could not open read/write handle to com port", GetLastError());
		return 0;
	}

	dwSize = sizeof(comConfig);
	if (GetCommConfig(hComm, &comConfig, &dwSize) == 0)
	{
		errorMsgGrowth = printf(L"Could not read com port configuration", GetLastError());
		errorMsg += errorMsgGrowth;
		errorMsgSize -= errorMsgGrowth;
	}

	comConfig.dcb.DCBlength = sizeof(comConfig.dcb);
	comConfig.dcb.BaudRate = comBaudRate;
	comConfig.dcb.fBinary = TRUE;
	comConfig.dcb.fParity = FALSE;		// Do not check and error on parity
	comConfig.dcb.fOutxCtsFlow = FALSE;	// Do not monitor CTS flow control
	comConfig.dcb.fOutxDsrFlow = FALSE; // Do not monitor DSR flow control
	comConfig.dcb.fDtrControl = DTR_CONTROL_ENABLE;	// Turn on DTR line and leave it asserted
	comConfig.dcb.fDsrSensitivity = FALSE;	// Always receive, independent of DSR state
	comConfig.dcb.fTXContinueOnXoff = TRUE;	//TRUE: transmission continues after the input buffer has come within XoffLim bytes of being full and the driver has transmitted the XoffChar character to stop receiving bytes
	comConfig.dcb.fOutX = FALSE;		// Do not enable XON/XOFF flow control to avoid remote receive buffer overflow
	comConfig.dcb.fInX = FALSE;			// Do not enable XON/XOFF flow control to avoid local receive buffer overflow
	comConfig.dcb.fErrorChar = FALSE;	// No character replacement on parity errors
	comConfig.dcb.fNull = FALSE;		// No NULL character discarding occurs
	comConfig.dcb.fRtsControl = RTS_CONTROL_ENABLE;	// Turn on Request-To-Send and leave it asserted
	comConfig.dcb.fAbortOnError = TRUE;	// Abort read/write on error and wait for ClearCommError to be called
	comConfig.dcb.fDummy2 = 0;			// Reserved
	comConfig.dcb.wReserved = 0;		// Reserved - must be 0
	comConfig.dcb.XonLim = 3;			// The bytes in uses in the input buffer before flow control is deactivated to allow transmission by the sender
	comConfig.dcb.XoffLim = 3;			// The free bytes left in the input before before flow control is activated to inhibit the sender
	comConfig.dcb.ByteSize = 8;
	comConfig.dcb.Parity = NOPARITY;
	comConfig.dcb.StopBits = ONESTOPBIT;
	comConfig.dcb.XonChar = 0x11;
	comConfig.dcb.XoffChar = 0x13;
	comConfig.dcb.ErrorChar = 0x00;
	comConfig.dcb.EofChar = 0x00;
	comConfig.dcb.EvtChar = 0x00;
	comConfig.dcb.wReserved1 = 0;

	if (SetCommConfig(hComm, &comConfig, sizeof(comConfig)) == 0)
	{
		errorMsgGrowth = printf(L"Could not set com port configuration", GetLastError());
		errorMsg += errorMsgGrowth;
		errorMsgSize -= errorMsgGrowth;
	}

	if (GetCommTimeouts(hComm, &comTimeouts) == 0)
	{
		errorMsgGrowth = printf(L"Could not get com timeouts", GetLastError());
		errorMsg += errorMsgGrowth;
		errorMsgSize -= errorMsgGrowth;
	}
	comTimeouts.ReadIntervalTimeout = 0;			// Abort ReadFile operations if the data isn't available and the COM is idle for 1 milliseconds
	comTimeouts.ReadTotalTimeoutMultiplier = 0;		// No extra time given per byte of requested data
	comTimeouts.ReadTotalTimeoutConstant = 0;		// Wait at least 1ms for data to arrive (fixed offset)
	comTimeouts.WriteTotalTimeoutMultiplier = 0;	// Don't use total write timeout
	comTimeouts.WriteTotalTimeoutConstant = 0;		// Don't use total write timeout
	if (SetCommTimeouts(hComm, &comTimeouts) == 0)
	{
		errorMsgGrowth = printf(L"Could not set com timeouts", GetLastError());
		errorMsg += errorMsgGrowth;
		errorMsgSize -= errorMsgGrowth;
	}

	// Receive events from COM to wake us from sleeping states
	if (!SetCommMask(hComm, EV_BREAK | EV_CTS | EV_ERR | EV_RXCHAR | EV_TXEMPTY))
	{
		errorMsgGrowth = printf(L"Could not set com event mask", GetLastError());
		errorMsg += errorMsgGrowth;
		errorMsgSize -= errorMsgGrowth;
	}

	return 1;
}


void UART_Write(uint8_t *data, uint32_t size)
{

	DWORD	dwActual;
	if (WriteFile(hComm, data, size, &dwActual, &comEvent) == 0)
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_IO_PENDING)
		{
			printf(L"Error write to com port: %d", dwError);
			return 0;
		}
	}
	return 1;

}


void UART_Read(uint8_t *data, uint32_t size)
{
	DWORD	dwActual;
	if (ReadFile(hComm, data, size, &dwActual, NULL) == 0)
	{
		DWORD dwError = GetLastError();
		printf(L"Error read from com port: ", dwError);
		return 0;
	}
	return 1;
}
