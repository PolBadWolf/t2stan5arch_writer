//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainFormUnit1.h"
#include "ComPort.h"
#include "crc8.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

#define BoxRead_MassPackLen     7
#define BoxReadMASSPACK_HEAD1   0
#define BoxReadMASSPACK_HEAD2   1
#define BoxReadMASSPACK_TYPE    2
#define BoxReadMASSPACK_SENSR   3
#define BoxReadMASSPACK_LEVEL   4
#define BoxReadMASSPACK_CRC     5
#define BoxReadMASSPACK_TAIL    6
unsigned char MassPack[BoxRead_MassPackLen];
#define CIRCLE_FORWARD          0       // колесо импульс вперед *
#define CIRCLE_BACK             1       // колесо импульс назад  *
#define KEY_BACK                0       // рольганг назад (ключ)
#define KEY_FORWARD             1       // рольганг вперед (ключ)
#define TUBE_HERE1              2       // наличие трубы д1 *
#define TUBE_HERE2              3       // наличие трубы д2 *
#define WELD_DEFECT             6       // дефект
#define MODE_CALIBROVKA         7       // калибровка
#define SENSOR_AT_TOP           8       // датчики дефектоскопа подняты *
#define SENSOR_AT_BOTTOM        9       // датчики дефектоскопа опущены *

TComPort *Port = NULL;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
        Port = new TComPort;
        Port->Open("COM3", BaudRate38400, NO);
        // сколько пикселей на метр
        OneMetr = abs(Shape_Sensor2->Left - Shape_Sensor1->Left);
        // состояние сенсора
        obj_sensor1 = 0;
        obj_sensor2 = 0;
        // штанга
        Top_Shape_ShtangaVerh = Shape_ShtangaVerh->Top;
        Top_Shape_StangaNiz   = Shape_StangaNiz->Top;
        Shape_ShtangaVerh->Top = Shape_ShtangaVerh->Top - 40;
        Shape_StangaNiz->Top   = Shape_StangaNiz->Top - 40;
        //
        TubeReb = 0;
        ShowRev();
        // процедура - труба в начало
        TubeGoBegin();
}
//---------------------------------------------------------------------------
// процедура - труба в начало
void __fastcall TForm1::TubeGoBegin()
{
        // установить длину трубы (длина в см)
        TubeLen = Edit_TubeLen->Text.ToInt();
        // позитция трубы на метр левее от первого датчика
        TubePos = - TubeLen - 20;
        // отрисовка трубы
        TubeShow();
        // управление трубой стоп
        Upr_TubeStop();
        SensorCheck();
}
//---------------------------------------------------------------------------
// отрисовка трубы
void __fastcall TForm1::TubeShow()
{
        int b = ( TubePos * OneMetr / 100 ) + Shape_Sensor1->Left;
        int d = ( TubeLen * OneMetr / 100 );
        Shape_Tube->Left = b;
        Shape_Tube->Width = d;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button_CircleStopClick(TObject *Sender)
{
        // управление трубой стоп
        Upr_TubeStop();
}
//---------------------------------------------------------------------------
// управление трубой стоп
void __fastcall TForm1::Upr_TubeStop()
{
        Timer_Circle->Enabled = false;
        // направление движение трубы
        TubeNaprav = 0;
        ShowRev();
        // кнопки подъема/опускания дефектоскопа
        Button_DefPosUp->Enabled = true;
        Button_DefPosDn->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ShowRev()
{
        switch (TubeReb)
        {
                case 0 : Shape_CircleMark0->Visible = true;
                         Shape_CircleMark1->Visible = false;
                         Shape_CircleMark2->Visible = false;
                         Shape_CircleMark3->Visible = false;
                         break;
                case 1 : Shape_CircleMark0->Visible = false;
                         Shape_CircleMark1->Visible = true;
                         Shape_CircleMark2->Visible = false;
                         Shape_CircleMark3->Visible = false;
                         break;
                case 2 : Shape_CircleMark0->Visible = false;
                         Shape_CircleMark1->Visible = false;
                         Shape_CircleMark2->Visible = true;
                         Shape_CircleMark3->Visible = false;
                         break;
                case 3 : Shape_CircleMark0->Visible = false;
                         Shape_CircleMark1->Visible = false;
                         Shape_CircleMark2->Visible = false;
                         Shape_CircleMark3->Visible = true;
                         break;
               default : Shape_CircleMark0->Visible = true;
                         Shape_CircleMark1->Visible = true;
                         Shape_CircleMark2->Visible = true;
                         Shape_CircleMark3->Visible = true;
        }
        if (TubeNaprav==0)
                Shape_Circle1->Brush->Color = clRed;
        else
                Shape_Circle1->Brush->Color = clLime;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button_CircleBackClick(TObject *Sender)
{
        Timer_Circle->Enabled = true;
        // направление движение трубы
        TubeNaprav = -1;
        ShowRev();
        // кнопки подъема/опускания дефектоскопа
        Button_DefPosUp->Enabled = false;
        Button_DefPosDn->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button_CircleForevClick(TObject *Sender)
{
        Timer_Circle->Enabled = true;
        // направление движение трубы
        TubeNaprav = 1;
        ShowRev();
        // кнопки подъема/опускания дефектоскопа
        Button_DefPosUp->Enabled = false;
        Button_DefPosDn->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_CircleTimer(TObject *Sender)
{
        int Tl = TubePos;
        int Tr = Tl + TubeLen;
        // останов
        if (TubeNaprav==0)
        {
                Upr_TubeStop();
                return;
        }
        if (TubeNaprav<0)
        {
                if (Tr<=-100)
                {
                        Upr_TubeStop();
                        return;
                }
                TubeNaprav = -1;
        }
        if (TubeNaprav>0)
        {
                if (Tl>=100)
                {
                        Upr_TubeStop();
                        return;
                }
                TubeNaprav  = 1;
        }
        // барабан
        TubeReb = TubeReb + TubeNaprav;
        if (TubeReb<0) TubeReb = 3;
        if (TubeReb>3) TubeReb = 0;
        // Show Revolver-Baraban
        ShowRev();
        // сдвиг трубы
        if (TubeNaprav<0)
        {
                TubePos = TubePos - Edit_TubeShift->Text.ToInt();
                // вызов события колеса // назад
                PackOut(0, CIRCLE_BACK, 1);
        }
        if (TubeNaprav>0)
        {
                TubePos = TubePos + Edit_TubeShift->Text.ToInt();
                // вызов события колеса вперед
                PackOut(0, CIRCLE_FORWARD, 1);
        }
        TubeShow();
        // ======================
        // Show Sensors & packed sensors
        SensorCheck();
        Timer_Circle->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SensorCheck()
{
        int Tl = TubePos;
        int Tr = Tl + TubeLen;
        int Cl = ((Shape_Sensor1->Width/2) + 0)*100/OneMetr ;
        int Cr = ((Shape_Sensor2->Width/2) + Shape_Sensor2->Left
                                           - Shape_Sensor1->Left)*100/OneMetr ;
        int cur_sensor1 = 0;
        int cur_sensor2 = 0;
        // левый датчик
        if ( (Tl<Cl) && (Tr>Cl) )
        {
                // на левом датчике
                Shape_Sensor1->Brush->Color = clLime;
                cur_sensor1 = 0;
        }
        else
        {
                // мимо
                Shape_Sensor1->Brush->Color = clRed;
                cur_sensor1 = 1;
        }
        // правый датчик
        if ( (Tl<Cr) && (Tr>Cr) )
        {
                // на левом датчике
                Shape_Sensor2->Brush->Color = clLime;
                cur_sensor2 = 0;
        }
        else
        {
                // мимо
                Shape_Sensor2->Brush->Color = clRed;
                cur_sensor2 = 1;
        }
        // начало трубы на датчике
        if ( !cur_sensor1 && obj_sensor1 && cur_sensor2 && obj_sensor2 )
        {
                // событие : состояние образец
                PackOut(1, MODE_CALIBROVKA, !SpeedButton_Obrazec->Down);
        }
        // событие
        if (obj_sensor1!=cur_sensor1)
        {
                obj_sensor1 = cur_sensor1;
                // вызов события
                PackOut(1, TUBE_HERE1, obj_sensor1);
        }
        // событие
        if (obj_sensor2!=cur_sensor2)
        {
                obj_sensor2 = cur_sensor2;
                // вызов события
                PackOut(1, TUBE_HERE2, obj_sensor2);
        }
        // датчики подъема/опускания дефектоскопа
        if (obj_DefPosUp!=Button_DefPosUp->Down)
        {
                obj_DefPosUp = Button_DefPosUp->Down;
                // событие
                PackOut(1, SENSOR_AT_TOP, !obj_DefPosUp);
        }
        Shape_DefPosUp->Brush->Color = (Button_DefPosUp->Down)?clLime:clRed;
        // Button_DefPosDn->Enabled = false;
        if (obj_DefPosDn!=Button_DefPosDn->Down)
        {
                obj_DefPosDn = Button_DefPosDn->Down;
                // событие
                PackOut(1, SENSOR_AT_BOTTOM, !obj_DefPosDn);
        }
        Shape_DefPosDn->Brush->Color = (Button_DefPosDn->Down)?clLime:clRed;
        // штанга верх
        if ( obj_DefPosUp && !obj_DefPosDn)
        {
                Shape_ShtangaVerh->Top = Top_Shape_ShtangaVerh - 50;
                Shape_StangaNiz->Top   = Top_Shape_StangaNiz   - 50;
                ShtangaStatus = 0;
                Shape_StangaNiz->Brush->Color = clWhite;
        }
        if ( obj_sensor1 && obj_sensor2 )
        {
                Shape_StangaNiz->Brush->Color = clWhite;
        }
        // штанга вниз
        if ( !obj_DefPosUp && obj_DefPosDn)
        {
                Shape_ShtangaVerh->Top = Top_Shape_ShtangaVerh;
                Shape_StangaNiz->Top   = Top_Shape_StangaNiz;
                ShtangaStatus = 1;
        }
        if ( ShtangaStatus && (!obj_sensor1 || !obj_sensor2) )
        {
                Button_Defect->Enabled = true;
        }
        else
        {
                Button_Defect->Enabled = false;
                Shape_StangaNiz->Brush->Color = clWhite;
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
        // процедура - труба в начало
        TubeGoBegin();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button_DefPosUpClick(TObject *Sender)
{
        SensorCheck();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button_DefPosDnClick(TObject *Sender)
{
        SensorCheck();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button_DefectClick(TObject *Sender)
{
        Timer_SetDefect->Enabled = true;
        Button_Defect->Enabled   = false;
        Shape_SetDefect->Brush->Color = clRed;
        // событие
        PackOut(1, WELD_DEFECT, 0);
        Timer_SetDefect->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_SetDefectTimer(TObject *Sender)
{
        Timer_SetDefect->Enabled = false;
        //Button_Defect->Enabled   = true;
                // событие
                PackOut(1, WELD_DEFECT, 1);
        if ( ShtangaStatus && !obj_sensor1 && !obj_sensor2 )
        {
                Shape_SetDefect->Brush->Color = clLime;
        }
        SensorCheck();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PackOut(int t, int c, int z)
{
        MassPack[BoxReadMASSPACK_HEAD1] = 0xe6;
        MassPack[BoxReadMASSPACK_HEAD2] = 0x16;
        MassPack[BoxReadMASSPACK_TYPE]  = t;
        MassPack[BoxReadMASSPACK_SENSR] = c;
        MassPack[BoxReadMASSPACK_LEVEL] = z;
        MassPack[BoxReadMASSPACK_CRC]   = crc8_buf(MassPack, BoxReadMASSPACK_CRC);
        MassPack[BoxReadMASSPACK_TAIL]  = 0x00;
        Port->Write(MassPack, BoxRead_MassPackLen);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
        delete Port;        
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton_ObrazecClick(TObject *Sender)
{
        // событие : состояние образец
        PackOut(1, MODE_CALIBROVKA, !SpeedButton_Obrazec->Down);
}
//---------------------------------------------------------------------------

