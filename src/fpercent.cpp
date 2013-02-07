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
#include <cmath>
#include <vector>
#include <algorithm>
#include "genericpixel.h"

using namespace std;
 
//Funcion auxiliar para ayudar a ordenadar el vector de entrada usando como
//elemento a ser ordenado el flujo
bool sortGenericPixel (GenericPixel pix1, GenericPixel pix2)
{
  return (pix1.getflux() < pix2.getflux());
}

//-----------------------------------------------------------------------------
//Calcula un percentil pesado del flujo (los pesos sirven para poder manejar
//las fracciones de pixel en los bordes de las bandas: pixels completos tienen
//un peso de 1 y pixels fraccionados tienen como peso la fraccion
//correspondiente). La incertidumbre se calcula mediante simulaciones (solo en
//el caso en el que lerr=true).
bool fpercent(vector <GenericPixel> &vec, const long percent, const bool lerr,
              double *fpercentPtr, double *e2fpercentPtr)
{
  //---------------------------------------------------------------------------
  //valores retornados en caso de error o imposibilidad de calculo
  *fpercentPtr=0;
  *e2fpercentPtr=0;

  //---------------------------------------------------------------------------
  //protecciones
  if ( (percent < 0) || (percent > 100) )
  {
    cout << "ERROR in function fpercent: percent = " << percent << endl;
    return(false);
  }

  long num=vec.size();
  if (num <= 0)
  {
    cout << "ERROR in function fpercent: vector size = 0" << endl;
    return(false);
  }

  double float_num=static_cast<double>(num);

  //---------------------------------------------------------------------------
  //para estimar la incertidumbre realizamos nsimulmax simulaciones; 
  vector <double> fpercentSimul; //aqui almacenamos resultados de simulaciones
  const long nsimulmax=100;
  const double pi2 = 4.*acos(0.0);
  const double sqrt2 = sqrt(2.0);
  const double fRAND_MAX = static_cast<double>(RAND_MAX);
  for (long isimul=0; isimul <= nsimulmax; isimul++)
  {
    //generamos un vector flujo aleatorizado (si isimul=0 se toman los datos
    //originales sin aleatorizar)
    vector <GenericPixel> simulatedFlux=vec;
    if (isimul > 0)
    {
      for (long i=0; i<num; i++)
      {
        long iran; //evitamos obtener ran1=1 y ran2=1
        while ( (iran=rand()) == RAND_MAX);
        const double ran1 = static_cast<double>(iran)/fRAND_MAX;
        while ( (iran=rand()) == RAND_MAX);
        const double ran2 = static_cast<double>(iran)/fRAND_MAX;
        double tempflux = vec[i].getflux();
        tempflux+=sqrt2*vec[i].geteflux()*sqrt(-1*log(1-ran1))*cos(pi2*ran2);
        simulatedFlux[i].setflux(tempflux);
      }
    }
    //procedemos al calculo del percentil
    if (num == 1)
    {
      fpercentSimul.push_back(simulatedFlux[0].getflux());
    }
    else
    {
      //ordenamos por flujo el vector simulatedFlux usando la funcion auxiliar 
      //definida mas arriba
      sort(simulatedFlux.begin(),simulatedFlux.end(),sortGenericPixel);
      //generamos un vector auxiliar con los valores del flujo ordenados
      vector <double> fluxsorted;
      for (long i=0; i<num; i++)
      {
        fluxsorted.push_back(simulatedFlux[i].getflux());
      }
      //calculamos las sumas parciales de los pesos
      vector <double> Sn;
      for (long i=0; i<num; i++)
      {
        if(i == 0)
        {
          Sn.push_back(simulatedFlux[i].getpixelfraction());
        }
        else
        {
          Sn.push_back(Sn[i-1]+simulatedFlux[i].getpixelfraction());
        }
      }
      //calculamos percentiles correspondientes a cada elemento de fluxsorted
      vector <double> pn;
      for (long i=0; i<num; i++)
      {
        pn.push_back(100./Sn[num-1]*
                     (Sn[i]-simulatedFlux[i].getpixelfraction()/2.));
      }
      //interpolamos el valor del percentil solicitado
      double float_percent=static_cast<double>(percent);
      if(float_percent <= pn[0]) //extremo izquierdo
      {
        fpercentSimul.push_back(fluxsorted[0]);
      }
      else if(float_percent >= pn[num-1]) //extremo derecho
      {
        fpercentSimul.push_back(fluxsorted[num-1]);
      }
      else //buscamos entre que dos valores se encuentra, e interpolamos
      {
        long k=0;
        while (float_percent > pn[k])
        {
          k++;
        }
        fpercentSimul.push_back(fluxsorted[k-1]+
                                (float_percent-pn[k-1])/(pn[k]-pn[k-1])*
                                (fluxsorted[k]-fluxsorted[k-1]));
      }
    }
    //si no hay errores, no hacemos las simulaciones
    *fpercentPtr=fpercentSimul[0];
    if (!lerr)
    {
      *e2fpercentPtr=0;
      return(true);
    }
    /*cout << "nsimul: " << isimul << " " 
         << fpercentSimul.size() << " "
         << fpercentSimul[isimul] << endl;*/
  }
  //calculamos media y r.m.s. de los valores simulados
  double meanSimul=0.0;
  for (long isimul=1; isimul <= nsimulmax; isimul++)
  {
    meanSimul+=fpercentSimul[isimul];
  }
  meanSimul/=static_cast<double>(nsimulmax);
  double sigmaSimul=0.0;
  for (long isimul=1; isimul <= nsimulmax; isimul++)
  {
    sigmaSimul+=(fpercentSimul[isimul]-meanSimul)*
                (fpercentSimul[isimul]-meanSimul);
  }
  sigmaSimul=sqrt(sigmaSimul/static_cast<double>(nsimulmax-1));
  *e2fpercentPtr=sigmaSimul*sigmaSimul;

  /*cout << "mean, sigma: " << meanSimul << " " << sigmaSimul << endl;*/
  return(true);
}
