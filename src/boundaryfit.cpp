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
#include <cmath>
#include <vector>
#include <algorithm>
#include "genericpixel.h"

using namespace std;
 
//-----------------------------------------------------------------------------
//Calcula el boundary fit a los datos en el vector vec
bool boundaryfit(vector <GenericPixel> &vec, const bool lerr,
                 double *fboundPtr, double *e2fboundPtr)
{
  //---------------------------------------------------------------------------
  //valores retornados en caso de error o imposibilidad de calculo
  *fboundPtr=0;
  *e2fboundPtr=0;

  long num=vec.size();
  if (num <= 0)
  {
    cout << "ERROR in function boundaryfit: vector size = 0" << endl;
    return(false);
  }

  //valores temporales
  *fboundPtr=1.0;
  *e2fboundPtr=0.01;
  return(true);
}
