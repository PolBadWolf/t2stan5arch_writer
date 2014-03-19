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
	TBoxRead(720, 340, 800);
}

__fastcall TBoxRead::TBoxRead(int hero1, int hero2, int circle)
{
    lenghHero1  = hero1;
	lenghHero2  = hero2;
	lenghCircle = circle;
	// clear massvive sensors
    for (int i=0; i<boxRead_MassSensorsLen; i++)
        massSensors[i] = -1;
    // sensorAt top
    vSensorAt = 1;
    // tube here
    vTubeHere1 = 1;
    vTubeHere2 = 1;
    // circle
    count = 0;
    circleOff = 1;
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
    int sn;
    sn = massPack[BoxReadMASSPACK_SENSR];
    if (EvCircleShow)
        EvCircleShow(sn);
    if (circleOff)
        return;
    if (sn==CIRCLE_FORWARD)
    {
        signed char def;
        // read & reset defect
        if (vSensorAt)
            def = -1;
        else
            def = summDefect;
        summDefect = 0;
        // mode
        if (defectMassInCircleBuff)
        { // cirle buff mode
            //safe
            if ( (segmentHero2<0) || (segmentHero2>=boxReadMaxLenMassive) )
                return;
            // shift buffer
            for (int i=0; i<segmentHero2; i++)
                defectMassIn[i] = defectMassIn[i+1];
            // write defect
                defectMassIn[segmentHero2] = def;
            // show
            if (EvCircleForward)
                EvCircleForward();
        }
        else
        { // normal mode
            // write
            if ( (count>=0) && (count<boxReadMaxLenMassive) )
                defectMassIn[count] = def;
            // show
            if (EvCircleForward)
                EvCircleForward();
            // count +1 safe
            if ( count<(boxReadMaxLenMassive-1) )
                count++;
        }
        return;
    }
    if (sn==CIRCLE_BACK)
    {
        // bad zone in begin way
        if (defectMassInCircleBuff)
            return;
        if (count<=0)
            return;
        count--;
        if (EvCircleBack)
            EvCircleBack();
        return;
    }
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
        SensorWild(sn, lvl);
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

void __fastcall TBoxRead::SensorWild(int sn, int lvl)
{
    if (vSensorAt)
        summDefect = -1;
    else
        summDefect = summDefect | (!lvl);
    if (EvWild)
        EvWild(lvl);
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
    //      tube run forever
    // Begin tube
    if ( (oldHere1) && (!vTubeHere1) && (oldHere2) && (vTubeHere2) )
    {
	    double lenSegment;
        // work circle on
        circleOff = 0;
        // clear count
        count = 0;
        // massive mode circle buff
        defectMassInCircleBuff = 1;
        // clear massive
        for (int i=0; i<boxReadMaxLenMassive; i++)
            defectMassIn[i] = -1;
        // max length tube
        defectMassInLen = 0;
        // new tube
        newTube = 1;
        // read len segment tube
		if (EvTubeBegin)
			lenSegment = EvTubeBegin();
		else
			lenSegment = 100;
        // calculate base lenght
		segmentHero1  = lenghHero1  / lenSegment;
		segmentHero2  = lenghHero2  / lenSegment;
		segmentCircle = lenghCircle / lenSegment;
        return;
    }
    // Begin record
    if ( (!oldHere1) && (!vTubeHere1) && (oldHere2) && (!vTubeHere2) )
    {
        count = segmentHero2;
        circleOff = 0;
    }
    // Begin count end
    if ( (!oldHere1) && (vTubeHere1) && (!oldHere2) && (!vTubeHere2) )
    {
    }
    // End tube
    if ( (oldHere1) && (vTubeHere1) && (!oldHere2) && (vTubeHere2) )
    {
        circleOff = 1;
        // call
        newTube   = 0;
        return;
    }
    //      tube run come back
    // Bad zone D1 d2
    if ( (!oldHere1) && (!vTubeHere1) && (!oldHere2) && (vTubeHere2) )
    {
        circleOff = 1;
        newTube   = 0;
        return;
    }
    // **************************************
    // Reset tube
    if ( (oldHere1) && (vTubeHere1) )
    {
        circleOff = 1;
        newTube   = 0;
        return;
    }
}

