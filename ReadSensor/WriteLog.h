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
protected:
    void __fastcall Execute();
public:
};
#endif
