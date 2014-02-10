//---------------------------------------------------------------------------

#ifndef WriteTubeFileH
#define WriteTubeFileH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
#define LenBufferWriteTubeFile 1024
class TWriteTubeFile : public TThread
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
        TBuffer Buffer[LenBufferWriteTubeFile];
        int __fastcall NormAdr(int is, int len);
        bool StatDat[14];
        int adr_push;
        int adr_pop;
        bool fl_RecFile;
        bool fl_OpenFile;
        AnsiString PathName;
        CRITICAL_SECTION cs_pp;
        bool FLC;
protected:
        void __fastcall Execute();
        TFileStream *pFileWr;
        void __fastcall SaveDat(TTime Vr, unsigned char Type, unsigned char N, unsigned char Stat);
public:
        __fastcall TWriteTubeFile(bool CreateSuspended);
        void __fastcall Init(AnsiString PathName);
        void __fastcall Push(TTime Vremya, unsigned char Type, unsigned char N, unsigned char Stat);
        void __fastcall Record();
        void __fastcall Stop();
        void __fastcall Stop(bool flc);
        int  __fastcall Pop( TBuffer *d );
};
//---------------------------------------------------------------------------
#endif


