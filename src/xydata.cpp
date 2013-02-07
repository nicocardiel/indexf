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

//Definicion de funciones miembro de la clase XYData, declarada en xydata.h
#include "xydata.h"

using namespace std;

//-----------------------------------------------------------------------------
//constructor por defecto
XYData::XYData()
{
  x=0.0;
  y=0.0;
}

//constructor con 2 parametros
XYData::XYData(double x, double y)
{
  setx(x);
  sety(y);
}

//-----------------------------------------------------------------------------
void XYData::setx(const double newx)
{
  x=newx;
}

//-----------------------------------------------------------------------------
void XYData::sety(const double newy)
{
  y=newy;
}

//-----------------------------------------------------------------------------
double XYData::getx() { return x; }


//-----------------------------------------------------------------------------
double XYData::gety() { return y; }

