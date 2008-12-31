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
#include <string.h>
#include "indexparam.h"
#include "indexdef.h"
#include "scidata.h"

using namespace std;

void c123(const double &, const double &,
          const double &, const double &,
          const double &, const double &,
          double &, double &, double &);

void verbose(IndexParam param, IndexDef myindex, SciData *imagePtr)
{
  //tipo de indice a medir
  const long type = myindex.gettype();
  //definimos un separador de 79 caracteres
  char separador[80];
  separador[0]='#';
  for (int i=1; i<=78; i++)
    separador[i]='-';
  separador[79]='\0';

  //mostramos informacion sobre el indice a medir
  cout << separador << endl;
  cout << "#Index.........................: " << param.get_index() << endl;
  if (type == 1) //indices moleculares
  {
    cout << "#                        <code>: " << type
         << " (molecular index)" << endl;
    const long nbands = myindex.getnbands();
    for (int i=1; i <= nbands; i++)
    {
      if (i == 1)
        cout << "#               <blue bandpass>: ";
      else if (i == 2)
        cout << "#            <central bandpass>: ";
      else
        cout << "#                <red bandpass>: ";
      cout << myindex.getldo1(i-1) << "  " 
           << myindex.getldo2(i-1) << endl;
    }
    double c1, c2, c3;
    c123(myindex.getldo1(0),myindex.getldo2(0),
         myindex.getldo1(1),myindex.getldo2(1),
         myindex.getldo1(2),myindex.getldo2(2),
         c1,c2,c3);
    cout << "#c1 (error estimation from S/N): " << c1 << endl;
    cout << "#c2 (error estimation from S/N): " << c2 << endl;
    cout << "#c3 (error estimation from S/N): " << c3 << endl;
  }
  else if (type == 2) //indices atomicos
  {
    cout << "#                        <code>: " << type
         << " (atomic index)";
    if(param.get_logindex())
    {
      cout << " in logarithmic units";
    }
    cout << endl;
    const long nbands = myindex.getnbands();
    for (int i=1; i <= nbands; i++)
    {
      if (i == 1)
        cout << "#               <blue bandpass>: ";
      else if (i == 2)
        cout << "#            <central bandpass>: ";
      else
        cout << "#                <red bandpass>: ";
      cout << myindex.getldo1(i-1) << "  " 
           << myindex.getldo2(i-1) << endl;
    }
    double c1, c2, c3;
    c123(myindex.getldo1(0),myindex.getldo2(0),
         myindex.getldo1(1),myindex.getldo2(1),
         myindex.getldo1(2),myindex.getldo2(2),
         c1,c2,c3);
    cout << "#c1 (error estimation from S/N): " << c1 << endl;
    cout << "#c2 (error estimation from S/N): " << c2 << endl;
    cout << "#c3 (error estimation from S/N): " << c3 << endl;
  }
  else if (type == 3) //indices tipo D4000
  {
    cout << "#                        <code>: " << type
         << " (D4000-like index)";
    if(param.get_logindex())
    {
      cout << " in logarithmic units";
    }
    cout << endl;
    const long nbands = myindex.getnbands();
    for (int i=1; i <= nbands; i++)
    {
      if (i == 1)
        cout << "#               <blue bandpass>: ";
      else if (i == 2)
        cout << "#                <red bandpass>: ";
      cout << myindex.getldo1(i-1) << "  " 
           << myindex.getldo2(i-1) << endl;
    }
  }
  else if (type == 4) //indices tipo B4000
  {
    cout << "#                        <code>: " << type
         << " (B4000-like index)";
    if(param.get_logindex())
    {
      cout << " in logarithmic units";
    }
    cout << endl;
    const long nbands = myindex.getnbands();
    for (int i=1; i <= nbands; i++)
    {
      if (i == 1)
        cout << "#               <blue bandpass>: ";
      else if (i == 2)
        cout << "#                <red bandpass>: ";
      cout << myindex.getldo1(i-1) << "  " 
           << myindex.getldo2(i-1) << endl;
    }
  }
  else if (type == 5) //indices tipo color
  {
    cout << "#                        <code>: " << type
         << " (color-like index)";
    if(param.get_logindex())
    {
      cout << " in logarithmic units";
    }
    cout << endl;
    const long nbands = myindex.getnbands();
    for (int i=1; i <= nbands; i++)
    {
      if (i == 1)
        cout << "#               <blue bandpass>: ";
      else if (i == 2)
        cout << "#                <red bandpass>: ";
      cout << myindex.getldo1(i-1) << "  "
           << myindex.getldo2(i-1) << endl;
    }
  }
  else if ( type == 10 )  //linea de emision
  {
    cout << "#                        <code>: " << type
         << " (emission line)";
    cout << endl;
    const long nbands = myindex.getnbands();
    for (int i=1; i <= nbands; i++)
    {
      const double factor = myindex.getfactor_el(i-1);
      if (factor == 0.0)
      {
        cout << "#          <continuum bandpass>: ";
      }
      else
      {
        cout << "#               <line bandpass>: ";
      }
      cout << myindex.getldo1(i-1) << "  " 
           << myindex.getldo2(i-1) << endl;
    }
  }
  else if ( (type >= 11) && (type <= 99) ) //discontinuidades genericas
  {
    cout << "#                        <code>: " << type
         << " (generic discontinuity)";
    if(param.get_logindex())
    {
      cout << " in logarithmic units";
    }
    cout << endl;
    const long nconti = myindex.getnconti();
    for (int i=1; i <= nconti; i++)
    {
      cout << "#          <continuum bandpass>: " 
           << myindex.getldo1(i-1) << "  " 
           << myindex.getldo2(i-1) << endl;
    }
    const long nlines = myindex.getnlines();
    for (int i=1; i <= nlines; i++)
    {
      cout << "#   <spectral-feature bandpass>: " 
           << myindex.getldo1(i-1+nconti) << "  " 
           << myindex.getldo2(i-1+nconti) << endl;
    }
  }
  else if ( (type >= 101) && (type <= 9999) ) //indices genericos
  {
    cout << "#                        <code>: " << type
         << " (generic index)";
    if(param.get_logindex())
    {
      cout << " in logarithmic units";
    }
    cout << endl;
    const long nconti = myindex.getnconti();
    for (int i=1; i <= nconti; i++)
    {
      cout << "#          <continuum bandpass>: " 
           << myindex.getldo1(i-1) << "  " 
           << myindex.getldo2(i-1) << endl;
    }
    const long nlines = myindex.getnlines();
    for (int i=1; i <= nlines; i++)
    {
      cout << "#   <spectral-feature bandpass>: " 
           << myindex.getldo1(i-1+nconti) << "  " 
           << myindex.getldo2(i-1+nconti) << "  "
           << myindex.getfactor(i-1) << endl;
    }
  }
  else if ( (type >= -99) && (type <= -2) ) //indices escalon
  {
    cout << "#                        <code>: " << type
         << " (slope index)";
    if(param.get_logindex())
    {
      cout << " in logarithmic units";
    }
    cout << endl;
    const long nconti = myindex.getnconti();
    for (int i=1; i <= nconti; i++)
    {
      cout << "#          <continuum bandpass>: " 
           << myindex.getldo1(i-1) << "  " 
           << myindex.getldo2(i-1) << endl;
    }
  }
  //Input FITS file,and NAXIS1, NAXIS2 and OBJECT keywords
  cout << "#Input FITS file...............: " << param.get_if() << endl;
  cout << "#                      <NAXIS1>: " << imagePtr->getnaxis1() 
       << endl;
  cout << "#                      <NAXIS2>: " << imagePtr->getnaxis2() 
       << endl;
  cout << "#                      <OBJECT>: " << imagePtr->getobject_data() 
       << endl;
  //Input error FITS file and OBJECT keyword
  if (strcmp(param.get_ief(),"undef") != 0)
  {
    cout << "#Error FITS file...............: " << param.get_ief() << endl;
    cout << "#                      <OBJECT>: " << imagePtr->getobject_data() 
         << endl;
  }
  //Espectros a medir
  cout << "#First & last spectrum.........: " << param.get_ns1() << ", " 
       << param.get_ns2() << endl;
  //longitud de onda inicial y dispersion
  cout << "#CRVAL1, CDELT1, CRPIX1........: " << imagePtr->getcrval1()
       << ", " << imagePtr->getcdelt1()
       << ", " << imagePtr->getcrpix1() << endl;
  //velocidad radial
  bool lshow_nseed = false;
  if (strcmp(param.get_rvfile(),"undef") != 0)
  {
    cout << "#Radial velocity & error from..: " << param.get_rvfile() << ","
         << param.get_rvc() << "," << param.get_rvce() << endl;
    if(param.get_rvce() > 0)
    {
      cout << "#No. of simulations  (Rvel.err): " << param.get_nsimul() << endl;
      lshow_nseed=true;
    }
  }
  else
  {
    cout << "#Radial velocity (km/s) & error: " << param.get_rv() << ", "
         << param.get_rve() << endl;
    if(param.get_rve() > 0)
    {
      cout << "#No. simulations for err_rvel..: " << param.get_nsimul() << endl;
      lshow_nseed=true;
    }
  }
  //Input file con etiquetas para cada espectro
  cout << "#Input label file..............: " << param.get_ilabfile() 
       << "," << param.get_nchar1() << "," << param.get_nchar2() << endl;
  //simulaciones con S/N diferentes
  if(param.get_nsimulsn() > 0)
  {
    cout << "#No. of simulations  (S/N rat.): " << param.get_nsimulsn() << endl;
    cout << "#Minimum and Maximum S/N ratios: " << param.get_minsn() << ", " 
                                               << param.get_maxsn() << endl;
    lshow_nseed=true;
  }
  if(lshow_nseed)
  {
    cout << "#Seed for random number........: " << param.get_nseed() << endl;
  }
  //percentile for continuum estimation
  cout << "#Percentile for continuum (%)..: " << param.get_contperc() << endl;
  //boundary fit for continuum estimation
  cout << "#Boundary fit for continuum....: " << param.get_boundfit() << endl;
  //systematic error (additive % of the continuum level)
  cout << "#Systematic error (additive %).: " << param.get_biaserr() << endl;
  //linearity error
  cout << "#Linearity error (power law)...: " << param.get_linearerr() << endl;
  //flux scale factor
  cout << "#Flux scale factor.............: " << param.get_fscale() << endl;
  //separador
  cout << separador << "\n#" << endl;
  //leyenda y unidades
  cout << "#spect.    index    err_phot    S/N    "
          "   RVel.  RVel.err  ind_rvel  err_rvel"<< endl;

  if (type == 1) //indices moleculares
    cout << "#number    (mag)      (mag)   per ang  "
            "  (km/s)   (km/s)     (mag)     (mag)" << endl;
  else if (type == 2) //indices atomicos
    if (param.get_logindex())
    {
      cout << "#number    (mag)      (mag)   per ang  "
              "  (km/s)   (km/s)     (mag)     (mag)" << endl;
    }
    else
    {
      cout << "#number    (ang)      (ang)   per ang  "
              "  (km/s)   (km/s)     (ang)     (ang)" << endl;
    }
  else if ( (type == 3) || (type == 4) || (type == 5) ) //D4000, B4000, color
    cout << "#number    value      value   S/N_ang  "
            "  (km/s)   (km/s)     value     value" << endl;
  else if ( (type >= 101) && (type <= 9999) ) //indices genericos
    cout << "#number    (ang)      (ang)   per ang  "
            "  (km/s)   (km/s)     (ang)     (ang)" << endl;
  else if ( (type >= -99) && (type <= -2) ) //indices pendiente
    cout << "#number    value      value   S/N_ang  "
            "  (km/s)   (km/s)     value     value" << endl;

  cout << "#======   =======   ========  =======  "
          "========  ========  ========  ========"
       << endl;
}
