//---------------------------------------------------------------------------

#ifndef WriteFlH
#define WriteFlH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
#include "ShablBuff.h"
class TDTypeBuff
{
public:
        __fastcall TDTypeBuff();
        __fastcall ~TDTypeBuff();
        AnsiString FileStr;
        AnsiString FileName;
};

class TNWriteFl : public TThread
{
private:
        TShablBuff<TDTypeBuff,256> Buff;
        void __fastcall dddbb(void);
        TDTypeBuff Dat;
protected:
        void __fastcall Execute();
public:
        __fastcall TNWriteFl();
        int  __fastcall SendData(TDTypeBuff &dat);
};
//---------------------------------------------------------------------------
#endif
