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

using namespace std;

//Funcion auxiliar para transformar una longitud de onda de aire a vacio. Las
//unidades de entrada (y salida) son Angstroms.
double ftovacuum(const long vacuum, const double wair)
{
  double wvac; // valor a retornar (en vacio)
  if (vacuum == 1)
  {
    //Greisen et al. 2006 (A&A, 446, 747; see Eq. 65, "Representation of 
    //spectral coordinates in FITS)
    double n,wmic;
    wmic = wair/10000.0; //pasamos de Angstroms a micras
    n=287.6155+1.62887/(wmic*wmic)+0.01360/(wmic*wmic*wmic*wmic);
    wvac=wair*(1.0+n*1.E-6);
  }
  else if (vacuum == 2)
  {
    //Utilizamos la expresion de Morton (1991, ApJS, 77, 119; Eq. 3)
    double sigma=10000.0/wair;
    double denom1=146.0-sigma*sigma;
    double denom2=41.0-sigma*sigma;
    double n=6.4328E-5+2.94981E-2/denom1+2.5540E-4/denom2;
    wvac=wair*(1.0+n);
  }
  else
  {
    //Calibracion citada por el SDSS en su web.
    //Invertimos la expresion mediante "fuerza bruta" (iteramos 10 veces, 
    //lo que es mas que suficiente).
    double factor;
    wvac = wair;
    for (long i=0; i < 10; i++)
    {
      factor = 1.0+2.735182E-4+
               131.4182/(wvac*wvac)+
               2.76249E8/(wvac*wvac*wvac*wvac);
      wvac = wair * factor;
    }
  }
  return wvac;
}
