/*
Author: Daniel Kim - NM Rocketry

All the functions to calculate aerodynamic properties of the model rocket.
These equations include both Barrowman's derivations along with solutions from the OpenRocket software

Certain aspects (such as motor mount diameter) do not serve much of a purpose, but will be integrated later

Coefficient of axial drag and pressure drag depend on the Polynomial Interpolation class, which needs refining, so the values can be ~+/- 0.003 off... Will fix it!


*/




#include "Aero.h"
#include "../Properties/Rocket.h"
#include "../utility/functions.h"
#include "../utility/MathUtility.h"


#include <iostream>
#include <vector>


RocketProperties properties;

void Aero::Init(double diameter, double surfaceHeight, double NCheight, double BTheight, short NCtype, short engine)
{
    properties.setRocketProperties(diameter, surfaceHeight); //diameter and surfaceheight in meters
    properties.setNoseCone(NCtype, NCheight);
    properties.setBodyTube(BTheight);
    properties.calculateLength();

    wetArea = properties.calculateSurfaceArea();

    //this bottom section is irrelevant for now: will use later for base drag coefficient calculations...
    if(engine == B4 || engine == B6 || engine == C6)
    {
        engineDiameter = 0.018; //18mm motors
    }

    else if(engine == C11 || engine == D12 || engine == E9 || engine == E12)
    {
        engineDiameter = 0.024; //24mm motors
    }

}

double Aero::setDensity(double pressure, double temperature, double humidity) //Pascal, Celcius, %  
{   
     this->temperature = temperature;
     this->pressure = pressure;

     double vaporPressure = pow(6.1078 * 10, 7.5 * temperature / (temperature + 257.3)) * humidity;

     double p_dry = pressure - vaporPressure;
     
     density = (p_dry / (287.058 * (temperature + C_TO_K))) + (vaporPressure / (461.495 * (temperature + C_TO_K)));
     return density;
}

double Aero::altitude(double temperature, double pressure) //calculate altitude from temp and pressure (Celsius, Pa)
{   
    double farenheit = (temperature * (9 / 5)) + 32;
    return ((double)pow((sl_pressurePa / 100) / (pressure / 100), 0.190223f) - 1.0f) * (temperature + C_TO_K) / 0.0065f; // Calculate the altitude in meters 
}

double Aero::updateSealevel(double newinput) //Pascals
{
    sl_pressurePa = newinput;
    return sl_pressurePa;
}

double Aero::convertPressure(short unit, double input) //Simple function to convert units
{
    double output;

    switch(unit)
    {
        case Millibar:
            output = input * 100;
            break;
        case mmHg:
            output = input * 133;
            break;
        case atm:
            output = input * 101325;
            break;
        case PSI:
            output = input * 6895;
            break;
        default:
            output = 101325; //defaults to sealevel pressure 
    }

    return output;
}

double Aero::calculateMach(double velocity)
{
    Mach = velocity / (sqrt(1.4f * 8.314 * (temperature + C_TO_K) / 0.0289645));
    MachSquared = Mach * Mach; //Mach^2 is used a lot in the Coefficient calculations so it is a good idea to assign to a var
    return Mach;
}

double Aero::calculateCf(double velocity) //this calculates coefficient of skin friction drag
{
     DynamicViscosity = 3.7291e-6 + (4.9944e-8 * (temperature + C_TO_K));
     KinematicViscosity = DynamicViscosity / density; 

     ReynoldsNumber = velocity * properties.length / KinematicViscosity;

     double c1 = 1.0, c2 = 1.0; //compressibility correction factors

     /*This is all assuming that we are in turbulent flow
       Since the laminar to turbulent region cannot be accurately defined, and the Reynold's number has many variables, we just assume that flow is turbulent.
     */
     if(ReynoldsNumber < 1e4)
     {
         Cf = 1.48e-2;
     }
     else
     {
         Cf = 1.0 / pow(1.50 * log(ReynoldsNumber) - 5.6, 2);
     } 

     //Applying compressibility corrections
     if(Mach < 1.1)
     {
         c1 = 1 - 0.1 * MachSquared;
     }
     if(Mach > 0.9)
     {
         c2 = 1 / pow(1 + 0.15 * MachSquared, 0.58);
     } 

     if(Mach < 0.9)
     {
         Cf *= c1; 
     }
     else if(Mach < 1.1)
     {
         Cf *= c2 * (Mach - 0.9) / 0.2 + c1 * (1.1 - Mach) / 0.2;
     }
     else
     {
         Cf *= c2;
     }
     
     //making corrections based on the surface height of the vehicle
     double roughCorrection;

     if(Mach < 0.9)
     {
         roughCorrection = 1 - 0.1 * MachSquared;
     }
     else if(Mach > 1.1)
     {
         roughCorrection = 1 / (1 + 0.18 * MachSquared);
     }
     else 
     {
         c1 = 0.919;  //1 - 0.1 * pow(0.9, 2) = 0.919....
		 c2 = 1.0 / (1 + 0.18 * 1.1 * 1.1);
		 roughCorrection = c2 * (Mach - 0.9) / 0.2 + c1 * (1.1 - Mach) / 0.2;
     }

     double bodyFriction;

     double turbCf = 0.032 * pow(properties.surfaceHeight / properties.length, 0.2) * roughCorrection;
     double componentCf = std::max(Cf, turbCf); //since we are assuming full turbulent flow, we just find the max 

     bodyFriction += componentCf * properties.surfaceArea;
     
     Cf = bodyFriction / properties.area;

     return Cf;
}

double Aero::calculateCp()
{
    if(Mach < 1)
    {
         Cp = 0.8 * sin(properties.NCjointAngle * DEG_TO_RAD) * sin(properties.NCjointAngle * DEG_TO_RAD);
    }
    else
    {
        Cp = sin(properties.NCjointAngle * DEG_TO_RAD);
    }
    
    return Cp;
}

double Aero::calculateCb() //more will be added to this when supersonic flight and exhaust plume additions are added...
{
    if(Mach < 1)
    {
        Cb = 0.12 + 0.13 * pow(Mach, 2);
    }
    else
    {
        Cb = 0.25 / Mach;
    }
    return Cb;
}

double Aero::calculateCd(double velocity)
{
    calculateMach(velocity);
    Cd = calculateCb() + calculateCp() + calculateCf(velocity);
    return Cd;
}

double Aero::calculateDragForce(double velocity)
{
    calculateCd(velocity);
    drag = Cd * (0.5 * density * velocity * velocity) * properties.area;
    return drag;
}

double Aero::calcuateAxialDrag(double aoa)
{
    std::vector<double> axialDragPoly1;
    std::vector<double> axialDragPoly2;

    std::vector<std::vector<double>> v1 
    {
        {0.0, 17.0 * PI / 180.0}, 
        {0.0, 17.0 * PI / 180.0}
    };
    PolyInterpolator interpolator1(v1);
    std::vector<double> vals1 {1.0, 1.3, 0.0, 0.0};
    axialDragPoly1 = interpolator1.interpolator(vals1);

    std::vector<std::vector<double>> v2
    {
        {17.0 * PI / 180, PI / 2.0},
        {17.0 * PI / 180, PI / 2.0},
        {PI / 2}
    };
    PolyInterpolator interpolator2(v2);
    std::vector<double> val2 {1.3, 0.0, 0.0, 0.0, 0.0};
    axialDragPoly2 = interpolator2.interpolator(val2);

    double mul = 0.0;
    aoa *= DEG_TO_RAD;
    aoa = clamp(aoa, 0.0, PI);

    if(aoa > PI / 2.0)
    {
        aoa = PI - aoa;
    }
    if(aoa < 17.0 * PI / 180)
    {
        mul = interpolator1.eval(aoa, axialDragPoly1);
    }
    else
    {
        mul = interpolator2.eval(aoa, axialDragPoly2);
    }

    if(aoa < PI / 2)
    {
        return mul * Cd;
    }
    else
    {
        return -1.0 * mul * Cd;
    }
    

}



















