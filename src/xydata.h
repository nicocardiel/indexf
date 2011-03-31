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

//Declaración de la clase XYData
//Las funciones miembro se definen en xydata.cpp

#ifndef XYDATA_H
#define XYDATA_H

class XYData{
  public:
    XYData(); //constructor por defecto
    XYData(double, double); //constructor con 2 parámetros
    void setx(const double);
    void sety(const double);
    double getx();
    double gety();
  private:
    double x; //x coordinate
    double y; //y coordinate
};

#endif
