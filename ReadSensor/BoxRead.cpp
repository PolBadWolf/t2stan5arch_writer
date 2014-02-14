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

// конструктор модуля ввода данных
__fastcall TNBoxRead::TNBoxRead()
        : TThread(true)
{
        // настройки COM порта и путь к архиву из ini файла
        Port = new TComPort;
        if (Port==NULL)
        {
                Terminate();
                Synchronize(ErrNewPort);
                //ShowMessage("Ошибка выделение памяти на COM порт");
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
        // флаг инверсии датчика
        for (int i=0;i<14;i++)
        {
                sd_idat[i] = ifile->ReadBool("Sensors Inversion", "Sn"+IntToStr(i), false );
                ifile->WriteBool("Sensors Inversion", "Sn"+IntToStr(i), sd_idat[i] );
        }
        delete ifile;
        // масив состояния датчиков
        for (int i=0;i<14;i++)
        {
                nsd_dat[i] = -1;
        }
        // датчики наличия трубы
        nsd_Dtr1 = false;
        nsd_Dtr2 = false;
        nsd_Dtr1Old = false;
        nsd_Dtr2Old = false;
        // ключь движение вперед
        nsd_RunTube = false;
        // тригер состояния наличия трубы
        nsd_Tr_Tube = false;
        // тригер состояния дефектоскопии
        nsd_Tr_Def = false;
        // состояние калибровка
        nsd_St_Calibrovka = false;
        // Тригер калибровка
        nsd_Tr_Calibrovka = false;
        // датчики подняты
        nsd_fl_up = true;
        // датчики опущены
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
        ShowMessage("Ошибка выделение памяти на COM порт");
        Application->Terminate();
}
void __fastcall TNBoxRead::ErrOpenPort()
{
        ShowMessage("Ошибка открытия COM порта");
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
                //ShowMessage("Ошибка открытия COM порта");
        }
        WriteFl = new TNWriteFl;
        // запись по трубе
        WriteTubeFile   = new TWriteTubeFile(false);
        // запись по калибровке
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
                // ************** колесо **************
                // отобразить импульс направления
                nsd_dat[1] = ((bool)sBuf[4]) ^ (sd_idat[ 1 ]);
                ViewKoleso->Flash(nsd_dat[1], clGreen, clWindow);
        }
        if (sBuf[2]==1)
        {
                // ************** сигналы сенсоров (sBuf[3])**************
                // 3 - рольганг назад (ключ)
                // 4 - рольганг вперед (ключ)
                // 5 - наличие трубы д1
                // 6 - наличие трубы д2
                // 9 - дефект
                // 10 - калибровка
                // 11 - датчики дефектоскопа подняты
                // 12 - датчики дефектоскопа опущены

                // смещение
                sBuf[3] = sBuf[3] + 3;

                // условия работы программы
                if ( sBuf[3]<3 ) return;
                if ( sBuf[3]>13 ) return;

                // сохранение и необходимая инверсия сенсоров
                {
                        bool dt = ((bool)sBuf[4]) ^ (sd_idat[ sBuf[3] ]);
                        nsd_dat[ sBuf[3] ] = dt;
                }

                // отобразить сенсоры
                Form1->LampDat[ sBuf[3]-3  ]->Brush->Color = nsd_dat[ sBuf[3] ] ? clWindow : clGreen;
        }
        // nsd_dat - массив хранения состояния датчиков
        // состояние датчиков 1 и 2 : nsd_Dtr1      nsd_Dtr2
        // предидущее состояние     : nsd_Dtr1Old   nsd_Dtr2Old
        // состояние ключ вперед    : nsd_RunTube
        // датчики подняты          : nsd_fl_up
        // датчики опущены          : nsd_fl_down
        // флаг дефекта при
        //       опущенных датчиках : UnicalFlDefect
        // ================================================
        //         состояние датчиков управляющих
        // наличие трубы датчик1
        if ( nsd_dat[5]>=0 )
        {
                nsd_Dtr1Old = nsd_Dtr1;
                nsd_Dtr1 = !((bool)nsd_dat[5]);
        }
        // наличие трубы датчик2
        if ( nsd_dat[6]>=0 )
        {
                nsd_Dtr2Old = nsd_Dtr2;
                nsd_Dtr2 = !((bool)nsd_dat[6]);
        }
        // ключь движение вперед
        if ( nsd_dat[4]>=0 )
        {
                nsd_RunTube = !((bool)nsd_dat[4]);
        }
        // дефект и датчики опущены - установить флаг
        if ( (nsd_dat[9]==0) && (nsd_dat[12]==0) )
        {
                UnicalFlDefect = true;
        }
        // состояние калибровка
        if ( nsd_dat[10]>=0 )
        {
                nsd_St_Calibrovka = !((bool)nsd_dat[10]);
        }
        // датчики подняты
        if ( nsd_dat[11]>=0 )
        {
                nsd_fl_up = !((bool)nsd_dat[11]);
        }
        // датчики опущены
        if ( nsd_dat[12]>=0 )
        {
                nsd_fl_down = !((bool)nsd_dat[12]);
        }
        // тригер состояния наличия трубы
        if (nsd_Tr_Tube)
        {
                // ожидание схода трубы
                if (  (!nsd_Dtr1Old)
                   && (nsd_Dtr2Old)
                   && (!nsd_Dtr1)
                   && (!nsd_Dtr2) )
                {
                        nsd_Tr_Tube = false;
                        // окончание записи дефектоскопии
                        WriteTubeFile->Stop(UnicalFlDefect);
                        UnicalFlDefect = false;
                        // флаг дефектоскопии
                        nsd_Tr_Def = false;
                }
        }
        else
        {
                // ожидание новой трубы
                if (  (!nsd_Dtr1Old)    // датчик №1 наличия трубы
                   && (!nsd_Dtr2Old)    // датчик №2 наличия трубы
                   && (nsd_Dtr1)
                   && (!nsd_Dtr2) )
                {
                        nsd_Tr_Tube = true;
                        // обнуление буффера
                        WriteTubeFile->Init(PathName);
                        // инициация массива
                        //MassDefect_Ind = 0;
                        //DefectS = 0;
                }
        }

        // ================================
        //              animation
        // отображение калибровки
        if (nsd_St_Calibrovka)
        {
                Form1->Shape14->Brush->Color = clRed;
                Form1->Shape14->Visible = true;
                Form1->Shape14->Left = 40;
                Form1->Shape14->Width = 490;
        }
        else
        {
                // не калибровка
                Form1->Shape14->Brush->Color = clWindow;
                // отсутствие/наличие трубы
                if ( !nsd_Tr_Tube )
                {
                        // труба отсутствует
                        Form1->Shape14->Visible = false;
                }
                else
                {
                        // труба в начале
                        if ( nsd_Dtr1 && (!nsd_Dtr2) )
                        {
                                Form1->Shape14->Visible = true;
                                Form1->Shape14->Left = 40;
                                Form1->Shape14->Width = 270;
                        }
                        // труба на двух датчиках
                        if ( nsd_Dtr1 && nsd_Dtr2 )
                        {
                                Form1->Shape14->Visible = true;
                                Form1->Shape14->Left = 40;
                                Form1->Shape14->Width = 490;
                        }
                        // труба в конце
                        if ( (!nsd_Dtr1) && nsd_Dtr2 )
                        {
                                Form1->Shape14->Visible = true;
                                Form1->Shape14->Left = 230;
                                Form1->Shape14->Width = 300;
                        }
                }
        }
        // датчики подняты
        if (nsd_fl_up)
        {
                Form1->Shape15->Top = 64;
        }
        // датчики опущены
        if (nsd_fl_down)
        {
                Form1->Shape15->Top = 112;
        }
        // отображение дефектов
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
        // сохранение в буффер/файл
        if (nsd_St_Calibrovka)
        {
                // режим калибровки датчиков
                if (!nsd_Tr_Calibrovka)
                {
                        nsd_Tr_Calibrovka = true;
                        WriteCalibrFile->StartRec(PathName);
                        // запись оптом
                        for (int i=0;i<14;i++)
                        {
                                WriteCalibrFile->Push( TimePack, 1, i, nsd_dat[i] );
                        }
                }
                else
                {
                        // запись одного датчика
                        if ( sBuf[2]<=1 )
                        {
                                WriteCalibrFile->Push( TimePack, sBuf[2], sBuf[3], nsd_dat[ sBuf[3] ] );
                        }
                }
        }
        else
        {       // обычный режим
                if (nsd_Tr_Calibrovka)
                {
                        nsd_Tr_Calibrovka = false;
                        // команда выключения записи калибровки
                        WriteCalibrFile->StopRec();
                }
                // условие записи датчиков, не события при обнаружении трубы
                if ( nsd_Tr_Tube && (sBuf[2]<=1) )
                {
                        // запись в буффер
                        WriteTubeFile->Push( TimePack, sBuf[2], sBuf[3], nsd_dat[ sBuf[3] ] );
                        // включение записи
                        if (!nsd_Tr_Def)
                        {
                                // событие "начало дефектоскопии"
                                if ( ( nsd_Tr_Tube)                     // наличие тригер трубы
                                  && ( nsd_Dtr1)                        // труба на датчике 1
                                  && ( nsd_Dtr2)  )                     // труба на датчике 2
                                {
                                        // блокировка повторного события
                                        nsd_Tr_Def = true;
                                        // запустить запись в файл из буффера
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


