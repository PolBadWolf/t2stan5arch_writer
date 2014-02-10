//---------------------------------------------------------------------------


#include <vcl.h>
#pragma hdrstop

#include "ViewKoleso.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TViewKoleso *ViewKoleso;

__fastcall TViewKoleso::TViewKoleso(TShape *Owner)
{
        MShape = NULL;
        try
        {
        //MShape = new TMShape(Form1->Shape13->Owner);
        //MShape->Parent = Form1->Shape13->Parent;
        //MShape->Left = Form1->Shape13->Left;
        //MShape->Top = Form1->Shape13->Top;
        //MShape->Width = Form1->Shape13->Width;
        //MShape->Height = Form1->Shape13->Height;
        //MShape->Brush->Color = Form1->Shape13->Brush->Color;
        MShape = new TMShape(Owner->Owner );
        MShape->Parent = Owner->Parent;
        MShape->Left = Owner->Left;
        MShape->Top = Owner->Top;
        MShape->Width = Owner->Width;
        MShape->Height = Owner->Height;
        MShape->Brush->Color = Owner->Brush->Color;
        MShape->Nap = 0;
        Owner->Visible = false;
        TimerFl = new TTimer(NULL);
        TimerFl->Enabled = false;
        TimerFl->OnTimer = TimerFlOn;
        }
        catch(...)
        {
                throw(1);
        }
}
//---------------------------------------------------------------------------
__fastcall TViewKoleso::~TViewKoleso()
{
        delete TimerFl;
        delete MShape;
        MShape = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TViewKoleso::Flash(int zn, TColor cl_set, TColor cl_res)
{
        this->cl_res = cl_res;
        MShape->Nap = 1-zn;
        MShape->Brush->Color = cl_set;
        TimerFl->Enabled = false;
        TimerFl->Interval = 200;
        TimerFl->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TViewKoleso::TimerFlOn(TObject *Sender)
{
        MShape->Brush->Color = cl_res;
        TimerFl->Enabled = false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


