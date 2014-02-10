//---------------------------------------------------------------------------

#ifndef WriteCalibrFileH
#define WriteCalibrFileH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
#define LenBufferWriteCalibrFile 1024
class TWriteCalibrFile : public TThread
{
private:
        class TBuffer
        {
        public:
                TTime Vremya;
                unsigned char Type;
                unsigned char N;
                unsigned char Stat;
        };
        TBuffer Buffer[LenBufferWriteCalibrFile];
        int __fastcall NormAdr(int is, int len);
        int adr_push;
        int adr_pop;
protected:
        void __fastcall Execute();
        TFileStream *pFileWr;
        //bool StatDat[14];
        bool fl_rec;
        CRITICAL_SECTION cs_cf;
        AnsiString PathName;
        void __fastcall SaveDat(TTime Vr, unsigned char Type, unsigned char N, unsigned char Stat);
public:
        __fastcall TWriteCalibrFile(bool CreateSuspended);
        void __fastcall Push(TTime Vremya, unsigned char Type, unsigned char N, unsigned char Stat);
        int  __fastcall Pop( TBuffer *d );
        void __fastcall StartRec(AnsiString path);
        void __fastcall StopRec();
};
//---------------------------------------------------------------------------
#endif
