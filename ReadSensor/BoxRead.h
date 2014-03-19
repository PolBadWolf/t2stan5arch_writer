//---------------------------------------------------------------------------

#ifndef BoxReadH
#define BoxReadH
//---------------------------------------------------------------------------

#define boxReadMaxLenMassive 256
#define boxRead_MassPackLen  7
#define boxRead_MassSensorsLen  14

class TBoxRead
{
public:
    __fastcall TBoxRead();
    __fastcall TBoxRead(int hero1, int hero2, int circle);
    __fastcall ~TBoxRead();
private:
    signed char defectMassIn[boxReadMaxLenMassive];
    //int         defectMassInPosition;
    int         defectMassInLen;
    //
	int         lenghHero1,  segmentHero1;
	int         lenghHero2,  segmentHero2;
	int         lenghCircle, segmentCircle;
    // massive read pack
    unsigned char massPack[boxRead_MassPackLen];
    unsigned char massSensors[boxRead_MassSensorsLen];
    int         vSensorAt;
    int         vSensorAtTop;
    int         vSensorAtBottom;
    unsigned char vTubeHere1;
    unsigned char vTubeHere2;
    // circle
    int         count;
    int         circleOff;
    int         defectMassInCircleBuff;
    int         newTube;
protected:
    void __fastcall SelectDo();
    void __fastcall Circle();
    void __fastcall Sensors();
    void __fastcall SensorsAt(int sn, int lvl);
    void __fastcall SensorsSample(int sn, int lvl);
    void __fastcall SensorsTubeHere(int sn, int lvl);
public:
    signed char defectMass  [boxReadMaxLenMassive];
    void __fastcall PushFromCommPort(unsigned char bt);
public:
    void __fastcall (__closure *EvAt)(int sn, int lvl);
    void __fastcall (__closure *EvAtTop)(int lvl);
    void __fastcall (__closure *EvAtBottom)(int lvl);
public:
    void __fastcall (__closure *EvSample)(int trg, int lvl);
public:
    void __fastcall   (__closure *EvTubeHere )(int sn, int lvl);
    double __fastcall (__closure *EvTubeBegin)();
public:
    void __fastcall   (__closure *EvCircleShow)(int sn);
    void __fastcall   (__closure *EvCircleForward)();
    void __fastcall   (__closure *EvCircleBack)();
};

#endif
