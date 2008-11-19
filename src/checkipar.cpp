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
#include <vector>
#include <fstream>
#include <cctype>
#include <typeinfo>
#include <cmath>
#include <string.h>
#include <stdlib.h>
#include "commandtok.h"
#include "indexdef.h"
#include "indexparam.h"

using namespace std;

//-----------------------------------------------------------------------------
//prototipos de funciones auxiliares
bool extract_file_2long(const char *, char *, bool &, long &, long &);

template < typename T >
bool extract_2numbers(const char *, T &, T &);

//-----------------------------------------------------------------------------
//funcion principal: comprueba los parametros de entrada
bool checkipar(vector< CommandToken > &cl, IndexParam &param,
               vector< IndexDef > &id)
{
  const char *labelPtr;
  const char *valuePtr;
  long maxfileSize;

  //--------------------------------------------
  //input file name,first spectrum,last spectrum
  //--------------------------------------------
  labelPtr = cl[0].getlabel();
  valuePtr = cl[0].getvalue();
  long ns1,ns2;
  bool logfile_if;
  maxfileSize=strlen(valuePtr);
  char *filePtr = new char[maxfileSize+1];
  if(!extract_file_2long(valuePtr,filePtr,logfile_if,ns1, ns2))
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    delete [] filePtr;
    return(false);
  }
  if(!logfile_if)
  {
    if(strcmp(filePtr,"undef") == 0)
    {
      cout << "FATAL ERROR: you must supply an input file name" << endl;
    }
    else
    {
      cout << "FATAL ERROR: the file <" << filePtr 
           << "> does not exist" << endl;
    }
    delete [] filePtr;
    return(false);
  }
  if( (ns1 < 0) || (ns2 < 0) || (ns2 < ns1) )
  {
    cout << "ns1, ns2: " << ns1 << " " << ns2 << endl;
    cout << "FATAL ERROR: invalid spectrum numbers" << endl;
    delete [] filePtr;
    return(false);
  }
  if( ( (ns1 == 0) && (ns2 != 0) ) || ( (ns1 != 0) && (ns2 == 0) ) )
  {
    cout << "ns1, ns2: " << ns1 << " " << ns2 << endl;
    cout << "FATAL ERROR: invalid spectrum numbers" << endl;
    delete [] filePtr;
    return(false);
  }
  param.set_if(filePtr);
  param.set_ns1(ns1);
  param.set_ns2(ns2);
  delete [] filePtr;

  //-------------------------
  //index identification name
  //-------------------------
  labelPtr = cl[1].getlabel();
  valuePtr = cl[1].getvalue();
  if(strcmp(valuePtr,"undef") == 0)
  {
    cout << "FATAL ERROR: you must supply an index identification name" << endl;
    return(false);
  }
  else
  {
    unsigned long idfound=0;
    for (unsigned long i=1; i <= id.size(); i++)
    {
      if (strcmp(valuePtr,id[i-1].getlabel()) == 0) idfound=i;
    }
    if(idfound)
    {
      param.set_index(valuePtr); //establecemos el indice
      param.set_nindex(idfound); //numero de indice(+1) dentro del vector
    }
    else
    {
      cout << "FATAL ERROR: index identification name <" << valuePtr 
           << "> is invalid" << endl;
      return(false);
    }
  }

  //----------------
  //input error file
  //----------------
  labelPtr = cl[2].getlabel();
  valuePtr = cl[2].getvalue();
  //si el nombre del fichero no es "undef", comprobamos si existe
  if(strcmp(valuePtr,"undef") != 0)
  {
    ifstream infile(valuePtr, ios::in); //abrimos en modo solo lectura
    if (!infile) //error: el fichero no existe
    {
      cout << "FATAL ERROR: the file <" << valuePtr 
           << "> does not exist" << endl;
      return(false);
    }
  }
  param.set_ief(valuePtr);

  //---------------------------------
  //estimate S/N from rms in spectrum
  //---------------------------------
  labelPtr = cl[3].getlabel();
  valuePtr = cl[3].getvalue();
  if(strcmp(valuePtr,"undef") != 0)
  {
    if(strcmp(param.get_ief(),"undef") != 0)
    {
      cout << "FATAL ERROR: the keyword <" << labelPtr
           << "> is incompatible with the keyword <ief>" << endl;
      return(false);
    }
    ifstream infile(valuePtr, ios::in); //abrimos en modo solo lectura
    if (!infile) //error: el fichero no existe
    {
      cout << "FATAL ERROR: the file <" << valuePtr 
           << "> does not exist" << endl;
      return(false);
    }
  }
  param.set_snf(valuePtr);

  //---------------------------------------------
  //radial velocity (km/s), radial velocity error
  //---------------------------------------------
  labelPtr = cl[4].getlabel();
  valuePtr = cl[4].getvalue();
  double rv,rve;
  if(!extract_2numbers(valuePtr,rv,rve))
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    return(false);
  }
  param.set_rv(rv,rve);

  //--------------------------------------------------
  //radial velocity file name,column data,column error
  //--------------------------------------------------
  labelPtr = cl[5].getlabel();
  valuePtr = cl[5].getvalue();
  long rvc,rvce;
  bool logfile_rv;
  maxfileSize=strlen(valuePtr);
  char *filervPtr = new char[maxfileSize+1];
  if(!extract_file_2long(valuePtr,filervPtr,logfile_rv,rvc,rvce))
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    delete [] filervPtr;
    return(false);
  }
  if( (rvc <= 0) || (rvce < 0) )
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> contains invalid column numbers for the keyword <" << labelPtr
         << ">" << endl;
    delete [] filervPtr;
    return(false);
  }
  param.set_rvf(filervPtr,rvc,rvce);
  delete [] filervPtr;

  //--------------------------------------------------------
  //number of simultations to estimate radial velocity error
  //--------------------------------------------------------
  labelPtr = cl[6].getlabel();
  valuePtr = cl[6].getvalue();
  for (const char *s=valuePtr; s[0] != '\0'; s++)
  {
    if (isdigit(s[0]) == 0) //error: no es un digito valido
    {
      cout << "FATAL ERROR: <" << valuePtr
           << "> is an invalid argument for the keyword <" << labelPtr
           << ">" << endl;
      return(false);
    }
  }
  const long nsimul = atol(valuePtr);
  if(nsimul < 30)
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    cout << "> Number of simulations must be >= 30" << endl;
    return(false);
  }
  param.set_nsimul(nsimul);

  //------------------------------------
  //measure indices in logarithmic units
  //------------------------------------
  labelPtr = cl[7].getlabel();
  valuePtr = cl[7].getvalue();
  if ((strcmp(valuePtr,"yes") == 0)||(strcmp(valuePtr,"y") == 0))
  {
    param.set_logindex(true);
  }
  else if ((strcmp(valuePtr,"no") == 0)||(strcmp(valuePtr,"n") == 0))
  {
    param.set_logindex(false);
  }
  else
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    return(false);
  }
 
  //--------------------------------
  //display intermediate information
  //--------------------------------
  labelPtr = cl[8].getlabel();
  valuePtr = cl[8].getvalue();
  if ((strcmp(valuePtr,"yes") == 0)||(strcmp(valuePtr,"y") == 0))
  {
    param.set_verbose(true);
  }
  else if ((strcmp(valuePtr,"no") == 0)||(strcmp(valuePtr,"n") == 0))
  {
    param.set_verbose(false);
  }
  else
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    return(false);
  }

  //------------------------------------------------
  //number of simultations with different S/N ratios
  //------------------------------------------------
  labelPtr = cl[9].getlabel();
  valuePtr = cl[9].getvalue();
  for (const char *s=valuePtr; s[0] != '\0'; s++)
  {
    if (isdigit(s[0]) == 0) //error: no es un digito valido
    {
      cout << "FATAL ERROR: <" << valuePtr
           << "> is an invalid argument for the keyword <" << labelPtr
           << ">" << endl;
      return(false);
    }
  }
  const long nsimulsn = atol(valuePtr);
  if(nsimulsn > 0)
  {
    if(strcmp(param.get_ief(),"undef") != 0)
    {
      cout << "FATAL ERROR: <" << valuePtr
           << "> is an invalid argument for the keyword <" << labelPtr
           << ">" << endl;
      cout << "> The keyword <ief> must be set to <undef>" << endl;
      return(false);
    }
  }
  param.set_nsimulsn(nsimulsn);

  //---------------------------------------------
  //minimum and maximum S/N ratios in simulations
  //---------------------------------------------
  labelPtr = cl[10].getlabel();
  valuePtr = cl[10].getvalue();
  double minsn,maxsn;
  if(!extract_2numbers(valuePtr,minsn,maxsn))
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    return(false);
  }
  if(minsn > maxsn)
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
      cout << "> Minimum S/N ratio must be <= Maximum S/N ratio" << endl;
    return(false);
  }
  if(minsn < 0.0)
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
      cout << "> Minimum S/N ratio must be >= 0.0" << endl;
    return(false);
  }
  //el caso maxsn < 0 no hace falta porque los dos anteriores lo cubren
  param.set_minmaxsn(minsn,maxsn);

  //----------------
  //input label file
  //----------------
  labelPtr = cl[11].getlabel();
  valuePtr = cl[11].getvalue();
  long nchar1,nchar2;
  bool logfile_ilab;
  maxfileSize=strlen(valuePtr);
  char *fileilabPtr = new char[maxfileSize+1];
  if(!extract_file_2long(valuePtr,fileilabPtr,logfile_ilab,nchar1,nchar2))
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" 
         << labelPtr << ">" << endl;
    delete [] fileilabPtr;
    return(false);
  }
  if(!logfile_ilab)
  {
    if(strcmp(fileilabPtr,"undef") != 0)
    {
      cout << "FATAL ERROR: the file <" << fileilabPtr 
           << "> does not exist" << endl;
      delete [] fileilabPtr;
      return(false);
    }
  }
  if( (nchar1 < 0) || (nchar2 < 0) || (nchar2 < nchar1) )
  {
    cout << "nchar1, nchar2: " << nchar1 << " " << nchar2 << endl;
    cout << "FATAL ERROR: <" << valuePtr
         << "> contains invalid numbers for the keyword <" 
         << labelPtr << ">" << endl;
    delete [] fileilabPtr;
    return(false);
  }
  if( ( (nchar1 == 0) && (nchar2 != 0) ) || ( (nchar1 !=0) && (nchar2 == 0) ) )
  {
    cout << "nchar1, nchar2: " << nchar1 << " " << nchar2 << endl;
    cout << "FATAL ERROR: <" << valuePtr
         << "> contains invalid numbers for the keyword <" 
         << labelPtr << ">" << endl;
    delete [] fileilabPtr;
    return(false);
  }
  param.set_ilabfile(fileilabPtr);
  param.set_nchar1(nchar1);
  param.set_nchar2(nchar2);
  delete [] fileilabPtr;

  //----------------------------------------------------
  //systematic error (additive % of the continuum level)
  //----------------------------------------------------
  labelPtr = cl[12].getlabel();
  valuePtr = cl[12].getvalue();
  for (const char *s=valuePtr; s[0] != '\0'; s++)
  {
    if (isdigit(s[0]) == 0) //no es un digito valido
    {
      if ( ( (s[0] == '.') || (s[0] == '+') 
          || (s[0] == 'E') || (s[0] == 'e')
          || (s[0] == 'D') || (s[0] == 'd')
          || (s[0] == '-') ) );
      else
      {
        cout << "FATAL ERROR: <" << valuePtr
             << "> is an invalid argument for the keyword <" << labelPtr
             << ">" << endl;
        return(false);
      }
    }
  }
  const double biaserr = atof(valuePtr);
  param.set_biaserr(biaserr);

  //---------------
  //linearity error
  //---------------
  labelPtr = cl[13].getlabel();
  valuePtr = cl[13].getvalue();
  for (const char *s=valuePtr; s[0] != '\0'; s++)
  {
    if (isdigit(s[0]) == 0) //no es un digito valido
    {
      if ( ( (s[0] == '.') || (s[0] == '+') 
          || (s[0] == 'E') || (s[0] == 'e')
          || (s[0] == 'D') || (s[0] == 'd')
          || (s[0] == '-') ) );
      else
      {
        cout << "FATAL ERROR: <" << valuePtr
             << "> is an invalid argument for the keyword <" << labelPtr
             << ">" << endl;
        return(false);
      }
    }
  }
  const double linearerr = atof(valuePtr);
  param.set_linearerr(linearerr);

  //no admitimos ejecutar simultaneamente con biaserr y con linearerr
  if ((fabs(biaserr) != 0) && (linearerr !=0))
  {
    cout << ">>> biaserr  = " << biaserr << endl;
    cout << ">>> linearerr= " << linearerr << endl;
    cout << "FATAL ERROR: biaserr and linearerr cannot be "
            "employed simultaneously" << endl;
    return(false);
  }

  //------------------------------------------------------------
  //plotmode (0: none; 1: pause, 2: no pause; -: with error bars
  //------------------------------------------------------------
  labelPtr = cl[14].getlabel();
  valuePtr = cl[14].getvalue();
  for (const char *s=valuePtr; s[0] != '\0'; s++)
  {
    if (isdigit(s[0]) == 0) //error: no es un digito valido
    {
      if ((s[0] != '+') && (s[0] != '-')) //error: no es un signo
      {
        cout << "FATAL ERROR: <" << valuePtr
             << "> is an invalid argument for the keyword <" << labelPtr
             << ">" << endl;
        return(false);
      }
    }
  }
  const long plotmode = atol(valuePtr);
  if((plotmode < -2) || (plotmode > 2))
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    return(false);
  }
  param.set_plotmode(plotmode);

  //----------------------
  //PGPLOT graphics device
  //----------------------
  labelPtr = cl[15].getlabel();
  valuePtr = cl[15].getvalue();
  param.set_grdev(valuePtr);

  //-----------------------------------------------
  //number of panels (NX,NY) in the plotting device
  //-----------------------------------------------
  labelPtr = cl[16].getlabel();
  valuePtr = cl[16].getvalue();
  long nwinx,nwiny;
  if(!extract_2numbers(valuePtr,nwinx,nwiny))
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    return(false);
  }
  if((nwinx < 1) || (nwiny < 1))
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    return(false);
  }
  param.set_nwindows(nwinx,nwiny);

  //--------------------------------------------------------------
  //plottype (0: simple plot; 1: plot with additional information)
  //--------------------------------------------------------------
  labelPtr = cl[17].getlabel();
  valuePtr = cl[17].getvalue();
  for (const char *s=valuePtr; s[0] != '\0'; s++)
  {
    if (isdigit(s[0]) == 0) //error: no es un digito valido
    {
      if ((s[0] != '+') && (s[0] != '-')) //error: no es un signo
      {
        cout << "FATAL ERROR: <" << valuePtr
             << "> is an invalid argument for the keyword <" << labelPtr
             << ">" << endl;
        return(false);
      }
    }
  }
  const long plottype = atol(valuePtr);
  if((plottype < 0) || (plottype > 2))
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    return(false);
  }
  param.set_plottype(plottype);

  //----------------------------
  //nseed (0: use computer time)
  //----------------------------
  labelPtr = cl[18].getlabel();
  valuePtr = cl[18].getvalue();
  for (const char *s=valuePtr; s[0] != '\0'; s++)
  {
    if (isdigit(s[0]) == 0) //error: no es un digito valido
    {
      if ((s[0] != '+') && (s[0] != '-')) //error: no es un signo
      {
        cout << "FATAL ERROR: <" << valuePtr
             << "> is an invalid argument for the keyword <" << labelPtr
             << ">" << endl;
        return(false);
      }
    }
  }
  const long nseed = atol(valuePtr);
  if(nseed < 0)
  {
    cout << "FATAL ERROR: <" << valuePtr
         << "> is an invalid argument for the keyword <" << labelPtr
         << ">" << endl;
    return(false);
  }
  param.set_nseed(nseed);

  //-----------------
  //flux scale factor
  //-----------------
  labelPtr = cl[19].getlabel();
  valuePtr = cl[19].getvalue();
  for (const char *s=valuePtr; s[0] != '\0'; s++)
  {
    if (isdigit(s[0]) == 0) //no es un digito valido
    {
      if ( ( (s[0] == '.') || (s[0] == '+') 
          || (s[0] == 'E') || (s[0] == 'e')
          || (s[0] == 'D') || (s[0] == 'd')
          || (s[0] == '-') ) );
      else
      {
        cout << "FATAL ERROR: <" << valuePtr
             << "> is an invalid argument for the keyword <" << labelPtr
             << ">" << endl;
        return(false);
      }
    }
  }
  const double fscale = atof(valuePtr);
  param.set_fscale(fscale);

  //no admitimos ejecutar simultaneamente con biaserr y con linearerr
  if ((fabs(biaserr) != 0) && (linearerr !=0))
  {
    cout << ">>> biaserr  = " << biaserr << endl;
    cout << ">>> linearerr= " << linearerr << endl;
    cout << "FATAL ERROR: biaserr and linearerr cannot be "
            "employed simultaneously" << endl;
    return(false);
  }

  //retornamos con exito
  return(true);
}

//-----------------------------------------------------------------------------
bool extract_file_2long(const char *valuePtr,
                       char *filePtr, bool &logfile, long &long1, long &long2)
{
  //separamos el nombre del fichero de los indicadores de primer y ultimo
  //numero entero
  const char *comaPtr = strchr(valuePtr,',');  //primera coma
  if (comaPtr == NULL) //error: no hay coma al final del nombre de fichero
  {
    cout << "FATAL ERROR: expected \",\" character is not present" << endl;
    return(false);
  }
  //hemos encontrado la coma; extraemos el nombre del fichero
  const long fileSize=strlen(valuePtr)-strlen(comaPtr);
  strncpy(filePtr,valuePtr,fileSize);
  filePtr[fileSize] = '\0';
  //comprobamos que el fichero existe
  ifstream infile(filePtr, ios::in); //abrimos en modo solo lectura
  logfile=infile;
  const char *numbersPtr = comaPtr+1;
  if (!extract_2numbers(numbersPtr, long1, long2))
  {
    return(false);
  }
  return(true);
}

//-----------------------------------------------------------------------------
//Extrae dos numeros a partir de una cadena. La funcion es una plantilla
//preparada para trabajar con numeros "long" y numeros "double".
template < typename T >
bool extract_2numbers(const char *numbersPtr, T &num1, T &num2)
{
  //extraemos tipo de variable
  const char *dataType = typeid(T).name();
  //comprobamos que el tipo es uno de los tipos esperados
  long sample_long;
  const char *longType = typeid(sample_long).name();
  double sample_double;
  const char *doubleType = typeid(sample_double).name();
  if( (strcmp(dataType,longType) != 0) &&
      (strcmp(dataType,doubleType) != 0) )
  {
    cout << "FATAL ERROR: Unexpected data type <" << dataType 
         << "> in extract_2numbers" << endl;
    return(false);
  }

  const char *comaPtr = strchr(numbersPtr,',');
  if (comaPtr == NULL) //error: no hay coma separando numeros
  {
    cout << "FATAL ERROR: expected \",\" character is not present" << endl;
    return(false);
  }
  //tamano del primer numero
  const long num1Size=comaPtr-numbersPtr;
  if (num1Size == 0) //error: no existe el primer numero
  {
    cout << "FATAL ERROR: missing number" << endl;
    return(false);
  }
  char *const num1Ptr = new char[num1Size+1];
  strncpy(num1Ptr,numbersPtr,num1Size);
  num1Ptr[num1Size] = '\0';
  for (long i=1; i <= num1Size; i++) //chequeamos cada digito
    if (isdigit(num1Ptr[i-1]) == 0) //no es un digito valido
    {
      if ( (strcmp(dataType,doubleType) == 0) &&  //posible numero real
           (  (num1Ptr[i-1] == '.') || (num1Ptr[i-1] == '+') 
           || (num1Ptr[i-1] == 'E') || (num1Ptr[i-1] == 'e')
           || (num1Ptr[i-1] == 'D') || (num1Ptr[i-1] == 'd')
           || (num1Ptr[i-1] == '-') ) );
      else
      {
        delete [] num1Ptr;
        return(false);
      }
    }
  if(strcmp(dataType,longType) == 0)
  {
    num1 = atol(num1Ptr); //atol = 0 si la cadena es invalida
    if (num1 == 0) //posible error: numero invalido
    {
      for (const char *s=num1Ptr; s < num1Ptr+num1Size; s++)
      {
        if(s[0] != '0')
        {
          delete [] num1Ptr;
          cout << "FATAL ERROR: invalid number" << endl;
          return(false);
        }
      }
    }
  }
  else if(strcmp(dataType,doubleType) == 0)
  {
    char *restnum1;
    //nota: static_cast aqui evita un warning que no tiene sentido: 
    //assignment to `long' from `double'
    num1 = static_cast< T >(strtod(num1Ptr, &restnum1));
    if (strlen(restnum1) != 0) //el numero no es valido
    {
      delete [] num1Ptr;
      cout << "FATAL ERROR: invalid number" << endl;
      return(false);
    }
  }
  delete [] num1Ptr;
  //extraemos segundo numero
  const char *sPtr = comaPtr+1;
  while (*sPtr != '\0') //chequeamos cada digito
  {
    if (isdigit(sPtr[0]) == 0) //no es un digito valido
    {
      if ( (strcmp(dataType,doubleType) == 0) &&  //posible numero real
           (  (sPtr[0] == '.') || (sPtr[0] == '+') 
           || (sPtr[0] == 'E') || (sPtr[0] == 'e')
           || (sPtr[0] == 'D') || (sPtr[0] == 'd')
           || (sPtr[0] == '-') ) );
      else
      {
        cout << "FATAL ERROR: invalid number" << endl;
        return(false);
      }
    }
    sPtr++;
  }
  if(strcmp(dataType,longType) == 0)
  {
    num2 = atol(comaPtr+1); //atol = 0 si la cadena es invalida
    if (num2 == 0) //posible error: numero invalido
    {
      for (const char *s=comaPtr+1; s[0] != '\0'; s++)
      {
        if(s[0] != '0')
        {
          delete [] num1Ptr;
          cout << "FATAL ERROR: invalid number" << endl;
          return(false);
        }
      }
    }
  }
  else if(strcmp(dataType,doubleType) == 0)
  {
    char *restnum2;
    //nota: static_cast aqui evita un warning: "assignment to `long' from 
    //`double'", que no tiene sentido
    num2 = static_cast< T >(strtod(comaPtr+1, &restnum2));
    if (strlen(restnum2) != 0) //el numero no es valido
    {
      cout << "FATAL ERROR: invalid number" << endl;
      return(false);
    }
  }
  //retorno con exito
  return(true);
}
