//---------------------------------------------------------------------------

#ifndef crc8H
#define crc8H
//---------------------------------------------------------------------------
// ������� �� � ������
unsigned char crc8_buf( unsigned char *buf, int len );
// ������ ������ �����
void crc8_byte( unsigned char *crc, unsigned char data );

#endif
