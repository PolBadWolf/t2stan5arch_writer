//---------------------------------------------------------------------------

#ifndef BoxRead1H
#define BoxRead1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
#include "ComPort.h"

#define BoxRead_AllNumersDat    14
#define BoxRead_MassPackLen     7

#define BoxReadMASSPACK_HEAD1   0
#define BoxReadMASSPACK_HEAD2   1
#define BoxReadMASSPACK_TYPE    2
#define BoxReadMASSPACK_SENSR   3
#define BoxReadMASSPACK_LEVEL   4
#define BoxReadMASSPACK_CRC     5
#define BoxReadMASSPACK_TAIL    6

#define BoxReadMassDefect_MAX   256

class TNBoxRead : public TThread
{
public:
        __fastcall TNBoxRead();
private:
        //=============================
        //        ini
        AnsiString      PortName;
        eBaudRate       PortBaud;
        AnsiString      PathName;
        //=============================
        TComPort *Port;
        //=============================
        // ������� ������ �������� � ��������� ��������
        bool dat_FlInv[BoxRead_AllNumersDat];
        bool dat_St[BoxRead_AllNumersDat];
        //=============================
        // ������ ��� ������� ������� �� ������
        unsigned char MassPack[BoxRead_MassPackLen];
protected:
        void __fastcall Execute();
        // ��������� �� ������ �������� �����
        void __fastcall ErrNewPort();
        // ��������� �� ������ �������� �����
        void __fastcall ErrOpenPort();
        //=============================
        // ������ ����� ������
        void __fastcall EventNewData(int RdByte);
        int     kik;
        // ������� ������� �� ������
        void __fastcall SelectPak(unsigned char *b, int len);
        // ��������� ������
        void __fastcall SelectDo();
        //=============================
        void __fastcall DoCirle(int sn, int lv);
        // ������� ���������
        int  count;
        // ������ ��������
        int  MassDefectCur[BoxReadMassDefect_MAX];
        // ����� ������� ����� � ���������
        int  count_max;
        void __fastcall TubeHere(int sn, int lv);
        bool Here1, OldHere1;
        bool Here2, OldHere2;
        //
        void __fastcall TubeHere1Up();
        void __fastcall TubeHere1Dn();

        void __fastcall TubeHere12Up();
        void __fastcall TubeHere12Dn();

        void __fastcall TubeHere1Dn2();
        void __fastcall TubeHere1Up2();

        void __fastcall TubeHere2Dn();
        void __fastcall TubeHere2Up();

        // ����� � ���� ������������
        bool TubeInDefect; // 1 - � ���� ������������, 0 - ��� ���� ������������
        //======================
        // ��������� �������
        void __fastcall SensorAt(int sn, int lv);
        bool SensorAtTr; // 0 - �� �����, 1 - ��� ������
        void __fastcall SetSummDefect();
        //======================
        // ������
        void __fastcall WeldDefect(int sn, int lv);
        bool SummDefect; // 0 - ��� �������, 1 - ������
        //======================
        // ����������
        void __fastcall ModeCalibrovka(int sn, int lv);
        bool TrModeCalibrovka; // 1 - ����� ����������, 0 - ������� �����
public:
        //==============================================
        //       ������� � ��������� ������� �����
        void __fastcall (__closure *EvTubeHereBeginUp)(); // ����� ����� � ������*
        void __fastcall (__closure *EvTubeHereBeginDn)(); // ����� ����� � ������
        void __fastcall (__closure *EvTubeHereEndUp)();   // ����� ����� � �����
        void __fastcall (__closure *EvTubeHereEndDn)();   // ����� ����� � �����**
        // *  - ������ ������ ��������
        // ** - ����� ������, ������ �������� ����� ��������
        // ������ ��������
        signed char  MassDefect[BoxReadMassDefect_MAX]; // ������ �������������� ��������
        int  MassDefectLen;                     // ����� �������������� ����� � ���������
        //          ���� ���������� ����� � ���� ������������� (�� ���� ��������): 1 - � ���� ������������, 0 - ��� ���� ������������
        __property bool FlTubeInDefect={read=TubeInDefect};

        // ��������� ������� : 0 - �� �����, 1 - ��� ������
        void __fastcall (__closure *EvSensorAt)(int lvl);

        // ���� ���������� �������� ������������ (������) : 0 - �� �����, 1 - ��� ������
        __property bool FlSensorAt={read=SensorAtTr};

        // ������� - �������� ������
        // Napravl    - ����������� �������� ����� : 0 - ������, 1 - �����
        // Dlina      - ������� ����� ����� � ���������
        // Position   - ������� �������� ������������ �� �����
        // MassDifect - ������ ��������
        void __fastcall (__closure *EvCircle)(int Napravl, int Dlina, int Position, int *MassDefect);
        // ��������� ��������� ��������
        // sn - ����� �������
        // lv - ������� : 0 - �����������, 1 �������������
        void __fastcall (__closure *EvSensor)(int sn, int lv);
        // ��������� ��������� �������� ������
        // sn - ����� �������
        void __fastcall (__closure *EvCircleSensor)(int sn);
        // ������� ������ lv : 0 - ��� ������� 1 - ������
        void __fastcall (__closure *EvWeldDefect)(int lv);
        // ������� ����� ���������� : 1 - �������, 0 - ��������
        void __fastcall (__closure *EvModeCalibrovka)(int lv, int fl_mod);
        // ���� ������ ������������
        __property bool FlModeCalibrovka={read=TrModeCalibrovka};
        // ������ ������� ��������
        bool BoxReadMassSensorsLevel[16];

        __property int  Count={read=count};
};

#endif


