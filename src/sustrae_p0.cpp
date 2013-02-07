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

//Funcion para sustraer un polinomio de grado 0 (la media) al valor de Y en
//un vector de XYData. El vector de entrada es modificado en esta funcion.

#include <vector>
#include "xydata.h"
#include <cmath>

using namespace std;

void sustrae_p0(vector<XYData> &region,
                double &mean_signal, double &mean_noise)
{
  //definimos un iterador para recorrer el vector
  vector<XYData>::iterator xypair;

  //calculamos la media
  double meany=0.0;
  for (xypair=region.begin(); xypair!=region.end(); xypair++)
  {
    meany+=xypair->gety();
  }
  double ndata=static_cast<double>(region.size());
  meany/=ndata;

  //calculamos la senal promedio
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
