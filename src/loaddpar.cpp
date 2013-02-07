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

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include "commandtok.h"

using namespace std;

//lee los valores por defecto de los parametros de entrada del fichero
//inputcl.dat
bool loaddpar(vector< CommandToken > &cl)
{
  //el directorio de instalacion esta definido como variable global
  extern const char *installdirPtr;
  //concatenamos el directorio y el nombre del fichero, usando una variable
  //generada con new y eliminada con delete
  long lsize = strlen(installdirPtr) + strlen("/inputcl.dat");
  char *infilenamePtr = new char[lsize+1];
  strcpy(infilenamePtr,installdirPtr); //copiamos directorio
  strcat(infilenamePtr,"/inputcl.dat"); //concatenamos nombre de fichero
  //abrimos el archivo mediante el constructor de ifstream
  ifstream inpfile(infilenamePtr,ios::in);
  if(!inpfile)
  {
    cout << "FATAL ERROR: while opening the file " << infilenamePtr << endl;
    delete [] infilenamePtr;
    return(false);
  }
  else
  {
    cout <<"#Keyword definitions: " << infilenamePtr << "\n#" << endl;
  }
  //leemos el fichero
  string s;
  char *labelPtr, *valuePtr;
  //saltamos las tres primeras lineas
  getline(inpfile,s);
  getline(inpfile,s);
  getline(inpfile,s);
  while (getline(inpfile,s))
  {
    //convertimos string en cadena char * (tipo C)
    lsize=s.length();
    char *linePtr = new char[lsize+1];
    s.copy(linePtr,lsize,0);
    linePtr[lsize] = '\0';
    //separamos los dos primeros tokens de cada linea (ignoramos el resto);
    //notar que esto exige que no haya espacios en blanco dentro de cada token
    labelPtr = strtok(linePtr," ");
    valuePtr = strtok(NULL," ");
    //asignamos a un objeto CommandToken el label y el value de cada comando
    CommandToken cpair;
    cpair.setlabel(labelPtr);
    cpair.setvalue(valuePtr);
    //anadimos un nuevo objeto al vector de CommandToken cl
    cl.push_back(cpair);
    delete [] linePtr;
  }
  delete [] infilenamePtr;

  return(true);
}
