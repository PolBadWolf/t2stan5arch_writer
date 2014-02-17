//---------------------------------------------------------------------------

#ifndef MainFormUnit1H
#define MainFormUnit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TShape *Shape_Circle1;
        TShape *Shape_CircleZ1;
        TShape *Shape_CircleZ2;
        TShape *Shape_CircleZ3;
        TShape *Shape_CircleMark0;
        TButton *Button_CircleBack;
        TButton *Button_CircleStop;
        TButton *Button_CircleForev;
        TShape *Shape_CircleMark1;
        TShape *Shape_CircleMark2;
        TShape *Shape_CircleMark3;
        TTimer *Timer_Circle;
        TShape *Shape_Tube;
        TShape *Shape_StangaNiz;
        TShape *Shape_ShtangaVerh;
        TShape *Shape_DefPosDn;
        TShape *Shape_DefPosUp;
        TShape *Shape_Sensor1;
        TShape *Shape_Sensor2;
        TButton *Button_Defect;
        TButton *Button2;
        TEdit *Edit_TubeLen;
        TEdit *Edit_TubeShift;
        TSpeedButton *Button_DefPosUp;
        TSpeedButton *Button_DefPosDn;
        TShape *Shape_SetDefect;
        TTimer *Timer_SetDefect;
        TSpeedButton *SpeedButton_Obrazec;
        void __fastcall Button_CircleStopClick(TObject *Sender);
        void __fastcall Button_CircleBackClick(TObject *Sender);
        void __fastcall Button_CircleForevClick(TObject *Sender);
        void __fastcall Timer_CircleTimer(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button_DefPosUpClick(TObject *Sender);
        void __fastcall Button_DefPosDnClick(TObject *Sender);
        void __fastcall Button_DefectClick(TObject *Sender);
        void __fastcall Timer_SetDefectTimer(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall SpeedButton_ObrazecClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        // ================
        // сколько пикселей на мерт
        int  OneMetr;
        // процедура - труба в начало
        void __fastcall TubeGoBegin();
        // длина трубы в см
        int TubeLen;
        // позитция левого края трубы
        int TubePos;
        // отрисовка трубы
        void __fastcall TubeShow();
        // управление трубой стоп
        void __fastcall Upr_TubeStop();
        // направление движение трубы
        int TubeNaprav;
        int TubeReb;
        void __fastcall ShowRev();
        void __fastcall SensorCheck();
        // состояние сенсора
        int obj_sensor1;
        int obj_sensor2;
        // штанга дефектоскопа
        int obj_DefPosUp;
        int obj_DefPosDn;
        int Top_Shape_ShtangaVerh;
        int Top_Shape_StangaNiz;
        int ShtangaStatus;
        //
        void __fastcall PackOut(int t, int c, int z);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
