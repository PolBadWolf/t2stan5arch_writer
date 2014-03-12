//---------------------------------------------------------------------------


#pragma hdrstop

#include "WriteLog.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
#define lenBuff 16384

__fastcall TWriteLog::TWriteLog()
    : TThread(false)
{
    InitializeCriticalSection(&csBuff);
    buffStr = new AnsiString[lenBuff];
    buffStrBg = 0;
    buffStrEd = 0;
    pFile = NULL;
}

void __fastcall TWriteLog::Execute()
{
    AnsiString stroka;
    try
    {
        pFile = new TFileStream("log.txt", fmOpenWrite + fmShareDenyNone);
        pFile->Seek(pFile->Size,1);
    }
    catch (...)
    {
        AnsiString s = "Log :\r\n";
        pFile = new TFileStream("log.txt", fmCreate);
        pFile->Write(s.c_str(),s.Length());
        delete pFile;
        pFile = new TFileStream("log.txt", fmOpenWrite + fmShareDenyNone);
        pFile->Seek(pFile->Size,1);
    }

    while(!Terminated)
    {
        if (Pop(&stroka))
        {
            Sleep(2);
            continue;
        }
        pFile->Write(stroka.c_str(), stroka.Length());
    }
    // ---------------------------
    // close
    if (buffStr)
    {
        delete[] buffStr;
        buffStr = NULL;
    }
    DeleteCriticalSection(&csBuff);
    if (pFile)
    {
        delete pFile;
        pFile = NULL;
    }
    // ******************************
}

void __fastcall TWriteLog::Push(AnsiString datLog)
{
    if (Terminated) return;
    if (!buffStr)   return;
    AnsiString vr = FormatDateTime("yyyy-mm-dd hh:nn:ss.zzz   ", Now());
    EnterCriticalSection(&csBuff);
    int buffStrNew = NormAddr(buffStrBg+1);
    if (buffStrNew==buffStrEd) return;
    buffStr[buffStrBg] = vr + datLog + "\r\n";
    buffStrBg = buffStrNew;
    LeaveCriticalSection(&csBuff);
}

int __fastcall TWriteLog::NormAddr(int addr)
{
    while( (addr<0) || (addr>=lenBuff) )
    {
        if (addr<0)
            addr = addr + lenBuff;
        else
            addr = addr - lenBuff;
    }
    return addr;
}

int __fastcall TWriteLog::Pop(AnsiString *datLog)
{
    int st = 1;
    EnterCriticalSection(&csBuff);
    if (buffStrBg!=buffStrEd)
    {
        st = 0;
        *datLog = buffStr[buffStrEd];
        buffStrEd = NormAddr(buffStrEd+1);
    }
    LeaveCriticalSection(&csBuff);
    return st;
}

