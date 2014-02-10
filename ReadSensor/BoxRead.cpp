//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BoxRead.h"
#include "crc8.h"
#include "WriteFl.h"
#include "Unit1.h"

#include "inifiles.hpp"
#include "ViewKoleso.h"
#include "WriteTubeFile.h"
#include "WriteCalibrFile.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TNBoxRead::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------
TNWriteFl        *WriteFl         = NULL;
TWriteTubeFile   *WriteTubeFile   = NULL;
TWriteCalibrFile *WriteCalibrFile = NULL;
#define MassDefect_Max 256
signed char MassDefect[MassDefect_Max];
signed char DefectS;
int MassDefect_Ind = 0;
bool MassDefect_Work = false;

// ����������� ������ ����� ������
__fastcall TNBoxRead::TNBoxRead()
        : TThread(true)
{
        // ��������� COM ����� � ���� � ������ �� ini �����
        Port = new TComPort;
        if (Port==NULL)
        {
                Terminate();
                Synchronize(ErrNewPort);
                //ShowMessage("������ ��������� ������ �� COM ����");
                return;
        }
        AnsiString fld = ChangeFileExt( Application->ExeName, ".ini" );
        TIniFile *ifile = new TIniFile( ChangeFileExt( Application->ExeName, ".ini" ) );
        PortName = ifile->ReadString("ini", "ComPort", "COM2" );
        PortBaud = (eBaudRate)ifile->ReadInteger("ini", "ComPortBaud", CBR_38400);
        ifile->WriteString("ini", "ComPort", PortName);
        ifile->WriteInteger("ini", "ComPortBaud", PortBaud);
        PathName = ifile->ReadString("ini", "PathArchiv", ExtractFilePath(Application->ExeName)+"arhiv\\" );
        ifile->WriteString("ini", "PathArchiv", PathName);
        // ���� �������� �������
        for (int i=0;i<14;i++)
        {
                sd_idat[i] = ifile->ReadBool("Sensors Inversion", "Sn"+IntToStr(i), false );
                ifile->WriteBool("Sensors Inversion", "Sn"+IntToStr(i), sd_idat[i] );
        }
        delete ifile;
        // ����� ��������� ��������
        for (int i=0;i<14;i++)
        {
                nsd_dat[i] = -1;
        }
        // ������� ������� �����
        nsd_Dtr1 = false;
        nsd_Dtr2 = false;
        nsd_Dtr1Old = false;
        nsd_Dtr2Old = false;
        // ����� �������� ������
        nsd_RunTube = false;
        // ������ ��������� ������� �����
        nsd_Tr_Tube = false;
        // ������ ��������� �������������
        nsd_Tr_Def = false;
        // ��������� ����������
        nsd_St_Calibrovka = false;
        // ������ ����������
        nsd_Tr_Calibrovka = false;
        // ������� �������
        nsd_fl_up = true;
        // ������� �������
        nsd_fl_down = false;

        sd_fl_truba = false;
        sd_fl_calibrovka = false;
        sd_fl_defektoskopiya = false;
        sd_fl_calibrovka_beg = true;
        UnicalFlDefect = false;
        Resume();
}

void __fastcall TNBoxRead::ErrNewPort()
{
        ShowMessage("������ ��������� ������ �� COM ����");
        Application->Terminate();
}
void __fastcall TNBoxRead::ErrOpenPort()
{
        ShowMessage("������ �������� COM �����");
        Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::Execute()
{
        unsigned char bt;
        unsigned long bt_ok;
        unsigned long bt_zak;
        int stat = 0;
        FreeOnTerminate = true;
        Port->EventNewDate = EventReadData;
//        stat = Port->Open(PortName, BaudRate38400, NO);
        stat = Port->Open(PortName, PortBaud, NO);
        if (stat>0)
        {
                Terminate();
                Synchronize(ErrOpenPort);
                //ShowMessage("������ �������� COM �����");
        }
        WriteFl = new TNWriteFl;
        // ������ �� �����
        WriteTubeFile   = new TWriteTubeFile(false);
        // ������ �� ����������
        WriteCalibrFile = new TWriteCalibrFile(false);
        //---- Place thread code here ----
        Suspend();
        while (!Terminated)
        {
                if (stat)
                {
                        Suspend();
                        stat = 0;
                        continue;
                }
                bt_zak = 1;
                stat = Port->ReadBuf(&bt, &bt_zak, &bt_ok);
                if (stat) continue;
                SelectPaks(bt);
        }
        //---- Place thread code here ----
        if (Port)
        {
                delete Port;
                Port = NULL;
        }
        if (WriteFl)
        {
                WriteFl->Terminate();
                WriteFl->Resume();
                WriteFl = NULL;
        }
        if (WriteTubeFile)
        {
                WriteTubeFile->Terminate();
                WriteTubeFile->Resume();
                WriteTubeFile = NULL;
        }
        if (WriteCalibrFile)
        {
                WriteCalibrFile->Terminate();
                WriteCalibrFile->Resume();
                WriteCalibrFile = NULL;
        }
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::EventReadData(int RdByte)
{
        Resume();
}
//---------------------------------------------------------------------------
void __fastcall TNBoxRead::SelectPaks(unsigned char &bt)
{
        for (int i=0;i<6;i++) sBuf[i] = sBuf[i+1];
        sBuf[6] = bt;
        if (sBuf[0]!=0xe6) return;
        if (sBuf[1]!=0x16) return;
        if (sBuf[6]!=0x00) return;
        if (sBuf[5]!=crc8_buf(sBuf,5)) return;
        //Synchronize(WriteFile);
        Synchronize(SelectDo);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TNBoxRead::IntToStr2s(int dg)
{
        AnsiString v = IntToStr(dg);
        if (v.Length()<2) v = "0"+v;
        return v;
}
//---------------------------------------------------------------------------
void _fastcall TNBoxRead::SelectDo()
{
        TimePack = Now();
        if (sBuf[2]==0)
        {
                // ************** ������ **************
                // ���������� ������� �����������
                nsd_dat[1] = ((bool)sBuf[4]) ^ (sd_idat[ 1 ]);
                ViewKoleso->Flash(nsd_dat[1], clGreen, clWindow);
        }
        if (sBuf[2]==1)
        {
                // ************** ������� �������� (sBuf[3])**************
                // 3 - �������� ����� (����)
                // 4 - �������� ������ (����)
                // 5 - ������� ����� �1
                // 6 - ������� ����� �2
                // 9 - ������
                // 10 - ����������
                // 11 - ������� ������������ �������
                // 12 - ������� ������������ �������

                // ��������
                sBuf[3] = sBuf[3] + 3;

                // ������� ������ ���������
                if ( sBuf[3]<3 ) return;
                if ( sBuf[3]>13 ) return;

                // ���������� � ����������� �������� ��������
                {
                        bool dt = ((bool)sBuf[4]) ^ (sd_idat[ sBuf[3] ]);
                        nsd_dat[ sBuf[3] ] = dt;
                }

                // ���������� �������
                Form1->LampDat[ sBuf[3]-3  ]->Brush->Color = nsd_dat[ sBuf[3] ] ? clWindow : clGreen;
        }
        // nsd_dat - ������ �������� ��������� ��������
        // ��������� �������� 1 � 2 : nsd_Dtr1      nsd_Dtr2
        // ���������� ���������     : nsd_Dtr1Old   nsd_Dtr2Old
        // ��������� ���� ������    : nsd_RunTube
        // ������� �������          : nsd_fl_up
        // ������� �������          : nsd_fl_down
        // ���� ������� ���
        //       ��������� �������� : UnicalFlDefect
        // ================================================
        //         ��������� �������� �����������
        // ������� ����� ������1
        if ( nsd_dat[5]>=0 )
        {
                nsd_Dtr1Old = nsd_Dtr1;
                nsd_Dtr1 = !((bool)nsd_dat[5]);
        }
        // ������� ����� ������2
        if ( nsd_dat[6]>=0 )
        {
                nsd_Dtr2Old = nsd_Dtr2;
                nsd_Dtr2 = !((bool)nsd_dat[6]);
        }
        // ����� �������� ������
        if ( nsd_dat[4]>=0 )
        {
                nsd_RunTube = !((bool)nsd_dat[4]);
        }
        // ������ � ������� ������� - ���������� ����
        if ( (nsd_dat[9]==0) && (nsd_dat[12]==0) )
        {
                UnicalFlDefect = true;
        }
        // ��������� ����������
        if ( nsd_dat[10]>=0 )
        {
                nsd_St_Calibrovka = !((bool)nsd_dat[10]);
        }
        // ������� �������
        if ( nsd_dat[11]>=0 )
        {
                nsd_fl_up = !((bool)nsd_dat[11]);
        }
        // ������� �������
        if ( nsd_dat[12]>=0 )
        {
                nsd_fl_down = !((bool)nsd_dat[12]);
        }
        // ������ ��������� ������� �����
        if (nsd_Tr_Tube)
        {
                // �������� ����� �����
                if (  (!nsd_Dtr1Old)
                   && (nsd_Dtr2Old)
                   && (!nsd_Dtr1)
                   && (!nsd_Dtr2) )
                {
                        nsd_Tr_Tube = false;
                        // ��������� ������ �������������
                        WriteTubeFile->Stop(UnicalFlDefect);
                        UnicalFlDefect = false;
                        // ���� �������������
                        nsd_Tr_Def = false;
                }
        }
        else
        {
                // �������� ����� �����
                if (  (!nsd_Dtr1Old)    // ������ �1 ������� �����
                   && (!nsd_Dtr2Old)    // ������ �2 ������� �����
                   && (nsd_Dtr1)
                   && (!nsd_Dtr2) )
                {
                        nsd_Tr_Tube = true;
                        // ��������� �������
                        WriteTubeFile->Init(PathName);
                        // ��������� �������
                        //MassDefect_Ind = 0;
                        //DefectS = 0;
                }
        }

        // ================================
        //              animation
        // ����������� ����������
        if (nsd_St_Calibrovka)
        {
                Form1->Shape14->Brush->Color = clRed;
                Form1->Shape14->Visible = true;
                Form1->Shape14->Left = 40;
                Form1->Shape14->Width = 490;
        }
        else
        {
                // �� ����������
                Form1->Shape14->Brush->Color = clWindow;
                // ����������/������� �����
                if ( !nsd_Tr_Tube )
                {
                        // ����� �����������
                        Form1->Shape14->Visible = false;
                }
                else
                {
                        // ����� � ������
                        if ( nsd_Dtr1 && (!nsd_Dtr2) )
                        {
                                Form1->Shape14->Visible = true;
                                Form1->Shape14->Left = 40;
                                Form1->Shape14->Width = 270;
                        }
                        // ����� �� ���� ��������
                        if ( nsd_Dtr1 && nsd_Dtr2 )
                        {
                                Form1->Shape14->Visible = true;
                                Form1->Shape14->Left = 40;
                                Form1->Shape14->Width = 490;
                        }
                        // ����� � �����
                        if ( (!nsd_Dtr1) && nsd_Dtr2 )
                        {
                                Form1->Shape14->Visible = true;
                                Form1->Shape14->Left = 230;
                                Form1->Shape14->Width = 300;
                        }
                }
        }
        // ������� �������
        if (nsd_fl_up)
        {
                Form1->Shape15->Top = 64;
        }
        // ������� �������
        if (nsd_fl_down)
        {
                Form1->Shape15->Top = 112;
        }
        // ����������� ��������
        if ( nsd_dat[9] )
        {
                if ( UnicalFlDefect )
                        Form1->Shape15->Brush->Color =clGreen;
                else
                        Form1->Shape15->Brush->Color =clLime;
        }
        else
        {
                Form1->Shape15->Brush->Color = clRed;
        }
        // ================================
        // ���������� � ������/����
        if (nsd_St_Calibrovka)
        {
                // ����� ���������� ��������
                if (!nsd_Tr_Calibrovka)
                {
                        nsd_Tr_Calibrovka = true;
                        WriteCalibrFile->StartRec(PathName);
                        // ������ �����
                        for (int i=0;i<14;i++)
                        {
                                WriteCalibrFile->Push( TimePack, 1, i, nsd_dat[i] );
                        }
                }
                else
                {
                        // ������ ������ �������
                        if ( sBuf[2]<=1 )
                        {
                                WriteCalibrFile->Push( TimePack, sBuf[2], sBuf[3], nsd_dat[ sBuf[3] ] );
                        }
                }
        }
        else
        {       // ������� �����
                if (nsd_Tr_Calibrovka)
                {
                        nsd_Tr_Calibrovka = false;
                        // ������� ���������� ������ ����������
                        WriteCalibrFile->StopRec();
                }
                // ������� ������ ��������, �� ������� ��� ����������� �����
                if ( nsd_Tr_Tube && (sBuf[2]<=1) )
                {
                        // ������ � ������
                        WriteTubeFile->Push( TimePack, sBuf[2], sBuf[3], nsd_dat[ sBuf[3] ] );
                        // ��������� ������
                        if (!nsd_Tr_Def)
                        {
                                // ������� "������ �������������"
                                if ( ( nsd_Tr_Tube)                     // ������� ������ �����
                                  && ( nsd_Dtr1)                        // ����� �� ������� 1
                                  && ( nsd_Dtr2)  )                     // ����� �� ������� 2
                                {
                                        // ���������� ���������� �������
                                        nsd_Tr_Def = true;
                                        // ��������� ������ � ���� �� �������
                                        WriteTubeFile->Record();
                                }
                        }
                }
        }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


