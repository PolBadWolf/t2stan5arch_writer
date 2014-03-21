//---------------------------------------------------------------------------

#ifndef UserFunction1H
#define UserFunction1H
#include <ADODB.hpp>
//---------------------------------------------------------------------------
// Diametr tube -> Len Segment
// Диаметр трубы в длину сегмента
double __fastcall FnDiametr2LenSegment(double D_Tube);
void __fastcall WriteBD_Datas(TADOConnection *connect, int NumberTube, signed char *Massive, int Len, int GlagDefect, int IdParametr);
void __fastcall Show_NumberTube(int nTube);
void __fastcall Show_Parametrs(int nTube, double SizeTube, double LenSegmentTube, AnsiString CodeMelt);
void __fastcall Show_LenTube(double lTube);
void __fastcall Show_LenTubeClear();
void __fastcall ShowSensorSample(TShape *Lamp, int lvl);
void __fastcall WriteFlTree(TADOConnection *connect);
//---------------------------------------------------------------------------
#endif


