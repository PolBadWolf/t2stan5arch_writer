//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BoxRead1.h"
#include "Unit1.h"
#include "inifiles.hpp"
#include "crc8.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

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

__fastcall TNBoxRead::TNBoxRead() : TThread(true)
{
        // ---------------------------------------------------------------------
        // настройка чтение настроек из ini файла
        TIniFile *ifile = new TIniFile( ChangeFileExt( Application->ExeName, ".ini" ) );
        PortName = ifile->ReadString("ini", "ComPortName", "COM2" );
        PortBaud = (eBaudRate)ifile->ReadInteger("ini", "ComPortBaud", CBR_38400);
        PathName = ifile->ReadString("ini", "PathArchiv", ExtractFilePath(Application->ExeName)+"arhiv\\" );
        ifile->WriteString ("ini", "ComPortName", PortName);
        ifile->WriteInteger("ini", "ComPortBaud", PortBaud);
        ifile->WriteString ("ini", "PathArchiv",  PathName);
        for (int i=0;i<BoxRead_AllNumersDat;i++)
        {
                dat_FlInv[i] = ifile->ReadBool("Sensors Inversion", "Sn"+IntToStr(i), false );
                ifile->WriteBool("Sensors Inversion", "Sn"+IntToStr(i), dat_FlInv[i] );
        }
        delete ifile;
        // ---------------------------------------------------------------------
        // создание объекта порта
        Port = new TComPort;
        if (Port==NULL)
        {
                // ошибка выделения памяти
                Terminate();
                Synchronize(ErrNewPort);
                return;
        }
        // ---------------------------------------------------------------------
        // сброс массива состояния датчиков
        for (int i=0; i<BoxRead_AllNumersDat; i++)
        {
                dat_St[i] = true;
                BoxReadMassSensorsLevel[i] = true;
        }
        Here1 = true;
        Here2 = true;
        // ---------------------------------------------------------------------
        // пуск нити после настроек событий
        // Resume();
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::ErrNewPort()
{
        ShowMessage("Ошибка выделение памяти на COM порт");
        Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::ErrOpenPort()
{
        ShowMessage("Ошибка открытия COM порта");
        Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::Execute()
{
        // ---------------------------------------------------------------------
        //        переменные
        int             stat;           // статус
        unsigned char   vbyte[BoxRead_MassPackLen];     // принятый байт
        unsigned long   vbyte_order;    // скольно нужно принять байт
        unsigned long   vbyte_reality;  // сколько байт принято
        // ---------------------------------------------------------------------
        // настройка события
        Port->EventNewDate = EventNewData;
        // открытие порта
        if ( Port->Open(PortName, PortBaud, NO)>0 )
        {
                Terminate();
                Synchronize(ErrOpenPort);
        }
        else
        {
                // инициация состояния готовности к приему
                stat = 0;
                kik = 0;
        }
        // ---------------------------------------------------------------------
        // основной цикл модуля
        while (!Terminated)
        {
                if (stat)
                {
                        if (kik>0)
                        {
                                kik--;
                                Sleep(5);
                                stat = 0;
                                continue;
                        }
                        // заморозка приема до прихода данных
                        Suspend();
                        // инициация состояния готовности к приему
                        stat = 0;
                        continue;
                }
                vbyte_order = BoxRead_MassPackLen;
                stat = Port->ReadBuf(vbyte, &vbyte_order, &vbyte_reality);
                if (stat)
                {
                        // буффер пуст
                        continue;
                }
                SelectPak(vbyte, vbyte_reality);
        }
        // ---------------------------------------------------------------------
        //  выход
        if (Port)
        {
                delete Port;
                Port = NULL;
        }
}
//---------------------------------------------------------------------------
// пришли новые данные
void __fastcall TNBoxRead::EventNewData(int RdByte)
{
        kik++;
        // запустить поток
        Resume();
}
//---------------------------------------------------------------------------
// выборка пакетов из потока
void __fastcall TNBoxRead::SelectPak(unsigned char *b, int len)
{
        while( len>0 )
        {
                len--;
                // сдвиг данных в массиве
                for (int i=0; i<(BoxRead_MassPackLen-1); i++)
                        MassPack[i] = MassPack[i+1];
                // новый байт
                MassPack[BoxRead_MassPackLen-1] = *b;
                b++;
                if (MassPack[BoxReadMASSPACK_HEAD1]!=0xe6)  continue;
                if (MassPack[BoxReadMASSPACK_HEAD2]!=0x16)  continue;
                if (MassPack[BoxReadMASSPACK_TAIL ]!=0x00)  continue;
                if (MassPack[BoxReadMASSPACK_CRC  ]!=crc8_buf(MassPack,BoxReadMASSPACK_CRC)) continue;
                Synchronize(SelectDo);
        }
}
//---------------------------------------------------------------------------
// обработка пакета
void __fastcall TNBoxRead::SelectDo()
{
        int sn, lv;
        if (MassPack[BoxReadMASSPACK_TYPE]==1)
        {       // другие сенсоры
                sn = MassPack[BoxReadMASSPACK_SENSR];
                lv = MassPack[BoxReadMASSPACK_LEVEL] ^ dat_FlInv[sn];
                // хранение состояния датчиков
                BoxReadMassSensorsLevel[sn] = lv;
                // сенсоры наличия трубы
                if ( (sn==TUBE_HERE1) ||
                     (sn==TUBE_HERE2) )
                {
                        TubeHere(sn, lv);
                }
                // сенсоры положение дефектоскопа
                if ( (sn==SENSOR_AT_TOP) ||
                     (sn==SENSOR_AT_BOTTOM) )
                {
                        SensorAt(sn, lv);
                }
                // сенсоры положение дефектоскопа
                if (sn==WELD_DEFECT)
                {
                        WeldDefect(sn, lv);
                }
                // сенсор режима калибровки
                if (sn==MODE_CALIBROVKA)
                {
                        ModeCalibrovka(sn, lv);
                }
                // событие - смена уровня датчика
                if (EvSensor)
                {
                        EvSensor(sn, lv);
                        return;
                }
        }
        // колесо
        if (MassPack[BoxReadMASSPACK_TYPE]==0)
        {       // колесо
                sn = MassPack[BoxReadMASSPACK_SENSR];
                lv = MassPack[BoxReadMASSPACK_LEVEL];
                if (EvCircleSensor)
                {
                        EvCircleSensor( sn );
                }
                DoCirle(sn, lv);
                return;
        }
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::TubeHere(int sn, int lv)
{
        // сохранение предидущего уровня датчиков
        OldHere1 = Here1;
        OldHere2 = Here2;
        // установка нового уровня датчиков
        if (sn==TUBE_HERE1) Here1 = lv;
        if (sn==TUBE_HERE2) Here2 = lv;
        // уровень 1-труба вне датчика
        // уровень 0-труба на датчике
        // =================================
        //      основные события :
        // =============================================1
        // 11 --> 01 въезд в начале
        if (
                OldHere1        &&
                !Here1          &&
                OldHere2        &&
                Here2
                )
        {
                TubeHere1Up();
                // событие
                if (EvTubeHereBeginUp)
                {
                        EvTubeHereBeginUp();
                }
                return;
        }
        // 01 <-- 11 съезд в начале
        if (
                !OldHere1       &&
                Here1           &&
                OldHere2        &&
                Here2
                )
        {
                TubeHere1Dn();
                // отработка события
                if (EvTubeHereBeginDn)
                {
                        EvTubeHereBeginDn();
                }
                return;
        }
        // =============================================2
        // 01 --> 00 въезд в зону измерений от начала
        if (
                !OldHere1       &&
                !Here1          &&
                OldHere2        &&
                !Here2
                )
        {
                TubeHere12Up();
                // событие
                //if (EvTubeHereBeginUp)
                //{
                 //       EvTubeHereBeginUp();
                //}
                return;
        }
        // 00 <-- 01 съезд из зоны измерения к началу
        if (
                !OldHere1       &&
                !Here1          &&
                !OldHere2       &&
                Here2
                )
        {
                TubeHere12Dn();
                // отработка события
                //if (EvTubeHereBeginDn)
                //{
                //        EvTubeHereBeginDn();
                //}
                return;
        }
        // =============================================3
        // 01 --> 00 въезд в зону измерений от начала
        if (
                !OldHere1       &&
                Here1           &&
                !OldHere2       &&
                !Here2
                )
        {
                TubeHere1Dn2();
                // событие
                //if (EvTubeHereBeginUp)
                //{
                //        EvTubeHereBeginUp();
                //}
                return;
        }
        // 10 <-- 00 въезд в зону измерений от конца
        if (
                OldHere1        &&
                !Here1          &&
                !OldHere2       &&
                !Here2
                )
        {
                TubeHere1Up2();
                // отработка события
                //if (EvTubeHereBeginDn)
                //{
                //        EvTubeHereBeginDn();
                //}
                return;
        }
        // =============================================4
        // 10 --> 11 съезд в конце
        if (
                OldHere1        &&
                Here1           &&
                !OldHere2       &&
                Here2
                )
        {
                TubeHere2Dn();
                // событие
                if (EvTubeHereEndDn)
                {
                        EvTubeHereEndDn();
                }
                return;
        }
        // 11 <-- 10 въезд в конце
        if (
                OldHere1        &&
                Here1           &&
                OldHere2        &&
                !Here2
                )
        {
                TubeHere2Up();
                // отработка события
                if (EvTubeHereEndUp)
                {
                        EvTubeHereEndUp();
                }
                return;
        }
}
//---------------------------------------------------------------------------
// 11 --> 01 въезд в начале
void __fastcall TNBoxRead::TubeHere1Up()
{
        // труба не в зоне дефектоскопа
        TubeInDefect = 0;

        // синхронизация счетчика с началом трубы
        count = 0;
        // длина текущей трубы в сегментах
        count_max = 0;
        // сброс массива сопровождения
        for (int i=0; i<BoxReadMassDefect_MAX; i++)
                MassDefectCur[i] = -1;
        // сброс режима калибровки
        if (BoxReadMassSensorsLevel[MODE_CALIBROVKA])
                TrModeCalibrovka = 0;
}
//---------------------------------------------------------------------------
// 01 <-- 11 съезд в начале
void __fastcall TNBoxRead::TubeHere1Dn()
{
        // труба не в зоне дефектоскопа
        TubeInDefect = 0;
}
//---------------------------------------------------------------------------
// 01 --> 00 въезд в зону измерений от начала
void __fastcall TNBoxRead::TubeHere12Up()
{
        // труба в зоне дефектоскопа
        TubeInDefect = 1;
}
//---------------------------------------------------------------------------
// 00 <-- 01 съезд из зоны измерения к началу
void __fastcall TNBoxRead::TubeHere12Dn()
{
        // труба не в зоне дефектоскопа
        TubeInDefect = 0;
}
//---------------------------------------------------------------------------
// 00 --> 10 съезд из зоны измерений к концу
void __fastcall TNBoxRead::TubeHere1Dn2()
{
        // труба не в зоне дефектоскопа
        TubeInDefect = 0;
}
//---------------------------------------------------------------------------
// 10 <-- 00 въезд в зону измерений от конца
void __fastcall TNBoxRead::TubeHere1Up2()
{
        // труба в зоне дефектоскопа
        TubeInDefect = 1;
}
//---------------------------------------------------------------------------
// 10 --> 11 съезд в конце
void __fastcall TNBoxRead::TubeHere2Dn()
{
        // труба не в зоне дефектоскопа
        TubeInDefect = 0;

        // длина массива дефектов
        MassDefectLen = count;
        // копирование массива дефектов
        for (int i=0; i<BoxReadMassDefect_MAX; i++)
                MassDefect[i] = MassDefectCur[i];
        // труба не в зоне дефектоскопа
        // TubeInDefect = 0;
}
//---------------------------------------------------------------------------
// 11 <-- 10 въезд в конце
void __fastcall TNBoxRead::TubeHere2Up()
{
        // труба не в зоне дефектоскопа
        TubeInDefect = 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// положение сенсора
void __fastcall TNBoxRead::SensorAt(int sn, int lv)
{
        bool fl_eve = false;
        // SensorAtTr : 1 - на трубе, 0 - над трубой
        if ( // проверка перехода датчиков дефектоскопа на трубу
                // тригер датчики дефектоскопа наверху
                !SensorAtTr &&
                // датчик верхнего положения - выключен
                BoxReadMassSensorsLevel[SENSOR_AT_TOP] &&
                // датчик нижнего положения - включен
                !BoxReadMassSensorsLevel[SENSOR_AT_BOTTOM]
                )
        { // датчики дефектоскопа на трубе
                SensorAtTr = 1;
                // установка события
                fl_eve = true;
        }
        else
        if ( // проверка перехода датчиков дефектоскопа от трубы
                // тригер датчики дефектоскопа на трубе
                SensorAtTr &&
                // датчик верхнего положения - включен
                !BoxReadMassSensorsLevel[SENSOR_AT_TOP] &&
                // датчик нижнего положения - выключен
                BoxReadMassSensorsLevel[SENSOR_AT_BOTTOM]
                )
        { // датчики дефектоскопа над трубой
                SensorAtTr = 0;
                // установка события
                fl_eve = true;
        }
        if (!fl_eve)
        { // событий не было
                return;
        }
        // событие
        if (EvSensorAt)
        { // 0 - на трубе, 1 - над трубой
                EvSensorAt(SensorAtTr);
        }
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::WeldDefect(int sn, int lv)
{
        if ( !lv && SensorAtTr ) // датчики на трубе и пришел дефект
        {
                // был дефект
                SummDefect = 1;
        }
        // событие дефект lv : 0 - нет дефекта 1 - дефект
        if (EvWeldDefect)
        {
                if (lv==0)
                {
                        EvWeldDefect(1);
                        return;
                }
                if (lv==1)
                {
                        EvWeldDefect(0);
                        return;
                }
                EvWeldDefect(-1);
        }
}
//---------------------------------------------------------------------------
// установка начальная накопительного регистра по сегменту
void __fastcall TNBoxRead::SetSummDefect()
{
        // положение датчиков дефектоскопа
        if ( !SensorAtTr )
        { // над трубой
                SummDefect = 0; // нет дефекта
                return;
        }
        // датчики на трубе
        // --
        /*
        // положение трубы
        if ( !TubeInDefect )
        { // труба вне зоны дефектоскопа
                SummDefect = 0; // нет дефекта
                return;
        }
        */
        // проверка последнего состояния датчика дефект
        if ( BoxReadMassSensorsLevel[WELD_DEFECT]==0 )
        { // датчик фиксирует дефект
                SummDefect = 1; // дефект
                return;
        }
        SummDefect = 0; // нет дефекта
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::DoCirle(int sn, int lv)
{
        if (sn==CIRCLE_FORWARD)
        { // колесо вперед
                // === сохранить дефект в сегменте ===
                // датчики дефектоскопа
                if ( SensorAtTr )
                { // на трубе
                        // размерность массива
                        if ( (count>=0) && (count<256) )
                        {
                                MassDefectCur[count] = SummDefect;
                        }
                }
                // установка состояния дефект
                SetSummDefect();
                // следующий сегмент
                count++;
                // сколько всего сегментов текущая труба
                if ( (count<256) && (count_max<count) )
                {
                        count_max = count;
                }
                if ( !BoxReadMassSensorsLevel[TUBE_HERE1] || !BoxReadMassSensorsLevel[TUBE_HERE2] )
                {
                // событие
                if (EvCircle)
                {
                        // колесо вперед/длина трубы в сегментах/ текущая позитция/ массив дефектов
                        EvCircle( 0, count_max, count, MassDefectCur);
                }
                }
                return;
        }
        if (sn==CIRCLE_BACK)
        { // колесо назад
                // предидущий сегмент
                count--;
                // установка состояния дефект
                SetSummDefect();
                if ( !BoxReadMassSensorsLevel[TUBE_HERE1] || !BoxReadMassSensorsLevel[TUBE_HERE2] )
                {
                // событие
                if (EvCircle && count>0 )
                {
                        // колесо назад/длина трубы в сегментах/ текущая позитция/ массив дефектов
                        EvCircle( 1, count_max, count, MassDefectCur);
                }
                }
                return;
        }
}
//---------------------------------------------------------------------------
// калибровка
void __fastcall TNBoxRead::ModeCalibrovka(int sn, int lv)
{
        if (lv==0)
        {
                TrModeCalibrovka = 1;
        }
        if (EvModeCalibrovka)
        {
                if (lv==0)
                {
                        EvModeCalibrovka(1, TrModeCalibrovka);
                        return;
                }
                if (lv==1)
                {
                        EvModeCalibrovka(0, TrModeCalibrovka);
                        return;
                }
        }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


