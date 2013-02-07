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
#include <fstream>

using namespace std;

void welcome(bool verbose)
{
  if (!verbose) return;
  //el directorio de instalacion esta definido como variable global
  extern const char *installdirPtr;
  //...........................................................................
  //(1) VERSION
  //la version esta definida como variable global
  extern const char *versionPtr;
  cout << "#\n#Using indexf version: " << versionPtr 
       << " (pyndexf compatible, "
#ifdef HAVE_CPGPLOT_H
       << "PGPLOT enabled)"
#else
       << "PGPLOT disabled)"
#endif /* HAVE_CPGPLOT_H */
       << "\n#" << endl;
  //...........................................................................
  //(2) INDEX DEFINITIONS
  //el fichero local (de existir) tiene un nombre predefinido
  const char *localindexfilePtr = "./myindexdef.dat";
  // comprobamos si hay fichero local
  bool iflocal;
  long lsize;
  ifstream local_file(localindexfilePtr);
  if (local_file.good())
  {
    lsize = strlen(localindexfilePtr);
    local_file.close();
    iflocal = true;
  }
  else
  {
    lsize = strlen(installdirPtr) + strlen("/indexdef.dat");
    iflocal = false;
  }
  //concatenamos el directorio y el nombre del fichero, usando una variable
  //generada con new y eliminada con delete
  char *infilename1Ptr = new char[lsize+1];
  if (iflocal)
  {
    strcpy(infilename1Ptr,localindexfilePtr);
  }
  else
  {
    strcpy(infilename1Ptr,installdirPtr); //copiamos directorio
    strcat(infilename1Ptr,"/indexdef.dat"); //concatenamos nombre de fichero
  }
  cout << "#Index definitions..: " << infilename1Ptr << endl;
  delete [] infilename1Ptr;
  //...........................................................................
  //(3) DEFAULT KEYWORDS:VALUES
  //concatenamos el directorio y el nombre del fichero, usando una variable
  //generada con new y eliminada con delete
  lsize = strlen(installdirPtr) + strlen("/inputcl.dat");
  char *infilename2Ptr = new char[lsize+1];
  strcpy(infilename2Ptr,installdirPtr); //copiamos directorio
  strcat(infilename2Ptr,"/inputcl.dat"); //concatenamos nombre de fichero
  cout << "#Keyword definitions: " << infilename2Ptr << "\n#" << endl;
  delete [] infilename2Ptr;
}
