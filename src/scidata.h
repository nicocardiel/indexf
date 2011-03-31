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

//Declaración de la clase SciData
//Las funciones miembro se definen en scidata.cpp

#ifndef SCIDATA_H
#define SCIDATA_H

#include "indexparam.h"

class SciData{
  public:
    SciData(IndexParam &); //constructor
    ~SciData(); //destructor (necesario para liberar memoria)
    char *getfilename_data();
    char *getfilename_error();
    char *getfilename_rvel();
    char *getobject_data();
    char *getobject_error();
    char *getctype1();
    char *getcunit1();
    long getnaxis1();
    long getnaxis2();
    double getcrval1() const;
    double getcdelt1() const;
    double getcrpix1() const;
    double *getdata() const;
    double *geterror() const;
    double *getrvel() const;
    double *getrvelerr() const;
    char **getlabelsp() const;
  private:
    char filename_data[256];
    char filename_error[256];
    char object_data[256];
    char object_error[256];
    char filename_rvel[256];
    char filename_label[256];
    char ctype1[9];
    char cunit1[9];
    long naxis[3];
    double crval1;
    double cdelt1;
    double crpix1;
    double *data;
    double *error;
    double *rvel;
    double *rvelerr;
    char **labelsp;
    void printerror(long); //función auxiliar
};

#endif
