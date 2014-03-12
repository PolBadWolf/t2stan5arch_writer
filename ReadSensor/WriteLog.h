//---------------------------------------------------------------------------

#ifndef WriteLogH
#define WriteLogH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TWriteLog : public TThread
{
public:
    __fastcall TWriteLog();
private:
    AnsiString *buffStr;
    int         buffStrBg;
    int         buffStrEd;
    int __fastcall NormAddr(int addr);
    CRITICAL_SECTION csBuff;
    int __fastcall Pop(AnsiString *datLog);  
protected:
    void __fastcall Execute();
public:
    void __fastcall Push(AnsiString datLog);
};
#endif
