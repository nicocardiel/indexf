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

//Declaración de la clase SNRegion
//Las funciones miembro se definen en snregion.cpp

#ifndef SNREGION_H
#define SNREGION_H

class SNRegion{
  public:
    SNRegion(); //constructor por defecto
    SNRegion(double, double, long, long, double);//constructor con 5 parámetros
    void setwave1(const double);
    void setwave2(const double);
    void setpoldeg(const long);
    void setnpixels(const long);
    void setsn(const double);
    double getwave1();
    double getwave2();
    long getpoldeg();
    long getnpixels();
    double getsn();
  private:
    double wave1;  //rest-frame initial wavelength
    double wave2;  //rest-frame final wavelength
    long poldeg;   //polynomial degree of the polynomial to be subtracted
    long npixels;  //number of pixels in the band
    double sn;     //measured S/N in the band
};

#endif
