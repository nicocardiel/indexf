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

//Definicion de funciones miembro de la clase GenericPixel, declarada en 
//GenericPixel.h
#include "genericpixel.h"

using namespace std;

//-----------------------------------------------------------------------------
//constructor por defecto
GenericPixel::GenericPixel()
{
  wave=0.0;
  flux=0.0;
  eflux=0.0;
  pixelfraction=1.0;
}

//constructor con 4 parametros
GenericPixel::GenericPixel(double wave, double flux, double eflux, 
                           double pixelfraction)
{
  setwave(wave);
  setflux(flux);
  seteflux(eflux);
  setpixelfraction(pixelfraction);
}

//-----------------------------------------------------------------------------
void GenericPixel::setwave(const double newwave)
{
  wave=newwave;
}

//-----------------------------------------------------------------------------
void GenericPixel::setflux(const double newflux)
{
  flux=newflux;
}

//-----------------------------------------------------------------------------
void GenericPixel::seteflux(const double neweflux)
{
  eflux=neweflux;
}

//-----------------------------------------------------------------------------
void GenericPixel::setpixelfraction(const double newpixelfraction)
{
  pixelfraction=newpixelfraction;
}

//-----------------------------------------------------------------------------
double GenericPixel::getwave() { return wave ; }

//-----------------------------------------------------------------------------
double GenericPixel::getflux() { return flux ; }

//-----------------------------------------------------------------------------
double GenericPixel::geteflux() { return eflux ; }

//-----------------------------------------------------------------------------
double GenericPixel::getpixelfraction() { return pixelfraction ; }

