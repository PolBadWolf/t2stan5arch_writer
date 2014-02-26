//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "MShape.h"
#include <ADODB.hpp>
#include <DB.hpp>

class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TShape *Shape_KEY_BACK;
        TShape *Shape_KEY_FORWARD;
        TShape *Shape_TUBE_HERE_L;
        TShape *Shape_TUBE_HERE_R;
        TShape *Shape5;
        TShape *Shape6;
        TShape *Shape_WELD_DEFECT;
        TShape *Shape_MODE_CALIBROVKA;
        TShape *Shape_SENSOR_AT_TOP;
        TShape *Shape_SENSOR_AT_BOTTOM;
        TShape *Shape_Circle;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *Label10;
        TLabel *Label13;
        TLabel *Label14;
        TImage *ImageVisual;
        TLabel *Label17;
        TADOConnection *ADOConnection1;
        TShape *ShapeCircle1;
        TShape *ShapeCircle2;
        TLabel *Label11;
        TLabel *Label12;
        TADOQuery *ADOQuery1;
        TTimer *TimerStart;
    TLabel *Label_temp;
    TLabel *Label15;
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall TimerStartTimer(TObject *Sender);
private:	// User declarations
        bool FlNewTube;
        void __fastcall Img_ClearAll(TImage *Img);
        void __fastcall Img_Clear(TImage *Img, int nX, int nY, int eX, int eY, int m);
        void __fastcall Img_Setka(TImage *Img, int nX, int nY, int eX, int eY, int m);
        int  ImgV_TubeMaxPix;
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        TShape *LampDat[16];
        // === ������� ===
        // ��������� ��������� ��������
        void __fastcall EvaSensor(int sn, int lvl);
        // ��������� ��������� �������� ������
        void __fastcall EvaCircleSensor(int sn);
        // ��������� ��������� ������� ������������
        void __fastcall EvaSensorAt(int lvl);
        // ��������� ��������� ������� ������
        void __fastcall EvaWeldDefect(int lvl);
        // ��������� ��������� ������� ����������
        // 1 - ����������, 0 - ������
        void __fastcall EvaModeCalibrovka(int lvl, int fl_mod);
        // �������� ������
        void __fastcall EvaCircle(int Napravl, int Dlina, int Position, int *MassDefect);
        // ���������� �������������
        void __fastcall TubeEnd();
        void __fastcall TubeBegin();
        // offset from left sensor tube, unit mm        // ������ � �� �� ������ �������
        double otLmm;
        // offset from left sensor tube, unit segment   // ����������� ������ � �����
        int otStep;
        // count segment for hold find defect in tube   // ������� ��������� ������ ����� ���������� ������ �������
        int otCount;
        // len last tube                                // ����� ����� ����������
        int dTube;
        // curent number tube
        int     CurentNumberTube;
        int  __fastcall ReadFromBDLastNumberTude(TADOQuery *dQuery);
        // **********
        int  __fastcall ReadFromBDLastParametrs(TADOQuery *dQuery, int *id_parametr, int *id_melt, AnsiString *CodeMelt, double *SizeTube);
        int  __fastcall ReadFromBDNewParametrs (TADOQuery *dQuery, int *id_parametr, int *id_melt, AnsiString *CodeMelt, double *SizeTube);
        // curent diametr tube
        double CurentDiametrTube;
        // id last melt
        int  IdParam;
        AnsiString CodeMelt;
        double SizeTube;
        double LenSegmentTube;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
extern bool UnicalFlDefect;
//---------------------------------------------------------------------------
#endif
 