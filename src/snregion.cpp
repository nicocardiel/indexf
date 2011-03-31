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

//Definicion de funciones miembro de la clase SNRegion, declarada en snregion.h
#include "snregion.h"

using namespace std;

//-----------------------------------------------------------------------------
//constructor por defecto
SNRegion::SNRegion()
{
  wave1=0.0;
  wave2=0.0;
  poldeg=0;
  npixels=0;
  sn=0.0;
}

//constructor con 5 parametros
SNRegion::SNRegion(double wave1, double wave2, 
                   long poldeg, long npixels, double sn)
{
  setwave1(wave1);
  setwave2(wave2);
  setpoldeg(poldeg);
  setnpixels(npixels);
  setsn(sn);
}

//-----------------------------------------------------------------------------
void SNRegion::setwave1(const double newwave1)
{
  wave1=newwave1;
}

//-----------------------------------------------------------------------------
void SNRegion::setwave2(const double newwave2)
{
  wave2=newwave2;
}

//-----------------------------------------------------------------------------
void SNRegion::setpoldeg(const long newpoldeg)
{
  poldeg=newpoldeg;
}

//-----------------------------------------------------------------------------
void SNRegion::setnpixels(const long newnpixels)
{
  npixels=newnpixels;
}

//-----------------------------------------------------------------------------
void SNRegion::setsn(const double newsn)
{
  sn=newsn;
}

//-----------------------------------------------------------------------------
double SNRegion::getwave1() { return wave1 ; }

//-----------------------------------------------------------------------------
double SNRegion::getwave2() { return wave2 ; }

//-----------------------------------------------------------------------------
long SNRegion::getpoldeg() { return poldeg ; }

//-----------------------------------------------------------------------------
long SNRegion::getnpixels() { return npixels ; }

//-----------------------------------------------------------------------------
double SNRegion::getsn() { return sn ; }

