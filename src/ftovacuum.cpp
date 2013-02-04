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

using namespace std;

//Funcion auxiliar para transformar una longitud de onda de aire a vacio.
//Utilizamos la expresion de Morton (1991, ApJS, 77, 119), citada en la
//calibracion en longitud de onda de los espectros del Sloan Digital Sky
//Survey. Invertimos la expresion mediante "fuerza bruta" (iteramos 10 veces,
//lo que es mas que suficiente).
double ftovacuum(const double wair)
{
  double wvac,factor;
  wvac = wair;
  for (long i=0; i < 10; i++)
  {
    factor = 1.0+2.735182E-4+
             131.4182/(wvac*wvac)+
             2.76249E8/(wvac*wvac*wvac*wvac);
    wvac = wair * factor;
  }
  return wvac;
}
