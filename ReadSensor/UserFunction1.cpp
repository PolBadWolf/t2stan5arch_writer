//---------------------------------------------------------------------------
// User Functions

#include <vcl.h>
#pragma hdrstop

#include "UserFunction1.h"
#include <math.h>
#include "Unit1.h"
#include "wstring.h"
//#include "BoxRead1.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//extern TNBoxRead *BoxRead;

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

int  __fastcall TForm1::ReadFromBDLastNumberTude(TADOQuery *dQuery)
{
        TField *Pole = NULL;
        int  Resultat = -1;
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
        {       // Date Base Error
                Application->MessageBox("Not found field \"NumberTube\" " , "Date Base Error", 0);
                Form1->Close();
                return Resultat;
        }
        if ( dQuery->RecordCount==0 )
        {       // no records - set number 0
                Resultat = 0;
        }
        else
        {
                // set number tube
                Resultat = Pole->Value;
        }
        dQuery->Close();
        return Resultat;
}

Variant __fastcall ReadField(TADOQuery *fQuery, AnsiString FieldName, int *status)
{
    TField *Pole = NULL;
    Pole = fQuery->FindField(FieldName);
    if ( !Pole )
    {
        *status = 1; 
        return NULL;
    }
    return Pole->Value;
}

int  __fastcall TForm1::ReadFromBDLastParametrs(TADOQuery *dQuery, int *id_parametr, int *id_melt, AnsiString *CodeMelt, double *SizeTube)
{
    int status = 0;
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
    }
    dQuery->Close();
    return status;
}

int  __fastcall TForm1::ReadFromBDNewParametrs(TADOQuery *dQuery, int *id_parametr, int *id_melt, AnsiString *CodeMelt, double *SizeTube)
{
    int status = 0;
    TADOQuery *cQuery = new TADOQuery(dQuery->Owner);
    int cid = 0;
    cQuery->Connection = dQuery->Connection;
    cQuery->SQL->Clear();
    cQuery->SQL->Add("SELECT");
    cQuery->SQL->Add(" `parameters`.`Id_Param` AS `id_parametr`");
    cQuery->SQL->Add("FROM");
    cQuery->SQL->Add("`parameters`");
    cQuery->SQL->Add("ORDER BY");
    cQuery->SQL->Add("`parameters`.`Id_Param` DESC");
    cQuery->SQL->Add("LIMIT 1");

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
        status = 1;
        *id_parametr = 0;
        *id_melt = 0;
        *CodeMelt = "No Melt";
        *SizeTube = 0;
    }
    else
    {
        cid          = ReadField( cQuery, "id_parametr", &status);
        *id_parametr = ReadField( dQuery, "id_parametr", &status);
        *id_melt     = ReadField( dQuery, "id_melt",     &status);
        *CodeMelt    = ReadField( dQuery, "CodeMelt",    &status);
        *SizeTube    = ReadField( dQuery, "SizeTube",    &status);
        if ( (*id_parametr)!=cid )
            status = -1; // ошибка BD
    }
    dQuery->Close();
    cQuery->Close();
    dQuery->SQL->Clear();
    cQuery->SQL->Clear();
    delete cQuery;
    cQuery = NULL;
    return status;

}

void __fastcall WriteBD_Datas(TADOConnection *connect, int NumberTube, signed char *Massive, int Len, int GlagDefect, int IdParametr)
{
    TADOQuery *Query = new TADOQuery(connect->Owner);
    TDateTime DT = Now();
    TMemoryStream *MS = new TMemoryStream;
    MS->Clear();
    MS->WriteBuffer(Massive, Len);
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
    /*
    TParameter *Parm;
    Parm = Query->Parameters->AddParameter();;
    Parm->Name = "A";
    Parm->Value = 5;
    */
//    Query->Parameters->ParamByName("A")->Value = NumberTube;
//    Form1->ListBox1->Items = ll->
    Query->Parameters->ParamByName("IDATE")      ->Value = FormatDateTime("yyyy-mm-dd", DT);
    Query->Parameters->ParamByName("ITIME")      ->Value = FormatDateTime("hh:nn:ss"  , DT);

    Query->Parameters->ParamByName("INUMBERTUBE")->Value = NumberTube;
    Query->Parameters->ParamByName("ILEN")       ->Value = Len;
    Query->Parameters->ParamByName("IMASS")      ->LoadFromStream(MS, ftBlob);
    Query->Parameters->ParamByName("IFLAG")      ->Value = GlagDefect;
    Query->Parameters->ParamByName("IPARAM")     ->Value = IdParametr;

    Query->ExecSQL();
    MS->Clear();
    delete MS;
    MS = NULL;
    delete Query;
}

void __fastcall Show_NumberTube(int nTube)
{
//    if (!BoxRead) return;
/*    if (BoxRead->FlModeCalibrovka)
        Form1->Label_NumberTube->Caption = "smpl";
    else
    {
        if (nTube==0)
            Form1->Label_NumberTube->Caption = "N S";
        else
            Form1->Label_NumberTube->Caption = nTube;
    }
    */
}

void __fastcall Show_Parametrs(int nTube, double SizeTube, double LenSegmentTube, int otstup, AnsiString CodeMelt)
{
    Show_NumberTube(nTube);
    // ----------------------------------------------
    Form1->Label_dTube->Caption = SizeTube;
    // ----------------------------------------------
    Form1->Memo1->Lines->Strings[0] = "Длина сегмента : " + FloatToStrF(LenSegmentTube, ffFixed, 6, 3);
    Form1->Memo1->Lines->Strings[1] = "Отступ от датчика : " + IntToStr(otstup);
}

void __fastcall ShowSensorSample(TShape *Lamp, int lvl)
{
    /*
    if (BoxRead->FlModeCalibrovka)
        Lamp->Brush->Color = (lvl)?clGreen:clLime;
    else
        Lamp->Brush->Color = clWhite;
    */
}

