//---------------------------------------------------------------------------

#ifndef MShapeH
#define MShapeH
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TMShape : public TShape
{
protected:
	virtual void __fastcall Paint(void);
        int  FNap;
        virtual void __fastcall SetNap(int Value);
public:
	__fastcall TMShape(Classes::TComponent* AOwner);
        __property int  Nap={read=FNap, write = SetNap, default=0};
};

#endif
