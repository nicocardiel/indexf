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

//Funcion para transformar una cadena tipo string en un numero double, 
//comprobando que no hay caracteres extranos

#include <string>
#include <cstring>
#include <cctype>
#include <cstdlib>

using namespace std;

bool issdouble(const string &snumber, double &number)
{
  number=0.0;

  //transformamos el string a cadena tipo C
  long lsize=snumber.length();
  if(lsize==0)
  {
    return(false);
  }
  char *numberPtr = new char[lsize+1];
  snumber.copy(numberPtr,lsize,0);
  numberPtr[lsize]='\0';
  for(long i=1; i<=lsize; i++) //chequeamos cada digito
  {
    if(isdigit(numberPtr[i-1]) == 0) //no es un digito valido
    {
      if (  (numberPtr[i-1] == '.') || (numberPtr[i-1] == '+') 
         || (numberPtr[i-1] == 'E') || (numberPtr[i-1] == 'e')
         || (numberPtr[i-1] == 'D') || (numberPtr[i-1] == 'd')
         || (numberPtr[i-1] == '-') )
      {
        //todo va bien
      }
      else
      {
        delete [] numberPtr;
        return(false);
      }
    }
  }
  char *restnumber;
  number=static_cast<double>(strtod(numberPtr, &restnumber));
  if(strlen(restnumber) != 0) //el numero no es valido
  {
    delete [] numberPtr;
    return(false);
  }
  delete [] numberPtr;
  return(true);
}
