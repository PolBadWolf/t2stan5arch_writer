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
TColor  ButtonColorDefault;
int D_MaxLenTube;
int D_LenRull;
int C_Begin, C_End, C_Seg;
#define D_ImageOffsetX 5
#define D_ImageOffsetY 20
bool UnicalFlDefect = false;

TConnSec vSec;

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
    Form1->DoubleBuffered = true;
    Caption = "версия от 2014.04.03";
    Form1->Constraints->MaxWidth  = Form1->Width;
    Form1->Constraints->MinWidth  = Form1->Width;
    Form1->Constraints->MaxHeight = Form1->Height;
    Form1->Constraints->MinHeight = Form1->Height;
    ViewKoleso = new TViewKoleso(Shape_Circle);
    FlNewTube = false;
    //
    Butt2 = new TColorBtn(Form1);
    Butt2->Parent = Button1->Parent;
    Butt2->Top = Button1->Top;
    Butt2->Width = Button1->Width;
    Butt2->Left = Button1->Left;
    Butt2->Height = Button1->Height;
    Butt2->Caption = Button1->Caption;
    Butt2->OnClick = Button1->OnClick;
    ButtonColorDefault = Butt2->ButtonColor;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if (Port)
    {
        Port->Close();
        delete Port;
        Port = NULL;
    }
    // checked close ADOConnection1
    if ( ADOConnRead->Connected )
            ADOConnRead->Close();
    if ( ADOConnWrite->Connected )
            ADOConnWrite->Close();
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
void __fastcall TForm1::Img_ClearAll(TImage *Img)
{
    ImgV_TubeMaxPix = Img->Width - D_ImageOffsetX;
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
    int m = D_LenRull;
    int h;
    AnsiString sstt;
    for(int x=0;x<(m+1);x++)
    {
        Img->Canvas->Pen->Width = 3;
        Img->Canvas->Pen->Color = clGreen;
        h = D_ImageOffsetX+x*(ImgV_TubeMaxPix/(D_MaxLenTube/1000));
        Img->Canvas->MoveTo(h, D_ImageOffsetY);
        Img->Canvas->LineTo(h, D_ImageOffsetY-(D_ImageOffsetY/3));
        Img->Canvas->Font->Size = 9;
        Img->Canvas->Font->Color = clYellow;
        sstt = IntToStr(x);
        Img->Canvas->TextOutA(h - Img->Canvas->TextWidth(sstt)/2 , D_ImageOffsetY-(D_ImageOffsetY/3)-Img->Canvas->TextHeight(sstt) , sstt);
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
    int kn, ke, kd;
    const int c = C_Seg;
    for (int x=0; x<m; x++)
    {
        kn = (x+0)*(ImgV_TubeMaxPix/(D_MaxLenTube/1000) );
        ke = (x+1)*(ImgV_TubeMaxPix/(D_MaxLenTube/1000) );
        kd = ke - kn;
        for (int x1=0; x1<c; x1++)
        {
            Img->Canvas->Pen->Width = 1;
            Img->Canvas->Pen->Color = clGreen;
            Img->Canvas->MoveTo(nX+ kn+ x1*kd/(c-1) -1 ,nY-10);
            Img->Canvas->LineTo(nX+ kn+ x1*kd/(c-1) -1 ,eY);
        }
    }
    const int d = 6;
    for (int y=0; y<d; y++)
    {
        Img->Canvas->MoveTo(nX, nY+y*(eY-nY)/(d-1) );
        Img->Canvas->LineTo(eX, nY+y*(eY-nY)/(d-1) );
    }
    ImageVisual->Canvas->Unlock();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TimerStartTimer(TObject *Sender)
{
    ((TTimer*)Sender)->Enabled = false;
    // ===========================================================================
    // Provider=MSDASQL.1;Persist Security Info=False;Data Source=T2Stan5MyCon
    TIniFile *ifile = new TIniFile( ChangeFileExt( Application->ExeName, ".ini" ) );
    // checked open ini file
    if (!ifile)
    {
        ShowMessage("Ошибка открытия ini файла");
        Form1->Close();
        return;
    }
    int   fDebugMode;
    int   lTubeHere1,    lTubeHere2;
    AnsiString bdSource;
    // read parametrs : len rull
    D_LenRull    =          ifile->ReadInteger("Ruler",       "Len",       15);
    // read parametrs : com port
    PortName   =            ifile->ReadString ("comm",        "Name",      "COM1" );
    PortBaud   = (eBaudRate)ifile->ReadInteger("comm",        "Baud",      CBR_38400);
    PortParity = (eParity)  ifile->ReadInteger("comm",        "Parity",    NO);
    // read parametrs : system debug
    fDebugMode =            ifile->ReadInteger("system",      "DebugMode", 0);
    bdSource =              ifile->ReadString("system",       "NameOBDC",  "T2Stan5MyCon");
    // read parametrs : lenght from sensor defectoskop to sensors tube here
    lTubeHere1 =            ifile->ReadInteger("SensorsTube", "Here1",     720);
    lTubeHere2 =            ifile->ReadInteger("SensorsTube", "Here2",     1000);
    // read parametrs : no control tube
    C_Begin =               ifile->ReadInteger("NoControlTube", "Begin",   2);
    C_End =                 ifile->ReadInteger("NoControlTube", "End",     2);
    // read parametrs : n lines to metr
    C_Seg =                 ifile->ReadInteger("LinesMetr",     "n",       5);
        // ----------------------------------------------------
    // write parametrs : len rull
    ifile->WriteInteger("Ruler",       "Len",       D_LenRull);
    // write parametrs : com port
    ifile->WriteString ("comm",        "Name",      PortName);
    ifile->WriteInteger("comm",        "Baud",      PortBaud);
    ifile->WriteInteger("comm",        "Parity",    PortParity);
    // write parametrs : system debug
    ifile->WriteInteger("system",      "DebugMode", fDebugMode);
    ifile->WriteString ("system",      "NameOBDC",  bdSource);
    // write parametrs : lenght from sensor defectoskop to sensors tube here
    ifile->WriteInteger("SensorsTube", "Here1",     lTubeHere1);
    ifile->WriteInteger("SensorsTube", "Here2",     lTubeHere2);
    // write parametrs : no control tube
    ifile->WriteInteger("NoControlTube", "Begin",   C_Begin);
    ifile->WriteInteger("NoControlTube", "End",     C_End);
    // read parametrs : n lines to metr
    ifile->WriteInteger("LinesMetr",     "n",       C_Seg);
    // close ini file
    delete ifile;
    ADOConnRead->ConnectionString = "Provider=MSDASQL.1;Persist Security Info=False;Data Source=" + bdSource;
    ADOConnWrite->ConnectionString = ADOConnRead->ConnectionString;
    D_MaxLenTube = D_LenRull * 1000;
    // ===========================================================================
            Img_ClearAll(ImageVisual);
    // ===========================================================================
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
    //ADOQuery1->Close();
    // ====================================================
    // read last number tube ( no sample )
    WriteLog->Push("read number tube");
    CurentNumberTube = ReadFromBDLastNumberTude(ADOConnRead);
    if (CurentNumberTube<0)
    {   // Date Base Error
        Application->MessageBox("Not found field \"NumberTube\" " , "Date Base Error", 0);
        Form1->Close();
        return;
    }
    // ====================================================
    // read last parametrs
    WriteLog->Push("read Last param BD");
    if ( !ReadFromBDLastParametrs(ADOConnRead, &IdParamLast, &IdMeltLast, &CodeMeltLast, &SizeTubeLast) )
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
    WriteLog->Push("show param");
    // Show_Parametrs(CurentNumberTube, SizeTube, LenSegmentTube, otStep, CodeMelt);
// ******************************************************************************************
// *************************** init Massibe indications *************************************
    WriteLog->Push("init massive shape");
// ******************************************************************************************
// Initialize com port
    //ifile = NULL;
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
    //BoxRead = new TBoxRead;
    BoxRead = new TBoxRead(lTubeHere1, lTubeHere2);
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
    if (fDebugMode==0)
        WriteLog->Push("log off");
    else
        WriteLog->Push("log on");
    WriteLog->debugMode = fDebugMode;
// ******************************************************************************************
// *************************** delete starting timer ****************************************
    delete ((TTimer*)Sender);
    TimerSecRead->Enabled = true;
    TimerSecWrite->Enabled = true;
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
    Shape_SENSOR_AT->Top = 0;
}
//---------------------------------------------------------------------------
// Sensors At Bottom
void __fastcall TForm1::EvaSensorAtBottom(int lvl)
{
    Shape_SENSOR_AT->Brush->Color = clLime;
    Shape_SENSOR_AT->Top = 56;
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
    WriteLog->Push("'TForm1::TubeBegin': ReadFromBDLastParametrs");
    statusLast = ReadFromBDLastParametrs(ADOConnRead, &IdParamLast, &IdMeltLast, &CodeMeltLast, &SizeTubeLast);
    WriteLog->Push("'TForm1::TubeBegin': ReadFromBDNewParametrs");
    statusNew  = ReadFromBDNewParametrs (ADOConnRead, &IdParamNew , &IdMeltNew , &CodeMeltNew , &SizeTubeNew );
    WriteLog->Push("'TForm1::TubeBegin': end ReadFromBD");
    //
    // checked load parametrs
    if ( (statusLast && statusNew) || statusNew )
    {   // error read parametrs
        if ( statusNew==-1 )
        {   // show error BD
            Butt2->ButtonColor = clRed;
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
    Show_Parametrs(CurentNumberTube, SizeTube, LenSegmentTube, CodeMelt);
    Show_LenTubeClear();
    // ==============================
    WriteLog->Push("'TForm1::TubeBegin': clear picture");
    // очистка имиджа + сетка
    int nX, nY, eX, eY;
    nX = D_ImageOffsetX + 1;
    nY = D_ImageOffsetY + 1;
    eX = ImageVisual->Width -1;
    eY = ImageVisual->Height -1;
    Img_Clear(ImageVisual, nX, nY, eX, eY, D_LenRull);
    Img_Setka(ImageVisual, nX, nY, eX, eY, D_LenRull);
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
    WriteLog->Push("'TForm1::TubeEnd': event");
    // =========================================================
    // no parametrs - no record
    if (!IdParam)
    {
        WriteLog->Push("'TForm1::TubeEnd': BD clear - no write tube");
        Butt2->ButtonColor = clRed;
        return;
    }
    // =========================================================
    // flag tube defect
    int  FlagDefectTube = 0;
    int  FlagNoControlTube = 0;
    int  nTube;
    // =========================================================
    // Number tube +1 if no sample
    if ( flagSample )
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
    int zn;//
    for (int i=0;i<len;i++)
    {
        zn = massDefect[i];
        if ( zn>0 )
            FlagDefectTube = 1;
        if ( (i>=C_Begin) && (i<(len-C_End)) )
        {
            if (zn<0)
                FlagNoControlTube = 1;
        }
    }
    if (FlagNoControlTube!=0)
        FlagDefectTube = 2;
    // =========================================================
    // set
    TDateTime DtTm = Now();
    AnsiString vDate = FormatDateTime("yyyy-mm-dd", DtTm);
    // write to BD
    WriteLog->Push("'TForm1::TubeEnd': write tube");
    try {
        //WriteBD_Datas(ADOConnWrite, nTube, massDefect, len, FlagDefectTube, IdParam);
        WriteBD_Datas(ADOConnRead, nTube, massDefect, len, FlagDefectTube, IdParam);
        WriteFlTree(ADOConnRead);
    } catch(...)
    {
        Butt2->ButtonColor = clYellow;
    }
    WriteLog->Push("'TForm1::TubeEnd': show numbertube & reset flag new tube");
    Show_NumberTube(nTube);
}
//---------------------------------------------------------------------------
// Sensor Sample
void __fastcall TForm1::EvaSample(int trg, int lvl)
{
    TColor clCvet;
    int clCvetF;
    if (!lvl)
        clCvet = clLime;
    else
        clCvet = (trg)?clGreen:clWhite;
    Shape_MODE_SAMPLE->Brush->Color = clCvet;
    clCvetF = 0;
    for (int i=0; i<3; i++)
        clCvetF = clCvetF +((unsigned char *) & clCvet)[i];
    clCvetF = clCvetF/2;
    if (clCvetF<110)
        Label1->Font->Color = clWhite;
    else
        Label1->Font->Color = clBlack;
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
    Img_Setka(ImageVisual, nX, nY, eX, eY, D_LenRull);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
    ((TColorBtn*)Sender)->ButtonColor = ButtonColorDefault;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TimerSecReadTimer(TObject *Sender)
{
    int x = vSec.SecRead+1;
    if ( x>=Sec_Porog )
    {
        try
        {
            WriteLog->Push("'TForm1::SecRead': enter to OBDC reconnect");
            ADOConnRead->Cancel();
            ADOConnRead->Close();
            ADOConnRead->Open();
            vSec.SecRead = 0;
            TimerSecRead->Interval = 1000;
            WriteLog->Push("'TForm1::SecRead': OBDC reconnect");
        }
        catch(...)
        {
            WriteLog->Push("'TForm1::SecRead': fail OBDC reconnect");
            TimerSecRead->Interval = 60000;
        }
    }
    else
        vSec.SecRead = x;
    //Label2->Caption = vSec.SecRead;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TimerSecWriteTimer(TObject *Sender)
{
    int x = vSec.SecWrite+1;
    if ( x>=Sec_Porog )
    {
        try
        {
            WriteLog->Push("'TForm1::SecWrite': enter to OBDC reconnect");
            ADOConnWrite->Cancel();
            ADOConnWrite->Close();
            ADOConnWrite->Open();
            vSec.SecWrite = 0;
            TimerSecRead->Interval = 1000;
            WriteLog->Push("'TForm1::SecWrite': OBDC reconnect");
        }
        catch(...)
        {
            WriteLog->Push("'TForm1::SecWrite': fail OBDC reconnect");
            TimerSecRead->Interval = 60000;
        }
    }
    else
        vSec.SecWrite = x;
    //Label3->Caption = vSec.SecWrite;
}
//---------------------------------------------------------------------------

