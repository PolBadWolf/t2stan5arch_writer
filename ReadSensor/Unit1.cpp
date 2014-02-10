//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "BoxRead1.h"
#include "WriteFl.h"
#include "ViewKoleso.h"
#include "PapkaName.h"
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
        Caption = "версия от 2013.11.11";
        ViewKoleso = new TViewKoleso(Shape_Circle);
        FlNewTube = false;
        Img_ClearAll(ImageVisual);
        // Step2mm[0] = 91.381076;         //245
        Step2mm[0] = 90;
        Step2mm[1] = 93.509505;         //273
        Step2mm[2] = 97.829195;         //325
        Step2mm[3] = 100.326761;        //355.6
        Step2mm[4] = 104.681794;        //406.4
        Step2mm[5] = 106.103365;        //426
        Step2mm[6] = 113.332955;        //508
        Step2mm[7] = 115.060831;        //530
        Step2mmD = 0; // 245
        // отступ в мм от левого датчика
        otLmm = 365;
        // расчитанный отступ в шагах
        otStep = otLmm/Step2mm[Step2mmD];
        // счетчик окончания замера после отключения левого датчика
        otCount = 0;
        // длина трубы замеренной
        dTube = -1;
        //
        ADOConnection1->GetTableNames(ListBox1->Items, false);
        ADOQuery1->Active = false;
        ADOQuery1->SQL->Clear();
        ADOQuery1->SQL->Add("Select *");
        ADOQuery1->SQL->Add("FROM defectsdata");
        ADOQuery1->SQL->Add("ORDER BY IndexData DESC");
        ADOQuery1->SQL->Add("LIMIT 1");
        ADOQuery1->Open();

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
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
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
                Label17->Caption = (dTube-otStep)*Step2mm[Step2mmD];
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
void __fastcall TForm1::EvaCircle(int Napravl, int Dlina, int Position, unsigned char *MassDefect)
{
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
                                Label17->Caption = (dTube-otStep)*Step2mm[Step2mmD];
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
        Label14->Caption = (Position-otStep)*Step2mm[Step2mmD];
        /*
        char m[256], p[256];
        for(int i=otStep+1;i<Dlina;i++)
        {
                if (MassDefect[i]==255) m[i-(otStep+1)] = '8';
                else
                {
                        m[i-(otStep+1)] = MassDefect[i]+'0';
                }
                p[i-(otStep+1)]=' ';
        }
        m[Dlina-(otStep+1)] = 0;
        p[Position-1-(otStep+1)] = 'I';
        p[Dlina-(otStep+1)] = 0;
        Label15->Caption = (char*)m;
        Label16->Caption = (char*)p;
        */
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
        n = Step2mm[Step2mmD]*(Position+0-(otStep+1) );
        e = Step2mm[Step2mmD]*(Position+1-(otStep+1) );
        nY1 = nY;
        eY1 = eY-(eY-nY)/10;
        nX1 = ImgV_TubeMaxPix*n/D_MaxLenTube + D_ImageOffsetX;
        eX1 = ImgV_TubeMaxPix*e/D_MaxLenTube + D_ImageOffsetX;
        // цвет
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
        static int Len = 0;
        int  *Mass = BoxRead->MassDefect;       // массив соправожденных дефектов
        bool FlObr = BoxRead->FlModeCalibrovka; // флаг калибровки/образца
        Label14->Caption = "-//-";
        if (FlNewTube)
        {
                FlNewTube = false;
                Len       = BoxRead->MassDefectLen;    // длина сопровожденной трубы в сегментах
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TubeBegin()
{
        FlNewTube = true;
        // очистка имиджа + сетка
        int nX, nY, eX, eY;
        nX = D_ImageOffsetX + 1;
        nY = D_ImageOffsetY + 1;
        eX = ImageVisual->Width -1;
        eY = ImageVisual->Height -1;
        Img_Clear(ImageVisual, nX, nY, eX, eY, 15);
        Img_Setka(ImageVisual, nX, nY, eX, eY, 15);
        // =====================================================
        // чтение индексов 
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
//---------------------------------------------------------------------------




