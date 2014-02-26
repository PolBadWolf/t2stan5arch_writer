//---------------------------------------------------------------------------
// User Functions

#include <vcl.h>
#pragma hdrstop

#include "UserFunction1.h"
#include <math.h>
#include "Unit1.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

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
        *CodeMelt = "";
        *SizeTube = 219;
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
    dQuery->SQL->Clear();
    dQuery->SQL->Add("SELECT");
    dQuery->SQL->Add(" `parameters`.`Id_Melt` AS `id_melt`");
    dQuery->SQL->Add(",`melts`.`NameMelt` AS `CodeMelt`");
    dQuery->SQL->Add(",`sizetube`.`SizeTube` AS `SizeTube`");
    dQuery->SQL->Add("FROM");
    dQuery->SQL->Add("`parameters`");
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
        *CodeMelt = "";
        *SizeTube = 219;
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

void __fastcall Show_NumberTube(int number)
{
    if (number)
        Form1->Label_temp->Caption = number;
    else
        Form1->Label_temp->Caption = "no tube";
}
