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
        // массив состояния датчиков
        int  nsd_dat[14];
        // датчики наличия трубы
        bool nsd_Dtr1;
        bool nsd_Dtr2;
        bool nsd_Dtr1Old;
        bool nsd_Dtr2Old;
        // ключь движение вперед
        bool nsd_RunTube;
        // тригер состояния наличия трубы
        bool nsd_Tr_Tube;
        // тригер состояния дефектоскопии
        bool nsd_Tr_Def;
        // состояние калибровка
        bool nsd_St_Calibrovka;
        // Тригер калибровка
        bool nsd_Tr_Calibrovka;
        // датчики подняты
        bool nsd_fl_up;
        // датчики опущены
        bool nsd_fl_down;
        // массив входной инверсии датчиков
        bool sd_idat[14];

        bool sd_dat[14];
};
//---------------------------------------------------------------------------
#endif
