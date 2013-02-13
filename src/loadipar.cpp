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
#include <vector>
#include "commandtok.h"

using namespace std;

//lee los parametros de entrada introducidos por el usuario en la linea
//de comandos
bool loadipar(const char *argv[], const int argc,vector< CommandToken > &cl)
{
  //concatenamos todos los parametros en una unica variable de tipo string
  string list_of_parameters;
  for (long i=1;i<=argc;i++)
  {
    list_of_parameters.append(argv[i-1]);
    list_of_parameters.append(" ");
  }

  //reemplazamos " =" por "="
  long x = list_of_parameters.find(" =");
  long l=list_of_parameters.length();
  while ( (x >= 0) && (x <= l) )
  {
    list_of_parameters.replace(x,2,"=");
    l=list_of_parameters.length();
    x = list_of_parameters.find(" =");
  }

  //reemplazamos "= " por "="
  x = list_of_parameters.find("= ");
  l=list_of_parameters.length();
  while ( (x >= 0) && (x <= l) )
  {
    list_of_parameters.replace(x,2,"=");
    l=list_of_parameters.length();
    x = list_of_parameters.find("= ");
  }

  //convertimos variable string en cadena char * (tipo C)
  l=list_of_parameters.length();
  char *listPtr = new char[l+1];
  list_of_parameters.copy(listPtr,l,0);
  listPtr[l] = '\0';

  //dividimos la cadena char * (tipo C) en tokens
  char *tokenPtr, *equalPtr, *equalrPtr;
  long keylabelSize,keyvalueSize;
  tokenPtr = strtok(listPtr," "); //primer token es nombre del programa
  tokenPtr = strtok(NULL," "); //pasamos al primer argumento (segundo token)
  while (tokenPtr != NULL)
  {
    //buscamos el caracter '=' comenzando desde el principio y desde el final
    equalPtr = strchr(tokenPtr,'=');
    equalrPtr = strrchr(tokenPtr,'=');
    //comprobamos que existe y es unico
    if (equalPtr == NULL)
    {
      cout << "FATAL ERROR: argument <" << tokenPtr
           << "> in command line is invalid" << endl;
      cout << "> Missing \"=\" character" << endl;
      delete [] listPtr;
      return(false);
    }
    if (equalPtr != equalrPtr)
    {
      cout << "FATAL ERROR: argument <" << tokenPtr
           << "> in command line is invalid" << endl;
      cout << "> Too many \"=\" characters found" << endl;
      delete [] listPtr;
      return(false);
    }
    //extraemos label (usamos aritmetica de punteros para calcular el tamano
    //del label, y asi poder realizar un dimensionado dinamico de la variable)
    keylabelSize = equalPtr - tokenPtr; //usamos aritmetica de punteros
    char *keylabelPtr = new char[keylabelSize+1];
    strncpy(keylabelPtr,tokenPtr,keylabelSize);
    keylabelPtr[keylabelSize] = '\0';
    //extraemos value (aqui tambien usamos aritmetica de punteros; notar que
    //en la llamada a strncpy tambien usamos esta aritmetica en el segundo
    //parametro)
    keyvalueSize = strlen(tokenPtr) - strlen(keylabelPtr) -1;
    char *keyvaluePtr = new char[keyvalueSize+1];
    strncpy(keyvaluePtr,tokenPtr+keylabelSize+1,keyvalueSize);
    keyvaluePtr[keyvalueSize] = '\0';
    //recorremos todos los label por defecto y comprobamos que el label
    //indicado en la linea de comandos es uno de dichos label esperados
    bool labelfound=false;
    for (unsigned long i = 1; i <= cl.size(); i++)
    {
      if (strcmp(cl[i-1].getlabel(),keylabelPtr) == 0)
      {
        labelfound=true;
        cl[i-1].setvalue(keyvaluePtr);
      }
    }
    if(!labelfound)
    {
      //comprobamos si es "infile", un alias de "if" que introducimos para
      //facilitar la compatibilidad con pyindexf (mejor generar un alias que
      //cambiar "if" por "infile" y no mantener la compatibilidad hacia atras
      //de indexf)
      if (strcmp(keylabelPtr, "infile") == 0)
      {
        for (unsigned long i = 1; i <= cl.size(); i++)
        {
          if (strcmp(cl[i-1].getlabel(),"if") == 0)
          {
            labelfound=true;
            cl[i-1].setvalue(keyvaluePtr);
          }
        }
      }
      else
      {
        cout << "FATAL ERROR: invalid keyword label <" 
             << keylabelPtr << ">" << endl;
        cout << "Valid keywords are only: ";
        for (unsigned long i = 1; i <= cl.size(); i++)
          cout << "<" << cl[i-1].getlabel() << "> ";
        cout << endl;
        delete [] keylabelPtr;
        delete [] keyvaluePtr;
        delete [] listPtr;
        return(false);
      }
    }
#ifndef HAVE_CPGPLOT_H
    else
    {
      bool erroneous_label=false;
      if (strcmp(keylabelPtr, "plotmode") == 0) erroneous_label=true;
      if (strcmp(keylabelPtr, "grdev")    == 0) erroneous_label=true;
      if (strcmp(keylabelPtr, "nwindows") == 0) erroneous_label=true;
      if (strcmp(keylabelPtr, "plottype") == 0) erroneous_label=true;
      if (strcmp(keylabelPtr, "xlim")     == 0) erroneous_label=true;
      if (strcmp(keylabelPtr, "ylim")     == 0) erroneous_label=true;
      if(erroneous_label)
      {
        cout << "FATAL ERROR: invalid keyword label <" 
             << keylabelPtr << ">" << endl;
        cout << "             The program was installed without PGPLOT!" 
             << endl;
        delete [] keylabelPtr;
        delete [] keyvaluePtr;
        delete [] listPtr;
        return(false);
      }
    }
#endif
    delete [] keylabelPtr;
    delete [] keyvaluePtr;
    tokenPtr = strtok(NULL," ");
  }

  delete [] listPtr;
  return(true);
}
