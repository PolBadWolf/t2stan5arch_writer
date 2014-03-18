//---------------------------------------------------------------------------


#pragma hdrstop

#include "BoxRead.h"
#include "crc8.h"

//---------------------------------------------------------------------------
#define BoxReadMASSPACK_HEAD1   0
#define BoxReadMASSPACK_HEAD2   1
#define BoxReadMASSPACK_TYPE    2
#define BoxReadMASSPACK_SENSR   3
#define BoxReadMASSPACK_LEVEL   4
#define BoxReadMASSPACK_CRC     5
#define BoxReadMASSPACK_TAIL    6

#define CIRCLE_FORWARD          0       // колесо импульс вперед *
#define CIRCLE_BACK             1       // колесо импульс назад  *
#define KEY_BACK                0       // рольганг назад (ключ)
#define KEY_FORWARD             1       // рольганг вперед (ключ)
#define TUBE_HERE1              2       // наличие трубы д1 *
#define TUBE_HERE2              3       // наличие трубы д2 *
#define WELD_DEFECT             6       // дефект
#define MODE_SAMPLE             7       // калибровка
#define SENSOR_AT_TOP           8       // датчики дефектоскопа подняты *
#define SENSOR_AT_BOTTOM        9       // датчики дефектоскопа опущены *

#pragma package(smart_init)

__fastcall TBoxRead::TBoxRead()
{
    // clear massvive sensors
    for (int i=0; i<boxRead_MassSensorsLen; i++)
        massSensors[i] = -1;
    // sensorAt top
    vSensorAt = 1;
    // tube here
    vTubeHere1 = 1;
    vTubeHere2 = 1;
}

__fastcall TBoxRead::~TBoxRead()
{
}

void __fastcall TBoxRead::PushFromCommPort(unsigned char bt)
{
    // shift buffer
    for (int i=1;i<boxRead_MassPackLen;i++)
        massPack[i-1] = massPack[i];
    // new data
    massPack[boxRead_MassPackLen-1] = bt;
    // ***************************
    if (massPack[BoxReadMASSPACK_HEAD1]!=0xe6)      return;
    if (massPack[BoxReadMASSPACK_HEAD2]!=0x16)      return;
    if (massPack[BoxReadMASSPACK_TAIL ]!=0x00)      return;
    if (massPack[BoxReadMASSPACK_CRC  ]!=
        crc8_buf(massPack,BoxReadMASSPACK_CRC))     return;
    SelectDo();
}

void __fastcall TBoxRead::SelectDo()
{
    int typ;
    typ = massPack[BoxReadMASSPACK_TYPE];
    if (typ==0)
        Circle();
    if (typ==1)
        Sensors();
}

void __fastcall TBoxRead::Circle()
{
}

void __fastcall TBoxRead::Sensors()
{
    int sn, lvl;
    sn  = massPack[BoxReadMASSPACK_SENSR];
    lvl = massPack[BoxReadMASSPACK_LEVEL];
    if (sn>=boxRead_MassSensorsLen)
        return; // error sn
    // save lvl to massive
    massSensors[sn] = lvl;
    // tube here
    if ( (sn==TUBE_HERE1) || (sn==TUBE_HERE2) )
    {
        return;
    }
    // defect
    if ( sn==WELD_DEFECT )
    {
        return;
    }
    // sample
    if ( sn==MODE_SAMPLE )
    {
        SensorsSample(sn, lvl);
        return;
    }
    // sensor at
    if ( (sn==SENSOR_AT_TOP) || (sn==SENSOR_AT_BOTTOM) )
    {
        SensorsAt(sn, lvl);
        return;
    }
}

void __fastcall TBoxRead::SensorsAt(int sn, int lvl)
{
    // new data
    if (sn==SENSOR_AT_TOP)
        vSensorAtTop = lvl;
    if (sn==SENSOR_AT_BOTTOM)
        vSensorAtBottom = lvl;
    if ( (!vSensorAtTop) && (vSensorAtBottom) )
    {
        vSensorAt = 1;
        // call at_top
        if (EvAtTop)
            EvAtTop(vSensorAt);
    }
    if ( (vSensorAtTop) && (!vSensorAtBottom) )
    {
        vSensorAt = 0;
        // call at_bottom
        if (EvAtBottom)
            EvAtBottom(vSensorAt);
    }
    if (EvAt)
        EvAt(sn, lvl);
}

void __fastcall TBoxRead::SensorsSample(int sn, int lvl)
{
    // установка тригера
    if (EvSample)
        EvSample(1, lvl);
}

void __fastcall TBoxRead::SensorsTubeHere(int sn, int lvl)
{
    int oldHere1, oldHere2;
    // old status
    oldHere1 = vTubeHere1;
    oldHere2 = vTubeHere2;
    // new status
    if (sn==TUBE_HERE1)
        vTubeHere1 = lvl;
    if (sn==TUBE_HERE2)
        vTubeHere2 = lvl;
    // ******************
    // Begin tube
    if ( (oldHere1) && (!vTubeHere1) && (oldHere2) && (vTubeHere2) )
    {
    }
    // Begin record
    if ( (!oldHere1) && (!vTubeHere1) && (oldHere2) && (!vTubeHere2) )
    {
    }
}

