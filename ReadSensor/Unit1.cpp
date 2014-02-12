//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "BoxRead1.h"
#include "WriteFl.h"
#include "ViewKoleso.h"
#include "PapkaName.h"
#include "UserFunction1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
TNBoxRead *BoxRead = NULL;

#define D_MaxLenTube  15000
#define D_ImageOffsetX 50
#define D_ImageOffsetY 50

#define TUBE_HERE1              2       // ������� ����� �1 *

bool UnicalFlDefect = false;

class TMassFields
{
public:
        static int n;
        AnsiString Name;
        TFieldType Type;
        Variant    Value;
        __fastcall TMassFields()
        {
                Name  = "";
                Type  = 0;
                Value = NULL;
        }
};
int TMassFields::n = 0;
TMassFields MassFields[64];

__fastcall TMShape::TMShape(Classes::TComponent* AOwner)
          : TShape(AOwner)
{
}
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
        Caption = "������ �� 2014.02.12";
        ViewKoleso = new TViewKoleso(Shape_Circle);
        FlNewTube = false;
        Img_ClearAll(ImageVisual);
        // ====================================================
        // find last set diametr
        ADOQuery1->Active = false;
        ADOQuery1->SQL->Clear();
        ADOQuery1->SQL->Add("SELECT *");
        ADOQuery1->SQL->Add("FROM `defectsdata`");
        ADOQuery1->SQL->Add("ORDER BY IndexData DESC");
        ADOQuery1->SQL->Add("LIMIT 1");
        ADOQuery1->Open();
        if ( ADOQuery1->RecordCount==0 )
        {
                // set default diametr tube
                CurentDiametrTube = 168;
        }
        else
        {
                // set diametr tube
                ADOQuery1->FindField("")
                CurentDiametrTube = 168;
        }
        // ====================================================
        // find last number tube
        int np = ADOQuery1->Fields->Count;
        MassFields->n = np;
        AnsiString fName;
        ListBox2->Items->Clear();
        ListBox2->Items->Add(np);
        for (int i=0; i<np; i++)
        {
                fName = ADOQuery1->Fields->FieldByNumber(i+1)->FieldName;
                MassFields[i].Name  = ADOQuery1->Fields->FieldByNumber(i+1)->FieldName;
                MassFields[i].Type  = ADOQuery1->Fields->FieldByNumber(i+1)->DataType;
                MassFields[i].Value = ADOQuery1->Fields->FieldByNumber(i+1)->Value;
                if (fName=="IndexData")         continue;
                if (fName=="DataSensors")       continue;
                if (fName=="DatePr")            continue;
                if (fName=="TimePr")            continue;
                ListBox2->Items->Add( fName+" : "+ADOQuery1->Fields->FieldByNumber(i+1)->Value);
        }
        ADOQuery1->Fields->FieldByNumber(1)->DataType;
        ADOQuery1->Active = false;
        ADOConnection1->Connected = false;
        // ������ � �� �� ������ �������
        otLmm = 365;
        // offset from left sensor tube, unit segment   // ����������� ������ � �����
        otStep = otLmm/FnDiametr2LenSegment(CurentDiametrTube);
        // ������� ��������� ������ ����� ���������� ������ �������
        otCount = 0;
        // ����� ����� ����������
        dTube = -1;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
        if (BoxRead)
        {
                BoxRead->Terminate();
                BoxRead->Resume();
                BoxRead = NULL;
        }
        delete ViewKoleso;
}
//---------------------------------------------------------------------------
// ��������� ��������� ��������
void __fastcall TForm1::EvaSensor(int sn, int lvl)
{
        if (sn<14)
        {
                LampDat[sn]->Brush->Color = (lvl==0)?clLime:clWhite;
        }
        /*
        // ��������� ������������
        if ( BoxRead->FlSensorAt )
        {
                //ShapePositionDefectoskop->Brush->Color = clLime;
        }
        else
        {
                //ShapePositionDefectoskop->Brush->Color = clRed;
        }
        */
        /*
        // ��������� �����
        if ( BoxRead->FlTubeInDefect )
        {
                //ShapePositionTube->Brush->Color = clLime;
        }
        else
        {
                //ShapePositionTube->Brush->Color = clRed;
        }
        */
        //Label14->Caption = BoxRead->Count;
        /*
        if ( (BoxRead->Count-otStep)>=0 )
                Label14->Caption = (BoxRead->Count-otStep)*Step2mm[Step2mmD];
        else
                Label14->Caption = "";
        */
        if (dTube>0)
                //Label17->Caption = dTube - otStep;
                Label17->Caption = (dTube-otStep)*FnDiametr2LenSegment(CurentDiametrTube);
        else
                Label17->Caption = dTube;
}
//---------------------------------------------------------------------------
// ��������� ��������� ������� ������������
// 0 - �� �����, 1 - ��� ������
void __fastcall TForm1::EvaSensorAt(int lvl)
{
}
//---------------------------------------------------------------------------
// ��������� ��������� ������� ������
// 0 - ��� �������, 1 - �������
void __fastcall TForm1::EvaWeldDefect(int lvl)
{
}
//---------------------------------------------------------------------------
// ��������� ��������� ������� ����������
// 1 - ����������, 0 - ������
void __fastcall TForm1::EvaModeCalibrovka(int lvl, int fl_mod)
{
}
//---------------------------------------------------------------------------
// �������� ������
void __fastcall TForm1::EvaCircle(int Napravl, int Dlina, int Position, unsigned char *MassDefect)
{
        // ��������� ������ �������
        if (!BoxRead->BoxReadMassSensorsLevel[TUBE_HERE1])
        {
                // ����� �� �������
                otCount = otStep;
                // ����� ����� ����������
                dTube = -1;
                Label17->Caption = dTube;
        }
        else
        {
                // ����� ���
                if (otCount>0) otCount--;
                else
                {
                        if (dTube<0)
                        {
                                dTube = Dlina;
                                //Label17->Caption = dTube - otStep;
                                Label17->Caption = (dTube-otStep)*FnDiametr2LenSegment(CurentDiametrTube);
                        }
                        else
                        {
                                Dlina = dTube;
                                if (Position>dTube)
                                        Position = dTube;
                        }
                }
        }
        if (Position<(otStep+1) ) return; // ��� ���� �������
        if (Dlina   <(otStep+1) ) return; // ��� ���� �������
        //Label14->Caption = BoxRead->Count;
        //Label14->Caption = (BoxRead->Count-otStep)*Step2mm[Step2mmD];
        Label14->Caption = (Position-otStep)*FnDiametr2LenSegment(CurentDiametrTube);
        // ���������
        int nX,  nY,  eX,  eY;
        int nX1, nY1, eX1, eY1;
        double n, e; // �������� �������� � ��
        TColor c;
        nX = D_ImageOffsetX + 1;
        nY = D_ImageOffsetY + 1;
        eX = ImageVisual->Width -1;
        eY = ImageVisual->Height -1;
        // ������� ������� �����
        ImageVisual->Canvas->Lock();
        ImageVisual->Canvas->Brush->Color = clBlack;
        ImageVisual->Canvas->FillRect(Rect(nX, nY, eX, nY+(eY-nY)/10) );
        ImageVisual->Canvas->Unlock();
        // ��������
        n = FnDiametr2LenSegment(CurentDiametrTube)*(Position+0-(otStep+1) );
        e = FnDiametr2LenSegment(CurentDiametrTube)*(Position+1-(otStep+1) );
        nY1 = nY;
        eY1 = eY-(eY-nY)/10;
        nX1 = ImgV_TubeMaxPix*n/D_MaxLenTube + D_ImageOffsetX;
        eX1 = ImgV_TubeMaxPix*e/D_MaxLenTube + D_ImageOffsetX;
        // ����
        if (MassDefect[otStep+Position-1-(otStep+1) ]==255)
                c = clGray;
        if (MassDefect[otStep+Position-1-(otStep+1) ]==0)
                c = clLime;
        if (MassDefect[otStep+Position-1-(otStep+1) ]==1)
                c = clRed;
        ImageVisual->Canvas->Lock();
        ImageVisual->Canvas->Brush->Color = c;
        ImageVisual->Canvas->FillRect(Rect(nX1, nY1, eX1, eY1) );
        ImageVisual->Canvas->Unlock();
        // �����
        Img_Setka(ImageVisual, nX, nY, eX, eY, 15);
        
}
//---------------------------------------------------------------------------
// ��������� ��������� �������� ������
void __fastcall TForm1::EvaCircleSensor(int sn)
{
        ViewKoleso->Flash(sn, clLime, clGreen);
}
//---------------------------------------------------------------------------
// ���������� �������������
void __fastcall TForm1::TubeEnd()
{
        static int Len = 0;
        int  *Mass = BoxRead->MassDefect;       // ������ �������������� ��������
        bool FlObr = BoxRead->FlModeCalibrovka; // ���� ����������/�������
        Label14->Caption = "-//-";
        if (FlNewTube)
        {
                FlNewTube = false;
                Len       = BoxRead->MassDefectLen;    // ����� �������������� ����� � ���������
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TubeBegin()
{
        FlNewTube = true;
        // ������� ������ + �����
        int nX, nY, eX, eY;
        nX = D_ImageOffsetX + 1;
        nY = D_ImageOffsetY + 1;
        eX = ImageVisual->Width -1;
        eY = ImageVisual->Height -1;
        Img_Clear(ImageVisual, nX, nY, eX, eY, 15);
        Img_Setka(ImageVisual, nX, nY, eX, eY, 15);
        // =====================================================
        // ������ �������� 
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Img_ClearAll(TImage *Img)
{
        ImgV_TubeMaxPix = Img->Width - D_ImageOffsetX;
        //
        int nX, nY, eX, eY;
        nX = D_ImageOffsetX + 1;
        nY = D_ImageOffsetY + 1;
        eX = ImageVisual->Width -1;
        eY = ImageVisual->Height -1;
        // clear
        Img->Canvas->Lock();
        Img->Canvas->Brush->Color = clBlack;
        Img->Canvas->FillRect(Rect(0,0,ImageVisual->Width,ImageVisual->Height));
        Img->Canvas->Unlock();
        // line
        Img->Canvas->Lock();
        Img->Canvas->Pen->Width = 3;
        Img->Canvas->Pen->Color = clGreen;
        Img->Canvas->MoveTo(D_ImageOffsetX    ,ImageVisual->Height);
        Img->Canvas->LineTo(D_ImageOffsetX    ,D_ImageOffsetY);
        Img->Canvas->LineTo(ImageVisual->Width,D_ImageOffsetY);
        Img->Canvas->Unlock();
        // line
        Img->Canvas->Lock();
        int m = D_MaxLenTube/1000;
        for(int x=0;x<(m+1);x++)
        {
                Img->Canvas->Pen->Width = 3;
                Img->Canvas->Pen->Color = clGreen;
                Img->Canvas->MoveTo(D_ImageOffsetX+x*(ImgV_TubeMaxPix/(D_MaxLenTube/1000)) ,D_ImageOffsetY);
                Img->Canvas->LineTo(D_ImageOffsetX+x*(ImgV_TubeMaxPix/(D_MaxLenTube/1000)) ,D_ImageOffsetY-(D_ImageOffsetY/3));
        }
        Img->Canvas->Unlock();
        Img_Clear(Img, nX, nY, eX, eY, m);
        Img_Setka(Img, nX, nY, eX, eY, m);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Img_Clear(TImage *Img, int nX, int nY, int eX, int eY, int m)
{
        ImageVisual->Canvas->Lock();
        ImageVisual->Canvas->Brush->Color = clBlack;
        ImageVisual->Canvas->FillRect(Rect(nX, nY, eX, eY) );
        ImageVisual->Canvas->Unlock();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Img_Setka(TImage *Img, int nX, int nY, int eX, int eY, int m)
{
        ImageVisual->Canvas->Lock();
        for (int x=0; x<(m+1); x++)
        {
                Img->Canvas->Pen->Width = 1;
                Img->Canvas->Pen->Color = clGreen;
                Img->Canvas->MoveTo(nX+ x*(ImgV_TubeMaxPix/(D_MaxLenTube/1000) )-1 ,nY);
                Img->Canvas->LineTo(nX+ x*(ImgV_TubeMaxPix/(D_MaxLenTube/1000) )-1 ,eY);
        }
        Img->Canvas->MoveTo(nX, nY+(eY-nY)/2 );
        Img->Canvas->LineTo(eX, nY+(eY-nY)/2 );
        ImageVisual->Canvas->Unlock();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TimerStartTimer(TObject *Sender)
{
        ((TTimer*)Sender)->Enabled = false;
        LampDat[0] = Shape_KEY_BACK;
        LampDat[1] = Shape_KEY_FORWARD;
        LampDat[2] = Shape_TUBE_HERE_L;
        LampDat[3] = Shape_TUBE_HERE_R;
        LampDat[4] = Shape5;
        LampDat[5] = Shape6;
        LampDat[6] = Shape_WELD_DEFECT;
        LampDat[7] = Shape_MODE_CALIBROVKA;
        LampDat[8] = Shape_SENSOR_AT_TOP;
        LampDat[9] = Shape_SENSOR_AT_BOTTOM;
        //LampDat[10] = Shape11;
        //LampDat[11] = Shape12;
        BoxRead = new TNBoxRead;
        // ����� � ������, ������ �������������
        BoxRead->EvTubeHereBeginUp = TubeBegin;
        BoxRead->EvTubeHereBeginDn;
        BoxRead->EvTubeHereEndUp;
        // c���� � �����, ����� �������������
        BoxRead->EvTubeHereEndDn   = TubeEnd;
        // ��������� ������� ������������ ( lvl : 0 - �� �����, 1 - ��� ������ )
        BoxRead->EvSensorAt        = EvaSensorAt;
        // ������� �������� ������
        BoxRead->EvCircle          = EvaCircle;
        // ����� - ��������� ��������� �������
        BoxRead->EvSensor          = EvaSensor;
        // ����� - ��������� ��������� ������� ������
        BoxRead->EvCircleSensor    = EvaCircleSensor;
        // ��������� ��������� ������� ��������
        BoxRead->EvWeldDefect      = EvaWeldDefect;
        // ��������� ��������� ����� ����������
        BoxRead->EvModeCalibrovka  = EvaModeCalibrovka;
        // ����
        BoxRead->Resume();
        delete ((TTimer*)Sender);
}
//---------------------------------------------------------------------------


