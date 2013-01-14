#include "stdafx.h"
#include "CRC.h"
//////////////////////////////////////////////////////////////////////////
// Code ends at Line 231
//////////////////////////////////////////////////////////////////////////
ULONG crc32_table[256]; // Lookup table arrays
void CRC_Init()
{
   // This is the official polynomial used by CRC-32 in PKZip, WinZip and Ethernet. 
   ULONG ulPolynomial = 0x04c11db7;

   // 256 values representing ASCII character codes.
  for(int i = 0; i <= 0xFF; i++)
  {
	crc32_table[i]=CRC_Reflect(i, 8) << 24;
	for (int j = 0; j < 8; j++)
		crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
	crc32_table[i] = CRC_Reflect(crc32_table[i], 32);
  }

}

ULONG CRC_Reflect(ULONG ref, char ch)
{// Used only by Init_CRC32_Table()

	ULONG value(0);

	// Swap bit 0 for bit 7 , bit 1 for bit 6, etc.
	for(int i = 1; i < (ch + 1); i++)
	{
		if(ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}

void CRC_Get(unsigned char* buffer, int nSize,unsigned char* lpoutbuf)
{
	// Be sure to use unsigned variables,
	// because negative values introduce high bits
	// where zero bits are required.
	char ch[20];

	ULONG  crc(0xffffffff);
	int len;

	len = nSize;
	// Save the text in the buffer.
	// Perform the algorithm on each character
	// in the string, using the lookup table values.
	while(len--)
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *buffer++];
	// Exclusive OR the result with the beginning value.
	int icrc;
	icrc = crc^0xffffffff;
	memset(ch,0x00,20);
	_itoa_s(icrc , ch,16);
	for (int i=0;i<8;i++)
	{
		if (ch[i]==0)
		{
			ch[i]=0x30;
		}
	}

	strcpy_s((char *)lpoutbuf, 20, ch);
	//return ch;
}
