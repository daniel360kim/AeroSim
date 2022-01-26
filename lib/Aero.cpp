/* Init Release 1.0
>>>>>>>>>>>>>>>>> PLEASE READ! <<<<<<<<<<<<<<<<<<<<

Only calculates Coefficient of drag, reynolds number, density, and altitude at zero angle of attack at one point in time
Cf calculations should be correct (cross reffed with OpenRocket) but please lmk if the calculations are off
This software is put together really sloppily (functions vars literally everything) so please be nice. this will be cleaned up later

As of now this sim assumes the nosecone and bodytube are the same diameter and made of the same material
When it asks for body tube and nose cone height, the values dont matter as long as it adds up to the total rocket height

*/

#include "Aero.h"
#include "Rocket.h"

RocketProperties properties;
void Aero::Init(double diameter, double surfaceHeight, double NCheight, double BTheight, short NCtype, short engine)
{
    properties.setRocketProperties(diameter, surfaceHeight); //diameter and surfaceheight in meters
    properties.setNoseCone(NCtype, NCheight);
    properties.setBodyTube(BTheight);
    properties.calculateLength();

    wetArea = properties.calculateSurfaceArea();

    //this bottom section is irrelevant for now: will use later....
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
     double vaporPressure = pow(6.1078f * 10, 7.5f * temperature / (temperature + 257.3f)) * humidity;
     double p_dry = pressure - vaporPressure;
     density = (p_dry / (287.058f * (temperature + C_TO_K))) + (vaporPressure / (461.495f * (temperature + C_TO_K)));
     return density;
}

double Aero::altitude(double temperature, double pressure) //calculate altitude from temp and pressure (Celsius, Pa)
{   
    double farenheit = (temperature * (9 / 5)) + 32;
    return ((double)powf((sl_pressurePa / 100) / (pressure / 100), 0.190223f) - 1.0f) * (temperature + C_TO_K) / 0.0065f; // Calculate the altitude in meters 
}

double Aero::updateSealevel(double newinput) //Pascals
{
    sl_pressurePa = newinput;
    return sl_pressurePa;
}

double Aero::convertPressure(short unit, double input) //Simple function to convert units
{
    double output;
    if(unit == Millibar)
    {
        output = input * 100;
    }

    if(unit == mmHg)
    {
        output = input * 133;
    }

    if(unit == atm)
    {
        output = input * 101325;
    }

    return output;
}

double Aero::calculateMach(double velocity)
{
    Mach = velocity / (sqrt(1.4f * 8.314 * (temperature + C_TO_K) / 0.0289645));
    return Mach;
}

double Aero::calculateCf(double velocity) //this calculates cf: this function is a MESS rn. plz allow time for some cleaning....
{

     DynamicViscosity = (0.004774183564 * temperature + 1.729f) / 100000; //this is a line of best fit for experimental d.viscosity values - will implement Sutherland's law soon :)
     KinematicViscosity = DynamicViscosity / density; 

     critRN = 51.0 * pow((properties.surfaceHeight / properties.length), -1.039);
     ReynoldsNumber = properties.length * velocity / KinematicViscosity;

     
     if(ReynoldsNumber < 1e5) //Low RN need to be filtered out (velocites less than ~1m/s)
     {
         Cf = 1.48e-2;
     }
     else if(ReynoldsNumber < critRN && ReynoldsNumber > 1e5) //Cf while the vehicle is in laminar flow
     {
         Cf = 1 / pow((1.50 * log(ReynoldsNumber) - 5.6), 2);
     }
     else if(ReynoldsNumber > critRN) //Cf while the vehicle is in turbulent flow
     {
         Cf = 0.032 * pow((properties.surfaceHeight / properties.length), 0.2);
        
     }

     Cf = Cf * (1 - (0.1 * pow(Mach, 2))); //acount for compressibility
    
     Cf = (Cf * (((1 + (1 / (2 * (properties.length / properties.diameter)))) * wetArea) / properties.area)); 
     return Cf;
     
}

 double Aero::calculateCp() //Very simple :)
{   
    
    if(properties.length < properties.radius) {
       Cp = 0.2; //account for very blunt nosecones which the equation does not account for
    }
    Cp = 0.8 * pow(sin(properties.NCjointAngle * DEG_TO_RAD), 2); //since this sim only accounts for 0 angle of attack, the Cpd Nosecone will be the total Cpd

    
    
    return Cp;
}

double Aero::calculateCb() //more will be added to this when supersonic flight and exhaust plume additions are added...
{
    Cb = 0.12 + 0.13 * pow(Mach, 2);
    return Cb;
}

double Aero::calculateCd(double velocity)
{
    Cd = calculateMach(velocity) + calculateCf(velocity) + calculateCp() + calculateCb();
    return Cd;
}

