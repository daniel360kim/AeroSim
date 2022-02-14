#include "Aero.h"
#include "../Properties/Rocket.h"
#include "../utility/functions.h"

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



double Aero::calculateCp() //Very simple :)//still a work in progress
{   
    double sinNC = sin(properties.NCjointAngle * DEG_TO_RAD);

    double Cp_trans = 0.8 * sinNC * sinNC;
    double Cp_super = 1.0 * sinNC;
    //double Cp_super = (2.1 * sinNC * sinNC) + (0.5 * (sinNC / sqrt(1.69026001 - 1.0)));

    Data Cp_values[] = {{0, Cp_trans}, {1.0, Cp_super}};

    Cp = interpolate(Cp_values, Mach, 2);

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
    Cd = calculateCf(velocity) + calculateCp() + calculateCb();
    return Cd;
}

double Aero::calculateCpm(double aoa)
{
    cpmnc = ((2.0 * sin(aoa * DEG_TO_RAD)) / (properties.NoseCone_SA * properties.diameter)) * (properties.NoseCone_L * properties.NoseCone_SA - properties.volume);
    cpmbt = ((2.0 * sin(aoa * DEG_TO_RAD)) / (properties.BodyTube_SA * properties.diameter)) * (properties.BodyTube_L * properties.BodyTube_SA - properties.volume); 

    Cpm = cpmnc - cpmbt; 
    return Cpm;
}

double Aero::calculateCn(double aoa)
{
    cnnc = ((2.0 * sin(aoa * DEG_TO_RAD)) / properties.NoseCone_SA) * properties.area;
    cnbt = ((2.0 * sin(aoa * DEG_TO_RAD)) / properties.BodyTube_SA) * properties.area;
    Cn = cnnc + cnbt;
    return Cn;
}