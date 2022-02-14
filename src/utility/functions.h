#ifndef function_h
#define function_h

struct Data
{
    double x, y;
};

// function to interpolate the given data points using Lagrange's formula
// param corresponds to the new data point whose value is to be obtained
// n represents the number of known data points
double interpolate(Data f[], double param, unsigned int n) 
{
    double result;

    for (int i=0; i<n; i++)
    {
        // Compute individual terms of above formula
        double term = f[i].y;
        for (int j=0;j<n;j++)
        {
            if (j!=i)
                term = term*(param - f[j].x)/double(f[i].x - f[j].x);
        }
 
        // Add current term to result
        result += term;
    }
 
    return result;
}







#endif