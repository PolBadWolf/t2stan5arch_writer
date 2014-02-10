//---------------------------------------------------------------------------

#ifndef ShablBuffH
#define ShablBuffH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
template<class T, int len>
class TShablBuff
{
private:
        T   *Buff;
        int BuffRead;
        int BuffWrite;
        CRITICAL_SECTION csT;
        int __fastcall Buff_Adr(int adr)
        {
                while (adr>=len)    adr = adr - len;
                return adr;
        }
public:
        __fastcall TShablBuff()
        {
                BuffRead  = 0;
                BuffWrite = 0;
                Buff = new T[len];
                InitializeCriticalSection(&csT);
        }
        __fastcall ~TShablBuff()
        {
                delete Buff;
                DeleteCriticalSection(&csT);
        }
        int __fastcall SendData(T data)
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
        int __fastcall ReadData(T *data)
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
};
#endif
