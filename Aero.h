#ifndef Aero_h
#define Aero_h

#include <math.h>


class Aero 
{
public:
    double density;
    double drag;
    double ReynoldsNumber; //Reynolds Number
    double Cf, Cd, Cp, Cb; //coefficient of skin friciton
    double Mach;

    void Init(double diameter, double surfaceHeight, double NCheight, double BTheight, short NCtype, short engine);

    double convertPressure(short unit, double input);
    double setDensity(double pressure, double temperature, double humidity = 0.0);
    double calculateMach(double velocity);

    double calculateCd(double velocity);
    double calculateCf(double velocity);
    double calculateCp();
    double calculateCb();

    //Convert barometric and temp data from sensors into an altitude reading
    double altitude(double temperature, double pressure);
    double updateSealevel(double newinput); //in case we want a more accurate reading

    double DynamicViscosity, KinematicViscosity; 
    double wetArea;
   
private:
    double temperature, pressure;
    double sl_pressurePa = 101325; //Stock sea level pressure in pascals
    double critRN; //we are just using a constant value for now :)
    double engineDiameter;

    bool flow = false; //true for laminar, false for turbulent: defaults to turbuient
    
};

//Macros for different pressure units
#define Millibar  0
#define mmHg      1
#define atm       2
#define PSI       3

#define C_TO_K    273.15f

#define DEG_TO_RAD  0.01745329251  
#define RAD_TO_DEG  57.2957795131

//Macros for Estes motors
#define B4   0
#define B6   1
#define C6   2
#define C11  3
#define D12  4
#define E9   5
#define E12  6





#endif