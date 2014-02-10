//---------------------------------------------------------------------------

#ifndef BoxReadH
#define BoxReadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
#include "ComPort.h"
class TNBoxRead : public TThread
{            
private:
        TComPort *Port;
        void __fastcall EventReadData(int RdByte);
        void __fastcall SelectPaks(unsigned char &bt);
        unsigned char sBuf[10];
        AnsiString __fastcall IntToStr2s(int dg);
        AnsiString PortName;
        eBaudRate  PortBaud;
        AnsiString PathName;
protected:
        void __fastcall Execute();
        void __fastcall ErrNewPort();
        void __fastcall ErrOpenPort();
public:
        __fastcall TNBoxRead();
        void _fastcall WriteFile();
        void _fastcall SelectDo();
        bool sd_fl_truba;
        bool sd_fl_calibrovka;
        bool sd_fl_calibrovka_beg;
        bool sd_fl_defektoskopiya;
        bool sd_dat_vpered;
        bool sd_dat_datchikiopusheny;
        bool sd_dat_nalichetruby1;
        bool sd_dat_nalichetruby2;
        TDateTime TimePack;
        // ������ ��������� ��������
        int  nsd_dat[14];
        // ������� ������� �����
        bool nsd_Dtr1;
        bool nsd_Dtr2;
        bool nsd_Dtr1Old;
        bool nsd_Dtr2Old;
        // ����� �������� ������
        bool nsd_RunTube;
        // ������ ��������� ������� �����
        bool nsd_Tr_Tube;
        // ������ ��������� �������������
        bool nsd_Tr_Def;
        // ��������� ����������
        bool nsd_St_Calibrovka;
        // ������ ����������
        bool nsd_Tr_Calibrovka;
        // ������� �������
        bool nsd_fl_up;
        // ������� �������
        bool nsd_fl_down;
        // ������ ������� �������� ��������
        bool sd_idat[14];

        bool sd_dat[14];
};
//---------------------------------------------------------------------------
#endif
