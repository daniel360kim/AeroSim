
#include "Rocket.h"
#include <math.h>

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