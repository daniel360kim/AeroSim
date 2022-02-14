#include "src/aero/Aero.h"
#include <math.h>
#include <iostream>
#include <iomanip>

Aero aero;

int main(int argc, char const *argv[])
{
    std::cout << "AeroSimulation 1.0: Will calculate Coefficients of drag at a certain velocity and at no angle of attack" << std::endl;
    
    double velocity, diameter, surfaceHeight, NCheight, BTheight;

    std::cout << "Please enter velocity (m/s)" << std::endl;
    std:: cin >> velocity;
    std::cout << "Please enter diameter (meters)" << std::endl;
    std::cin >> diameter;
    std::cout << "Please enter nosecone height (meters)" << std::endl;
    std::cin >> NCheight;
    std::cout << "Please enter body tube height (meters)" << std::endl;
    std::cin >> BTheight;
    std::cout << "Please enter surface height (micrometers) - OpenRocket default is 60" << std::endl;
    std::cin >> surfaceHeight;

    aero.setDensity(aero.convertPressure(atm,1), 15, 0); //imma just assume ur at sea level at 15c. 1atm will produce most accurate results .... for now.... :)
    aero.Init(diameter, surfaceHeight / 1e6, NCheight, BTheight, 0, D12); //all the parameters of the rocket entered. Only conical nosecones supported rn - 0
    std::cout << std::setprecision(10) <<std::fixed; //PRECISE!! lol

    std::cout << "Cd: " << aero.calculateCd(velocity) << std::endl;
    std::cout << "Cf: " << aero.Cf << std::endl;
    std::cout << "Cb: " << aero.Cb << std::endl;
    std::cout << "Cp: " << aero.Cp << std::endl;
    std::cout << "Rocket Surface Area: " << aero.wetArea << std::endl;
  
    std::cout << "Mach: " << aero.Mach << std::endl;    
    std::cout <<"KV: " << aero.KinematicViscosity << std::endl;
    std::cout <<"Reynold's Number: " << aero.ReynoldsNumber << std::endl;
 
 
/*

   double aoa;
   std::cout << "Please enter angle of attack: " << std::endl;
   std::cin >> aoa;
   aero.setDensity(aero.convertPressure(atm,1), 15, 0); //imma just assume ur at sea level at 15c. 1atm will produce most accurate results .... for now.... :)
   aero.Init(0.05, 6e-5, 0.15, 0.6, 0, D12);

   std::cout << std::setprecision(30) <<std::fixed; //PRECISE!! lol
   std::cout << "Cpm: " << aero.calculateCpm(aoa) << std::endl;
   std::cout << "Cn: " << aero.calculateCn(aoa) << std::endl;
 
   */
    
}