//---------------------------------------------------------------------------

#ifndef UserFunction1H
#define UserFunction1H
#include <ADODB.hpp>
//---------------------------------------------------------------------------
// Diametr tube -> Len Segment
// Диаметр трубы в длину сегмента
double __fastcall FnDiametr2LenSegment(double D_Tube);
void __fastcall WriteBD_Datas(TADOConnection *connect, int NumberTube, signed char *Massive, int Len, int GlagDefect, int IdParametr);
void __fastcall Show_NumberTube(int number);
void __fastcall Show_Parametrs(AnsiString CodeMelt, double SizeTube);
//---------------------------------------------------------------------------
#endif


