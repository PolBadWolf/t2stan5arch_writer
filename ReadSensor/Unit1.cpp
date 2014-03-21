//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "ViewKoleso.h"
#include "UserFunction1.h"
#include "WriteLog.h"
#include "inifiles.hpp"
#include "BoxRead.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
TComPort   *Port = NULL;
AnsiString PortName;
eBaudRate  PortBaud;
eParity    PortParity;
TBoxRead  *BoxRead = NULL;
TWriteLog *WriteLog = NULL;

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
        Caption = "версия от 2014.03.21";
        ViewKoleso = new TViewKoleso(Shape_Circle);
        FlNewTube = false;
        Img_ClearAll(ImageVisual);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
        Port->Close();
        delete Port;
        Port = NULL;
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
                delete BoxRead;
                BoxRead = NULL;
        }
        delete ViewKoleso;
        if (WriteLog)
        {
            delete WriteLog;
            WriteLog = NULL;
        }
}
//---------------------------------------------------------------------------
/*
// изменение состояния датчиков
void __fastcall TForm1::EvaSensor(int sn, int lvl)
{
        WriteLog->Push("'TForm1::EvaSensor': sn:"+IntToStr(sn)+"  lvl:"+IntToStr(lvl));
        if (sn<10)
        {
            if ( sn==7 ) // sensor "sample"
            {
                WriteLog->Push("'TForm1::EvaSensor': key sample no show");
            //    ShowSensorSample(LampDat[7], lvl);
            }
            else
            {
                if ( sn==6 )
                {
                    WriteLog->Push("'TForm1::EvaSensor': defect show");
                    LampDat[sn]->Brush->Color = (lvl==0)?clRed:clWhite;
                }
                else
                {
                    WriteLog->Push("'TForm1::EvaSensor': other sensors show");
                    LampDat[sn]->Brush->Color = (lvl==0)?clLime:clWhite;
                }
            }
        }
        if (dTube>0)
        {
            WriteLog->Push("'TForm1::EvaSensor': show len tube - label17");
            Label17->Caption = (dTube-otStep)*LenSegmentTube;
        }
        else
                Label17->Caption = dTube;
}
*/
//---------------------------------------------------------------------------
/*
// изменение положения датчика дефектоскопа
// 0 - на трубе, 1 - над трубой
void __fastcall TForm1::EvaSensorAt(int lvl)
{
}
*/
//---------------------------------------------------------------------------
/*
// изменение состояние датчика дефект
// 0 - нет дефекта, 1 - дефекта
void __fastcall TForm1::EvaWeldDefect(int lvl)
{
}
*/
//---------------------------------------------------------------------------
/*
// изменение состояние датчика калибровки
// 1 - калибровка, 0 - работа
void __fastcall TForm1::EvaModeCalibrovka(int lvl, int fl_mod)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ??????????????????????????
    Show_NumberTube(CurentNumberTube);
}
*/
//---------------------------------------------------------------------------
/*
// сработка колеса
void __fastcall TForm1::EvaCircle(int Napravl, int Dlina, int Position, int *MassDefect)
{
    WriteLog->Push("'TForm1::EvaCircle': event circle");
        Label15->Caption = Position;
        // состояние левого датчика
        //
        if (!BoxRead->BoxReadMassSensorsLevel[TUBE_HERE1])
        {
                WriteLog->Push("'TForm1::EvaCircle': LeftSensor ON");
                // труба на датчике
                otCount = otStep;
                // длина трубы замеренной
                dTube = -1;
                Label17->Caption = dTube;
        }
        else
        {
                WriteLog->Push("'TForm1::EvaCircle': LeftSensor OFF");
                // трубы нет
                if (otCount>0) otCount--;
                else
                {
                        WriteLog->Push("'TForm1::EvaCircle': count otstup zero");
                        if (dTube<0)
                        {
                                WriteLog->Push("'TForm1::EvaCircle': show dlina & set dTube");
                                dTube = Dlina;
                                // len tube ?
                                Label17->Caption = (dTube-otStep)*LenSegmentTube;
                        }
                        else
                        {
                                WriteLog->Push("'TForm1::EvaCircle': reset Dlina = dTube");
                                Dlina = dTube;
                                if (Position>dTube)
                                        Position = dTube;
                        }
                }
        }
        //
        if (Position<(otStep+0) ) return; // вне зоны датчика
        if (Dlina   <(otStep+0) ) return; // вне зоны датчика
        //Label14->Caption = BoxRead->Count;
        //Label14->Caption = (BoxRead->Count-otStep)*Step2mm[Step2mmD];
        WriteLog->Push("'TForm1::EvaCircle': show position-otstup mm");
        Label14->Caption = (Position-otStep)*LenSegmentTube;
        // рисование
        WriteLog->Push("'TForm1::EvaCircle': picture segment");
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
        n = LenSegmentTube*(Position+0-(otStep+0) );
        e = LenSegmentTube*(Position+1-(otStep+0) );
        nY1 = nY;
        eY1 = eY-(eY-nY)/10;
        nX1 = ImgV_TubeMaxPix*n/D_MaxLenTube + D_ImageOffsetX;
        eX1 = ImgV_TubeMaxPix*e/D_MaxLenTube + D_ImageOffsetX;
        // цвет
        if (MassDefect[otStep+Position-0-(otStep+1) ]==-1)
                c = clGray;
        if (MassDefect[otStep+Position-0-(otStep+1) ]==0)
                c = clLime;
        if (MassDefect[otStep+Position-0-(otStep+1) ]==1)
                c = clRed;
        ImageVisual->Canvas->Lock();
        ImageVisual->Canvas->Brush->Color = c;
        ImageVisual->Canvas->FillRect(Rect(nX1, nY1, eX1, eY1) );
        ImageVisual->Canvas->Unlock();
        // сетка
        Img_Setka(ImageVisual, nX, nY, eX, eY, 15);

}
*/
//---------------------------------------------------------------------------
/*
// изменение состояния датчиков колеса
void __fastcall TForm1::EvaCircleSensor(int sn)
{
        ViewKoleso->Flash(sn, clLime, clGreen);
}
*/
//---------------------------------------------------------------------------
/*
// завершение дефектоскопии
void __fastcall TForm1::TubeEnd()
{
    WriteLog->Push("'TForm1::TubeEnd': event");
    // new tube ?
    if (!FlNewTube)
    {
        WriteLog->Push("'TForm1::TubeEnd': no new tube");
        return;
    }
    // =========================================================
    // no parametrs - no record
    if (!IdParam)
    {
        WriteLog->Push("'TForm1::TubeEnd': BD clear - no write tube");
        FlNewTube = false;
        return;
    }
    // =========================================================
    // massive dafects
    signed char  *Mass;
    // len massive dafects
    int  Len = 0;
    // flag tube defect
    bool FlagDefectTube = false;
    // flag Sample
    bool FlObr;
    int  nTube;
    // =========================================================
    // read status : "it`s sample"
    //FlObr = BoxRead->FlModeCalibrovka;
    // =========================================================
    // Number tube +1 if no sample
    if ( FlObr )
    {   // sample mark number 0
        WriteLog->Push("'TForm1::TubeEnd': this tube sample");
        nTube = 0;
    }
    else
    {   // number tube +1
        CurentNumberTube++;
        nTube = CurentNumberTube;
    }
    // =========================================================
    // set address massive
    //Mass = BoxRead->MassDefect + otStep;
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
    // write to BD
    WriteLog->Push("'TForm1::TubeEnd': write tube");
    //WriteBD_Datas(ADOConnection1, nTube, Mass, Len, FlagDefectTube, IdParam);
    WriteLog->Push("'TForm1::TubeEnd': show numbertube & reset flag new tube");
    Show_NumberTube(nTube);
    // reset flag new tube
    FlNewTube = false;
}
*/
//---------------------------------------------------------------------------
/*
void __fastcall TForm1::TubeBegin()
{
        WriteLog->Push("'TForm1::TubeBegin': event");
        // read last parametrs
        int         IdParamLast  , IdParamNew;
        int         IdMeltLast   , IdMeltNew;
        AnsiString  CodeMeltLast , CodeMeltNew;
        double      SizeTubeLast , SizeTubeNew;
        int         statusLast   , statusNew;
        WriteLog->Push("'TForm1::TubeBegin': ReadFromBDLastParametrs");
        statusLast = ReadFromBDLastParametrs(ADOQuery1, &IdParamLast, &IdMeltLast, &CodeMeltLast, &SizeTubeLast);
        WriteLog->Push("'TForm1::TubeBegin': ReadFromBDNewParametrs");
        statusNew  = ReadFromBDNewParametrs (ADOQuery1, &IdParamNew , &IdMeltNew , &CodeMeltNew , &SizeTubeNew );
        WriteLog->Push("'TForm1::TubeBegin': end ReadFromBD");
        if ( (statusLast && statusNew) || statusNew )
        {   // error read parametrs
            if ( statusNew==-1 )
            {   // show error BD
            }
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
        WriteLog->Push("'TForm1::TubeBegin': show param");
        otStep = otLmm/LenSegmentTube;
        Show_Parametrs(CurentNumberTube, SizeTube, LenSegmentTube, otStep, CodeMelt);
        // ==============================
        FlNewTube = true;
        WriteLog->Push("'TForm1::TubeBegin': clear picture");
        // очистка имиджа + сетка
        int nX, nY, eX, eY;
        nX = D_ImageOffsetX + 1;
        nY = D_ImageOffsetY + 1;
        eX = ImageVisual->Width -1;
        eY = ImageVisual->Height -1;
        Img_Clear(ImageVisual, nX, nY, eX, eY, 15);
        Img_Setka(ImageVisual, nX, nY, eX, eY, 15);
}
*/
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
        WriteLog = new TWriteLog;
// ******************************************************************************************
// *************************** init variable from Data Base *********************************
        WriteLog->Push("Init programm :");
        WriteLog->Push("close query1");
        // close ado
        ADOQuery1->Close();
        // ====================================================
        // read last number tube ( no sample )
        WriteLog->Push("read number tube");
        CurentNumberTube = ReadFromBDLastNumberTude(ADOQuery1);
        if (CurentNumberTube<0)
        {       // Date Base Error
                Application->MessageBox("Not found field \"NumberTube\" " , "Date Base Error", 0);
                Form1->Close();
                return;
        }
        // ====================================================
        // read last parametrs
        WriteLog->Push("read Last param BD");
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
//        otStep = otLmm/LenSegmentTube;
        WriteLog->Push("show param");
//        Show_Parametrs(CurentNumberTube, SizeTube, LenSegmentTube, otStep, CodeMelt);
// ******************************************************************************************
// *************************** init variable Base Lengh sensors tube ************************
        // отступ в мм от левого датчика
//        otLmm = 365;
        // offset from left sensor tube, unit segment   // расчитанный отступ в шагах
//        otStep = otLmm/LenSegmentTube;
        // счетчик окончания замера после отключения левого датчика
//        otCount = 0;
        // длина трубы замеренной
//        dTube = -1;
// ******************************************************************************************
// *************************** init Massibe indications *************************************
        WriteLog->Push("init massive shape");
// ******************************************************************************************
// Initialize com port
    TIniFile *ifile = new TIniFile( ChangeFileExt( Application->ExeName, ".ini" ) );
    // checked open ini file
    if (!ifile)
    {
        ShowMessage("Ошибка открытия ini файла");
        Form1->Close();
        return;
    }
    // read parametrs com port
    PortName   =            ifile->ReadString ("comm", "Name",  "COM2" );
    PortBaud   = (eBaudRate)ifile->ReadInteger("comm", "Baud",   CBR_38400);
    PortParity = (eParity)  ifile->ReadInteger("comm", "Parity", NO);
    // write default parametrs
    ifile->WriteString ("comm", "Name",   PortName);
    ifile->WriteInteger("comm", "Baud",   PortBaud);
    ifile->WriteInteger("comm", "Parity", PortParity);
    // close ini file
    delete ifile;
    ifile = NULL;
    // create
    Port = new TComPort;
    if (!Port)
    {
        ShowMessage("Ошибка создания ком порта");
        Form1->Close();
        return;
    }
    Port->aOwner = Port;
    Port->EventNewDate = PortNewDate;
// ******************************************************************************************
// *************************** init BoxRead - Read from controller **************************
        WriteLog->Push("create boxread");
        //BoxRead = new TBoxRead(720, 340, 800);
        BoxRead = new TBoxRead;
// ******************************************************************************************
        // Other sensors
        // BoxRead->EvSensorsOther = // (int sn, int lvl);
// ******************************************************************************************
        // Sensor Wild
        BoxRead->EvSensorWild = EvaSensorWild; // (int lvl);
// ******************************************************************************************
        //                             Sensors At
        // Sensors At Top
        BoxRead->EvSensorAtTop = EvaSensorAtTop;
        // Sensors At Bottom
        BoxRead->EvSensorAtBottom = EvaSensorAtBottom;
        // Sensors At
        BoxRead->EvSensorsAtShow = EvaSensorsAtShow;
// ******************************************************************************************
        //                         Sensors Tube here
        // Sensors Tube here Show
        BoxRead->EvSensorsTubeHereShow = EvaSensorsTubeHereShow;
        // Sensors Tube here Tube begin
        BoxRead->EvSensorTubeBegin = EvaSensorTubeBegin;
        // Sensors Tube here Begin Record
        //BoxRead->EvSensorTubeBeginRecord =
        // Sensors Tube here Len Tube
        BoxRead->EvSensorTubeLen = EvaSensorTubeLen;
        // Sensors Tube here End Tube
        BoxRead->EvSensorTubeEnd = EvaSensorTubeEnd;
        // Sensors Tube Reset tube
        // BoxRead->EvSensorTubeReset =
// ******************************************************************************************
        // Sensors Tube Sample
        BoxRead->EvSample = EvaSample;
// ******************************************************************************************
        //                         Sensor Circle
        // Sensor Circle Show flash
        BoxRead->EvCircleShow = EvaCircleShow;
        // Sensor Circle move
        BoxRead->EvCircleForward = EvaCircleMove;
        BoxRead->EvCircleBack = EvaCircleMove;
// ******************************************************************************************
    // open com port
    if (Port->Open(PortName, PortBaud, PortParity)>0)
    {
        ShowMessage("Ошибка открытия ком порта");
        Form1->Close();
        return;
    }
// ******************************************************************************************
// *************************** delete starting timer ****************************************
        delete ((TTimer*)Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PortNewDate(TComPort *cPort, int RdByte)
{
    unsigned char Buf;
    unsigned long zak, ok;
    while( !cPort->ReadBuf(&Buf, &(zak=1), &ok) )
    {
        BoxRead->PushFromCommPort(Buf);
    }
}
//---------------------------------------------------------------------------
// Sensor Wild
void __fastcall TForm1::EvaSensorWild(int lvl)
{
    Shape_WELD_DEFECT->Brush->Color = (lvl)?clWhite:clRed;
}
//---------------------------------------------------------------------------
// Sensors At Top
void __fastcall TForm1::EvaSensorAtTop(int lvl)
{
    Shape_SENSOR_AT->Brush->Color = clRed;
    Shape_SENSOR_AT->Top = 8;
}
//---------------------------------------------------------------------------
// Sensors At Bottom
void __fastcall TForm1::EvaSensorAtBottom(int lvl)
{
    Shape_SENSOR_AT->Brush->Color = clLime;
    Shape_SENSOR_AT->Top = 70;
}
//---------------------------------------------------------------------------
// Sensors At Show
void __fastcall TForm1::EvaSensorsAtShow(int n, int lvl)
{
    if (n==0)
        Shape_SENSOR_AT_TOP->Brush->Color    = (lvl)?clWhite:clLime;
    if (n==1)
        Shape_SENSOR_AT_BOTTOM->Brush->Color = (lvl)?clWhite:clLime;
}
//---------------------------------------------------------------------------
// Sensors Tube here Show
void __fastcall TForm1::EvaSensorsTubeHereShow(int n, int lvl)
{
    if (n==0)
        Shape_TUBE_HERE_L->Brush->Color = (lvl)?clWhite:clLime;
    if (n==1)
        Shape_TUBE_HERE_R->Brush->Color = (lvl)?clWhite:clLime;
}
//---------------------------------------------------------------------------
// Sensors Tube here Tube begin
double __fastcall TForm1::EvaSensorTubeBegin()
{
    // read last parametrs
    int         IdParamLast  , IdParamNew;
    int         IdMeltLast   , IdMeltNew;
    AnsiString  CodeMeltLast , CodeMeltNew;
    double      SizeTubeLast , SizeTubeNew;
    int         statusLast   , statusNew;
    // WriteLog->Push("'TForm1::TubeBegin': ReadFromBDLastParametrs");
    statusLast = ReadFromBDLastParametrs(ADOQuery1, &IdParamLast, &IdMeltLast, &CodeMeltLast, &SizeTubeLast);
    // WriteLog->Push("'TForm1::TubeBegin': ReadFromBDNewParametrs");
    statusNew  = ReadFromBDNewParametrs (ADOQuery1, &IdParamNew , &IdMeltNew , &CodeMeltNew , &SizeTubeNew );
    // WriteLog->Push("'TForm1::TubeBegin': end ReadFromBD");
    //
    // checked load parametrs
    if ( (statusLast && statusNew) || statusNew )
    {   // error read parametrs
        if ( statusNew==-1 )
        {   // show error BD
        }
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
    // ******************
    CurentNumberTube = ReadFromBDLastNumberTude(ADOQuery1);
    Show_Parametrs(CurentNumberTube, SizeTube, LenSegmentTube, CodeMelt);
    // ==============================
    WriteLog->Push("'TForm1::TubeBegin': clear picture");
    // очистка имиджа + сетка
    int nX, nY, eX, eY;
    nX = D_ImageOffsetX + 1;
    nY = D_ImageOffsetY + 1;
    eX = ImageVisual->Width -1;
    eY = ImageVisual->Height -1;
    Img_Clear(ImageVisual, nX, nY, eX, eY, 15);
    Img_Setka(ImageVisual, nX, nY, eX, eY, 15);
    vLenTubeSegment = D_MaxLenTube/LenSegmentTube;
    return LenSegmentTube;
}
//---------------------------------------------------------------------------
// Sensors Tube here Tube Len
void __fastcall TForm1::EvaSensorTubeLen(int segments, double lenght)
{
    vLenTubeMM = lenght;
    vLenTubeSegment = segments;
    Show_LenTube(lenght);
}
//---------------------------------------------------------------------------
// Sensors Tube here End Tube
void __fastcall TForm1::EvaSensorTubeEnd(int len, signed char *massDefect, int flagSample)
{
    //WriteLog->Push("'TForm1::TubeEnd': event");
    // =========================================================
    // no parametrs - no record
    if (!IdParam)
    {
        //WriteLog->Push("'TForm1::TubeEnd': BD clear - no write tube");
        return;
    }
    // =========================================================
    // flag tube defect
    bool FlagDefectTube = false;
    int  nTube;
    // =========================================================
    // Number tube +1 if no sample
    if ( flagSample )
    {   // sample mark number 0
        //WriteLog->Push("'TForm1::TubeEnd': this tube sample");
        nTube = 0;
    }
    else
    {   // number tube +1
        CurentNumberTube = ReadFromBDLastNumberTude(ADOQuery1);
        CurentNumberTube++;
        nTube = CurentNumberTube;
    }
    // =========================================================
    // set address massive
    int zn = -1;
    for (int i=0;i<len;i++)
    {
        zn = massDefect[i];
        if ( zn>0 )
        {
            FlagDefectTube = FlagDefectTube | true;
        }
    }
    // =========================================================
    //Label14->Caption = "-//-";
    // set
    TDateTime DtTm = Now();
    AnsiString vDate = FormatDateTime("yyyy-mm-dd", DtTm);
    // write to BD
    //WriteLog->Push("'TForm1::TubeEnd': write tube");
    WriteBD_Datas(ADOConnection1, nTube, massDefect, len, FlagDefectTube, IdParam);
    //WriteLog->Push("'TForm1::TubeEnd': show numbertube & reset flag new tube");
    Show_NumberTube(nTube);
}
//---------------------------------------------------------------------------
// Sensor Sample
void __fastcall TForm1::EvaSample(int trg, int lvl)
{
    if (!lvl)
        Shape_MODE_SAMPLE->Brush->Color = clLime;
    else
        Shape_MODE_SAMPLE->Brush->Color = (trg)?clGreen:clWhite;
}
//---------------------------------------------------------------------------
// Sensor Circle Show flash
void __fastcall TForm1::EvaCircleShow(int sn)
{
    ViewKoleso->Flash(sn, clLime, clGreen);
}
//---------------------------------------------------------------------------
// Sensor Circle move
void __fastcall TForm1::EvaCircleMove(signed char *massDefect, int curPosition)
{
    // рисование
    // WriteLog->Push("'TForm1::EvaCircle': picture segment");
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
    ImageVisual->Canvas->Lock();
    if (curPosition>vLenTubeSegment)
        curPosition = vLenTubeSegment;
    for (int pos=0; pos<curPosition; pos++)
    {
        // позитция
        n = LenSegmentTube*(pos+0);
        e = LenSegmentTube*(pos+1);
        nY1 = nY;
        eY1 = eY-(eY-nY)/10;
        nX1 = ImgV_TubeMaxPix*n/D_MaxLenTube + D_ImageOffsetX;
        eX1 = ImgV_TubeMaxPix*e/D_MaxLenTube + D_ImageOffsetX;
        // цвет
        if (massDefect[pos]==-1)
            c = clGray;
        if (massDefect[pos]==0)
            c = clLime;
        if (massDefect[pos]==1)
            c = clRed;
        ImageVisual->Canvas->Brush->Color = c;
        ImageVisual->Canvas->FillRect(Rect(nX1, nY1, eX1, eY1) );
    }
    ImageVisual->Canvas->Unlock();
    // сетка
    Img_Setka(ImageVisual, nX, nY, eX, eY, 15);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


