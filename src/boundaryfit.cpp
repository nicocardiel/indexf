/*
 * Copyright 2008-2013 Nicolas Cardiel
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
using std::cerr;
using std::cout;
using std::endl;
using std::ios;

#include <fstream>
using std::ofstream;
using std::ifstream;

#include <vector>
using std::vector;

#include <stdlib.h>

#include "genericpixel.h"

//-----------------------------------------------------------------------------
//Calcula un boundary fit, ajustando los datos en el vector vec. El resultado
//del ajuste, evaluado en los mismos pixeles, se almacena en el vector fit.
//Finalmente tambien se evalua el boundary fit en todos los pixeles contenidos
//en el vector eval. De esta forma, este vector puede usarse para calcular un
//unico valor en el centro de una banda, o todo un conjunto de valores.
//Si boundfit > 0, el ajuste es polinomico.
//Si boundfit < 0, el ajuste se realiza con splines.
bool boundaryfit(const long boundfit, 
                 vector <GenericPixel> &vec, const bool lerr,
                 vector <GenericPixel> &fit,
                 vector <GenericPixel> &eval)
{
  //---------------------------------------------------------------------------
  //protecciones
  if (boundfit == 0)
  {
    cout << "ERROR in function boundaryfit: boundfit = 0" << endl;
    return(false);
  }
  long num=vec.size();
  if (num <= 0)
  {
    cout << "ERROR in function boundaryfit: vector size = 0" << endl;
    return(false);
  }
  if (num != fit.size())
  {
    cout << "ERROR in function boundaryfit: size of vectors vec and fit "
         << "does not match" << endl;
    cout << "size(vec): " << num << endl;
    cout << "size(fit): " << fit.size() << endl;
    return(false);
  }
  
  //---------------------------------------------------------------------------
  //normalizamos la longitud de onda
  double wnorm = 0.0;
  for (long i=0; i<num; i++)
  {
    wnorm += vec[i].getwave();
  }
  wnorm /= static_cast<double>(num);

  //---------------------------------------------------------------------------
  //generamos un fichero ASCII con los datos a ajustar
  ofstream outdatafile("boundfit.dat",ios::out);
  if(!outdatafile)
  {
    cerr << "File boundfit.dat could not be opened" << endl;
    return(false);
  }
  for (long i=0; i<num; i++)
  {
    outdatafile << vec[i].getwave()/wnorm
                << "   " << vec[i].getflux() << endl;
  }
  outdatafile.close();
  //calculamos ajuste
  if (boundfit > 0)
  {
    system("./boundfit_pol.sh > boundfit_pol.log");
  }
  else
  {
    system("./boundfit_spl.sh > boundfit_spl.log");
  }
  //leemos el ajuste
  ifstream inputfitfile("boundfit.out",ios::in);
  if(!inputfitfile)
  {
    cerr << "File boundfit.out could not be opened" << endl;
    return(false);
  }
  for (long i=0; i<num; i++)
  {
    double tempwave,tempflux;
    inputfitfile >> tempwave >> tempflux;
    fit[i].setwave(tempwave*wnorm);
    fit[i].setflux(tempflux);
    fit[i].seteflux(0.0);
    fit[i].setpixelfraction(vec[i].getpixelfraction());
  }
  inputfitfile.close();
  //---------------------------------------------------------------------------
  //evaluamos el ajuste en los puntos solicitados
  if (boundfit > 0) //........................................ajuste polinomico
  {
    //leemos grado y coeficientes del ajuste polinomico
    ifstream inputcoeff("boundfit.coeff",ios::in);
    if(!inputcoeff)
    {
      cerr << "File boundfit.coeff could not be opened" << endl;
      return(false);
    }
    long polydeg;
    inputcoeff >> polydeg;
    double * polycoeff = new double [polydeg+1];
    long idum;
    for (long i=0; i<=polydeg; i++)
    {
      inputcoeff >> idum >> polycoeff[i];
    }
    inputcoeff.close();
    //evaluamos el polinomio ajustado en los pixeles solicitados
    long num_eval=eval.size();
    if(num_eval > 0)
    {
      for (long i=0; i<num_eval; i++)
      {
        double tempwave=eval[i].getwave()/wnorm;
        double tempflux=polycoeff[polydeg];
        if(polydeg > 0)
        {
          for (long k=polydeg; k>=1; k--)
          {
            tempflux=tempflux*tempwave+polycoeff[k-1];
          }
        }
        eval[i].setflux(tempflux);
        eval[i].seteflux(0.01); //ToDo: calcular errores
      }
    }
    delete [] polycoeff;
  }
  else //...............................................ajuste mediante splines
  {
    //leemos numero de knots, posicion de los knots y coeficientes de los 
    //splines
    ifstream inputcoeff("boundfit.coeff",ios::in);
    if(!inputcoeff)
    {
      cerr << "File boundfit.coeff could not be opened" << endl;
      return(false);
    }
    long nknots;
    inputcoeff >> nknots;
    double * xknot = new double [nknots];
    double * yknot = new double [nknots];
    double * s1 = new double [nknots-1];
    double * s2 = new double [nknots-1];
    double * s3 = new double [nknots-1];
    long idum;
    for (long i=0; i<nknots; i++)
    {
      inputcoeff >> idum >> xknot[i] >> yknot[i];
    }
    for (long i=0; i<nknots-1; i++)
    {
      inputcoeff >> idum >> s3[i] >> s2[i] >> s1[i];
    }
    inputcoeff.close();
    //evaluamos el spline ajustado en los pixeles solicitados
    long num_eval=eval.size();
    if(num_eval > 0)
    {
      for (long i=0; i<num_eval; i++)
      {
        double tempwave=eval[i].getwave()/wnorm;
        long i0 = 0;
        for (long i=0; i<nknots; i++)
        {
          if (tempwave >= xknot[i]) 
          {
            i0 = i;
          }
        }
        if (i0 == nknots-1)
        {
          i0 = nknots-2;
        }
        double dx = tempwave-xknot[i0];
        double tempflux=yknot[i0];
        tempflux += dx*(s1[i0]+dx*(s2[i0]+dx*s3[i0]));
        eval[i].setflux(tempflux);
        eval[i].seteflux(0.01); //ToDo: calcular errores
      }
    }
    delete [] xknot;
    delete [] yknot;
    delete [] s1;
    delete [] s2;
    delete [] s3;
  }
  //---------------------------------------------------------------------------
  return(true);
}
