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

//Declaración de la clase GenericPixel
//Las funciones miembro se definen en genericpixel.cpp

#ifndef GENERICPIXEL_H
#define GENERICPIXEL_H

class GenericPixel{
  public:
    GenericPixel(); //constructor por defecto
    GenericPixel(double, double, double, double);//constructor con 4 parámetros
    void setwave(const double);
    void setflux(const double);
    void seteflux(const double);
    void setpixelfraction(const double);
    double getwave();
    double getflux();
    double geteflux();
    double getpixelfraction();
  private:
    double wave;        //wavelength
    double flux;        //flux
    double eflux;       //uncertainty in flux
    double pixelfraction; //pixel fraction
};

#endif
