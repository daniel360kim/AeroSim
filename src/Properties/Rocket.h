/*

Stores and calculates all the properties of the rocket for use by the simulation. 

*/
#ifndef Rocket_h
#define Rocket_h

#define CONIC 0 //one nosecone type for now :))

#define DEG_TO_RAD  0.01745329251  
#define RAD_TO_DEG  57.2957795131

#include <math.h>

//All units are SI
class RocketProperties
{
public:
    double surfaceArea; //also going to be the wet area since we are only simlating a straight flight
    double surfaceHeight; //for now we are assuming the body tube and the nosecone are made of the same material and therefore have the same surface height - in meters
    double length; //how schlong the rocket is
    double area; //for now the reference area is the area of the body tube/nose cone since we are only simulating a straight flight
    double volume;
    double diameter, radius; //in meters

    void setRocketProperties(double diameter, double surfaceHeight); //diameter is the same for both the nose cone and body tube

    void setNoseCone(short type, double height); //type is the type of nose cone (conical for now) and the height of the nosecone in meters
    void setBodyTube(double height);

    double calculateSurfaceArea(); //total S.A
    double calculateLength(); //total Length
    double calculateVolume();
    
    double NCjointAngle, NCfineness;
    
    double NoseCone_L, BodyTube_L; //length of the nosecone and bodytube seperately
    double NoseCone_V, BodyTube_V;
    
    double NoseCone_SA, BodyTube_SA; //surface areas of the nosecone and bodytube seperately 
private:

};


#define PI 3.1415926535897932384626433832795028841971693993751   

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
        NCfineness = NoseCone_L / radius; 
        NCjointAngle = 90.0 -RAD_TO_DEG * atan(NCfineness);
    }

    NoseCone_V = PI * radius * radius * height / 3;
}

void RocketProperties::setBodyTube(double height)
{
    BodyTube_L = height;
    BodyTube_SA = (2.0 * PI * radius * height) + (2.0 * PI * radius * radius);

    BodyTube_V = PI * radius * radius * height;
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

double RocketProperties::calculateVolume()
{
    volume = NoseCone_V + BodyTube_V;
    return volume;
}



#endif