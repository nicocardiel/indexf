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

//Funcion para sustraer la pendiente del ajuste a un polinomio de grado 1
//(manteniendo la señal promedio) a un vector de XYData.  El vector de entrada
//es modificado en esta funcion. Como el orden de magnitud de las variables X e
//Y puede ser muy distinto, antes de realizar el ajuste normalizamos el
//recorrido de las variables.

#include <vector>
#include <cmath>
#include "xydata.h"

using namespace std;

void sustrae_p1(vector<XYData> &region, 
                double &mean_signal, double &mean_noise)
{
  //definimos un iterador para recorrer el vector
  vector<XYData>::iterator xypair;

  //determinamos la media en X y en Y para normalizar las variables
  double meanx=0.0;
  double meany=0.0;
  for (xypair=region.begin(); xypair!=region.end(); xypair++)
  {
    double x=xypair->getx();
    double y=xypair->gety();
    meanx+=x;
    meany+=y;
  }
  double ndata=static_cast<double>(region.size());
  meanx/=ndata;
  meany/=ndata;
  for (xypair=region.begin(); xypair!=region.end(); xypair++)
  {
    double x=xypair->getx();
    double y=xypair->gety();
    x/=meanx;
    y/=meany;
    xypair->setx(x);
    xypair->sety(y);
  }

  //ajustamos una recta por minimos cuadrados
  double sumx=0.0;
  double sumy=0.0;
  double sumxy=0.0;
  double sumxx=0.0;
  for (xypair=region.begin(); xypair!=region.end(); xypair++)
  {
    double x=xypair->getx();
    double y=xypair->gety();
    sumx+=x;
    sumy+=y;
    sumxy+=x*y;
    sumxx+=x*x;
  }
  double det=ndata*sumxx-sumx*sumx;
  double a=(ndata*sumxy-sumx*sumy)/det;
  double b=(sumxx*sumy-sumx*sumxy)/det;

  //restamos la recta
  for (xypair=region.begin(); xypair!=region.end(); xypair++)
  {
    double x=xypair->getx();
    double yfit=a*x+b;
    double ydum=xypair->gety()-yfit;
    xypair->sety(ydum);
  }

  //deshacemos la normalizacion de los datos
  for (xypair=region.begin(); xypair!=region.end(); xypair++)
  {
    double x=xypair->getx();
    double y=xypair->gety();
    x*=meanx;
    y*=meany;
    xypair->setx(x);
    xypair->sety(y);
  }

  //sumamos a los datos el valor promedio en y
  for (xypair=region.begin(); xypair!=region.end(); xypair++)
  {
    double y=xypair->gety();
    y+=meany;
    xypair->sety(y);
  }

  //calculamos la señal promedio
  mean_signal=meany;

  //calculamos el ruido promedio
  mean_noise=0.0;
  for (xypair=region.begin(); xypair!=region.end(); xypair++)
  {
    double y=xypair->gety();
    mean_noise+=(y-meany)*(y-meany);
    xypair->sety(y);
  }
  mean_noise=sqrt(mean_noise/(ndata-1));
}
