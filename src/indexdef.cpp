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

//Definición de funciones miembro de la clase IndexDef, declarada en 
//indexdef.h
#include <iostream>
#include <string.h>
#include <cstdlib>
#include "indexdef.h"

using namespace std;

//-----------------------------------------------------------------------------
//constructor
IndexDef::IndexDef(const char *idlabel, const long &idtype)
{
  long length = strlen(idlabel);
  length = (length < 9 ? length : 8);
  strncpy(label,idlabel,length);
  label[length] = '\0';
  type = idtype;
  if(type == 1) //índices moleculares
  {
    nbands=3;
    nconti=2;
    nlines=1;
  }
  else if(type == 2) //índices atómicos
  {
    nbands=3;
    nconti=2;
    nlines=1;
  }
  else if(type == 3) //D4000
  {
    nbands=2;
    nconti=2;
    nlines=0;
  }
  else if(type == 4) //B4000
  {
    nbands=2;
    nconti=2;
    nlines=0;
  }
  else if(type == 5) //discontinuidad
  {
    nbands=2;
    nconti=2;
    nlines=0;
  }
  else if(type == 10) //emission line (in flux units)
  {
    nbands=0;
    nconti=0;
    nlines=0;
  }
  else if( (type >= 11) && (type <= 99) ) //discontinuidad genérica
  {
    nconti=type/10;        //number of blue bands
    nlines=type-nconti*10; //number of red bands
    nbands=nconti+nlines;
  }
  else if( (type >= 101) && (type <= 9999) ) //índices genéricos
  {
    nconti=type/100;
    nlines=type-nconti*100;
    nbands=nconti+nlines;
  }
  else if( (type >= -99) && (type <= -2) ) //índices pendiente
  {
    nbands=-type;
    nconti=-type;
    nlines=0;
  }
  else
  {
    cout << "FATAL ERROR: index type=" << type << " is not valid" << endl;
    cout << "--> Check file indexdef.dat, index: " << idlabel << endl;
    exit(1);
  }
}

//-----------------------------------------------------------------------------
bool IndexDef::setnbands(const long &nb)
{
  if( nb <= 0 )
  {
    cout << "FATAL ERROR: number of bands must be > 0)" << endl;
    cout << "--> nb=" << nb << endl;
    exit(1);
  }
  nbands=nb;
  return(true);
}

//-----------------------------------------------------------------------------
bool IndexDef::setnconti(const long &nb)
{
  if( nb <= 0 )
  {
    cout << "FATAL ERROR: number of continuum bands must be > 0)" << endl;
    cout << "--> nb=" << nb << endl;
    exit(1);
  }
  nconti=nb;
  return(true);
}

//-----------------------------------------------------------------------------
bool IndexDef::setnlines(const long &nb)
{
  if( nb <= 0 )
  {
    cout << "FATAL ERROR: number of line bands must be > 0)" << endl;
    cout << "--> nb=" << nb << endl;
    exit(1);
  }
  nlines=nb;
  return(true);
}

//-----------------------------------------------------------------------------
bool IndexDef::setldo(const long &nb, const double &l1, const double &l2)
{
  if( (nb < 1) || (nb > nbands) )
  {
    cout << "FATAL ERROR: nb=" << nb << " (nbands=" << nbands << ")" << endl;
    exit(1);
  }
  ldo1[nb-1]=l1;
  ldo2[nb-1]=l2;
  return(true);
}

//-----------------------------------------------------------------------------
bool IndexDef::setldo(const long &nb, const double &l1, const double &l2, 
                      const double &fact)
{
  if( (nb < 1) || (nb > nbands) )
  {
    cout << "FATAL ERROR: in indexdef.cpp while defining index " << label 
         << endl;
    cout << "             nb=" << nb << " (nbands=" << nbands << ")" << endl;
    exit(1);
  }
  ldo1[nb-1]=l1;
  ldo2[nb-1]=l2;
  if (nb > nconti)
  {
    factor[nb-nconti-1]=fact;
  }
  else
  {
    cout << "FATAL ERROR: in indexdef.cpp while defining index " << label 
         << endl;
    cout << "             nb=" << nb << " (nconti=" << nconti << ")" << endl;
    exit(1);
  }
  return(true);
}

//-----------------------------------------------------------------------------
char *IndexDef::getlabel() { return label; }

//-----------------------------------------------------------------------------
long IndexDef::gettype() const { return type; }

//-----------------------------------------------------------------------------
long IndexDef::getnbands() const { return nbands; } 

//-----------------------------------------------------------------------------
long IndexDef::getnconti() const { return nconti; }

//-----------------------------------------------------------------------------
long IndexDef::getnlines() const { return nlines; }

//-----------------------------------------------------------------------------
double IndexDef::getldo1(const long &nb) const
{ 
  if ( (nb >= 0) && (nb <= nbands-1) )
    return ldo1[nb];
  else
  {
    cout << "FATAL ERROR: invalid band number (nb =" << nb+1 << ", " 
         << "nbands=" << nbands << ") in IndexDef::getldo1" << endl;
    exit(1);
  }
}

//-----------------------------------------------------------------------------
double IndexDef::getldo2(const long &nb) const
{ 
  if ( (nb >= 0) && (nb <= nbands-1) )
    return ldo2[nb];
  else
  {
    cout << "FATAL ERROR: invalid band number (nb =" << nb+1 << ", " 
         << "nbands=" << nbands << ") in IndexDef::getldo2" << endl;
    exit(1);
  }
}

//-----------------------------------------------------------------------------
double IndexDef::getfactor(const long &nb) const 
{
  if ( (nb >= 0) && (nb <= nlines-1) )
    return factor[nb];
  else
  {
    cout << "FATAL ERROR: invalid band number (nb=" << nb+1 << ", " 
         << "nlines=" << nlines << ") in IndexDef::getfactor" << endl;
    exit(1);
  }
}

//-----------------------------------------------------------------------------
double IndexDef::getfactor_el(const long &nb) const 
{
  if ( (nb >= 0) && (nb <= nbands-1) )
    return factor[nb];
  else
  {
    cout << "FATAL ERROR: invalid band number (nb=" << nb+1 << ", " 
         << "nbands=" << nbands << ") in IndexDef::getfactor_el" << endl;
    exit(1);
  }
}
