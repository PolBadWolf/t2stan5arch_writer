//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WriteTubeFile.h"
#include "inifiles.hpp"
//#include "PapkaName.h"
#pragma package(smart_init)
#include "BoxRead.h"
extern TNBoxRead *BoxRead;
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TWriteTubeFile::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TWriteTubeFile::TWriteTubeFile(bool CreateSuspended)
        : TThread(CreateSuspended)
{
        adr_push = 0;
        adr_pop = 0;
        fl_RecFile = false;
        fl_OpenFile = false;
        pFileWr = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TWriteTubeFile::Execute()
{
        AnsiString FNC = "", FNI = "";
        FLC = false;
        InitializeCriticalSection(&cs_pp);
        //---- Place thread code here ----
        while(!Terminated)
        {
                if (!fl_RecFile)
                {
                        if (pFileWr)
                        {
                                delete pFileWr;
                                pFileWr = NULL;
                                if (FLC)
                                {
                                        AnsiString fnn;
                                        fnn = FNC.SubString(1, FNC.Length()-4) + "D.txt";
                                        RenameFile(FNC, fnn);
                                        int nt_d, nd_d, nc_d;
                                        TIniFile *ifile = new TIniFile( FNI );
                                        nt_d = ifile->ReadInteger("count", "tube", 0);
                                        nd_d = ifile->ReadInteger("count", "defect", 0);
                                        nc_d = ifile->ReadInteger("count", "calibrovka", 0);
                                        nd_d++;
                                        ifile->WriteString("count", "tube", nt_d);
                                        ifile->WriteString("count", "defect", nd_d);
                                        ifile->WriteString("count", "calibrovka", nc_d);
                                        delete ifile;
                                        ifile = NULL;
                                }
                                FLC = false;
                        }
                        Suspend();
                        continue;
                }
                if (pFileWr)
                {
                        // запись в файл
                        TBuffer b;
                        while ( Pop(&b) )
                        {
                                SaveDat(b.Vremya, b.Type, b.N, b.Stat);
                        }
                        // перекур
                        Sleep(20);
                        continue;
                }
                if ( (!pFileWr) && (adr_push!=adr_pop) )
                {
                        // начало файла и его заголовки
                        AnsiString papka = mfPapkaName(Buffer[adr_pop].Vremya);
                        //papka = FormatDateTime("yyyy-mm-dd", Buffer[adr_pop].Vremya);
                        int nt_d, nd_d, nc_d;
                        AnsiString nt_str;
                        if (!DirectoryExists(PathName+papka))
                        {
                                CreateDirectory((PathName+papka).c_str(), NULL);
                        }
                        AnsiString marsh = PathName + papka;
                        FNI = marsh + "\\Count.ini";
                        TIniFile *ifile = new TIniFile( FNI );
                        nt_d = ifile->ReadInteger("count", "tube", 0);
                        nd_d = ifile->ReadInteger("count", "defect", 0);
                        nc_d = ifile->ReadInteger("count", "calibrovka", 0);
                        nt_d++;
                        ifile->WriteString("count", "tube", nt_d);
                        ifile->WriteString("count", "defect", nd_d);
                        ifile->WriteString("count", "calibrovka", nc_d);
                        delete ifile;
                        ifile = NULL;
                        nt_str = IntToStr(nt_d);
                        while ( nt_str.Length()<3 ) nt_str = "0" + nt_str;
                        // ---
                        EnterCriticalSection(&cs_pp);
                        // ---
                        AnsiString FN = PathName + papka + "\\";
                        FN = FN + "tube";
                        FN = FN + nt_str;
                        FN = FN + "_";
                        FN = FN + FormatDateTime("yyyy-mm-dd_hh-nn-ss", Buffer[adr_pop].Vremya);
                        FN = FN + ".txt";
                        // проверка наличи€ файла
                        if ( !FileExists(FN) )
                        {
                                pFileWr = new TFileStream(FN.c_str(), fmCreate);
                                // заголовок
                                AnsiString s;
                                s = "# " + ExtractFileName(FN) + "\r\n";
                                pFileWr->Write(s.c_str(), s.Length());
                                s = "# ArhivDefectTube V01 T2Stan5 \r\n";
                                pFileWr->Write(s.c_str(), s.Length());
                                s = "# -----------------------------------------------\r\n";
                                pFileWr->Write(s.c_str(), s.Length());
                                delete pFileWr;
                        }
                        // открыть файл на дозапись
                        pFileWr = new TFileStream(FN.c_str(), fmOpenWrite + fmShareDenyNone);
                        pFileWr->Seek(pFileWr->Size,1);
                        FNC = FN;
                        // оптом состо€ние датчиков
                        TTime Vr = Buffer[adr_pop].Vremya;
                        for (int i=3;i<14;i++)
                        {
                                SaveDat(Vr, 1, i, StatDat[i]);
                        }
                        LeaveCriticalSection(&cs_pp);
                        continue;
                }
        }
        if (pFileWr)
        {
                delete pFileWr;
                pFileWr = NULL;
        }
        DeleteCriticalSection(&cs_pp);
}
//---------------------------------------------------------------------------
void __fastcall TWriteTubeFile::Init(AnsiString PathName)
{
        if (fl_OpenFile)
        {
                // закрыть файл
                if (pFileWr)
                {
                        delete pFileWr;
                        pFileWr = NULL;
                }
        }
        adr_push = 0;
        adr_pop = 0;
        for (int i=0;i<14;i++)
        {
                if (BoxRead->nsd_dat[i]<0)
                {
                        StatDat[i] = true;
                }
                else
                {
                        StatDat[i] = (bool)BoxRead->nsd_dat[i];
                }
        }
        fl_RecFile = false;
        this->PathName = PathName;
}
//---------------------------------------------------------------------------
int __fastcall TWriteTubeFile::NormAdr(int is, int len)
{
        while (is>=len) is = is - len;
        return is;
}
//---------------------------------------------------------------------------
void __fastcall TWriteTubeFile::Push(TTime Vremya, unsigned char Type, unsigned char N, unsigned char Stat)
{
        TBuffer d;
        int idx = NormAdr(adr_push+1, LenBufferWriteTubeFile);
        if (idx==adr_pop)
        {       // освободить одну €чейку
                Pop(&d);
        }
        d.Vremya = Vremya;
        d.Type = Type;
        d.N = N;
        d.Stat = Stat;
        EnterCriticalSection(&cs_pp);
        Buffer[adr_push] = d;
        adr_push = idx;
        LeaveCriticalSection(&cs_pp);
}
//---------------------------------------------------------------------------
int  __fastcall TWriteTubeFile::Pop(TBuffer *d)
{
        if (adr_pop==adr_push) return 0;
        EnterCriticalSection(&cs_pp);
        *d = Buffer[adr_pop];
        if (d->Type==1)
                StatDat[d->N] = d->Stat;
        adr_pop = NormAdr(adr_pop+1, LenBufferWriteTubeFile);
        LeaveCriticalSection(&cs_pp);
        return 1;
}
//---------------------------------------------------------------------------
void __fastcall TWriteTubeFile::Record()
{
        fl_RecFile = true;
        Resume();
}
//---------------------------------------------------------------------------
void __fastcall TWriteTubeFile::Stop()
{
        FLC = false;
        fl_RecFile = false;
        Resume();
}
void __fastcall TWriteTubeFile::Stop(bool flc)
{
        FLC = flc;
        fl_RecFile = false;
        Resume();
}
//---------------------------------------------------------------------------
void __fastcall TWriteTubeFile::SaveDat(TTime Vr, unsigned char Type, unsigned char N, unsigned char Stat)
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

