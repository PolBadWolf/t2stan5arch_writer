//---------------------------------------------------------------------------


#pragma hdrstop

#include "ComPort.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TComPort::TComPort()
{
        COMport = NULL;
        NitWrite = NULL;
        //
        OutxCtsFlow = false;
        OutxDsrFlow = false;
        DTR_CONTROL = DTR_DISABLE;
        DsrSensitivity = false;
        TXContinueOnXoff = true;
        OutX = false;
        InX = false;
        ErrorChar = false;
        pNull = false;
        RTS_CONTROL = RTS_DISABLE;
        AbortOnError = false;
        pXonLim = 2048;
        pXoffLim = 512;
        ByteSize = ByteSize8;
        StopBits = StopBitsTWO;
        pXonChar = 17;
        pXoffChar = 19;
        pErrorChar = 0;
        pEofChar = 0;
        pEvtChar = 0;
        //
}
//---------------------------------------------------------------------------
__fastcall TComPort::~TComPort()
{
        Close();
}
//---------------------------------------------------------------------------
int  __fastcall TComPort::Open(AnsiString PortName,
                                eBaudRate BaudRate,
                                  eParity Parity)
{
        if (COMport) {
                Close();
        }
        COMport = CreateFile( PortName.c_str(),
                              GENERIC_READ | GENERIC_WRITE,
                              0,
                              NULL,
                              OPEN_EXISTING,
                              FILE_FLAG_OVERLAPPED, //FILE_ATTRIBUTE_NORMAL, //NULL,
                              NULL );
        if (COMport == INVALID_HANDLE_VALUE) {
                COMport = NULL;
                return 1;
        }
        DCB dcb;
        dcb.DCBlength = sizeof(DCB);
        if (!GetCommState(COMport, &dcb) ) {
                Close();
                return 2;
        }
        dcb.BaudRate = BaudRate;
        dcb.fParity = Parity;
        //
        dcb.fOutxCtsFlow      = OutxCtsFlow;
        dcb.fOutxDsrFlow      = OutxDsrFlow;
        dcb.fDtrControl       = DTR_CONTROL;
        dcb.fDsrSensitivity   = DsrSensitivity;
        dcb.fTXContinueOnXoff = TXContinueOnXoff;
        dcb.fOutX             = OutX;
        dcb.fInX              = InX;
        dcb.fErrorChar        = ErrorChar;
        dcb.fNull             = pNull;
        dcb.fRtsControl       = RTS_CONTROL;
        dcb.fAbortOnError     = AbortOnError;
        dcb.XonLim            = pXonLim;
        dcb.XoffLim           = pXoffLim;
        dcb.ByteSize          = ByteSize;
        dcb.StopBits          = StopBits;
        dcb.XonChar           = pXonChar;
        dcb.XoffChar          = pXoffChar;
        dcb.ErrorChar         = pErrorChar;
        dcb.EofChar           = pEofChar;
        dcb.EvtChar           = pEvtChar;
        if (!SetCommState(COMport, &dcb)) {
                Close();
                return 3;
        }
        COMMTIMEOUTS touts;
        touts.ReadIntervalTimeout         = 0;
        touts.ReadTotalTimeoutMultiplier  = 0;
        touts.ReadTotalTimeoutConstant    = 0;
        touts.WriteTotalTimeoutMultiplier = 0;
        touts.WriteTotalTimeoutConstant   = 0;
        if (!SetCommTimeouts(COMport, &touts)) {
                Close();
                return 4;
        }
        //
        SetupComm(COMport, 4096, 32768);
        NitWrite = new TComPortWr(COMport);
        PurgeComm(COMport, PURGE_TXCLEAR);
        NitWrite->Resume();
        NitRead = new TComPortRd(COMport);
        PurgeComm(COMport, PURGE_RXCLEAR);
        NitRead->EventNewDate = fEventNewDate;
        NitRead->Resume();
        return 0;
}
//---------------------------------------------------------------------------
void  __fastcall TComPort::Close()
{
        unsigned long ec = 0;
        if (NitWrite) {
                NitWrite->Terminate();
                NitWrite->Resume();
                //
                GetExitCodeThread( (HANDLE)NitWrite->Handle, &ec );
                while(ec) {
                        Sleep(20);
                        PurgeComm(COMport, PURGE_TXABORT);
                        GetExitCodeThread( (HANDLE)NitWrite->Handle, &ec );
                }
                delete NitWrite;
                NitWrite = NULL;
        }
        if (NitRead) {
                NitRead->Terminate();
                NitRead->Resume();
                while(ec) {
                        Sleep(20);
                        PurgeComm(COMport, PURGE_RXABORT);
                        GetExitCodeThread( (HANDLE)NitRead->Handle, &ec );
                }
                delete NitRead;
                NitRead = NULL;
        }
        CloseHandle(COMport);
}
//---------------------------------------------------------------------------
__fastcall TComPortWr::TComPortWr(HANDLE HnComPort) : TThread(true)
{
        FreeOnTerminate = false;
        ComPortWr = HnComPort;
}
//---------------------------------------------------------------------------
void __fastcall TComPortWr::Execute()
{
        unsigned char *BuffWr;
        int dlina;
        DWORD temp, signal;
        bool flnowr;
        OverlapsWR.hEvent = CreateEvent(NULL, true, true, NULL);
        flnowr = true;
        while(!Terminated) {
                if (flnowr) {
                        if ( Mass.Read(&BuffWr, &dlina) ) {
                                Sleep(20);
                                continue;
                        }
                        WriteFile(ComPortWr, BuffWr, dlina, &temp, &OverlapsWR);
                        flnowr = false;
                }
                else {
                        signal = WaitForSingleObject(OverlapsWR.hEvent, 20);
                        if((signal == WAIT_OBJECT_0) && (GetOverlappedResult(ComPortWr, &OverlapsWR, &temp, true))) {
                                //ok
                                Mass.Next();
                                flnowr = true;
                        }
                }
        }
        CloseHandle(OverlapsWR.hEvent);
}
//---------------------------------------------------------------------------
__fastcall TComPortWr::TMass::TMassStruct::TMassStruct()
{
        BufferLen = 0;
}
//---------------------------------------------------------------------------
__fastcall TComPortWr::TMass::TMass()
{
        ind_rd = 0;
        ind_wr = 0;
        InitializeCriticalSection(&cs_PP);
}
//---------------------------------------------------------------------------
__fastcall TComPortWr::TMass::~TMass()
{
        DeleteCriticalSection(&cs_PP);
}
//---------------------------------------------------------------------------
int __fastcall TComPortWr::TMass::AdrNorm(int adr)
{
        while(adr<0)
                adr = adr + 32;
        while(adr>=32)
                adr = adr - 32;
        return adr;
}
//---------------------------------------------------------------------------
void __fastcall TComPortWr::TMass::Push(unsigned char *Buf, int Len)
{
        EnterCriticalSection(&cs_PP);
        if ( (Len>0) && (Len<=32768) ) {
                int future = AdrNorm(ind_wr+1);
                if (future!=ind_rd) {
                        for (int i=0;i<Len;i++)
                                MassStruct[ind_wr].Buffer[i] = Buf[i];
                        MassStruct[ind_wr].BufferLen = Len;
                        ind_wr = future;
                }
        }
        LeaveCriticalSection(&cs_PP);
}
//---------------------------------------------------------------------------
int  __fastcall TComPortWr::TMass::Read(unsigned char **Buf, int *Len)
{
        int stat = 1;
        EnterCriticalSection(&cs_PP);
        if (ind_rd!=ind_wr) {
                *Buf = MassStruct[ind_rd].Buffer;
                *Len = MassStruct[ind_rd].BufferLen;
                stat = 0;
        }
        LeaveCriticalSection(&cs_PP);
        return stat;
}
//---------------------------------------------------------------------------
void __fastcall TComPortWr::TMass::Next()
{
        EnterCriticalSection(&cs_PP);
        if (ind_rd!=ind_wr)
                ind_rd = AdrNorm(ind_rd+1);
        LeaveCriticalSection(&cs_PP);
}
//---------------------------------------------------------------------------
int  __fastcall TComPortWr::TMass::Deep()
{
        int dl;
        EnterCriticalSection(&cs_PP);
        dl = AdrNorm(ind_wr-ind_rd);
        LeaveCriticalSection(&cs_PP);
        return dl;
}
//---------------------------------------------------------------------------
void __fastcall TComPort::Write(unsigned char *Bufferx, int Lenx)
{
        NitWrite->Push(Bufferx, Lenx);
}
//---------------------------------------------------------------------------
void __fastcall TComPortWr::Push(unsigned char *Buf, int Len)
{
        Mass.Push(Buf, Len);
}
//---------------------------------------------------------------------------
int  __fastcall TComPortWr::Deep()
{
        return Mass.Deep();
}
//---------------------------------------------------------------------------
int  __fastcall TComPort::Deep()
{
        return NitWrite->Deep();
}
//---------------------------------------------------------------------------
__fastcall TComPortRd::TComPortRd(HANDLE HnComPort) : TThread(true)
{
        FreeOnTerminate = false;
        ComPortRd = HnComPort;
        EventNewDate = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TComPortRd::Execute()
{
        FlCallBlock = false;
        FlCallSend = false;
        OverlapsRD.hEvent = CreateEvent(NULL, true, true, NULL);
        SetCommMask(ComPortRd, EV_RXCHAR);
        while (!Terminated) {
                WaitCommEvent(ComPortRd, &mask, &OverlapsRD);
                signal = WaitForSingleObject(OverlapsRD.hEvent, 1500);
                if (signal==WAIT_OBJECT_0) {
                        if (GetOverlappedResult(ComPortRd, &OverlapsRD, &temp1, true) ) {
                                if ( (mask & EV_RXCHAR) != 0) {
                                        FlCallSend = true;
                                }
                        }
                }
                else {
                        Sleep(20);
                }
                if ( !EventNewDate ) continue;
                if ( FlCallBlock ) continue;
                if (  FlCallSend ) {
                        ClearCommError(ComPortRd, &temp2, &curstat);
                        btr = curstat.cbInQue;
                        if ( (btr) && (!Terminated) )
                                Synchronize(EventNewDateSynhr);
                        else
                                FlCallSend = false;
                }
        }
        CloseHandle(OverlapsRD.hEvent);
}
//---------------------------------------------------------------------------
void __fastcall TComPortRd::EventNewDateSynhr()
{
        if (EventNewDate) {
                EventNewDate(btr);
        }
}
//---------------------------------------------------------------------------
int  __fastcall TComPortRd::ReadBuf(unsigned char *Buf, DWORD *len_zakaz, DWORD *len_ok)
{
        DWORD Len;
        int stat;
        FlCallBlock = true;
        ClearCommError(ComPortRd, &temp2, &curstat);
        Len = curstat.cbInQue;
        if ( (*len_zakaz)>Len )
                *len_zakaz = Len;
        if ( (*len_zakaz)>0 ) {
                stat = ReadFile(ComPortRd, Buf, *len_zakaz, len_ok, &OverlapsRD);
                if (stat) stat = 0;
                else {
                        stat = 2;
                        len_ok = 0;
                }
        }
        else {
                stat = 1;
                len_ok = 0;
        }
        FlCallBlock = false;
        return stat;
}
//---------------------------------------------------------------------------
int  __fastcall TComPort::ReadBuf(unsigned char *Buf, DWORD *len_zakaz, DWORD *len_ok)
{
        if (NitRead)
                return NitRead->ReadBuf(Buf, len_zakaz, len_ok);
        else
                return 7;
}
//---------------------------------------------------------------------------
void __fastcall TComPort::fEventNewDate(int RdByte)
{
        if (EventNewDate)
                EventNewDate(RdByte);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

