/*
 * Copyright 2008-2011 Nicolas Cardiel
 *
 * This file is part of indexf.
 *
 * Indexf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Indexf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with indexf.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

bool fmean(const long n, const double *x, const bool *ifx, 
           double *fmeanPtr, double *sigmaPtr)
{
  if (n <= 0)
  {
    *fmeanPtr=*sigmaPtr=0;
    return(false);
  }

  //calculamos la media
  long neff=0; 
  double sum=0.0;
  for (long i=1; i<=n; i++)
  {
    if(ifx[i-1])
    {
      neff++;
      sum+=x[i-1];
    }
  }
  if(neff >= 1)
    *fmeanPtr=sum/static_cast<double>(neff);
  else
    return(false);

  //calculamos la desviacion tipica
  if (neff <= 1)
  {
    *sigmaPtr=0.0;
  }
  else
  {
    sum=0.0;
    for (long i=1; i<=n; i++)
    {
      if(ifx[i-1])
      {
        sum+=(x[i-1]-(*fmeanPtr))*(x[i-1]-(*fmeanPtr));
      }
    }
    *sigmaPtr=sqrt(sum/static_cast<double>(neff-1));
  }

  return(true);
}
