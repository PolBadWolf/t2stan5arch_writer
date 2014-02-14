//---------------------------------------------------------------------------


#pragma hdrstop

#include "PapkaName.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

AnsiString mfPapkaName(TDateTime vr)
{
        int chas, subd;
        AnsiString papka = "";
        chas = FormatDateTime("h", vr).ToInt();
        if (chas<8)
        {
                papka = FormatDateTime("yyyy-mm-dd",vr - 1)+"_N";
        }
        if ( (chas>=8) && (chas<20) )
        {
                papka = FormatDateTime("yyyy-mm-dd",vr)+"_D";
        }
        if ( chas>=20 )
        {
                papka = FormatDateTime("yyyy-mm-dd",vr)+"_N";
        }
        return papka;
}
