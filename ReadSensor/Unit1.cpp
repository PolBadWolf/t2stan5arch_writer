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

#define TUBE_HERE1              2       // наличие трубы д1 *


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
                Type  = ftUnknown;
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
        Caption = "версия от 2014.02.12";
        ViewKoleso = new TViewKoleso(Shape_Circle);
        FlNewTube = false;
        Img_ClearAll(ImageVisual);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
        // checked close ADOQuery1
        if ( ADOQuery1->Active )
        {       //ADOQuery1 - no terminated
                ADOQuery1->Close();
        }
        // checked close ADOConnection1
        if ( ADOConnection1->Connected )
        {
                ADOConnection1->Close();
        }
        if (BoxRead)
        {
                BoxRead->Terminate();
                BoxRead->Resume();
                BoxRead = NULL;
        }
        delete ViewKoleso;
}
//---------------------------------------------------------------------------
// изменение состояния датчиков
void __fastcall TForm1::EvaSensor(int sn, int lvl)
{
        if (sn<14)
        {
                LampDat[sn]->Brush->Color = (lvl==0)?clLime:clWhite;
        }
        /*
        // положение дефектоскопа
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
        // положение трубы
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
                Label17->Caption = (dTube-otStep)*LenSegmentTube;
        else
                Label17->Caption = dTube;
}
//---------------------------------------------------------------------------
// изменение положения датчика дефектоскопа
// 0 - на трубе, 1 - над трубой
void __fastcall TForm1::EvaSensorAt(int lvl)
{
}
//---------------------------------------------------------------------------
// изменение состояние датчика дефект
// 0 - нет дефекта, 1 - дефекта
void __fastcall TForm1::EvaWeldDefect(int lvl)
{
}
//---------------------------------------------------------------------------
// изменение состояние датчика калибровки
// 1 - калибровка, 0 - работа
void __fastcall TForm1::EvaModeCalibrovka(int lvl, int fl_mod)
{
}
//---------------------------------------------------------------------------
// сработка колеса
void __fastcall TForm1::EvaCircle(int Napravl, int Dlina, int Position, int *MassDefect)
{
        Label15->Caption = Position;
        // состояние левого датчика
        if (!BoxRead->BoxReadMassSensorsLevel[TUBE_HERE1])
        {
                // труба на датчике
                otCount = otStep;
                // длина трубы замеренной
                dTube = -1;
                Label17->Caption = dTube;
        }
        else
        {
                // трубы нет
                if (otCount>0) otCount--;
                else
                {
                        if (dTube<0)
                        {
                                dTube = Dlina;
                                //Label17->Caption = dTube - otStep;
                                // len tube ?
                                Label17->Caption = (dTube-otStep)*LenSegmentTube;
                        }
                        else
                        {
                                Dlina = dTube;
                                if (Position>dTube)
                                        Position = dTube;
                        }
                }
        }
        if (Position<(otStep+1) ) return; // вне зоны датчика
        if (Dlina   <(otStep+1) ) return; // вне зоны датчика
        //Label14->Caption = BoxRead->Count;
        //Label14->Caption = (BoxRead->Count-otStep)*Step2mm[Step2mmD];
        Label14->Caption = (Position-otStep)*LenSegmentTube;
        // рисование
        int nX,  nY,  eX,  eY;
        int nX1, nY1, eX1, eY1;
        double n, e; // позитция сегмента в мм
        TColor c;
        nX = D_ImageOffsetX + 1;
        nY = D_ImageOffsetY + 1;
        eX = ImageVisual->Width -1;
        eY = ImageVisual->Height -1;
        // очистка верхней части
        ImageVisual->Canvas->Lock();
        ImageVisual->Canvas->Brush->Color = clBlack;
        ImageVisual->Canvas->FillRect(Rect(nX, nY, eX, nY+(eY-nY)/10) );
        ImageVisual->Canvas->Unlock();
        // позитция
        n = LenSegmentTube*(Position+0-(otStep+1) );
        e = LenSegmentTube*(Position+1-(otStep+1) );
        nY1 = nY;
        eY1 = eY-(eY-nY)/10;
        nX1 = ImgV_TubeMaxPix*n/D_MaxLenTube + D_ImageOffsetX;
        eX1 = ImgV_TubeMaxPix*e/D_MaxLenTube + D_ImageOffsetX;
        // цвет
        if (MassDefect[otStep+Position-1-(otStep+1) ]==-1)
                c = clGray;
        if (MassDefect[otStep+Position-1-(otStep+1) ]==0)
                c = clLime;
        if (MassDefect[otStep+Position-1-(otStep+1) ]==1)
                c = clRed;
        ImageVisual->Canvas->Lock();
        ImageVisual->Canvas->Brush->Color = c;
        ImageVisual->Canvas->FillRect(Rect(nX1, nY1, eX1, eY1) );
        ImageVisual->Canvas->Unlock();
        // сетка
        Img_Setka(ImageVisual, nX, nY, eX, eY, 15);
        
}
//---------------------------------------------------------------------------
// изменение состояния датчиков колеса
void __fastcall TForm1::EvaCircleSensor(int sn)
{
        ViewKoleso->Flash(sn, clLime, clGreen);
}
//---------------------------------------------------------------------------
// завершение дефектоскопии
void __fastcall TForm1::TubeEnd()
{
    // new tube ?
    if (!FlNewTube) return;
    // =========================================================
    // no parametrs - no record
    if (!IdParam)
    {
        FlNewTube = false;
        return;
    }
    // =========================================================
    // massive dafects
    int  *Mass;
    // len massive dafects
    int  Len = 0;
    // flag tube defect
    bool FlagDefectTube = false;
    // flag Sample
    bool FlObr;
    int  nTube;
    // =========================================================
    // read status : "it`s sample"
    FlObr = BoxRead->FlModeCalibrovka;
    // =========================================================
    // Number tube +1 if no sample
    if ( FlObr )
    {   // sample mark number 0
        nTube = 0;
    }
    else
    {   // number tube +1
        CurentNumberTube++;
        nTube = CurentNumberTube;
    }
    // =========================================================
    // set address massive
    Mass = BoxRead->MassDefect + otStep;
    // Len       = BoxRead->MassDefectLen;
    Len       = dTube - otStep;
    int zn = -1;
    for (int i=0;i<Len;i++)
    {
        zn = Mass[i];
        if ( zn>0 )
        {
            FlagDefectTube = FlagDefectTube | true;
        }
    }
    // =========================================================
    Label14->Caption = "-//-";
    // set
    TDateTime DtTm = Now();
    AnsiString vDate = FormatDateTime("yyyy-mm-dd", DtTm);
    // reset flag new tube
    FlNewTube = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TubeBegin()
{
        // read last parametrs
        int         IdParamLast  , IdParamNew;
        int         IdMeltLast   , IdMeltNew;
        AnsiString  CodeMeltLast , CodeMeltNew;
        double      SizeTubeLast , SizeTubeNew;
        int         statusLast   , statusNew;
        statusLast = ReadFromBDLastParametrs(ADOQuery1, &IdParamLast, &IdMeltLast, &CodeMeltLast, &SizeTubeLast);
        statusNew  = ReadFromBDNewParametrs (ADOQuery1, &IdParamNew , &IdMeltNew , &CodeMeltNew , &SizeTubeNew );
        if ( (statusLast && statusNew) || statusNew )
        {   // error read parametrs
            IdParam  = 0;
            CodeMelt = "No melt";
            SizeTube = 0;
            LenSegmentTube = 1000/8;
        }
        else
        {   // ok
            // new melt ?
            if ( IdMeltLast!=IdMeltNew)
            {   // new melt
                CurentNumberTube = 0;
            }
            IdParam  = IdParamNew;
            CodeMelt = CodeMeltNew;
            SizeTube = SizeTubeNew;
            LenSegmentTube = FnDiametr2LenSegment(SizeTube);
        }
        // offset from left sensor tube, unit segment   // расчитанный отступ в шагах
        otStep = otLmm/LenSegmentTube;
        Show_Parametrs(CodeMelt, SizeTube);
        // ==============================
        FlNewTube = true;
        // очистка имиджа + сетка
        int nX, nY, eX, eY;
        nX = D_ImageOffsetX + 1;
        nY = D_ImageOffsetY + 1;
        eX = ImageVisual->Width -1;
        eY = ImageVisual->Height -1;
        Img_Clear(ImageVisual, nX, nY, eX, eY, 15);
        Img_Setka(ImageVisual, nX, nY, eX, eY, 15);
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
        int         IdParamLast  , IdParamNew;
        int         IdMeltLast   , IdMeltNew;
        AnsiString  CodeMeltLast , CodeMeltNew;
        double      SizeTubeLast , SizeTubeNew;
        int         statusLast   , statusNew;

// ******************************************************************************************
// *************************** init variable from Data Base *********************************
        // close ado
        ADOQuery1->Close();
        // ====================================================
        // read last number tube ( no sample )
        CurentNumberTube = ReadFromBDLastNumberTude(ADOQuery1);
        if (CurentNumberTube<0)
        {       // Date Base Error
                Application->MessageBox("Not found field \"NumberTube\" " , "Date Base Error", 0);
                Form1->Close();
                return;
        }
        // show last number tube
        Show_NumberTube(CurentNumberTube);
        // ====================================================
        // read last parametrs
        if ( !ReadFromBDLastParametrs(ADOQuery1, &IdParamLast, &IdMeltLast, &CodeMeltLast, &SizeTubeLast) )
        {   // ok
            IdParam   = IdParamLast;
            CodeMelt  = CodeMeltLast;
            SizeTube  = SizeTubeLast;
            LenSegmentTube = FnDiametr2LenSegment(SizeTube);
        }
        else
        {   // no param
            IdParam   = 0;
            CodeMelt  = "no melt tube";
            SizeTube  = 0;
            LenSegmentTube = 1000/8;
        }
        Show_Parametrs(CodeMelt, SizeTube);
// ******************************************************************************************
// *************************** init variable Base Lengh sensors tube ************************
        // отступ в мм от левого датчика
        otLmm = 365;
        // offset from left sensor tube, unit segment   // расчитанный отступ в шагах
        otStep = otLmm/LenSegmentTube;
        // счетчик окончания замера после отключения левого датчика
        otCount = 0;
        // длина трубы замеренной
        dTube = -1;
// ******************************************************************************************
// *************************** init Massibe indications *************************************
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
// ******************************************************************************************
// *************************** init BoxRead - Read from controller **************************
        BoxRead = new TNBoxRead;
        // въезд в начале, начало дефектоскопии
        BoxRead->EvTubeHereBeginUp = TubeBegin;
        BoxRead->EvTubeHereBeginDn;
        BoxRead->EvTubeHereEndUp;
        // cъезд в конце, конец дефектоскопии
        BoxRead->EvTubeHereEndDn   = TubeEnd;
        // положение головки дефектоскопа ( lvl : 0 - на трубе, 1 - над трубой )
        BoxRead->EvSensorAt        = EvaSensorAt;
        // событие сработка колеса
        BoxRead->EvCircle          = EvaCircle;
        // общее - изменение состояния датчика
        BoxRead->EvSensor          = EvaSensor;
        // общее - изменение состояния датчика колеса
        BoxRead->EvCircleSensor    = EvaCircleSensor;
        // изменение состояния датчика дефектов
        BoxRead->EvWeldDefect      = EvaWeldDefect;
        // изменение состояния ключа калибровка
        BoxRead->EvModeCalibrovka  = EvaModeCalibrovka;
        // пуск
        BoxRead->Resume();
// ******************************************************************************************
// *************************** delete starting timer ****************************************
        delete ((TTimer*)Sender);
}
//---------------------------------------------------------------------------



