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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <iostream>
#include <string.h>

using namespace std;

//hace una busqueda inicial del keyword pyindexf para saber pronto si, en caso
//de error, el programa debe salir o no con exit(1) o exit(0)
bool checkpyind(const char *argv[], const int argc)
{
  if (argc == 1)
  {
    return false;
  }
  //concatenamos todos los parametros en una unica variable de tipo string
  string list_of_parameters;
  for (long i=1; i<=argc; i++)
  {
    list_of_parameters.append(argv[i-1]);
  }
  //reemplazamos todos los espacios en blanco
  unsigned long x=list_of_parameters.find(" ");
  unsigned long l=list_of_parameters.length();
  while ( (x != string::npos) && (x <= l) )
  {
    list_of_parameters.replace(x,1,"");
    l=list_of_parameters.length();
    x=list_of_parameters.find(" ");
  }
  //el metodo find() the string retorna string::npos si no encuentra la cadena
  //que estamos buscando
  bool pyindexf = (list_of_parameters.find("pyindexf=yes") != string::npos);
  return pyindexf;
}
