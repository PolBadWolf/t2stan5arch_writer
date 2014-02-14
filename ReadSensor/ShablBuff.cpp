//---------------------------------------------------------------------------


#pragma hdrstop

#include "ShablBuff.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

/*
template<class T>
__fastcall TShablBuff<T>::TShablBuff()
{
        int len;
        Read  = 0;
        Write = 0;
        Buff = new T[len];
        LenBuff = len;
        InitializeCriticalSection(&csT);
}
*/
//---------------------------------------------------------------------------
/*
template<class T>
__fastcall TShablBuff<T>::~TShablBuff<T>()
{
        delete Buff;
        DeleteCriticalSection(&csT);
}
*/
//---------------------------------------------------------------------------
/*
template<class T, int len>
int __fastcall TShablBuff<T, len>::Buff_Adr(int adr)
{
        while (adr>=LenBuff)    adr = adr - LenBuff;
        return adr;
}
*/
//---------------------------------------------------------------------------
/*
template<class T, int len>
int __fastcall TShablBuff<T, len>::SendData(T data)
{
        EnterCriticalSection(&csT);
        int stat = 1;
        int NewAdr = Buff_Adr(BuffWrite+1);
        if (NewAdr!=BuffRead)
        {
                Buff[BuffWrite] = data;
                BuffWrite = NewAdr;
                stat = 0;
        }
        LeaveCriticalSection(&csT);
        return stat;
}
*/
//---------------------------------------------------------------------------
/*
template<class T, int len>
int __fastcall TShablBuff<T, len>::ReadData(T *data)
{
        EnterCriticalSection(&csT);
        int stat = 1;
        if (BuffWrite!=BuffRead)
        {
                *data = Buff[BuffRead];
                BuffRead = Buff_Adr(BuffRead+1);
                stat = 0;
        }
        LeaveCriticalSection(&csT);
        return stat;
}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

