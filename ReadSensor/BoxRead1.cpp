//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BoxRead1.h"
#include "Unit1.h"
#include "inifiles.hpp"
#include "crc8.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define CIRCLE_FORWARD          0       // ������ ������� ������ *
#define CIRCLE_BACK             1       // ������ ������� �����  *
#define KEY_BACK                0       // �������� ����� (����)
#define KEY_FORWARD             1       // �������� ������ (����)
#define TUBE_HERE1              2       // ������� ����� �1 *
#define TUBE_HERE2              3       // ������� ����� �2 *
#define WELD_DEFECT             6       // ������
#define MODE_CALIBROVKA         7       // ����������
#define SENSOR_AT_TOP           8       // ������� ������������ ������� *
#define SENSOR_AT_BOTTOM        9       // ������� ������������ ������� *

__fastcall TNBoxRead::TNBoxRead() : TThread(true)
{
        // ---------------------------------------------------------------------
        // ��������� ������ �������� �� ini �����
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
        // �������� ������� �����
        Port = new TComPort;
        if (Port==NULL)
        {
                // ������ ��������� ������
                Terminate();
                Synchronize(ErrNewPort);
                return;
        }
        // ---------------------------------------------------------------------
        // ����� ������� ��������� ��������
        for (int i=0; i<BoxRead_AllNumersDat; i++)
        {
                dat_St[i] = true;
                BoxReadMassSensorsLevel[i] = true;
        }
        Here1 = true;
        Here2 = true;
        // ---------------------------------------------------------------------
        // ���� ���� ����� �������� �������
        // Resume();
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::ErrNewPort()
{
        ShowMessage("������ ��������� ������ �� COM ����");
        Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::ErrOpenPort()
{
        ShowMessage("������ �������� COM �����");
        Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::Execute()
{
        // ---------------------------------------------------------------------
        //        ����������
        int             stat;           // ������
        unsigned char   vbyte[BoxRead_MassPackLen];     // �������� ����
        unsigned long   vbyte_order;    // ������� ����� ������� ����
        unsigned long   vbyte_reality;  // ������� ���� �������
        // ---------------------------------------------------------------------
        // ��������� �������
        Port->EventNewDate = EventNewData;
        // �������� �����
        if ( Port->Open(PortName, PortBaud, NO)>0 )
        {
                Terminate();
                Synchronize(ErrOpenPort);
        }
        else
        {
                // ��������� ��������� ���������� � ������
                stat = 0;
                kik = 0;
        }
        // ---------------------------------------------------------------------
        // �������� ���� ������
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
                        // ��������� ������ �� ������� ������
                        Suspend();
                        // ��������� ��������� ���������� � ������
                        stat = 0;
                        continue;
                }
                vbyte_order = BoxRead_MassPackLen;
                stat = Port->ReadBuf(vbyte, &vbyte_order, &vbyte_reality);
                if (stat)
                {
                        // ������ ����
                        continue;
                }
                SelectPak(vbyte, vbyte_reality);
        }
        // ---------------------------------------------------------------------
        //  �����
        if (Port)
        {
                delete Port;
                Port = NULL;
        }
}
//---------------------------------------------------------------------------
// ������ ����� ������
void __fastcall TNBoxRead::EventNewData(int RdByte)
{
        kik++;
        // ��������� �����
        Resume();
}
//---------------------------------------------------------------------------
// ������� ������� �� ������
void __fastcall TNBoxRead::SelectPak(unsigned char *b, int len)
{
        while( len>0 )
        {
                len--;
                // ����� ������ � �������
                for (int i=0; i<(BoxRead_MassPackLen-1); i++)
                        MassPack[i] = MassPack[i+1];
                // ����� ����
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
// ��������� ������
void __fastcall TNBoxRead::SelectDo()
{
        int sn, lv;
        if (MassPack[BoxReadMASSPACK_TYPE]==1)
        {       // ������ �������
                sn = MassPack[BoxReadMASSPACK_SENSR];
                lv = MassPack[BoxReadMASSPACK_LEVEL] ^ dat_FlInv[sn];
                // �������� ��������� ��������
                BoxReadMassSensorsLevel[sn] = lv;
                // ������� ������� �����
                if ( (sn==TUBE_HERE1) ||
                     (sn==TUBE_HERE2) )
                {
                        TubeHere(sn, lv);
                }
                // ������� ��������� ������������
                if ( (sn==SENSOR_AT_TOP) ||
                     (sn==SENSOR_AT_BOTTOM) )
                {
                        SensorAt(sn, lv);
                }
                // ������� ��������� ������������
                if (sn==WELD_DEFECT)
                {
                        WeldDefect(sn, lv);
                }
                // ������ ������ ����������
                if (sn==MODE_CALIBROVKA)
                {
                        ModeCalibrovka(sn, lv);
                }
                // ������� - ����� ������ �������
                if (EvSensor)
                {
                        EvSensor(sn, lv);
                        return;
                }
        }
        // ������
        if (MassPack[BoxReadMASSPACK_TYPE]==0)
        {       // ������
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
        // ���������� ����������� ������ ��������
        OldHere1 = Here1;
        OldHere2 = Here2;
        // ��������� ������ ������ ��������
        if (sn==TUBE_HERE1) Here1 = lv;
        if (sn==TUBE_HERE2) Here2 = lv;
        // ������� 1-����� ��� �������
        // ������� 0-����� �� �������
        // =================================
        //      �������� ������� :
        // =============================================1
        // 11 --> 01 ����� � ������
        if (
                OldHere1        &&
                !Here1          &&
                OldHere2        &&
                Here2
                )
        {
                TubeHere1Up();
                // �������
                if (EvTubeHereBeginUp)
                {
                        EvTubeHereBeginUp();
                }
                return;
        }
        // 01 <-- 11 ����� � ������
        if (
                !OldHere1       &&
                Here1           &&
                OldHere2        &&
                Here2
                )
        {
                TubeHere1Dn();
                // ��������� �������
                if (EvTubeHereBeginDn)
                {
                        EvTubeHereBeginDn();
                }
                return;
        }
        // =============================================2
        // 01 --> 00 ����� � ���� ��������� �� ������
        if (
                !OldHere1       &&
                !Here1          &&
                OldHere2        &&
                !Here2
                )
        {
                TubeHere12Up();
                // �������
                //if (EvTubeHereBeginUp)
                //{
                 //       EvTubeHereBeginUp();
                //}
                return;
        }
        // 00 <-- 01 ����� �� ���� ��������� � ������
        if (
                !OldHere1       &&
                !Here1          &&
                !OldHere2       &&
                Here2
                )
        {
                TubeHere12Dn();
                // ��������� �������
                //if (EvTubeHereBeginDn)
                //{
                //        EvTubeHereBeginDn();
                //}
                return;
        }
        // =============================================3
        // 01 --> 00 ����� � ���� ��������� �� ������
        if (
                !OldHere1       &&
                Here1           &&
                !OldHere2       &&
                !Here2
                )
        {
                TubeHere1Dn2();
                // �������
                //if (EvTubeHereBeginUp)
                //{
                //        EvTubeHereBeginUp();
                //}
                return;
        }
        // 10 <-- 00 ����� � ���� ��������� �� �����
        if (
                OldHere1        &&
                !Here1          &&
                !OldHere2       &&
                !Here2
                )
        {
                TubeHere1Up2();
                // ��������� �������
                //if (EvTubeHereBeginDn)
                //{
                //        EvTubeHereBeginDn();
                //}
                return;
        }
        // =============================================4
        // 10 --> 11 ����� � �����
        if (
                OldHere1        &&
                Here1           &&
                !OldHere2       &&
                Here2
                )
        {
                TubeHere2Dn();
                // �������
                if (EvTubeHereEndDn)
                {
                        EvTubeHereEndDn();
                }
                return;
        }
        // 11 <-- 10 ����� � �����
        if (
                OldHere1        &&
                Here1           &&
                OldHere2        &&
                !Here2
                )
        {
                TubeHere2Up();
                // ��������� �������
                if (EvTubeHereEndUp)
                {
                        EvTubeHereEndUp();
                }
                return;
        }
}
//---------------------------------------------------------------------------
// 11 --> 01 ����� � ������
void __fastcall TNBoxRead::TubeHere1Up()
{
        // ����� �� � ���� ������������
        TubeInDefect = 0;

        // ������������� �������� � ������� �����
        count = 0;
        // ����� ������� ����� � ���������
        count_max = 0;
        // ����� ������� �������������
        for (int i=0; i<BoxReadMassDefect_MAX; i++)
                MassDefectCur[i] = -1;
        // ����� ������ ����������
        if (BoxReadMassSensorsLevel[MODE_CALIBROVKA])
                TrModeCalibrovka = 0;
}
//---------------------------------------------------------------------------
// 01 <-- 11 ����� � ������
void __fastcall TNBoxRead::TubeHere1Dn()
{
        // ����� �� � ���� ������������
        TubeInDefect = 0;
}
//---------------------------------------------------------------------------
// 01 --> 00 ����� � ���� ��������� �� ������
void __fastcall TNBoxRead::TubeHere12Up()
{
        // ����� � ���� ������������
        TubeInDefect = 1;
}
//---------------------------------------------------------------------------
// 00 <-- 01 ����� �� ���� ��������� � ������
void __fastcall TNBoxRead::TubeHere12Dn()
{
        // ����� �� � ���� ������������
        TubeInDefect = 0;
}
//---------------------------------------------------------------------------
// 00 --> 10 ����� �� ���� ��������� � �����
void __fastcall TNBoxRead::TubeHere1Dn2()
{
        // ����� �� � ���� ������������
        TubeInDefect = 0;
}
//---------------------------------------------------------------------------
// 10 <-- 00 ����� � ���� ��������� �� �����
void __fastcall TNBoxRead::TubeHere1Up2()
{
        // ����� � ���� ������������
        TubeInDefect = 1;
}
//---------------------------------------------------------------------------
// 10 --> 11 ����� � �����
void __fastcall TNBoxRead::TubeHere2Dn()
{
        // ����� �� � ���� ������������
        TubeInDefect = 0;

        // ����� ������� ��������
        MassDefectLen = count;
        // ����������� ������� ��������
        for (int i=0; i<BoxReadMassDefect_MAX; i++)
                MassDefect[i] = MassDefectCur[i];
        // ����� �� � ���� ������������
        // TubeInDefect = 0;
}
//---------------------------------------------------------------------------
// 11 <-- 10 ����� � �����
void __fastcall TNBoxRead::TubeHere2Up()
{
        // ����� �� � ���� ������������
        TubeInDefect = 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ��������� �������
void __fastcall TNBoxRead::SensorAt(int sn, int lv)
{
        bool fl_eve = false;
        // SensorAtTr : 1 - �� �����, 0 - ��� ������
        if ( // �������� �������� �������� ������������ �� �����
                // ������ ������� ������������ �������
                !SensorAtTr &&
                // ������ �������� ��������� - ��������
                BoxReadMassSensorsLevel[SENSOR_AT_TOP] &&
                // ������ ������� ��������� - �������
                !BoxReadMassSensorsLevel[SENSOR_AT_BOTTOM]
                )
        { // ������� ������������ �� �����
                SensorAtTr = 1;
                // ��������� �������
                fl_eve = true;
        }
        else
        if ( // �������� �������� �������� ������������ �� �����
                // ������ ������� ������������ �� �����
                SensorAtTr &&
                // ������ �������� ��������� - �������
                !BoxReadMassSensorsLevel[SENSOR_AT_TOP] &&
                // ������ ������� ��������� - ��������
                BoxReadMassSensorsLevel[SENSOR_AT_BOTTOM]
                )
        { // ������� ������������ ��� ������
                SensorAtTr = 0;
                // ��������� �������
                fl_eve = true;
        }
        if (!fl_eve)
        { // ������� �� ����
                return;
        }
        // �������
        if (EvSensorAt)
        { // 0 - �� �����, 1 - ��� ������
                EvSensorAt(SensorAtTr);
        }
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::WeldDefect(int sn, int lv)
{
        if ( !lv && SensorAtTr ) // ������� �� ����� � ������ ������
        {
                // ��� ������
                SummDefect = 1;
        }
        // ������� ������ lv : 0 - ��� ������� 1 - ������
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
// ��������� ��������� �������������� �������� �� ��������
void __fastcall TNBoxRead::SetSummDefect()
{
        // ��������� �������� ������������
        if ( !SensorAtTr )
        { // ��� ������
                SummDefect = 0; // ��� �������
                return;
        }
        // ������� �� �����
        // --
        /*
        // ��������� �����
        if ( !TubeInDefect )
        { // ����� ��� ���� ������������
                SummDefect = 0; // ��� �������
                return;
        }
        */
        // �������� ���������� ��������� ������� ������
        if ( BoxReadMassSensorsLevel[WELD_DEFECT]==0 )
        { // ������ ��������� ������
                SummDefect = 1; // ������
                return;
        }
        SummDefect = 0; // ��� �������
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::DoCirle(int sn, int lv)
{
        if (sn==CIRCLE_FORWARD)
        { // ������ ������
                // === ��������� ������ � �������� ===
                // ������� ������������
                if ( SensorAtTr )
                { // �� �����
                        // ����������� �������
                        if ( (count>=0) && (count<256) )
                        {
                                MassDefectCur[count] = SummDefect;
                        }
                }
                // ��������� ��������� ������
                SetSummDefect();
                // ��������� �������
                count++;
                // ������� ����� ��������� ������� �����
                if ( (count<256) && (count_max<count) )
                {
                        count_max = count;
                }
                if ( !BoxReadMassSensorsLevel[TUBE_HERE1] || !BoxReadMassSensorsLevel[TUBE_HERE2] )
                {
                // �������
                if (EvCircle)
                {
                        // ������ ������/����� ����� � ���������/ ������� ��������/ ������ ��������
                        EvCircle( 0, count_max, count, MassDefectCur);
                }
                }
                return;
        }
        if (sn==CIRCLE_BACK)
        { // ������ �����
                // ���������� �������
                count--;
                // ��������� ��������� ������
                SetSummDefect();
                if ( !BoxReadMassSensorsLevel[TUBE_HERE1] || !BoxReadMassSensorsLevel[TUBE_HERE2] )
                {
                // �������
                if (EvCircle && count>0 )
                {
                        // ������ �����/����� ����� � ���������/ ������� ��������/ ������ ��������
                        EvCircle( 1, count_max, count, MassDefectCur);
                }
                }
                return;
        }
}
//---------------------------------------------------------------------------
// ����������
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


