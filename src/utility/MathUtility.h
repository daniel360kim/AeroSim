#ifndef MathUtility_h
#define MathUtility_h



double clamp(double x, double min, double max)
{
    if(x < min)
    {
        return min;
    }
    if(x > max)
    {
        return max;
    }
    return x;
}




#endif