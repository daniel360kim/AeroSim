/*

Stores and calculates all the properties of the rocket for use by the simulation. 

*/
#ifndef Rocket_h
#define Rocket_h

#define CONIC 0 //one nosecone type for now :))

#include <math.h>

//All units are SI
class RocketProperties
{
public:
    double surfaceArea; //also going to be the wet area since we are only simlating a straight flight
    double surfaceHeight; //for now we are assuming the body tube and the nosecone are made of the same material and therefore have the same surface height
    double length; //how schlong the rocket is
    double area; //for now the reference area is the area of the body tube/nose cone since we are only simulating a straight flight

    double diameter, radius; //in meters

    void setRocketProperties(double diameter, double surfaceHeight); //diameter is the same for both the nose cone and body tube

    void setNoseCone(short type, double height); //type is the type of nose cone (conical for now) and the height of the nosecone in meters
    void setBodyTube(double height);

    double calculateSurfaceArea(); //total S.A
    double calculateLength(); //total Length

    double NoseCone_SA, BodyTube_SA; //surface areas of the nosecone and bodytube seperately 
    double NCjointAngle;
    

private:
    double NoseCone_L, BodyTube_L; //length of the nosecone and bodytube seperately
};


#define PI 3.1415926535897    

void RocketProperties::setRocketProperties(double diameter, double surfaceHeight)
{
    this->diameter = diameter;
    radius = diameter / 2;

    this->surfaceHeight = surfaceHeight;

    area = PI * radius * radius;
}

void RocketProperties::setNoseCone(short type, double height)
{
    NoseCone_L = height;
    if(type == CONIC)
    {
        NoseCone_SA = PI * radius * (radius + sqrt((height * height) + (radius * radius)));
        NCjointAngle = 90.0 - (atan((height / radius)) * RAD_TO_DEG); 
    }
}

void RocketProperties::setBodyTube(double height)
{
    BodyTube_L = height;
    BodyTube_SA = (2.0 * PI * radius * height) + (2.0 * PI * radius * radius);
}

double RocketProperties::calculateSurfaceArea()
{
    surfaceArea = NoseCone_SA + BodyTube_SA;
    return surfaceArea;
}

double RocketProperties::calculateLength()
{
    length = NoseCone_L + BodyTube_L;
    return length;
}

#endif