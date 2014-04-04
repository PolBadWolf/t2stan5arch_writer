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
#include "ColorBtn.hpp"
//---------------------------------------------------------------------------


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
    TADOConnection *ADOConnRead;
        TTimer *TimerStart;
    TPanel *Panel1;
    TPanel *Panel2;
    TLabel *Label_NumberTube;
    TPanel *Panel3;
    TPanel *Panel4;
    TLabel *Label_dTube;
    TMemo *Memo1;
    TShape *Shape_SENSOR_AT;
    TLabel *Label1;
    TADOConnection *ADOConnWrite;
    TButton *Button1;
    TTimer *TimerSecRead;
    TTimer *TimerSecWrite;
    TLabel *Label2;
    TLabel *Label3;
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall TimerStartTimer(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall TimerSecReadTimer(TObject *Sender);
    void __fastcall TimerSecWriteTimer(TObject *Sender);
private:	// User declarations
        bool FlNewTube;
        void __fastcall Img_ClearAll(TImage *Img);
        void __fastcall Img_Clear(TImage *Img, int nX, int nY, int eX, int eY, int m);
        void __fastcall Img_Setka(TImage *Img, int nX, int nY, int eX, int eY, int m);
        int  ImgV_TubeMaxPix;
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
    TColorBtn *Butt2;
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
        int  __fastcall ReadFromBDLastNumberTude(TADOConnection *connect);
        // **********
        int  __fastcall ReadFromBDLastParametrs(TADOConnection *connect, int *id_parametr, int *id_melt, AnsiString *CodeMelt, double *SizeTube);
        int  __fastcall ReadFromBDNewParametrs (TADOConnection *connect, int *id_parametr, int *id_melt, AnsiString *CodeMelt, double *SizeTube);
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
class TConnSec
{
private:
    int vSecRead;
    int vSecWrite;
    CRITICAL_SECTION secR;
    CRITICAL_SECTION secW;
    int __fastcall FrSecRead()
    {
        int x;
        EnterCriticalSection(&secR);
        x = vSecRead;
        LeaveCriticalSection(&secR);
        return x;
    }
    int __fastcall FrSecWrite()
    {
        int x;
        EnterCriticalSection(&secW);
        x = vSecWrite;
        LeaveCriticalSection(&secW);
        return x;
    }
    void __fastcall FwSecRead(int x)
    {
        EnterCriticalSection(&secR);
        vSecRead = x;
        LeaveCriticalSection(&secR);
    }
    void __fastcall FwSecWrite(int x)
    {
        EnterCriticalSection(&secW);
        vSecWrite = x;
        LeaveCriticalSection(&secW);
    }
public:
    __fastcall TConnSec()
    {
        InitializeCriticalSection(&secR);
        InitializeCriticalSection(&secW);
        vSecRead = 0;
        vSecWrite = 0;
    }
    __fastcall ~TConnSec()
    {
        DeleteCriticalSection(&secW);
        DeleteCriticalSection(&secR);
    }
    __property int SecRead ={read=FrSecRead,  write=FwSecRead};
    __property int SecWrite={read=FrSecWrite, write=FwSecWrite};
};
extern TConnSec vSec;
extern PACKAGE TForm1 *Form1;
extern bool UnicalFlDefect;
//---------------------------------------------------------------------------
#endif
