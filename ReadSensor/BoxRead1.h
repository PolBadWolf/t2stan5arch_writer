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
        // массивы флагов инверсии и состояния датчиков
        bool dat_FlInv[BoxRead_AllNumersDat];
        bool dat_St[BoxRead_AllNumersDat];
        //=============================
        // массив для выборки пакетов из потока
        unsigned char MassPack[BoxRead_MassPackLen];
protected:
        void __fastcall Execute();
        // сообщение об ошибке создания порта
        void __fastcall ErrNewPort();
        // сообщение об ошибке открытия порта
        void __fastcall ErrOpenPort();
        //=============================
        // пришли новые данные
        void __fastcall EventNewData(int RdByte);
        int     kik;
        // выборка пакетов из потока
        void __fastcall SelectPak(unsigned char *b, int len);
        // обработка пакета
        void __fastcall SelectDo();
        //=============================
        void __fastcall DoCirle(int sn, int lv);
        // счетчик сегментов
        int  count;
        // массив дефектов
        int  MassDefectCur[BoxReadMassDefect_MAX];
        // длина текущей трубы в сегментах
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

        // труба в зоне дефектоскопа
        bool TubeInDefect; // 1 - в зоне дефектоскопа, 0 - вне зоны дефектоскопа
        //======================
        // положение сенсора
        void __fastcall SensorAt(int sn, int lv);
        bool SensorAtTr; // 0 - на трубе, 1 - над трубой
        void __fastcall SetSummDefect();
        //======================
        // дефект
        void __fastcall WeldDefect(int sn, int lv);
        bool SummDefect; // 0 - нет дефекта, 1 - дефект
        //======================
        // калибровка
        void __fastcall ModeCalibrovka(int sn, int lv);
        bool TrModeCalibrovka; // 1 - режим калибровка, 0 - обычный режим
public:
        //==============================================
        //       события с датчиками наличия трубы
        void __fastcall (__closure *EvTubeHereBeginUp)(); // въезд трубы в начале*
        void __fastcall (__closure *EvTubeHereBeginDn)(); // съезд трубы в начале
        void __fastcall (__closure *EvTubeHereEndUp)();   // наезд трубы в конце
        void __fastcall (__closure *EvTubeHereEndDn)();   // съезд трубы в конце**
        // *  - начало записи дефектов
        // ** - конец записи, массив дефектов можно забирать
        // массив дефектов
        signed char  MassDefect[BoxReadMassDefect_MAX]; // массив соправожденных дефектов
        int  MassDefectLen;                     // длина сопровожденной трубы в сегментах
        //          флаг нахождение трубы в зоне дефектоскопов (на двух датчиках): 1 - в зоне дефектоскопа, 0 - вне зоны дефектоскопа
        __property bool FlTubeInDefect={read=TubeInDefect};

        // положение сенсора : 0 - на трубе, 1 - над трубой
        void __fastcall (__closure *EvSensorAt)(int lvl);

        // флаг нахождение датчиков дефектоскопа (штанга) : 0 - на трубе, 1 - над трубой
        __property bool FlSensorAt={read=SensorAtTr};

        // событие - сработка колеса
        // Napravl    - направление движение трубы : 0 - вперед, 1 - назад
        // Dlina      - текущая длина трубы в сегментах
        // Position   - текущая позитция дефектоскопа на трубе
        // MassDifect - массив дефектов
        void __fastcall (__closure *EvCircle)(int Napravl, int Dlina, int Position, int *MassDefect);
        // изменение состояния датчиков
        // sn - номер датчика
        // lv - уровень : 0 - сработанный, 1 несработанный
        void __fastcall (__closure *EvSensor)(int sn, int lv);
        // изменение состояния датчиков колеса
        // sn - номер датчика
        void __fastcall (__closure *EvCircleSensor)(int sn);
        // событие дефект lv : 0 - нет дефекта 1 - дефект
        void __fastcall (__closure *EvWeldDefect)(int lv);
        // событие режим калибровки : 1 - включен, 0 - выключен
        void __fastcall (__closure *EvModeCalibrovka)(int lv, int fl_mod);
        // флаг режима дефектоскопа
        __property bool FlModeCalibrovka={read=TrModeCalibrovka};
        // массив уровней датчиков
        bool BoxReadMassSensorsLevel[16];

        __property int  Count={read=count};
};

#endif


