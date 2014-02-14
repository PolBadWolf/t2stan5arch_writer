//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WriteCalibrFile.h"
#include "inifiles.hpp"
#include "PapkaName.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TWriteCalivrFile::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TWriteCalibrFile::TWriteCalibrFile(bool CreateSuspended)
        : TThread(CreateSuspended)
{
        adr_push = 0;
        adr_pop  = 0;
        fl_rec = false;
        pFileWr = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TWriteCalibrFile::Execute()
{
        InitializeCriticalSection(&cs_cf);
        //---- Place thread code here ----
        while(!Terminated)
        {
                TBuffer b;
                if (fl_rec)
                {
                        while( Pop(&b) )
                        {
                                SaveDat(b.Vremya, b.Type, b.N, b.Stat);
                        }
                Sleep(20);
                }
                else
                {
                        if (pFileWr)
                        {
                                delete pFileWr;
                                pFileWr = NULL;
                        }
                        Suspend();
                }
        }
        fl_rec = false;
        if (pFileWr)
        {
                 delete pFileWr;
                 pFileWr = NULL;
        }
        DeleteCriticalSection(&cs_cf);
}
//---------------------------------------------------------------------------
int __fastcall TWriteCalibrFile::NormAdr(int is, int len)
{
        while (is>=len) is = is - len;
        return is;
}
//---------------------------------------------------------------------------
void __fastcall TWriteCalibrFile::Push(TTime Vremya, unsigned char Type, unsigned char N, unsigned char Stat)
{
        TBuffer b;
        int idx = NormAdr(adr_push+1, LenBufferWriteCalibrFile);
        b.Vremya = Vremya;
        b.Type = Type;
        b.N = N;
        b.Stat = Stat;
        EnterCriticalSection(&cs_cf);
        Buffer[adr_push] = b;
        adr_push = idx;
        LeaveCriticalSection(&cs_cf);
}
//---------------------------------------------------------------------------
int  __fastcall TWriteCalibrFile::Pop(TBuffer *d)
{
        if (adr_pop==adr_push) return 0;
        EnterCriticalSection(&cs_cf);
        *d = Buffer[adr_pop];
        //if (d->Type==1)
        //        StatDat[d->N] = d->Stat;
        adr_pop = NormAdr(adr_pop+1, LenBufferWriteCalibrFile);
        LeaveCriticalSection(&cs_cf);
        return 1;
}
//---------------------------------------------------------------------------
void __fastcall TWriteCalibrFile::StartRec(AnsiString path)
{
        PathName = path;
        // папка для сохранения файлов
        TDateTime Vremya = Now();
        //AnsiString papka = FormatDateTime("yyyy-mm-dd", Vremya);
        AnsiString papka = mfPapkaName(Vremya);
        if (!DirectoryExists(PathName+papka))
        {
                CreateDirectory((PathName+papka).c_str(), NULL);
        }
        // счетчик файлов
        int nt_d, nc_d;
        AnsiString nc_str;
        AnsiString marsh = PathName + papka;
        TIniFile *ifile = new TIniFile( (marsh + "\\Count.ini") );
        nt_d = ifile->ReadInteger("count", "tube", 0);
        nc_d = ifile->ReadInteger("count", "calibrovka", 0);
        nc_d++;
        ifile->WriteString("count", "tube", nt_d);
        ifile->WriteString("count", "calibrovka", nc_d);
        delete ifile;
        ifile = NULL;
        nc_str = IntToStr(nc_d);
        while ( nc_str.Length()<3 ) nc_str = "0" + nc_str;
        // создание имени файла
        AnsiString FN = PathName + papka + "\\";
        FN = FN + "calibrovka";
        FN = FN + nc_str;
        FN = FN + "_";
        FN = FN + FormatDateTime("yyyy-mm-dd_hh-nn-ss", Vremya);
        FN = FN + ".txt";
        // создание файла
        if ( !FileExists(FN) )
        {
                pFileWr = new TFileStream(FN.c_str(), fmCreate);
                // заголовок
                AnsiString s;
                s = "# " + ExtractFileName(FN) + "\r\n";
                pFileWr->Write(s.c_str(), s.Length());
                s = "# ArhivDefectCalibrovka V01 T2Stan5 \r\n";
                pFileWr->Write(s.c_str(), s.Length());
                s = "# -----------------------------------------------\r\n";
                pFileWr->Write(s.c_str(), s.Length());
                delete pFileWr;
        }
        // сброс буффера
        adr_push = 0;
        adr_pop  = 0;
        // открыть файл на запись
        pFileWr = new TFileStream(FN.c_str(), fmOpenWrite + fmShareDenyNone);
        pFileWr->Seek(pFileWr->Size,1);
        // установить флаг записи
        fl_rec = true;
        // включить цикл записи
        Resume();
}
//---------------------------------------------------------------------------
void __fastcall TWriteCalibrFile::StopRec()
{
        fl_rec = false;
}
//---------------------------------------------------------------------------
void __fastcall TWriteCalibrFile::SaveDat(TTime Vr, unsigned char Type, unsigned char N, unsigned char Stat)
{
        if (!pFileWr) return;
        AnsiString stroka;
        stroka = FormatDateTime("hh:nn:ss.zzz", Vr);
        stroka = stroka + "\t";
        if (Type==0)   // (номер датчика "0")
        {
                stroka = stroka + "\t00 ";
                if (Stat==0)
                        stroka = stroka + "+";
                if (Stat==1)
                        stroka = stroka + "-";
                stroka = stroka + "\r\n";
                pFileWr->Write(stroka.c_str(), stroka.Length());
                return;
        }
        if (Type==1)    // (номер датчика "3...13")
        {
                stroka = stroka + "\t";
                AnsiString sss = IntToStr(N);
                while ( sss.Length()<2 ) sss = "0" + sss;
                stroka = stroka + sss;
                stroka = stroka + " ";
                stroka = stroka + (Stat ? "1" : "0");
                stroka = stroka + "\r\n";
                pFileWr->Write(stroka.c_str(), stroka.Length());
                return;
        }
}
//---------------------------------------------------------------------------


