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
    int         defectMassInLen;
    double      lenSegment;
    //
	double      lenghHero1,   lenghHero2,   lenghCircle;
    int         segmentHero1, segmentHero2, segmentCircle;
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
    int         vSampleTriger;
    signed char summDefect;
    // wild

protected:
    void __fastcall SelectDo();
    void __fastcall Circle();
    void __fastcall Sensors();
    void __fastcall SensorsAt(int sn, int lvl);
    void __fastcall SensorsSample(int sn, int lvl);
    void __fastcall SensorsTubeHere(int sn, int lvl);
    void __fastcall SensorWild(int sn, int lvl);
public:
    signed char defectMass  [boxReadMaxLenMassive];
    int         defectMassLen;
    void __fastcall PushFromCommPort(unsigned char bt);
// ******************************************************************
//                      Sensors
    // Other Sensors
    void __fastcall (__closure *EvSensorsOther)(int sn, int lvl);
    // Sensor Wild
    void __fastcall (__closure *EvSensorWild)(int lvl);
    // Sensors At
    void __fastcall (__closure *EvSensorAtTop)(int lvl);
    void __fastcall (__closure *EvSensorAtBottom)(int lvl);
    void __fastcall (__closure *EvSensorsAtShow)(int n, int lvl);
    // Sensors Tube here
    void __fastcall   (__closure *EvSensorsTubeHereShow )(int n, int lvl);
    double __fastcall (__closure *EvSensorTubeBegin)();
    void __fastcall   (__closure *EvSensorTubeBeginRecord)();
    void __fastcall   (__closure *EvSensorTubeLen)(int segments, double lenght);
    void __fastcall   (__closure *EvSensorTubeEnd)(int len, signed char *massDefect, int flagSample);
    void __fastcall   (__closure *EvSensorTubeReset)();
    // Sensor Sample
    void __fastcall (__closure *EvSample)(int trg, int lvl);
    __property int sampleTriger={read=vSampleTriger};
    // Sensor Circle
    void __fastcall   (__closure *EvCircleShow)(int sn);
    void __fastcall   (__closure *EvCircleForward)(signed char *massDefect, int curPosition);
    void __fastcall   (__closure *EvCircleBack)(signed char *massDefect, int curPosition);
    void __fastcall   (__closure *EvCircleBackBad)();
};

#endif
