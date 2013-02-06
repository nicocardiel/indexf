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

//Definicion de funciones miembro de la clase SciData, declarada en
//scidata.h
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include <cmath>
#include <vector>
#include "scidata.h"
#include "fitsio.h"
#include "longnam.h"
#include "indexparam.h"
#include "snregion.h"
#include "xydata.h"

using namespace std;

//-----------------------------------------------------------------------------
//prototipos de funciones auxiliares
bool issdouble(const string &, double &);
bool isslong(const string &, long &);
void sustrae_p0(vector<XYData> &, double &, double &);
void sustrae_p1(vector<XYData> &, double &, double &);

//-----------------------------------------------------------------------------
//constructor
SciData::SciData(IndexParam &param)
{
  //-----------------------------
  //inicializamos imagen de datos
  //-----------------------------
  const char *const file_data = param.get_if();
  long length = strlen(file_data);
  if ( length > 255 )
  {
    cout << "FATAL ERROR: strlen(filename_data) > 255. "
         << "(redim filename_data in scidata.cpp)" << endl;
    exit(1);
  }
  strncpy(filename_data,file_data,length);
  filename_data[length] = '\0';
  //abrimos fichero
  fitsfile *fptr;
  int nfound;
  int status=0;
  long naxes[2];
  if ( fits_open_file(&fptr, file_data, READONLY, &status) )
       printerror( status );
  // read the NAXIS1 and NAXIS2 keywords to get the image size
  if ( fits_read_keys_lng(fptr, "NAXIS", 1, 2, naxes, &nfound, &status) )
       printerror( status );
  if (nfound == 1)
  {
    naxis[0]=1;
    naxis[1]=naxes[0];
    naxis[2]=1;
  }
  else if (nfound == 2)
  {
    naxis[0]=2;
    naxis[1]=naxes[0];
    naxis[2]=naxes[1];
  }
  else
  {
    cout << "FATAL ERROR: this program cannot handle NAXIS > 2" << endl;
    if ( fits_close_file(fptr, &status) )
         printerror( status );
    exit(1);
  }
  //chequeamos los espectros a medir con los espectros disponibles
  const long ns1=param.get_ns1();
  const long ns2=param.get_ns2();
  if ( (ns1 == 0) && (ns2 == 0) )
  {
    param.set_ns1(1);
    param.set_ns2(naxis[2]);
  }
  else if ( ( ns1 < 1 ) || ( ns2 > naxis[2] ) )
  {
    cout << "FATAL ERROR: spectra numbers " << ns1 << " and " 
         << ns2 << " out of range." << endl;
    if ( fits_close_file(fptr, &status) )
         printerror( status );
    exit (1);
  }
  //leemos object
  char object_[256];
  if ( fits_read_key(fptr, TSTRING, "OBJECT", object_, NULL, &status) )
  {
    strncpy(object_,"[not found]",11);
  }
  strncpy(object_data,object_,strlen(object_));
  object_data[strlen(object_)]='\0';
  //leemos ctype1
  char ctype1_[9];
  if ( fits_read_key(fptr, TSTRING, "CTYPE1", ctype1_, NULL, &status) )
  {
    strncpy(ctype1_,"WAVE",4);
    ctype1_[4]='\0';
    if(param.get_verbose())
    {
      cout << "#WARNING: keyword CTYPE1 not found. Assuming CTYPE1=" 
           << ctype1_ << endl;
    } 
    status=0;
  }
  strncpy(ctype1,ctype1_,strlen(ctype1_));
  ctype1[strlen(ctype1_)]='\0';
  if ((strcmp(ctype1,"WAVE") != 0) && 
      (strcmp(ctype1,"WAVE-LOG") != 0) &&
      (strcmp(ctype1,"LINEAR") != 0))
  {
    cout << "CTYPE1=" << ctype1 << endl;
    cout << "ERROR: CTYPE1 != 'WAVE'." << endl;
    cout << "ERROR: CTYPE1 != 'WAVE-LOG'." << endl;
    cout << "ERROR: CTYPE1 != 'LINEAR'." << endl;
    cout << "This CTYPE1 value cannot be handled." << endl;
    exit(1);
  }

  //---------------------------------------------------------------------------
  //(09-Febrero-2006)
  //leemos, si esta presente, DC-FLAG (esto es para que podamos leer espectros
  //del Sloan Digital Sky Survey (DR4). Si DC-FLAG=1, significa que tenemos
  //una escala en longitud de onda logaritmica.
  long dcflag;
  if ( fits_read_key(fptr, TLONG, "DC-FLAG", &dcflag, NULL, &status) )
  {
    status=0;
  }
  else
  {
    if(dcflag == 1)
    {
      strncpy(ctype1,"WAVE-LOG",8);
      ctype1[8]='\0';
      cout << "#WARNING: keyword DC-FLAG found. Assuming CTYPE1=" 
           << ctype1 << endl;
    }
  }
  //---------------------------------------------------------------------------

  //leemos cunit1
  char cunit1_[9];
  if ( fits_read_key(fptr, TSTRING, "CUNIT1", cunit1_, NULL, &status) )
  {
    strncpy(cunit1_,"Angstrom",8);
    cunit1_[8]='\0';
    if(param.get_verbose())
    {
      cout << "#WARNING: keyword CUNIT1 not found. Assuming CUNIT1=" 
           << cunit1_ << endl;
    }
    status=0;
  }
  strncpy(cunit1,cunit1_,strlen(cunit1_));
  cunit1[strlen(cunit1_)]='\0';
  if (strcmp(cunit1,"Angstrom") != 0)
  {
    cout << "CUNIT1=" << cunit1 << endl;
    cout << "ERROR: CUNIT1 != 'Angstrom'. This CUNIT1 value cannot be handled." 
         << endl;
    exit(1);
  }
  //leemos crval1, cdelt1 (o cd1_1) y crpix1
  double crval1_,cdelt1_,crpix1_;
  if ( fits_read_key(fptr, TDOUBLE, "CRVAL1", &crval1_, NULL, &status) )
  {
    cout << "Error with keyword: CRVAL1" << endl;
    printerror( status );
  }
  crval1 = crval1_;
  if ( fits_read_key(fptr, TDOUBLE, "CDELT1", &cdelt1_, NULL, &status) )
  {
    cout << "#WARNING: keyword CDELT1 not found. Looking for CD1_1..." << endl;
    int status_bis=0;
    if ( fits_read_key(fptr, TDOUBLE, "CD1_1", &cdelt1_, NULL, &status_bis) )
    {
      cout << "Error with keyword: CD1_1" << endl;
      printerror( status_bis );
    }
  }
  cdelt1 = cdelt1_;
  if ( fits_read_key(fptr, TDOUBLE, "CRPIX1", &crpix1_, NULL, &status) )
  {
    crpix1_=1.0;
    if(param.get_verbose())
    {
      cout << "#WARNING: keyword CRPIX1 not found. Assuming CRPIX1=" 
           << crpix1_ << endl;
    }
    status=0;
  }
  crpix1 = crpix1_;
  if ( crpix1 != 1.0)
  {
    cout << "CRPIX1=" << crpix1 << endl;
    cout << "ERROR: CRPIX1 != 1.0. This CRPIX1 value cannot be handled." 
         << endl;
    exit(1);
  }
  //leemos imagen de datos
  long fpixel[2] = {1,1};
  long nelements = naxis[1] * naxis[2];
  data = new double [nelements];
  int anynull;
  if ( fits_read_pix(fptr, TDOUBLE, fpixel, nelements, 0, 
       data, &anynull, &status) )
       printerror( status );
  //cerramos fichero
  if ( fits_close_file(fptr, &status) )
       printerror( status );
  if(anynull !=0)
  {
    cout << "FATAL ERROR: the data spectra contain NULL values." << endl;
    exit(1);
  }
  //en caso necesario, aplicamos cambio de escala en la imagen
  double fscale = param.get_fscale();
  if (fscale != 1.0)
  {
    for (long i=1; i<=nelements; i++)
    {
      data[i-1]/=fscale;
    }
  }

  //-------------------------------
  //inicializamos imagen de errores
  //-------------------------------
  const char *const file_error = param.get_ief();
  length = strlen(file_error);
  if ( length > 255 )
  {
    cout << "FATAL ERROR: strlen(file_error) > 255. "
         << "(redim filename_error in scidata.cpp)" << endl;
    exit(1);
  }
  strncpy(filename_error,file_error,length);
  filename_error[length] = '\0';
  //si el fichero de errores no es "undef" lo abrimos
  if (strcmp(filename_error,"undef") != 0)
  {
    if ( fits_open_file(&fptr, file_error, READONLY, &status) )
         printerror( status );
    //comprobamos que NAXISn en imagen de errores coincide con los valores
    //en la imagen de datos
    int nfound_error;
    if ( fits_read_keys_lng(fptr, "NAXIS", 1, 2, naxes, &nfound_error, 
         &status) )
         printerror( status );
    if (nfound != nfound_error)
    {
      cout << "FATAL ERROR: NAXISn in data and error frames are different" 
           << endl;
      if ( fits_close_file(fptr, &status) )
           printerror( status );
      exit(1);
    }
    for ( long i = 1; i <= nfound; i++ )
    {
      if ( naxes[i-1] != naxis[i] )
      {
        cout << "FATAL ERROR: NAXISn in data and error frames are different" 
             << endl;
        if ( fits_close_file(fptr, &status) )
             printerror( status );
        exit(1);
      }
    }
    //leemos object
    if ( fits_read_key(fptr, TSTRING, "OBJECT", object_, NULL, &status) )
    {
      strncpy(object_,"[not found]",11);
    }
    strncpy(object_error,object_,strlen(object_));
    object_error[strlen(object_)]='\0';
    //leemos ctype1
    if ( fits_read_key(fptr, TSTRING, "CTYPE1", ctype1_, NULL, &status) )
    {
      strncpy(ctype1_,"WAVE",4);
      ctype1_[4]='\0';
      if(param.get_verbose())
      {
        cout << "#WARNING: keyword CTYPE1 not found. Assuming CTYPE1=" 
             << ctype1_ << endl;
      }
      status=0;
    }
    //-------------------------------------------------------------------------
    //(09-Febrero-2006)
    //leemos, si esta presente, DC-FLAG (esto es para que podamos leer espectros
    //del Sloan Digital Sky Survey (DR4). Si DC-FLAG=1, significa que tenemos
    //una escala en longitud de onda logaritmica.
    long dcflag_;
    if ( fits_read_key(fptr, TLONG, "DC-FLAG", &dcflag_, NULL, &status) )
    {
      status=0;
    }
    else
    {
      if(dcflag_ != dcflag)
      {
        cout << "FATAL ERROR: DC-FLAG in data and error frames are different"
             << endl;
        exit(1);
      }
      if(dcflag_ == 1)
      {
        strncpy(ctype1_,"WAVE-LOG",8);
        ctype1_[8]='\0';
        cout << "#WARNING: keyword DC-FLAG found. Assuming CTYPE1=" 
             << ctype1_ << endl;
      }
    }
    //-------------------------------------------------------------------------
    if ( strcmp(ctype1,ctype1_) != 0)
    {
      cout << "CTYPE1=" << ctype1 << " (in data frame)" << endl;
      cout << "CTYPE1=" << ctype1_ << " (in error frame)" << endl;
      cout << "FATAL ERROR: CTYPE1 in data and error frames are different" 
           << endl;
      if ( fits_close_file(fptr, &status) )
           printerror( status );
      exit(1);
    }
    //leemos cunit1
    if ( fits_read_key(fptr, TSTRING, "CUNIT1", cunit1_, NULL, &status) )
    {
      strncpy(cunit1_,"Angstrom",8);
      cunit1_[8]='\0';
      if(param.get_verbose())
      {
        cout << "#WARNING: keyword CUNIT1 not found. Assuming CUNIT1=" 
             << cunit1_ << endl;
      }
      status=0;
    }
    if ( strcmp(cunit1,cunit1_) != 0)
    {
      cout << "CUNIT1=" << cunit1 << " (in data frame)" << endl;
      cout << "CUNIT1=" << cunit1_ << " (in error frame)" << endl;
      cout << "FATAL ERROR: CUNIT1 in data and error frames are different" 
           << endl;
      if ( fits_close_file(fptr, &status) )
           printerror( status );
      exit(1);
    }
    //leemos crval1, cdelt1 y crpix1
    if ( fits_read_key(fptr, TDOUBLE, "CRVAL1", &crval1_, NULL, &status) )
    {
      cout << "Error with keyword: CRVAL1" << endl;
      printerror( status );
    }
    if ( crval1_ != crval1 )
    {
      cout << "FATAL ERROR: CRVAL1 in data and error frames are different" 
           << endl;
      if ( fits_close_file(fptr, &status) )
           printerror( status );
      exit(1);
    }
    if ( fits_read_key(fptr, TDOUBLE, "CDELT1", &cdelt1_, NULL, &status) )
    {
      cout << "Error with keyword: CDELT1" << endl;
      printerror( status );
    }
    if ( cdelt1_ != cdelt1 )
    {
      cout << "FATAL ERROR: CDELT1 in data and error frames are different" 
           << endl;
      if ( fits_close_file(fptr, &status) )
           printerror( status );
      exit(1);
    }
    if ( fits_read_key(fptr, TDOUBLE, "CRPIX1", &crpix1_, NULL, &status) )
    {
      if ( crpix1_ != 1.0 )
      {
        cout << "Error with keyword: CRPIX1" << endl;
        printerror( status );
      }
      status = 0;
    }
    if ( crpix1_ != crpix1 )
    {
      cout << "FATAL ERROR: CRPIX1 in data and error frames are different" 
           << endl;
      if ( fits_close_file(fptr, &status) )
           printerror( status );
      exit(1);
    }
    //leemos imagen de errores
    error = new double [nelements];
    if ( fits_read_pix(fptr, TDOUBLE, fpixel, nelements, 0, 
         error, &anynull, &status) )
         printerror( status );
    //cerramos fichero
    if ( fits_close_file(fptr, &status) )
         printerror( status );
    if(anynull !=0)
    {
      cout << "FATAL ERROR: the error spectra contain NULL values." << endl;
      exit(1);
    }
    //en caso necesario, aplicamos cambio de escala en la imagen
    if (fscale != 1.0) //ya hemos definido fscale mas arriba
    {
      for (long i=1; i<=nelements; i++)
      {
        error[i-1]/=fscale;
      }
    }
  }
 
  //-------------------------------------------------------------------
  //inicializamos imagen de errores a partir de la estimacion de la S/N
  //-------------------------------------------------------------------
  //Como para realizar esta estimacion es necesario medir el r.m.s. de los
  //espectros en un numero arbitrario de bandas, las cuales estan definidas por
  //longitudes de onda "rest-frame", necesitamos conocer las velocidades
  //radiales de cada espectro. Como llegados a este punto todavia no las
  //conocemos, retrasamos este calculo hasta el final del constructor. Esto, de
  //paso, nos facilita las cosas porque si los espectros estan en escala
  //logaritmica, en el siguiente paso ya los transformamos a escala lineal.

  //-------------------------------------------------------------------------
  //si los espectros estan en escala logaritmica, los pasamos a escala lineal
  //(Nota: asumimos logaritmos decimales)
  //IMPORTANTE: el reescalado preserva el numero de cuentas/pixel, es decir,
  //que al tomar una dispersion promedio para la escala lineal en longitud de
  //onda, el flujo en la parte azul del espectro aumenta y en la parte roja
  //disminuye. Si el flujo de los espectros en escala logaritmica esta dado por
  //Angstrom (y no por pixel) lo anterior no es lo que uno quiere. En este
  //ultimo caso, uno deberia interpolar directamente (sin conservar flujo). En
  //cualquier caso, el efecto en los indices debe ser pequeño si uno no lo hace
  //bien.
  //-------------------------------------------------------------------------
  if ( strcmp(ctype1,"WAVE-LOG") == 0)
  {
    //calculamos parametros de la transformacion lineal conservando el mismo
    //numero de pixels 
    double stwv2,disp2;
    double wlmin,wlmax;
    double fnaxis1=static_cast<double>(naxis[1]);
    wlmin=pow(10.0,crval1+cdelt1*(0.5-crpix1));
    wlmax=pow(10.0,crval1+cdelt1*(fnaxis1+0.5-crpix1));
    disp2=(wlmax-wlmin)/fnaxis1;
    stwv2=wlmin+0.5*disp2;
    //pasamos a escala lineal, introduciendo temporalmente cada espectro
    //en la variable temporal tempsp (y errores en tempesp)
    long k;
    double *tempsp = new double [naxis[1]];
    double *tempesp = new double [naxis[1]];
    for (long ns = 1; ns <= naxis[2]; ns++)
    {
      for (long nc = 1; nc <= naxis[1]; nc++)
      {
        double w=stwv2+static_cast<double>(nc-1)*disp2;       //l.d.o. pixel nc
        double w1=w-0.5*disp2;                   //borde izquierdo del pixel nc
        double w2=w1+disp2;                        //borde derecho del pixel nc
        double fj1=(log10(w1)-crval1)/cdelt1+crpix1;
        double fj2=(log10(w2)-crval1)/cdelt1+crpix1;
        long j1=static_cast<long>(fj1);
        long j2=static_cast<long>(fj2);
        tempsp[nc-1]=0.0;
        if (strcmp(filename_error,"undef") != 0)
          tempesp[nc-1]=0.0;
        if(j1 == j2)
        {
          if( (j1 >= 1) && (j1 <= naxis[1]) )
          {
            k=(ns-1)*naxis[1]+j1-1;
            tempsp[nc-1]=tempsp[nc-1]+data[k]*(fj2-fj1);
            if (strcmp(filename_error,"undef") != 0)
              tempesp[nc-1]=tempesp[nc-1]+error[k]*(fj2-fj1);
          }
        }
        else
        {
          if( (j1 >= 1) && (j1 <= naxis[1]) )
          {
            k=(ns-1)*naxis[1]+j1-1;
            tempsp[nc-1]=tempsp[nc-1]
                         +data[k]*(static_cast<double>(j1)+0.5-fj1);
            if (strcmp(filename_error,"undef") != 0)
              tempesp[nc-1]=tempesp[nc-1]
                           +error[k]*(static_cast<double>(j1)+0.5-fj1);
          }
          if( (j2 >= 1) && (j2 <= naxis[1]) )
          {
            k=(ns-1)*naxis[1]+j2-1;
            tempsp[nc-1]=tempsp[nc-1]
                         +data[k]*(fj2-(static_cast<double>(j2)-0.5));
            if (strcmp(filename_error,"undef") != 0)
              tempesp[nc-1]=tempesp[nc-1]
                           +error[k]*(fj2-(static_cast<double>(j2)-0.5));
          }
          if( (j2-j1) > 1 )
          {
            for (long j=j1+1; j<=j2-1; j++)
            {
              if( (j >= 1) && (j <= naxis[1]) )
              {
                k=(ns-1)*naxis[1]+j-1;
                tempsp[nc-1]=tempsp[nc-1]+data[k];
                if (strcmp(filename_error,"undef") != 0)
                  tempesp[nc-1]=tempesp[nc-1]+error[k];
              }
            }
          }
        }
      }
      //actualizamos el espectro con el nuevo muestreo lineal
      for (long nc = 1; nc <= naxis[1]; nc++)
      {
        k=(ns-1)*naxis[1]+nc-1;
        data[k]=tempsp[nc-1];
      }
      if (strcmp(filename_error,"undef") != 0)
      {
        for (long nc = 1; nc <= naxis[1]; nc++)
        {
          k=(ns-1)*naxis[1]+nc-1;
          error[k]=tempesp[nc-1];
        }
      }
    }
    delete [] tempsp;
    delete [] tempesp;
    //mostramos el efecto del nuevo cambio de escala
    crval1=stwv2;
    cdelt1=disp2;
    crpix1=1.0;
    cout << "#WARNING: the spectra have been transformed to a lineal wavelength"
         << "\n#         calibration (assuming base-10 logarithm) with:" 
         << "\n#         > CRVAL1=" << crval1 
         << "\n#         > CDELT1=" << cdelt1
         << "\n#         > CRPIX1=" << crpix1
         << "\n#         and preserving the flux/pixel!" << endl;
  }

  //--------------------------------------------
  //inicializamos velocidades radiales y errores
  //--------------------------------------------
  rvel = new double [naxis[2]];
  rvelerr = new double [naxis[2]];
  //si existe un fichero externo, lo abrimos
  const char *const file_rvel = param.get_rvfile();
  length = strlen(file_rvel);
  if ( length > 255 )
  {
    cout << "FATAL ERROR: strlen(filename_rvel) > 255. "
         << "(redim filename_rvel in scidata.h)" << endl;
    exit(1);
  }
  strncpy(filename_rvel,file_rvel,length);
  filename_rvel[length] = '\0';
  //si el fichero de errores no es "undef" lo abrimos
  if (strcmp(filename_rvel,"undef") != 0)
  {
    ifstream infile (filename_rvel,ios::in);
    if ( !infile )
    {
      cout << "FATAL ERROR: while opening the file " << filename_rvel << endl;
      exit(1);
    }
    //leemos el fichero con velocidades radiales
    string s;
    long nlines=0;
    long rvc = param.get_rvc();
    long rvce = param.get_rvce();
    long ncolmax = ( rvc > rvce ? rvc : rvce );
    while (getline(infile,s))
    {
      if(s[0] != '#') //no es un comentario
      {
        nlines++; //contamos el numero de lineas en el fichero
        //comprobamos que el fichero no contiene demasiadas lineas
        if (nlines > naxis[2])
        {
          cout << "FATAL ERROR: number of lines in file " << filename_rvel
               << " = " << nlines << " is larger than the expected value "
               "(naxis[2]=" << naxis[2] << ")" << endl;
          exit(1);
        }
        //si no usamos errores en velocidad radial, inicializamos a cero
        if (rvce == 0)
        {
          rvelerr[nlines-1] = 0;
        }
        //convertimos string en cadena char * (tipo C)
        long lsize=s.length();
        char *linePtr = new char[lsize+1];
        char *remainderPtr; //para la funcion strtod
        s.copy(linePtr,lsize,0);
        linePtr[lsize] = '\0';
        char *tokenPtr;
        for (long ncol=1; ncol <= ncolmax; ncol++)
        {
          if (ncol == 1)
            tokenPtr = strtok(linePtr," ");
          else
            tokenPtr = strtok(NULL," ");
          if (tokenPtr == NULL)
          {
            cout << "FATAL ERROR: missing number in file "
                 << filename_rvel << ", while reading line number "
                 << nlines << " and column number " << ncol << endl;
            exit(1);
          }
          if (ncol == rvc)
          {
            rvel[nlines-1] = strtod(tokenPtr,&remainderPtr);
          }
          else if (ncol == rvce)
          {
            rvelerr[nlines-1] = strtod(tokenPtr,&remainderPtr);
          }
        }
        delete [] linePtr;
      }
      else
      {
        //ignoramos porque es un comentario
      }
    }
    //si el fichero contiene un numero de lineas diferente al esperado,
    //abortamos la ejecucion del programa
    if (nlines != naxis[2])
    {
      cout << "FATAL ERROR: number of lines in file " << filename_rvel
           << " = " << nlines << " is different to the expected value "
           "(naxis[2]=" << naxis[2] << ")" << endl;
      exit(1);
    }
  }
  else //no existe fichero de velocidad radial
  {
    //usamos el mismo valor de velocidad radial y error para todos los
    //espectros
    for ( long ns = 1; ns <= naxis[2]; ns++ )
    {
      rvel[ns-1] = param.get_rv();
      rvelerr[ns-1] = param.get_rve();
    }
  }

  //------------------------------------------
  //inicializamos etiquetas para los espectros
  //------------------------------------------
  labelsp = new char* [naxis[2]];
  //si existe un fichero externo, lo abrimos
  const char *const file_label = param.get_ilabfile();
  length = strlen(file_label);
  if ( length > 255 )
  {
    cout << "FATAL ERROR: strlen(filename_label) > 255. "
         << "(redim filename_label in scidata.h)" << endl;
    exit(1);
  }
  strncpy(filename_label,file_label,length);
  filename_label[length] = '\0';
  //si el fichero de etiquetas no es "undef" lo abrimos
  if (strcmp(filename_label,"undef") != 0)
  {
    ifstream infile (filename_label,ios::in);
    if ( !infile )
    {
      cout << "FATAL ERROR: while opening the file " << filename_label << endl;
      exit(1);
    }
    //leemos el fichero con etiquetas
    long nlines=0;
    const long nchar1 = param.get_nchar1();
    const long nchar2 = param.get_nchar2();
    const long lsizelabel = nchar2-nchar1+1; //=1 para nchar1=nchar2=0
    string s;
    while (getline(infile,s))
    {
      long lsize=s.length();
      if(lsize > 0)
      {
        if(s[0] != '#')
        {
          if(lsize >= nchar2)
          {
            if(nchar2 == 0) //leemos toda la cadena
            {
              labelsp[nlines] = new char[lsize+1];
              s.copy(labelsp[nlines],lsize,0);
              (labelsp[nlines])[lsize] = '\0';
            }
            else //extraemos subcadena completa
            {
              labelsp[nlines] = new char[lsizelabel+1];
              s.copy(labelsp[nlines],lsizelabel,nchar1-1);
              (labelsp[nlines])[lsizelabel] = '\0';
            }
          }
          else //la linea contiene menos caracteres de lo esperado
          {
            if(lsize >= nchar1) //podemos leer una etiqueta parcial
            {
              const long lsizelabel_eff = lsize-nchar1+1;
              labelsp[nlines] = new char[lsizelabel+1];
              s.copy(labelsp[nlines],lsizelabel_eff,nchar1-1);
              for (long l=lsizelabel_eff+1; l<=lsizelabel; l++)
              {
                (labelsp[nlines])[l-1]=' ';
              }
              (labelsp[nlines])[lsizelabel] = '\0';
            }
            else //no hay nada que leer
            {
              labelsp[nlines] = new char[lsizelabel+1];
              for (long l=1; l<=lsizelabel;l++)
              {
                (labelsp[nlines])[l-1] = ' ';
              }
              (labelsp[nlines])[lsizelabel] = '\0';
            }
          }
          nlines++;
          //comprobamos que el fichero no contiene demasiadas lineas
          if (nlines > naxis[2])
          {
            cout << "FATAL ERROR: number of lines in file " << filename_label
                 << " = " << nlines << " is larger than the expected value "
                 "(naxis[2]=" << naxis[2] << ")" << endl;
            exit(1);
          }
        }
        else
        {
          //es un comentario y se ignora
        }
      }
      else
      {
        labelsp[nlines] = new char[lsizelabel+1];
        for (long l=1; l<=lsizelabel;l++)
        {
          (labelsp[nlines])[l-1] = ' ';
        }
        (labelsp[nlines])[lsizelabel] = '\0';
        nlines++;
      }
    }
    //si el fichero contiene un numero de lineas diferente al esperado,
    //abortamos la ejecucion del programa
    if (nlines != naxis[2])
    {
      cout << "FATAL ERROR: number of lines in file " << filename_label
           << " = " << nlines << " is different to the expected value "
           "(naxis[2]=" << naxis[2] << ")" << endl;
      exit(1);
    }
  }
  else //no existe fichero de etiquetas
  {
    for ( long ns = 1; ns <= naxis[2]; ns++ )
    {
      labelsp[ns-1] = new char[1];
      (labelsp[ns-1])[0] = '\0';
    }
  }

  //-------------------------------------------------------------------
  //inicializamos imagen de errores a partir de la estimacion de la S/N
  //-------------------------------------------------------------------
  if (strcmp(filename_error,"undef") == 0) //no hay imagen real de errores
  {
    const char *const file_snguess = param.get_snf();
    length = strlen(file_snguess);
    if ( length > 255 )
    {
      cout << "FATAL ERROR: strlen(file_snguess) > 255. "
           << "(redim filename_error in scidata.cpp)" << endl;
      exit(1);
    }
    strncpy(filename_error,file_snguess,length);
    filename_error[length] = '\0';
    //si el fichero no es "undef", continuamos
    if (strcmp(filename_error,"undef") != 0)
    {
      //determinamos cuales son los limites en l.d.o. de los espectros para
      //poder comprobar que las regiones que vamos a leer estan definidas
      //dentro del recorrido en longitud de onda de los espectros (si la escala
      //en longitud de onda era logaritmica, mas arriba ya se ha transformado
      //esta a una escala lineal)
      double wlmin,wlmax;
      double fnaxis1=static_cast<double>(naxis[1]);
      wlmin=crval1+cdelt1*(0.5-crpix1);
      wlmax=crval1+cdelt1*(fnaxis1+0.5-crpix1);
      //abrimos el archivo con las regiones mediante el constructor de ifstream
      ifstream inpfile(filename_error,ios::in);
      if(!inpfile)
      {
        cout << "FATAL ERROR: while opening the file " 
             << filename_error << endl;
        exit(1);
      }
      vector <SNRegion> snregion;
      //leemos el fichero
      string s;
      while(getline(inpfile,s)) //vamos leyendo cada linea
      {
        if(s[0] != '#') //no es un comentario
        {
          //leemos lineas y comprobamos que tenemos tres numeros
          istringstream inputString(s);
          string swave1, swave2, spoldeg;
          inputString >> swave1 >> swave2 >> spoldeg;
          long lsize1=swave1.length();
          long lsize2=swave2.length();
          long lsize3=spoldeg.length();
          if (lsize1*lsize2*lsize3 == 0)
          {
            cout << "FATAL ERROR: missing number(s) in file "
                 << filename_error << endl;
            cout << "-> Check that the file does not contain an empty end line"
                 << endl;
            exit(1);
          }
          //leemos el primer limite en l.d.o.
          double wave1;
          if(!issdouble(swave1,wave1))
          {
            cout << "FATAL ERROR: reading 1st float number in file "
                 << filename_error << endl;
            exit(1);
          }
          //leemos el segundo limite en l.d.o.
          double wave2;
          if(!issdouble(swave2,wave2))
          {
            cout << "FATAL ERROR: reading 2nd float number in file "
                 << filename_error << endl;
            exit(1);
          }
          if(wave2 < wave1)
          {
            cout << "FATAL ERROR: reading wavelengths in file "
                 << filename_error << endl;
            cout << "==> w1,w2: " << wave1 << "," << wave2 << endl;
            cout << "==> These numbers must verify w1 <= w2" << endl;
            exit(1);
          }
          if( (wave1 < wlmin) || (wave2 > wlmax) )
          {
            cout << "FATAL ERROR: reading wavelengths in file "
                 << filename_error << endl;
            cout << "==> w1,w2: " << wave1 << "," << wave2 << endl;
            cout << "==> These numbers are outside the expected range:"
                 << endl;
            cout << "==> wlmin,wlmax: " << wlmin << "," << wlmax << endl;
            exit(1);
          }
          //leemos el grado del polinomio a sustraer
          long poldeg;
          if(!isslong(spoldeg,poldeg))
          {
            cout << "FATAL ERROR: reading long number in file "
                 << filename_error << endl;
            exit(1);
          }
          if( (poldeg != 0) && (poldeg != 1) )
          {
            cout << "FATAL ERROR: reading polynomial degree in file "
                 << filename_error << endl;
            cout << "==> poldeg: " << poldeg << endl;
            cout << "==> This number must be 0 or 1" << endl;
            exit(1);
          }
          //almacenamos los nuevos valores
          SNRegion dumregion;
          dumregion.setwave1(wave1);
          dumregion.setwave2(wave2);
          dumregion.setpoldeg(poldeg);
          snregion.push_back(dumregion);
        }
      }
      //comprobamos si se ha definido al menos una region
      const long nSNregions=snregion.size();
      if (nSNregions <= 0)
      {
        cout << "FATAL ERROR: number of regions to estimate S/N is < 1"
             << endl;
        exit(1);
      }
      //comprobamos que las regiones no solapan
      if (nSNregions > 1)
      {
        for (long nr1=1; nr1<=nSNregions-1; nr1++)
        {
          double w1r1=snregion[nr1-1].getwave1();
          double w2r1=snregion[nr1-1].getwave2();
          for (long nr2=nr1+1; nr2<=nSNregions; nr2++)
          {
            double w1r2=snregion[nr2-1].getwave1();
            double w2r2=snregion[nr2-1].getwave2();
            if ( (w1r1 >= w1r2) && (w1r1 <= w2r2) || //w1r1 dentro de r2
                 (w2r1 >= w1r2) && (w2r1 <= w2r2) || //w2r1 dentro de r2
                 (w1r2 >= w1r1) && (w1r2 <= w2r1) || //w1r2 dentro de r1
                 (w2r2 >= w1r1) && (w2r2 <= w2r1) )  //w2r2 dentro de r1
            {
              cout << "FATAL ERROR: reading the file " 
                   << filename_error << endl;
              cout << "==> there are intersections between different regions"
                   << endl;
              cout << "==> " << w1r1 << "," << w2r1 << " and "
                   << w1r2 << "," << w2r2 << endl;
              exit(1);
            }
          }
        }
      }
      //generamos una imagen simulada de errores a partir de la S/N promedio en
      //las regiones indicadas
      error = new double [nelements]; //imagen 2D simulada de errores
      for (long ns=param.get_ns1(); ns<=param.get_ns2(); ns++)
      {
        //para cada espectro calculamos la S/N promedio en las distintas
        //regiones
        long nSNregions_effective=0;
        for (long nreg=1; nreg<=nSNregions; nreg++)
        {
          double w1=snregion[nreg-1].getwave1(); //rest-frame
          double w2=snregion[nreg-1].getwave2(); //rest-frame
          const double c=2.9979246E+5; //velocidad de la luz (km/s)
          double rcvel=rvel[ns-1]/c; //aproximacion clasica, v/c
          double rcvel1=(1.0+rcvel)/sqrt(1.0-rcvel*rcvel); //corr.relativista
          w1*=rcvel1; //observed wavelength
          w2*=rcvel1; //observed wavelength
          double fj1=(w1-crval1)/cdelt1+crpix1;
          double fj2=(w2-crval1)/cdelt1+crpix1;
          double fnaxis1=static_cast<double>(naxis[1]);
          if( (fj1 < 1.0) || (fj2 > fnaxis1) )
          {
            double wvalid1, wvalid2;
            wvalid1=crval1+cdelt1*(1.0-crpix1); //centro del primer pixel
            wvalid2=crval1+cdelt1*(fnaxis1-crpix1); //centro del ultimo pixel
            cout << "FATAL ERROR: walength region to estimate S/N "
                 << "is outside valid range" << endl;
            cout << "w1,w2......: " << w1 << "," << w2 << endl;
            cout << "Valid range: " << wvalid1 << "," << wvalid2 << endl;
            exit(1);
          }
          //generamos un vector que contenga la region a ajustar con el
          //polinomio (para la escala en X utilizamos el numero de pixel)
          long j1=static_cast<long>(fj1+0.5);
          long j2=static_cast<long>(fj2+0.5);
          vector<XYData> fit_region;
          long k;
          for (long j=j1; j<=j2; j++)
          {
            k=(ns-1)*naxis[1]+j-1;
            XYData xydum;
            xydum.setx(static_cast<double>(j));
            xydum.sety(data[k]);
            fit_region.push_back(xydum);
          }
          if(fit_region.size() >= 2) //para poder medir el r.m.s.
          {
            double pdeg=snregion[nreg-1].getpoldeg();
            double mean_signal, mean_noise;
            if(pdeg == 0) //polynomial degree=0
            {
              sustrae_p0(fit_region,mean_signal,mean_noise);
            }
            else //polynomial degree=1
            {
              sustrae_p1(fit_region,mean_signal,mean_noise);
            }
            double temp_sn=mean_signal/mean_noise;
            snregion[nreg-1].setsn(temp_sn);
            snregion[nreg-1].setnpixels(fit_region.size());
            nSNregions_effective++;
          }
        }
        if(nSNregions_effective == 0)
        {
          cout << "FATAL ERROR: useful number of SN regions = 0" << endl;
          exit(1);
        }
        //calculamos el numero total de pixels en todas las bandas utiles
        double ntotpixels=0;
        for (long nreg=1; nreg<=nSNregions; nreg++)
        {
          if(snregion[nreg-1].getnpixels() >= 2)
          {
            ntotpixels+=static_cast<double>(snregion[nreg-1].getnpixels());
          }
        }
        //calculamos la S/N promedio de todas las regiones utiles, pesando con
        //el numero de pixels
        double mean_SN=0.0;
        for (long nreg=1; nreg<=nSNregions; nreg++)
        {
          if(snregion[nreg-1].getnpixels() >= 2)
          {
            double weight=snregion[nreg-1].getnpixels()/ntotpixels;
            mean_SN+=snregion[nreg-1].getsn()*weight;
          }
        }
        //generamos el espectro simulado de errores con la S/N promedio
        long k;
        for (long j=1; j<=naxis[1]; j++)
        {
          k=(ns-1)*naxis[1]+j-1;
          error[k]=data[k]/mean_SN;
        }
      }
    }
  }

  //final del constructor
}

//-----------------------------------------------------------------------------
//destructor
SciData::~SciData()
{
  delete [] data;
  if (strcmp(filename_error,"undef") != 0) delete [] error;
  delete [] rvel;
  delete [] rvelerr;
  delete [] *labelsp;
}

//-----------------------------------------------------------------------------
char *SciData::getfilename_data() { return filename_data; }

//-----------------------------------------------------------------------------
char *SciData::getfilename_error() { return filename_error; }

//-----------------------------------------------------------------------------
char *SciData::getfilename_rvel() { return filename_rvel; }

//-----------------------------------------------------------------------------
char *SciData::getobject_data() { return object_data; }

//-----------------------------------------------------------------------------
char *SciData::getobject_error() { return object_error; }

//-----------------------------------------------------------------------------
char *SciData::getctype1() { return ctype1; }

//-----------------------------------------------------------------------------
long SciData::getnaxis1() { return naxis[1]; }

//-----------------------------------------------------------------------------
long SciData::getnaxis2() { return naxis[2]; }

//-----------------------------------------------------------------------------
double SciData::getcrval1() const { return crval1; }

//-----------------------------------------------------------------------------
double SciData::getcdelt1() const { return cdelt1; }

//-----------------------------------------------------------------------------
double SciData::getcrpix1() const { return crpix1; }

//-----------------------------------------------------------------------------
double *SciData::getdata() const { return data; }

//-----------------------------------------------------------------------------
double *SciData::geterror() const { return error; }

//-----------------------------------------------------------------------------
double *SciData::getrvel() const { return rvel; }

//-----------------------------------------------------------------------------
double *SciData::getrvelerr() const { return rvelerr; }

//-----------------------------------------------------------------------------
char **SciData::getlabelsp() const { return labelsp; }

//-----------------------------------------------------------------------------
void SciData::printerror( long status)
{
    /*****************************************************/
    /* Print out cfitsio error messages and exit program */
    /*****************************************************/

    if (status)
    {
       fits_report_error(stderr, status); /* print error report */

       exit( status );    /* terminate the program, returning error status */
    }
    return;
}
