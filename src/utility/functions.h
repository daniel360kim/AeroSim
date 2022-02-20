#ifndef function_h
#define function_h

#include <iostream>
#include <vector>
#include <assert.h>

class PolyInterpolator
{

public:
	std::vector<std::vector <double>> interpolationMatrix;
    PolyInterpolator(std::vector<std::vector<double>> points)
    {
		int myCount = 0;
		for(int i = 0; i < points.size(); i++)
		{
			myCount += points[i].size();
		}
		if(myCount == 0)
		{
			std::cout << "ERROR myCount = 0" << std::endl;
		}
		this->count = myCount;

		std::vector <int> mul(myCount, 1); //making a vector with the number of constraints all filled with 1

		std::vector<std::vector<double>> matrix(myCount, std::vector<double> (myCount, 0));
		unsigned int row = 0;
		for(int j = 0; j < points.size(); j++)
		{
			for(int i = 0; i < points[j].size(); i++)
			{
				double x = 1.0;
				for(int col = myCount - 1 - j; col >= 0; col--)
				{
					matrix.at(row).at(col) = x * mul.at(col);
					x *= points.at(j).at(i);
				}
				row++;
			}

			for(int i = 0; i < myCount; i++)
			{
				mul.at(i) *= (myCount - i - j - 1);
			}
		}

		assert(row == myCount);


		interpolationMatrix = inverse(matrix);
		
    }

	std::vector<double> interpolator(std::vector<double> values)
	{
		if(values.size() != count)
		{
			std::cout << "ERROR! values don't match count" << std::endl;
		}

		std::vector<double> ret(count, 0.0);

		for(int j = 0; j < count; j++)
		{
			for(int i = 0; i < count; i++)
			{
				ret.at(j) += interpolationMatrix.at(j).at(i) * values.at(i);
			}
		}
		return ret;
	}

	static double eval(double x, std::vector<double> coefficients)
	{
		double v = 1.0;
		double result = 0;
		for(int i = coefficients.size() - 1; i >= 0; i--)
		{
			result += coefficients[i] * v;
			v *= x;
		}
		return result;
	}

	double interpolate(double x, std::vector<double> values)
	{
		return eval(x, interpolator(values));
	}




private:

    unsigned int numberPoints;
	
	int count;

	static void gaussian(std::vector<std::vector<double>> a, std::vector<int> &index)
	{
		int n = index.size();
		std::vector<double> c(n, 0.0);
		//Init the index
		for(int i = 0; i < n; i++) 
		{	
			index.at(i) = i;
		}

		//Finding the rescaling factors, one from each row
		for(int i = 0; i < n; i++)
		{
			double c1 = 0.0;
			for(int j = 0; j < n; j++)
			{
				double c0 = abs(a.at(i).at(j));
				if(c0 > c1)
				{
					c1 = c0;
				}
			}
			c.at(i) = c1;
		}

		//Searching for the pivoting element from each column
		int k = 0;
		for(int j = 0; j < n - 1; j++)
		{
			double pi1 = 0.0;
			for(int i = j; i < n; i++)
			{
				double pi0 = abs(a.at(index.at(i)).at(j));
				pi0 /= c.at(index.at(i));
				if(pi0 > pi1)
				{
					pi1 = pi0;
					k = i;
				}
			}

			//Interchanging rows according to the pivoting order
			int itmp = index.at(j);
			index.at(j) = index.at(k);
			index.at(k) = itmp;

			for(int i = j + 1; i < n; i++)
			{
				double pj = a.at(index.at(i)).at(j) / a.at(index.at(j)).at(j);

				//Recording pivoting ratios below the diagonal
				a.at(index.at(i)).at(j) = pj;

				//Modiying other elements accordingly
				for(int l = j + 1; l < n; l++)
				{
					a.at(index.at(i)).at(l) -= pj * a.at(index.at(j)).at(l);
				}
			}
		}
		
	}

	static std::vector<std::vector<double>> inverse(std::vector<std::vector <double>> matrix)
	{
		int n = matrix.size();

		std::vector<std::vector<double>> x(n, std::vector<double> (n, 0));
		std::vector<std::vector<double>> b(n, std::vector<double> (n, 0));

		std::vector<int> index(n, 0);
		for (int i = 0; i < n; i++)
		{
			b.at(i).at(i) = 1;
		}

		gaussian(matrix, index); //Transforming the matrix into an upper triangle

		//Updating vector b at i,j with the ratios stored
		for (int i = 0; i < n - 1; i++)
		{
			for (int j = i + 1; j < n; j++)
			{
				for (int k = 0; k < n; k++)
				{
					b.at(index.at(j)).at(k) -= matrix.at(index.at(j)).at(i) * b.at(index.at(i)).at(k);
				}
				
			}
			
		}
		//Performing Backwards Substitutions
		for(int i = 0; i < n; i++)
		{
			x.at(n-1).at(i) = b.at(index.at(n-1)).at(i) / matrix.at(index.at(n-1)).at(n-1);
			for(int j = n - 2; j >= 0; j--)
			{
				x.at(j).at(i) = b.at(index.at(j)).at(i);
				for (int k = j + 1; k < n; k++)
				{
					x.at(j).at(i) -= matrix.at(index.at(j)).at(k) * x.at(k).at(i);
				}
				x.at(j).at(i) /= matrix.at(index.at(j)).at(j);
				
			}
		}
		return x;
		
	}

};    

#endif