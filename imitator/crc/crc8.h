//---------------------------------------------------------------------------

#ifndef crc8H
#define crc8H
//---------------------------------------------------------------------------
// подсчет кс в буфере
unsigned char crc8_buf( unsigned char *buf, int len );
// расчет одного байта
void crc8_byte( unsigned char *crc, unsigned char data );

#endif
