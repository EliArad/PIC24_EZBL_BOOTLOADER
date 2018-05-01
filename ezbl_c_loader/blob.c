#include "ezbl_c_loader.h"



static BL2_FILE_FORMAT m_bl2Blob;

void GetBlob(BL2_FILE_FORMAT *blob)
{
	blob = &m_bl2Blob;
}

 
EZBL_Errors BLOB_Read(char *fileName)
{
	FILE * pFile;
	int r;
	long lSize;
	size_t result;
	uint32_t crc = 0;


	pFile = fopen(fileName, "rb");
	if (pFile == NULL)
		return ERROR_OPEN_FILE;

	fseek(pFile, 0, SEEK_END);
	long lFileSize = ftell(pFile);
	rewind(pFile);

	// Read 16 bytes SYNC
	// not using sizeof here just to show the sizes:
	
	r = fread(m_bl2Blob.bl2H.SyncData, 1, 16, pFile);
	if (r != 16)
		return ERROR_NO_DATA;

	if (strncmp(m_bl2Blob.bl2H.SyncData, "UUUUUUUUMCUPHCME", 16) != 0)
		return ERROR_NOT_BL2_FILE;

	r = fread(&m_bl2Blob.bl2H.FILE_ID, 1, 4, pFile);
	if (r != 4)
		return ERROR_NO_DATA;
	if (m_bl2Blob.bl2H.FILE_ID != 0x42324c42)
		return ERROR_INVALID_FILE_ID;

	r = fread(&m_bl2Blob.bl2H.FILE_LEN, 1, 4, pFile);
	if (r != 4)
		return ERROR_NO_DATA;

	r = fread(&m_bl2Blob.bl2H.BOODID_HASH, 1, 16, pFile);
	if (r != 16)
		return ERROR_NO_DATA;

	r = fread(&m_bl2Blob.bl2H.APPID_VER_BUILD, 1, 4, pFile);
	if (r != 4)
		return ERROR_NO_DATA;

	r = fread(&m_bl2Blob.bl2H.APPID_VER_MINOR, 1, 2, pFile);
	if (r != 2)
		return ERROR_NO_DATA;
	

	r = fread(&m_bl2Blob.bl2H.APPID_VER_MAJOR, 1, 2, pFile);
	if (r != 2)
		return ERROR_NO_DATA;
	

	r = fread(m_bl2Blob.bl2H.HMAC_SHA_256, 1, 16, pFile);
	if (r != 16)
		return ERROR_NO_DATA;

	//DATA_RECORDn(repeated an arbitrary number of times to store all data

	r = fread(&m_bl2Blob.bl2H.REC_LEN, 1, 4, pFile);
	if (r != 4)
		return ERROR_NO_DATA;

	r = fread(&m_bl2Blob.bl2H.REC_ADDR, 1, 4, pFile);
	if (r != 4)
		return ERROR_NO_DATA;

	long payloadSize = lFileSize - sizeof(m_bl2Blob.bl2H) - 36;
	
	m_bl2Blob.bl2D.Data = malloc(payloadSize + sizeof(m_bl2Blob.bl2H));
	r = fread(m_bl2Blob.bl2D.Data + sizeof(m_bl2Blob.bl2H), 1, payloadSize, pFile);
	if (r != payloadSize)
		return ERROR_NO_DATA;

	m_bl2Blob.bl2D.dataSize = payloadSize;
	memcpy(m_bl2Blob.bl2D.Data, &m_bl2Blob.bl2H, sizeof(m_bl2Blob.bl2H));
	 
	r = fread(m_bl2Blob.bl2D.FILE_HASH, 1, 32, pFile);
	if (r != 32)
		return ERROR_NO_DATA;

	r = fread(&m_bl2Blob.bl2D.CRC32, 1, 4, pFile);
	if (r != 4)
		return ERROR_NO_DATA;
 
	
	crc32_recalculate(m_bl2Blob.bl2D.Data, payloadSize + sizeof(m_bl2Blob.bl2H), &crc);

	return EZBL_OK;
	
}

