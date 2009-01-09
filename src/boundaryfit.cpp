/*
 * Copyright 2008 Nicolas Cardiel
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
bool boundaryfit(vector <GenericPixel> &vec, const bool lerr,
                 vector <GenericPixel> &fit,
                 vector <GenericPixel> &eval)
{
  const double magicfactor = 1000.0;
  //---------------------------------------------------------------------------
  //protecciones
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
  //generamos un fichero ASCII con los datos a ajustar
  ofstream outdatafile("boundfit.dat",ios::out);
  if(!outdatafile)
  {
    cerr << "File boundfit.dat could not be opened" << endl;
    return(false);
  }
  for (long i=0; i<num; i++)
  {
    outdatafile << vec[i].getwave()/magicfactor 
                << "   " << vec[i].getflux() << endl;
  }
  outdatafile.close();
  //calculamos ajuste
  system("source boundfit_pol.tcsh > boundfit_pol.log");
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
    fit[i].setwave(tempwave*magicfactor);
    fit[i].setflux(tempflux);
    fit[i].seteflux(0.0);
    fit[i].setpixelfraction(vec[i].getpixelfraction());
  }
  inputfitfile.close();
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
      double tempwave=eval[i].getwave()/magicfactor;
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
  return(true);
}
