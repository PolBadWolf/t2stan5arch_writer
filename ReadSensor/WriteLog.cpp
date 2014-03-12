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
}

void __fastcall TWriteLog::Execute()
{
    while(!Terminated)
    {
    }
    // ---------------------------
    // close
    if (buffStr)
    {
        delete[] buffStr;
        buffStr = NULL;
    }
    DeleteCriticalSection(&csBuff);
    // ******************************
}

void __fastcall TWriteLog::Push(AnsiString datLog)
{
    if (Terminated) return;
    if (!buffStr)   return;
    EnterCriticalSection(&csBuff);
    int buffStrNew = NormAddr(buffStrBg+1);
    if (buffStrNew==buffStrEd) return;
    buffStr[buffStrBg] = datLog;
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

