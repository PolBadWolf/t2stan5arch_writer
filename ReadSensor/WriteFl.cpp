//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WriteFl.h"
#include "Unit1.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TNWriteFl::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------
__fastcall TNWriteFl::TNWriteFl()
        : TThread(false)
{
//Buff.
}
//---------------------------------------------------------------------------
void __fastcall TNWriteFl::Execute()
{

        int stat = 0;
        int fl = 1;
        TFileStream *pFileWr = NULL;
        AnsiString old_nam = "";
        FreeOnTerminate = true;
        //---- Place thread code here ----
        Suspend();
        while(!Terminated)
        {
                if (stat>0)
                {

                        if (pFileWr)
                        {
                                delete pFileWr;
                                pFileWr = NULL;
                        }

                        Suspend();
                        Sleep(250);
                        stat = 0;
                        continue;
                }
                if (fl)
                {
                        stat = Buff.ReadData(&Dat);
                        if (stat>0) continue;
                }

                if (Dat.FileName!=old_nam)
                {
                        old_nam = Dat.FileName;
                        if (pFileWr)
                        {
                                delete pFileWr;
                                pFileWr = NULL;
                        }
                }

                if (pFileWr==NULL)
                {
                        try
                        {
                                pFileWr = new TFileStream(old_nam.c_str(), fmOpenWrite + fmShareDenyNone);
                                pFileWr->Seek(pFileWr->Size,1);

                        }
                        catch(...)
                        {
                                pFileWr = NULL;
                        }
                        if (pFileWr==NULL)
                        {
                                try
                                {
                                        AnsiString s = "";
                                        pFileWr = new TFileStream(old_nam.c_str(), fmCreate);
                                        s = "# " + ExtractFileName(Dat.FileName) + "\r\n";
                                        pFileWr->Write(s.c_str(), s.Length());
                                        s = "# ArhivDefect V01 T2Stan5 \r\n";
                                        pFileWr->Write(s.c_str(), s.Length());
                                        s = "# -----------------------------------------------\r\n";
                                        pFileWr->Write(s.c_str(), s.Length());
                                }
                                catch(...)
                                {
                                        pFileWr = NULL;
                                        fl = 0;
                                        Sleep(100);
                                        continue;
                                }
                        }
                }
                fl = 1;
                pFileWr->Write(Dat.FileStr.c_str(), Dat.FileStr.Length());
        }
        //---- Place thread code here ----
        if (pFileWr)
        {
                delete pFileWr;
                pFileWr = NULL;
        }
}
//---------------------------------------------------------------------------
__fastcall TDTypeBuff::TDTypeBuff()
{
        FileName = "";
        FileStr = "";
}
//---------------------------------------------------------------------------
__fastcall TDTypeBuff::~TDTypeBuff()
{
}
//---------------------------------------------------------------------------
int  __fastcall TNWriteFl::SendData(TDTypeBuff &dat)
{
        int stat = Buff.SendData(dat);
        if ( (stat==0) && Suspended ) Resume();
        return stat;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

