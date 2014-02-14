//---------------------------------------------------------------------------

#ifndef ViewKolesoH
#define ViewKolesoH
//---------------------------------------------------------------------------
#include "Unit1.h"
class TViewKoleso
{
public:
__fastcall TViewKoleso(TShape *Owner);
__fastcall ~TViewKoleso();
protected:
        TMShape *MShape;
        TTimer  *TimerFl;
        TColor cl_res;
        void __fastcall TimerFlOn(TObject *Sender);
public:
void __fastcall Flash(int zn, TColor cl_set, TColor cl_res);
};
extern TViewKoleso *ViewKoleso;
#endif
