//---------------------------------------------------------------------------


#include <vcl.h>
#pragma hdrstop

#include "MShape.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void __fastcall TMShape::Paint(void)
{
        int X,Y,W,H,S;
        Canvas->Pen = Pen;
        Canvas->Brush = Brush;
        W = Width - Pen->Width;
        H = Height - Pen->Width;
        X = 0;
        Y = 0;
        if (Pen->Width==0)
        {
                W--;
                H--;
        }
        if (FNap==0)
        {
                TPoint r[4] = {TPoint(X+W,Y),TPoint(X,Y+H/2),TPoint(X+W,Y+H),TPoint(X+W,Y)};
                Canvas->Polygon(r,3);
        }
        else if (FNap==1)
        {
                TPoint r[4] = {TPoint(X,Y),TPoint(X+W,Y+H/2),TPoint(X,Y+H),TPoint(X,Y)};
                Canvas->Polygon(r,3);
        }
}

void __fastcall TMShape::SetNap(int Value)
{
  if (FNap != Value)
  {
    FNap = Value;
    Invalidate();
  }
}


