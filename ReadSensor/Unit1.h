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
#include "ComPort.h"
#include "MShape.h"
#include <ADODB.hpp>
#include <DB.hpp>

class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TShape *Shape_TUBE_HERE_L;
        TShape *Shape_TUBE_HERE_R;
        TShape *Shape_WELD_DEFECT;
    TShape *Shape_MODE_SAMPLE;
        TShape *Shape_SENSOR_AT_TOP;
        TShape *Shape_SENSOR_AT_BOTTOM;
        TShape *Shape_Circle;
        TImage *ImageVisual;
        TADOConnection *ADOConnection1;
        TADOQuery *ADOQuery1;
        TTimer *TimerStart;
    TPanel *Panel1;
    TPanel *Panel2;
    TLabel *Label_NumberTube;
    TPanel *Panel3;
    TPanel *Panel4;
    TLabel *Label_dTube;
    TMemo *Memo1;
    TShape *Shape_SENSOR_AT;
    TMemo *Memo2;
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
        /*
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
        void __fastcall ShowSensorSample(TShape *Lamp, int lvl);
        // offset from left sensor tube, unit mm        // ������ � �� �� ������ �������
        double otLmm;
        // offset from left sensor tube, unit segment   // ����������� ������ � �����
        int otStep;
        // count segment for hold find defect in tube   // ������� ��������� ������ ����� ���������� ������ �������
        int otCount;
        // len last tube                                // ����� ����� ����������
        int dTube;
        */
        double vLenTubeMM;
        int    vLenTubeSegment;
        // *********************************************************************************
        // Sensor Wild
        void __fastcall EvaSensorWild(int lvl);
        // *********************************************************************************
        // Sensors At Top
        void __fastcall EvaSensorAtTop(int lvl);
        // Sensors At Bottom
        void __fastcall EvaSensorAtBottom(int lvl);
        // Sensors At Show
        void __fastcall EvaSensorsAtShow(int n, int lvl);
        // *********************************************************************************
        // Sensors Tube here Show
        void __fastcall EvaSensorsTubeHereShow(int n, int lvl);
        // Sensors Tube here Tube begin
        double __fastcall EvaSensorTubeBegin();
        // Sensors Tube here Tube Len
        void __fastcall EvaSensorTubeLen(int segments, double lenght);
        // Sensors Tube here End Tube
        void __fastcall EvaSensorTubeEnd(int len, signed char *massDefect, int flagSample);
        // *********************************************************************************
        // Sensor Sample
        void __fastcall EvaSample(int trg, int lvl);
        // *********************************************************************************
        // Sensor Circle Show flash
        void __fastcall EvaCircleShow(int sn);
        // Sensor Circle move
        void __fastcall EvaCircleMove(signed char *massDefect, int curPosition);





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
        void __fastcall PortNewDate(TComPort *cPort, int RdByte);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
extern bool UnicalFlDefect;
//---------------------------------------------------------------------------
#endif
