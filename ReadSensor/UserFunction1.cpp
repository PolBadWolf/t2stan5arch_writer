//---------------------------------------------------------------------------
// User Functions

#pragma hdrstop

#include "UserFunction1.h"
#include <math.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

// Diametr tube -> Len Segment
// ������� ����� � ����� ��������
double __fastcall FnDiametr2LenSegment(double D_Tube)
{
        const double Dmin     = 180;
        const double PI       = 3.1415926;
        const double alfa     = 25.95*PI/180;
        const int    nSegment = 8;
        double B;    // ������� �� ����� ��������������� ������ � ������
        double Ds;   // ������� ����� ��������������� ������ � ������
        double Ls;   // ����� ���������� ����� ��������������� ������ � ������
        B  = (D_Tube/2)*tan(alfa)*sin(alfa);
        Ds = Dmin + (B*2);
        Ls = Ds * PI;
        return Ls/nSegment;
}


