//---------------------------------------------------------------------------
// User Functions

#include <vcl.h>
#pragma hdrstop

#include "UserFunction1.h"
#include <math.h>
#include "Unit1.h"
#include "wstring.h"
#include "BoxRead.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

extern TBoxRead *BoxRead;

// Diametr tube -> Len Segment
// Диаметр трубы в длину сегмента
double __fastcall FnDiametr2LenSegment(double D_Tube)
{
        const double Dmin     = 180;
        const double PI       = 3.1415926;
        const double alfa     = 25.95*PI/180;
        const int    nSegment = 8;
        double B;    // добавка до точки соприкосновения ролика с трубой
        double Ds;   // диаметр точки соприкосновения ролика с трубой
        double Ls;   // длина окружности точки соприкосновения ролика с трубой
        B  = (D_Tube/2)*tan(alfa)*sin(alfa);
        Ds = Dmin + (B*2);
        Ls = Ds * PI;
        return Ls/nSegment;
}

int  __fastcall TForm1::ReadFromBDLastNumberTude(TADOConnection *connect)
{
    TField *Pole;
    int  Resultat = -1;
    TADOQuery *dQuery = new TADOQuery(connect->Owner);
    if (dQuery==NULL)
        return Resultat;
    __try
    {
        if (connect->Connected==false)
            connect->Open();
        dQuery->Connection = connect;
        dQuery->CommandTimeout = 1;
        dQuery->Close();
        // ====================================================
        // find last number tube ( no sample )
        dQuery->SQL->Clear();
        dQuery->SQL->Add("SELECT *");
        dQuery->SQL->Add("FROM `defectsdata`");
        dQuery->SQL->Add("WHERE");
        dQuery->SQL->Add("   `NumberTube` <>  '0'");
        dQuery->SQL->Add("ORDER BY");
        dQuery->SQL->Add("   `IndexData` DESC");
        dQuery->SQL->Add("LIMIT 1");
        dQuery->Open();
        // find field
        Pole = dQuery->FindField("NumberTube");
        if (Pole==NULL)
        {   // Date Base Error
            Application->MessageBox("Not found field \"NumberTube\" " , "Date Base Error", 0);
            Form1->Close();
        } else
        if ( dQuery->RecordCount==0 )
        {   // no records - set number 0
            Resultat = 0;
        }
        else
        {
            // set number tube
            Resultat = Pole->Value;
            vSec.SecRead = 0;
        }
    }
    __finally
    {
        __try
        {
            dQuery->Close();
        }
        __finally
        {
            return Resultat;
        }
    }
}

Variant __fastcall ReadField(TADOQuery *fQuery, AnsiString FieldName, int *status)
{
    TField *Pole;
    Variant resultat;
    Pole = fQuery->FindField(FieldName);
    if ( !Pole )
    {
        *status = 1; 
        return NULL;
    }
    try
    {
        resultat = Pole->Value;
    }
    catch(...)
    {
        resultat = NULL;
    }
    return resultat;
}

int  __fastcall TForm1::ReadFromBDLastParametrs(TADOConnection *connect, int *id_parametr, int *id_melt, AnsiString *CodeMelt, double *SizeTube)
{
    int status = 1;
    TADOQuery *dQuery = new TADOQuery(connect->Owner);
    if (dQuery==NULL)
        return 1;
    __try
    {
        if (connect->Connected==false)
            connect->Open();
        dQuery->Connection = connect;
        dQuery->CommandTimeout = 1;
        dQuery->Close();
        dQuery->SQL->Clear();
        dQuery->SQL->Add("SELECT");
        dQuery->SQL->Add(" `defectsdata`.`Id_Param` AS `id_parametr`");
        dQuery->SQL->Add(",`parameters`.`Id_Melt` AS `id_melt`");
        dQuery->SQL->Add(",`melts`.`NameMelt` AS `CodeMelt`");
        dQuery->SQL->Add(",`sizetube`.`SizeTube` AS `SizeTube`");
        dQuery->SQL->Add("FROM");
        dQuery->SQL->Add("`defectsdata`");
        dQuery->SQL->Add("Inner Join `parameters` ON `defectsdata`.`Id_Param` = `parameters`.`Id_Param`");
        dQuery->SQL->Add("Inner Join `melts` ON `parameters`.`Id_Melt` = `melts`.`Id_Melt`");
        dQuery->SQL->Add("Inner Join `sizetube` ON `melts`.`Id_SizeTube` = `sizetube`.`Id_SizeTube`");
        dQuery->SQL->Add("ORDER BY");
        dQuery->SQL->Add("`defectsdata`.`IndexData` DESC");
        dQuery->SQL->Add("LIMIT 1");
        dQuery->Open();
        if (dQuery->RecordCount==0)
        { // no records
            status = 1;
            *id_parametr = 0;
            *id_melt = 0;
            *CodeMelt = "No Melt";
            *SizeTube = 0;
        }
        else
        {
            *id_parametr = ReadField( dQuery, "id_parametr", &status);
            *id_melt     = ReadField( dQuery, "id_melt",     &status);
            *CodeMelt    = ReadField( dQuery, "CodeMelt",    &status);
            *SizeTube    = ReadField( dQuery, "SizeTube",    &status);
            vSec.SecRead = 0;
        }
    }
    __finally
    {
        __try
        {
            dQuery->Close();
        }
        __finally
        {
            return status;
        }
    }
}

int  __fastcall TForm1::ReadFromBDNewParametrs(TADOConnection *connect, int *id_parametr, int *id_melt, AnsiString *CodeMelt, double *SizeTube)
{
    int status = 1;
    TADOQuery *dQuery = new TADOQuery(connect->Owner);
    TADOQuery *cQuery = new TADOQuery(connect->Owner);
    if (dQuery==NULL)
        return status;
    if (cQuery==NULL)
        return status;
    int cid;
    __try
    {
        if (connect->Connected==false)
            connect->Open();
        cQuery->Connection = connect;
        cQuery->CommandTimeout = 1;
        cQuery->SQL->Clear();
        cQuery->SQL->Add("SELECT");
        cQuery->SQL->Add(" `parameters`.`Id_Param` AS `id_parametr`");
        cQuery->SQL->Add("FROM");
        cQuery->SQL->Add("`parameters`");
        cQuery->SQL->Add("ORDER BY");
        cQuery->SQL->Add("`parameters`.`Id_Param` DESC");
        cQuery->SQL->Add("LIMIT 1");

        dQuery->Connection = connect;
        dQuery->CommandTimeout = 1;
        dQuery->SQL->Clear();
        dQuery->SQL->Add("SELECT");
        dQuery->SQL->Add(" `parameters`.`Id_Param` AS `id_parametr`");
        dQuery->SQL->Add(",`parameters`.`Id_Melt` AS `id_melt`");
        dQuery->SQL->Add(",`melts`.`NameMelt` AS `CodeMelt`");
        dQuery->SQL->Add(",`sizetube`.`SizeTube` AS `SizeTube`");
        dQuery->SQL->Add("FROM");
        dQuery->SQL->Add("`parameters`");
        dQuery->SQL->Add("Inner Join `melts` ON `parameters`.`Id_Melt` = `melts`.`Id_Melt`");
        dQuery->SQL->Add("Inner Join `sizetube` ON `melts`.`Id_SizeTube` = `sizetube`.`Id_SizeTube`");
        dQuery->SQL->Add("ORDER BY");
        dQuery->SQL->Add("`parameters`.`Id_Param` DESC");
        dQuery->SQL->Add("LIMIT 1");

        dQuery->Open();
        cQuery->Open();
        if (dQuery->RecordCount==0)
        { // no records
            *id_parametr = 0;
            *id_melt = 0;
            *CodeMelt = "No Melt";
            *SizeTube = 0;
        }
        else
        {
            status = 0;
            cid          = ReadField( cQuery, "id_parametr", &status);
            *id_parametr = ReadField( dQuery, "id_parametr", &status);
            *id_melt     = ReadField( dQuery, "id_melt",     &status);
            *CodeMelt    = ReadField( dQuery, "CodeMelt",    &status);
            *SizeTube    = ReadField( dQuery, "SizeTube",    &status);
            if ( (*id_parametr)!=cid )
                status = -1; // ошибка BD
            vSec.SecRead = 0;
        }
    }
    __finally
    {
        __try
        {
    dQuery->Close();
    cQuery->Close();
        }
        __finally
        {
            dQuery->SQL->Clear();
            cQuery->SQL->Clear();
            delete cQuery;
        }
    }
    return status;
}

void __fastcall WriteBD_Datas(TADOConnection *connect, int NumberTube, signed char *Massive, int Len, int GlagDefect, int IdParametr)
{
    TADOQuery *Query = new TADOQuery(connect->Owner);
    if (Query==NULL)
        return;
    TDateTime DT = Now();
    TMemoryStream *MS = new TMemoryStream;
    MS->Clear();
    MS->WriteBuffer(Massive, Len);
        if (connect->Connected==false)
            connect->Open();
    Query->Connection = connect;
    Query->Close();
    Query->Parameters->Clear();
    Query->SQL->Clear();
    Query->SQL->Add("INSERT INTO");
    Query->SQL->Add("`defectsdata`");
    Query->SQL->Add("(");
    Query->SQL->Add(" `DatePr`");
    Query->SQL->Add(",`TimePr`");
    Query->SQL->Add(",`NumberTube`");
    Query->SQL->Add(",`CountSegments`");
    Query->SQL->Add(",`DataSensors`");
    Query->SQL->Add(",`FlDefectTube`");
    Query->SQL->Add(",`Id_Param`");
    Query->SQL->Add(")");
    Query->SQL->Add("values");
    Query->SQL->Add("(");
    Query->SQL->Add(" :IDATE");
    Query->SQL->Add(",:ITIME");
    Query->SQL->Add(", :INUMBERTUBE ");
    Query->SQL->Add(", :ILEN ");
    Query->SQL->Add(", :IMASS ");
    Query->SQL->Add(", :IFLAG ");
    Query->SQL->Add(", :IPARAM ");
    Query->SQL->Add(")");
    Query->Parameters->ParamByName("IDATE")      ->Value = FormatDateTime("yyyy-mm-dd", DT);
    Query->Parameters->ParamByName("ITIME")      ->Value = FormatDateTime("hh:nn:ss"  , DT);

    Query->Parameters->ParamByName("INUMBERTUBE")->Value = NumberTube;
    Query->Parameters->ParamByName("ILEN")       ->Value = Len;
    Query->Parameters->ParamByName("IMASS")      ->LoadFromStream(MS, ftBlob);
    Query->Parameters->ParamByName("IFLAG")      ->Value = GlagDefect;
    Query->Parameters->ParamByName("IPARAM")     ->Value = IdParametr;
    __try
    {
        Query->ExecSQL();
        vSec.SecWrite = 0;
    }
    __finally
    {
        MS->Clear();
        delete MS;
        delete Query;
    }
    // connect->Close();
}

void __fastcall Show_NumberTube(int nTube)
{
    if (!BoxRead) return;
    if (BoxRead->sampleTriger)
        Form1->Label_NumberTube->Caption = "smpl";
    else
    {
        if (nTube==0)
            Form1->Label_NumberTube->Caption = "NS";
        else
            Form1->Label_NumberTube->Caption = nTube;
    }
}

void __fastcall Show_Parametrs(int nTube, double SizeTube, double LenSegmentTube, AnsiString CodeMelt)
{
    Show_NumberTube(nTube);
    // ----------------------------------------------
    Form1->Label_dTube->Caption = SizeTube;
    // ----------------------------------------------
    Form1->Memo1->Lines->Strings[0] = "Длина сегмента : " + FloatToStrF(LenSegmentTube, ffFixed, 6, 3);
}

void __fastcall Show_LenTube(double lTube)
{
    Form1->Memo1->Lines->Strings[1] = "Длина трубы : " + FloatToStrF(lTube/1000, ffFixed, 6, 3);
}

void __fastcall Show_LenTubeClear()
{
    Form1->Memo1->Lines->Strings[1] = "Длина трубы : ";
}

AnsiString __fastcall WriteFlTreeComp(int p,
        AnsiString year, AnsiString mes, AnsiString day,
        AnsiString smena, AnsiString melt,
        int rulon, int tube, AnsiString aTime, int defTube)
{
    if (p>6)
        return "";
    AnsiString sstring = "";
    switch (p)
    {
        case 0 : sstring =                            year;
                 break;
        case 1 : sstring = "\t"                      + mes;
                 break;
        case 2 : sstring = "\t\t"                    + day;
                 break;
        case 3 : sstring = "\t\t\t"                  + smena;
                 break;
        case 4 : sstring = "\t\t\t\tПлавка№ "        + melt;
                 break;
        case 5 : sstring = "\t\t\t\t\tРулон № "      + IntToStr(rulon);
                 break;
        case 6 : if (tube==0)
                    sstring = "\t\t\t\t\t\tК.О. "    + aTime;
                 else
                    sstring = "\t\t\t\t\t\tТруба № " + IntToStr(tube);
                 if (defTube==0)
                    sstring = sstring;
                 if (defTube==1)
                    sstring = sstring + " ";
                 if (defTube==2)
                    sstring = sstring + "\t";
                 break;
    }
     sstring = sstring + "\r\n";
	return sstring;
}
void __fastcall WriteFlTree(TADOConnection *connect)
{
    TADOQuery *Query = new TADOQuery(connect->Owner);
    if (!Query)
        return;

    Query->Connection = connect;
    Query->Close();
    Query->SQL->Clear();
    Query->SQL->Add("SELECT");
    Query->SQL->Add(" `defectsdata`.`IndexData`");
    Query->SQL->Add(",`defectsdata`.`DatePr`");
    Query->SQL->Add(",`defectsdata`.`TimePr`");
    Query->SQL->Add(",`defectsdata`.`NumberTube`");
    Query->SQL->Add(",`defectsdata`.`FlDefectTube`");
    Query->SQL->Add(",`melts`.`NameMelt`");
    Query->SQL->Add(",`parameters`.`NumberRoll`");
    Query->SQL->Add(",`parameters`.`Id_WorkSmen`");
    Query->SQL->Add(",`parameters`.`Id_Melt`");
    Query->SQL->Add(",`worksmens`.`NameSmen`");
    Query->SQL->Add("FROM");
    Query->SQL->Add("`defectsdata`");
    Query->SQL->Add("Left Join `parameters` ON `defectsdata`.`Id_Param` = `parameters`.`Id_Param`");
    Query->SQL->Add("Left Join `melts` ON `parameters`.`Id_Melt` = `melts`.`Id_Melt`");
    Query->SQL->Add("Left Join `worksmens` ON `parameters`.`Id_WorkSmen` = `worksmens`.`Id_WorkSmen`");
    Query->SQL->Add("ORDER BY");
    Query->SQL->Add("`defectsdata`.`IndexData` DESC");
    Query->SQL->Add("LIMIT 2");
    Query->Open();
    // -----------------
    TDate nDate, nTime;
    AnsiString nYear, nMesiac, nDay;
    int nIdSmena, nIdMelt;
    AnsiString aSmena, aMelt, aTime;
    int nRoll, numberTube, aDefTube;
    //
    TDate oDate;
    AnsiString oYear, oMesiac, oDay;
    int oIdSmena, oIdMelt;
    int oRoll;

    nDate      = Query->FieldByName("DatePr")->AsDateTime;
    nTime      = Query->FieldByName("TimePr")->AsDateTime;
    nYear      = FormatDateTime("yyyy", nDate);
    nMesiac    = FormatDateTime("yyyy-mm", nDate);
    nDay       = FormatDateTime("yyyy-mm-dd", nDate);
    nIdSmena   = Query->FieldByName("Id_WorkSmen")->AsInteger;
    nIdMelt    = Query->FieldByName("Id_Melt")->AsInteger;
    aSmena     = Query->FieldByName("NameSmen")->AsString;
    aMelt      = Query->FieldByName("NameMelt")->AsString;
    aTime      = FormatDateTime("(h:nn:ss)", nTime);
    nRoll      = Query->FieldByName("NumberRoll")->AsInteger;
    numberTube = Query->FieldByName("NumberTube")->AsInteger;
    aDefTube   = Query->FieldByName("FlDefectTube")->AsInteger;
    Query->Next();
    oDate      = Query->FieldByName("DatePr")->AsDateTime;
    oYear      = FormatDateTime("yyyy", oDate);
    oMesiac    = FormatDateTime("yyyy-mm", oDate);
    oDay       = FormatDateTime("yyyy-mm-dd", oDate);
    oIdSmena   = Query->FieldByName("Id_WorkSmen")->AsInteger;
    oIdMelt    = Query->FieldByName("Id_Melt")->AsInteger;
    oRoll      = Query->FieldByName("NumberRoll")->AsInteger;

    Query->Close();
    TFileStream *pFileTree;
    AnsiString nam = "Structnew.txt";
    try
    {
        pFileTree = new TFileStream(nam.c_str(), fmOpenWrite + fmShareDenyNone);
        pFileTree->Seek(pFileTree->Size,1);
    }
    catch(...)
    {
        pFileTree = new TFileStream(nam.c_str(), fmCreate);
    }

    int flg = 0;
	AnsiString s;
    flg = flg | (nYear!=oYear);
    if (flg)
	{	s=WriteFlTreeComp(0, nYear, nMesiac, nDay, aSmena, aMelt, nRoll, numberTube, aTime, aDefTube);
		pFileTree->Write(s.c_str(), s.Length()); }
    flg = flg | (nMesiac!=oMesiac);
    if (flg)
	{	s=WriteFlTreeComp(1, nYear, nMesiac, nDay, aSmena, aMelt, nRoll, numberTube, aTime, aDefTube);
		pFileTree->Write(s.c_str(), s.Length()); }
    flg = flg | (nDay!=oDay);
    if (flg)
	{	s=WriteFlTreeComp(2, nYear, nMesiac, nDay, aSmena, aMelt, nRoll, numberTube, aTime, aDefTube);
		pFileTree->Write(s.c_str(), s.Length()); }
    flg = flg | (nIdSmena!=oIdSmena);
    if (flg)
	{	s=WriteFlTreeComp(3, nYear, nMesiac, nDay, aSmena, aMelt, nRoll, numberTube, aTime, aDefTube);
		pFileTree->Write(s.c_str(), s.Length()); }
    flg = flg | (nIdMelt!=oIdMelt);
    if (flg)
	{	s=WriteFlTreeComp(4, nYear, nMesiac, nDay, aSmena, aMelt, nRoll, numberTube, aTime, aDefTube);
		pFileTree->Write(s.c_str(), s.Length()); }
    flg = flg | (nRoll!=oRoll);
    if (flg)
	{	s=WriteFlTreeComp(5, nYear, nMesiac, nDay, aSmena, aMelt, nRoll, numberTube, aTime, aDefTube);
		pFileTree->Write(s.c_str(), s.Length()); }
    s=WriteFlTreeComp(6, nYear, nMesiac, nDay, aSmena, aMelt, nRoll, numberTube, aTime, aDefTube);
	pFileTree->Write(s.c_str(), s.Length());

	delete pFileTree;
    //pFileTree = NULL;

    delete Query;
    Query = NULL;
}

