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

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include "indexdef.h"

using namespace std;

//lee las definiciones de los índices del fichero indexdef.dat
bool loadidef(vector< IndexDef > &id)
{
  //el directorio de instalación está definido como variable global
  extern const char *installdirPtr;
  //concatenamos el directorio y el nombre del fichero, usando una variable
  //generada con new y eliminada con delete
  long lsize = strlen(installdirPtr) + strlen("indexdef.dat");
  char *infilenamePtr = new char[lsize+1];
  strcpy(infilenamePtr,installdirPtr); //copiamos directorio
  strcat(infilenamePtr,"indexdef.dat"); //concatenamos nombre de fichero
  //abrimos el archivo mediante el constructor de ifstream
  ifstream inpfile(infilenamePtr,ios::in);
  if(!inpfile)
  {
    cout << "FATAL ERROR: while opening the file " << infilenamePtr << endl;
    delete [] infilenamePtr;
    return(false);
  }
  //leemos el fichero
  string s, ss;
  char *labelPtr, *valuePtr, *remainderPtr;
  long type, nbands, nconti, nlines;
  double ldo1, ldo2, factor;
  getline(inpfile,s); //ignoramos dos primeras líneas
  getline(inpfile,s);
  while (getline(inpfile,s))
  {
    //convertimos string en cadena char * (tipo C)
    lsize=s.length();
    char *linePtr = new char[lsize+1];
    s.copy(linePtr,lsize,0);
    linePtr[lsize] = '\0';
    labelPtr = strtok(linePtr," "); //nombre del índice
    valuePtr = strtok(NULL," "); // valores siguientes del índice
    type = static_cast<long>(strtol(valuePtr,&remainderPtr,0));
    //construimos un objeto de tipo IndexDef
    IndexDef idread(labelPtr,type);
    nbands = idread.getnbands();
    nconti = idread.getnconti();
    nlines = idread.getnlines();
    //-------------------------------------------------------------------------
    //índices moleculares, atómicos, D4000, B4000 y colores
    if ( (type >= 1) && (type <= 5) )
    {
      for (long i=1; i <= nbands; i++)
      {
        ldo1 = strtod(strtok(NULL," "),&remainderPtr);
        ldo2 = strtod(strtok(NULL," "),&remainderPtr);
        idread.setldo(i,ldo1,ldo2);
      }
    }
    //-------------------------------------------------------------------------
    //líneas de emissión
    else if( type == 10 )
    {
      //como todavia no sabemos cuantas bandas hay, leemos ese numero
      getline(inpfile,ss);
      lsize=ss.length();
      char *linePtrAux = new char[lsize+1];
      ss.copy(linePtrAux,lsize,0);
      linePtrAux[lsize] = '\0';
      nbands = strtol(strtok(linePtrAux," "),&remainderPtr, 0);
      idread.setnbands(nbands);
      delete [] linePtrAux;
      if ( nbands > 99 )
      {
        cout << "FATAL ERROR: number of bands too large." << endl;
        cout << "--> index= " << labelPtr << endl;
        cout << "--> nb= " << nbands << endl;
        cout << "--> check file " << infilenamePtr << endl;
        delete [] linePtr;
        delete [] infilenamePtr;
        return(false);
      }
      //ahora ya podemos ir leyendo todas las bandas necesarias 
      //(NO TIENEN QUE ESTAR EN ORDEN)
      nconti=0;
      nlines=0;
      for (long i=1; i <= nbands; i++)
      {
        getline(inpfile,ss);
        lsize=ss.length();
        char *linePtrAux = new char[lsize+1];
        ss.copy(linePtrAux,lsize,0);
        linePtrAux[lsize] = '\0';
        ldo1 = strtod(strtok(linePtrAux," "),&remainderPtr);
        ldo2 = strtod(strtok(NULL," "),&remainderPtr);
        factor = strtod(strtok(NULL," "),&remainderPtr);
        idread.setldo(i,ldo1,ldo2,factor);
        if(factor == 0.0)
        {
          nconti++;
        }
        else
        {
          nlines++;
        }
        delete [] linePtrAux;
      }
      idread.setnconti(nconti);
      idread.setnlines(nlines);
      if ( nconti < 1 )
      {
        cout << "FATAL ERROR: number of continuum bands too small" << endl;
        cout << "--> index= " << labelPtr << endl;
        cout << "--> nconti= " << nconti << endl;
        cout << "--> check file " << infilenamePtr << endl;
        delete [] linePtr;
        delete [] infilenamePtr;
        return(false);
      }
      if ( nlines < 1 )
      {
        cout << "FATAL ERROR: number of emission-line bands too small" << endl;
        cout << "--> index= " << labelPtr << endl;
        cout << "--> nlines= " << nlines << endl;
        cout << "--> check file " << infilenamePtr << endl;
        delete [] linePtr;
        delete [] infilenamePtr;
        return(false);
      }
    }
    //-------------------------------------------------------------------------
    //índices genéricos (discontinuidad, atómicos, pendiente)
    else
    {
      if ( nconti < 1 )
      {
        cout << "FATAL ERROR: number of continuum bands too small" << endl;
        cout << "--> index= " << labelPtr << endl;
        cout << "--> nconti= " << nconti << endl;
        cout << "--> check file " << infilenamePtr << endl;
        delete [] linePtr;
        delete [] infilenamePtr;
        return(false);
      }
      for (long i=1; i <= nconti; i++)
      {
        getline(inpfile,ss);
        lsize=ss.length();
        char *linePtrAux = new char[lsize+1];
        ss.copy(linePtrAux,lsize,0);
        linePtrAux[lsize] = '\0';
        ldo1 = strtod(strtok(linePtrAux," "),&remainderPtr);
        ldo2 = strtod(strtok(NULL," "),&remainderPtr);
        idread.setldo(i,ldo1,ldo2);
        delete [] linePtrAux;
      }
      //discontinuidad genérica
      if ( (type >= 11) && (type <= 99) )
      {
        if ( nlines < 1 )
        {
          cout << "FATAL ERROR: number of absorption bands too small" << endl;
          cout << "--> index= " << labelPtr << endl;
          cout << "--> nbands= " << nlines << endl;
          cout << "--> check file " << infilenamePtr << endl;
          delete [] linePtr;
          delete [] infilenamePtr;
          return(false);
        }
        for (long i=1; i <= nlines; i++)
        {
          getline(inpfile,ss);
          lsize=ss.length();
          char *linePtrAux = new char[lsize+1];
          ss.copy(linePtrAux,lsize,0);
          linePtrAux[lsize] = '\0';
          ldo1 = strtod(strtok(linePtrAux," "),&remainderPtr);
          ldo2 = strtod(strtok(NULL," "),&remainderPtr);
          idread.setldo(i+nconti,ldo1,ldo2);
          delete [] linePtrAux;
        }
      }
      //índice genérico
      else if ( (type >= 101) && (type <= 9999) )
      {
        if ( nlines < 1 )
        {
          cout << "FATAL ERROR: number of feature bands too small" << endl;
          cout << "--> index= " << labelPtr << endl;
          cout << "--> nbands= " << nlines << endl;
          cout << "--> check file " << infilenamePtr << endl;
          delete [] linePtr;
          delete [] infilenamePtr;
          return(false);
        }
        for (long i=1; i <= nlines; i++)
        {
          getline(inpfile,ss);
          lsize=ss.length();
          char *linePtrAux = new char[lsize+1];
          ss.copy(linePtrAux,lsize,0);
          linePtrAux[lsize] = '\0';
          ldo1 = strtod(strtok(linePtrAux," "),&remainderPtr);
          ldo2 = strtod(strtok(NULL," "),&remainderPtr);
          factor = strtod(strtok(NULL," "),&remainderPtr);
          idread.setldo(i+nconti,ldo1,ldo2,factor);
          delete [] linePtrAux;
        }
      }
      //índice pendiente
      else
      {
        //ya hemos leido todas las bandas; no hay que hacer nada mas
      }
    }
    //-------------------------------------------------------------------------
    id.push_back(idread);
    delete [] linePtr;
  } //while (getline(inpfile,s))
  delete [] infilenamePtr;

  return(true);
}
