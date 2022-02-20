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


    aero.setDensity(aero.convertPressure(atm,1), 15, 0); 
    aero.Init(diameter, surfaceHeight / 1e6, NCheight, BTheight, 0, D12); //all the parameters of the rocket entered. Only conical nosecones supported rn - 0
    std::cout << std::setprecision(10) <<std::fixed; //PRECISE!! lol

    std::cout << "Drag Force: " << aero.calculateDragForce(velocity) << std::endl;
    std::cout << "Cd: " << aero.Cd << std::endl;
    std::cout << "Cf: " << aero.Cf << std::endl;
    std::cout << "Cb: " << aero.Cb << std::endl;
    std::cout << "Cp: " << aero.Cp << std::endl;
    std::cout << "Cd: " << aero.Cd << std::endl;
    std::cout << "Rocket Surface Area: " << aero.wetArea << std::endl;

    double aoa = 0;
    std::cout << "Please enter angle of attack: " << std::endl;
    std::cin >> aoa;

    std::cout << "Axial Drag: " << aero.calcuateAxialDrag(aoa) << std::endl;
  
    std::cout << "Mach: " << aero.Mach << std::endl;    
    std::cout <<"KV: " << aero.KinematicViscosity << std::endl;
    std::cout <<"Reynold's Number: " << aero.ReynoldsNumber << std::endl;

 

    
}