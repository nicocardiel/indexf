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
#include <cmath>
#include <cstdlib>
#include <vector>
#include "indexdef.h"
#include "cpgplot.h"
#include "cpgplot_d.h"
#include "genericpixel.h"

using namespace std;

bool fpercent(vector <GenericPixel> &, const long, const bool, 
              double *, double *);
bool boundaryfit(vector <GenericPixel> &, const bool, 
                 double *, double *);

bool mideindex(const bool &lerr, const double *sp_data, const double *sp_error, 
               const long &naxis1,
               const double &crval1, 
               const double &cdelt1, 
               const double &crpix1,
               const IndexDef &myindex,
               const long &contperc,
               const long &boundfit,
               const bool &logindex,
               const double &rvel, const double &rvelerr,
               const double &biaserr, const double &linearerr,
               const long &plotmode, const long &plottype,
               bool &out_of_limits, bool &negative_error,
               double &findex, double &eindex, double &sn)
{
  //---------------------------------------------------------------------------
  //protecciones
  if ( ( contperc >= 0 ) && ( boundfit > 0) )
  {
    cout << "ERROR: contperc and boundfit cannot be used simultaneously"
         << endl;
    exit(1);
  }
  //---------------------------------------------------------------------------
  //constantes numericas
  const double c = 2.9979246E+5; //velocidad de la luz (km/s)
  const double cte_log_exp = 2.5 * log10 (exp(1.0));
  //(1+z) corregido de efecto relativista
  const double rcvel = rvel/c; // v/c
  const double rcvel1 = (1.0+rcvel)/sqrt(1.0-rcvel*rcvel);
  //longitud de onda inferior del primer pixel
  const double wlmin = crval1-cdelt1/2.0-(crpix1-1.0)*cdelt1;
  //Nota: recordar que la longitud de onda en un pixel arbitrario j-esimo 
  //se calcula como:
  //lambda=crval1+(j-crpix1)*cdelt1
  //con j definido en el intervalo [1,NAXIS1]

  //mientras no se demuestre lo contario, no hay problemas al medir
  out_of_limits=false;
  negative_error=false;

  //---------------------------------------------------------------------------
  //calculamos parametros de cada banda a medir
  double *ca = new double [myindex.getnbands()];
  double *cb = new double [myindex.getnbands()];
  double *c3 = new double [myindex.getnbands()];
  double *c4 = new double [myindex.getnbands()];
  long *j1 = new long [myindex.getnbands()];
  long *j2 = new long [myindex.getnbands()];
  double *d1 = new double [myindex.getnbands()];
  double *d2 = new double [myindex.getnbands()];
  double *rl = new double [myindex.getnbands()];
  double *rg = new double [myindex.getnbands()];
  for (long nb=0; nb < myindex.getnbands(); nb++)
  {
    ca[nb] = myindex.getldo1(nb)*rcvel1;             //redshifted wavelength
    cb[nb] = myindex.getldo2(nb)*rcvel1;             //redshifted wavelength
    c3[nb] = (ca[nb]-wlmin)/cdelt1+1.0;              //band limit (channel)
    c4[nb] = (cb[nb]-wlmin)/cdelt1;                  //band limit (channel)
    if ( (c3[nb] < 1.0) || (c4[nb] > static_cast<double>(naxis1)) )
    {
      out_of_limits=true;          //indice fuera de limites: no se puede medir
      return(false);
    }
    j1[nb] = static_cast<long>(c3[nb]);         //band limit: integer (channel)
    j2[nb] = static_cast<long>(c4[nb]);         //band limit: integer (channel)
    d1[nb] = c3[nb]-static_cast<double>(j1[nb]);//fraction (excess left chan.)
    d2[nb] = c4[nb]-static_cast<double>(j2[nb]);//fraction (defect right chan.)
    rl[nb] = cb[nb]-ca[nb];                 //redshifted band width (angstroms)
    rg[nb] = c4[nb]-c3[nb]+1;               //redshifted band width (channels)
  }
  //calculamos limites en j1 y j2, por si las bandas no estan en orden
  long j1min = j1[0];
  long j2max = j2[0];
  for (long nb=0; nb < myindex.getnbands(); nb++ )
  {
    if (j1min > j1[nb]) j1min=j1[nb];
    if (j2max < j2[nb]) j2max=j2[nb];
  }
  //calculamos limites en longitud de onda (rest frame)
  double wvmin= myindex.getldo1(0);
  double wvmax= myindex.getldo2(0);
  double wv1temp,wv2temp;
  for (long nb=0; nb < myindex.getnbands(); nb++ )
  {
    wv1temp=myindex.getldo1(nb);
    wv2temp=myindex.getldo2(nb);
    if(wvmin > wv1temp) wvmin=wv1temp;
    if(wvmax < wv2temp) wvmax=wv2temp;
  }

  //---------------------------------------------------------------------------
  //fijamos los canales a usar para medir el indice (usando la variable
  //logica evitamos el problema de la posible superposicion de las bandas)
  bool *ifchan = new bool [naxis1];
  for (long j=1; j <= naxis1; j++)
  {
    ifchan[j-1] = false;
  }
  for (long nb=0; nb < myindex.getnbands(); nb++)
  {
    for (long j=j1[nb]; j <= j2[nb]+1; j++)
    {
      ifchan[j-1]=true;
    }
  }
  long nceff=0;
  for (long j=1; j <= naxis1; j++)
  {
    if (ifchan[j-1]) nceff++;
  }

  //---------------------------------------------------------------------------
  //normalizamos datos usando la senal solo en la region del indice a medir
  double *s = new double [naxis1];
  double *es = new double [naxis1];
  double smean=0.0;
  for (long j=1; j <= naxis1; j++)
  {
    if (ifchan[j-1]) smean+=sp_data[j-1];
  }
  smean/=static_cast<double>(nceff);
  smean = ( smean != 0 ? smean : 1.0); //evitamos division por cero
  for (long j=1; j <= naxis1; j++)
  {
    s[j-1]=sp_data[j-1]/smean;
  }
  if(lerr)
  {
    for (long j=1; j <= naxis1; j++)
    {
      es[j-1]=sp_error[j-1]/smean;
    }
  }

  //===========================================================================
  //dibujamos
  if(plotmode != 0)
  {
    //calculamos un array temporal para el eje X (en unidades de pixeles)
    double *x = new double [naxis1];
    for (long j=1; j<=naxis1; j++)
    {
      x[j-1]=static_cast<double>(j);
    }
    //calculamos limites en el eje X (en pixels)
    double xmin,xmax,dx;
    xmin=(ca[0]-crval1)/cdelt1+crpix1;
    xmax=(cb[0]-crval1)/cdelt1+crpix1;
    double chan1, chan2;
    for (long nb=0; nb < myindex.getnbands(); nb++)
    {
      chan1=(ca[nb]-crval1)/cdelt1+crpix1;
      chan2=(cb[nb]-crval1)/cdelt1+crpix1;
      if(chan1 < xmin) xmin=chan1;
      if(chan2 > xmax) xmax=chan2;
    }
    dx=xmax-xmin;
    xmin=xmin-dx/3.0;
    xmax=xmax+dx/3.0;
    //calculamos limites en el eje Y (flujo)
    double ymin,ymax,dy;
    bool firstpixel=true;
    for (long j=1; j<=naxis1; j++)
    {
      if((j >= xmin) && (j <= xmax))
      {
        if(firstpixel)
        {
          if((lerr) && (plotmode < 0))
          {
            firstpixel=false;
            ymin=sp_data[j-1]-sp_error[j-1];
            ymax=sp_data[j-1]+sp_error[j-1];
          }
          else
          {
            firstpixel=false;
            ymin=ymax=sp_data[j-1];
          }
        }
        else
        {
          if((lerr) && (plotmode < 0))
          {
            if(ymin > sp_data[j-1]-sp_error[j-1]) 
             ymin=sp_data[j-1]-sp_error[j-1];
            if(ymax < sp_data[j-1]+sp_error[j-1]) 
             ymax=sp_data[j-1]+sp_error[j-1];
          }
          else
          {
            if(ymin > sp_data[j-1]) ymin=sp_data[j-1];
            if(ymax < sp_data[j-1]) ymax=sp_data[j-1];
          }
        }
      }
    }
    if(fabs(biaserr) !=0)
    {
      if(ymin > ymin*(1+1.5*biaserr/100) ) ymin*=(1+1.5*biaserr/100);
      if(ymax < ymax*(1+biaserr/100) ) ymax*=(1+biaserr/100);
    }
    else if(fabs(linearerr) !=0)
    {
      if(ymin > ymin*pow(ymin,linearerr) ) ymin*=pow(ymin,linearerr);
      if(ymax < ymax*pow(ymax,linearerr) ) ymax*=pow(ymax,linearerr);
    }
    dy=ymax-ymin;
    ymin=ymin-dy/10.0;
    ymax=ymax+dy/10.0;
    //dibujamos caja del plot con diferentes escalas en X
    double xminl,xmaxl;
    xminl=crval1+(xmin-crpix1)*cdelt1;
    xmaxl=crval1+(xmax-crpix1)*cdelt1;
    cpgenv_d(xminl,xmaxl,ymin,ymax,0,-2);
    if(plottype)
    {
      float xv1,xv2,yv1,yv2,dyv;
      cpgqvp(0,&xv1,&xv2,&yv1,&yv2);
      dyv=yv2-yv1;
      cpgsvp(xv1,xv2,yv1,yv2-dyv*0.20);
      cpgswin(xminl,xmaxl,ymin,ymax);
      cpgbox("btsn",0.0,0,"bctsn",0.0,0);
      cpgswin(xminl/rcvel1,xmaxl/rcvel1,ymin,ymax);
      cpgbox("ctsm",0.0,0," ",0.0,0);
      cpgmtxt("t",2.0,0.5,0.5,"rest-frame wavelength (\\A)");
      cpgsvp(xv1,xv2,yv1,yv2);
      cpgswin_d(xmin,xmax,ymin,ymax);
      cpgsci(15);
      cpgbox("ctsm",0.0,0," ",0.0,0);
      float ch;
      cpgqch(&ch);
      cpgsch(0.9);
      cpgmtxt("t",-1.5,0.0,0.0,"pixel (wavelength direction)");
      cpgmtxt("t",-1.5,1.0,1.0,"pixel (wavelength direction)");
      cpgsch(ch);
      cpgsci(1);
      cpgsvp(xv1,xv2,yv1,yv2-dyv*0.20);
    }
    else
    {
      cpgbox("bctsn",0.0,0,"bctsn",0.0,0);
    }
    cpglab("observed wavelength (\\A)","flux"," ");
    cpgswin_d(xmin,xmax,ymin,ymax);
    //dibujamos errores
    if((lerr) && (plotmode < 0))
    {
      cpgbbuf();
      cpgsci(15);
      cpgslw(1);
      for (long j=1; j<=naxis1; j++)
      {
        cpgmove_d(x[j-1],sp_data[j-1]-sp_error[j-1]);
        cpgdraw_d(x[j-1],sp_data[j-1]+sp_error[j-1]);
        cpgmove_d(x[j-1]-0.2,sp_data[j-1]-sp_error[j-1]);
        cpgdraw_d(x[j-1]+0.2,sp_data[j-1]-sp_error[j-1]);
        cpgmove_d(x[j-1]-0.2,sp_data[j-1]+sp_error[j-1]);
        cpgdraw_d(x[j-1]+0.2,sp_data[j-1]+sp_error[j-1]);
      }
      cpgslw(2);
      cpgsci(1);
      cpgebuf();
    }
    //dibujamos el espectro
    cpgbin_d(naxis1,x,sp_data,true);
    //dibujamos bandas
    dy=ymax-ymin;
    for (long nb=0; nb<myindex.getnbands(); nb++)
    {
      double ddy=0.00;
      if((myindex.gettype() == 1) || //........................indice molecular
         (myindex.gettype() == 2))   //..........................indice atomico
      {
        ddy=0.04;
        if(nb==0)
          cpgsci(4);
        else if(nb==1)
          cpgsci(3);
        else
          cpgsci(2);
      }
      else if((myindex.gettype() == 3) || //..............................D4000
              (myindex.gettype() == 4) || //..............................B4000
              (myindex.gettype() == 5))   //..............................CO_KH
      {
        ddy=0.04;
        if(nb==0)
          cpgsci(4);
        else
          cpgsci(2);
      }
      else if(myindex.gettype() == 10)//..........................emission line
      {
        if(myindex.getfactor_el(nb) == 0.0) //continuo
        {
          ddy=0.04;
          cpgsci(5);
        }
        else //linea
        {
          ddy=0.06;
          cpgsci(3);
        }
      }
      else if((myindex.gettype() >= 11) && //...........discontinuidad generica
              (myindex.gettype()<=99))
      {
        if(nb < myindex.getnconti())
        {
          ddy=0.04;
          cpgsci(5);
        }
        else
        {
          ddy=0.06;
          cpgsci(3);
        }
      }
      else if((myindex.gettype() >= 101) && //..................indice generico
              (myindex.gettype()<=9999))
      {
        if(nb < myindex.getnconti())
        {
          ddy=0.04;
          cpgsci(5);
        }
        else
        {
          ddy=0.06;
          cpgsci(3);
        }
      }
      else if((myindex.gettype() >= -99) && //...........................slopes
              (myindex.gettype()<=-2))
      {
        cpgsci(5);
        ddy=0.04;
      }
      else //.......................................................sin definir
      {
        cout << "Invalid index type=" << myindex.gettype() << endl;
        exit(1);
      }
      const double xc1=(ca[nb]-crval1)/cdelt1+crpix1;
      const double xc2=(cb[nb]-crval1)/cdelt1+crpix1;
      cpgrect_d(xc1,xc2,ymin+dy*0.04,ymin+dy*0.05);
      cpgrect_d(xc1,xc2,ymax-dy*(ddy+0.01),ymax-dy*ddy);
      cpgmove_d(xc1,ymin+dy*0.04); cpgdraw_d(xc1,ymax-dy*ddy);
      cpgmove_d(xc2,ymin+dy*0.04); cpgdraw_d(xc2,ymax-dy*ddy);
    }
    cpgsci(1);
    //borramos array temporal para el eje X
    delete [] x;
  }//==========================================================================

  //---------------------------------------------------------------------------
  //senal/ruido promedio en las bandas del indice (vigilando que no haya
  //valores de error <= 0)
  sn=0.0;
  if(lerr)
  {
    for (long j=1; j <= naxis1; j++)
    {
      if (ifchan[j-1])
      {
        if (sp_error[j-1] <= 0)
        {
          negative_error=true;
          return(false);
        }
        sn+=s[j-1]/es[j-1];
      }
    }
    sn/=static_cast<double>(nceff);
    sn/=sqrt(cdelt1); //calculamos senal/ruido por angstrom
  }

  //---------------------------------------------------------------------------
  //Si se ha solicitado, introducimos un error sistematico modificando el 
  //espectro por un factor aditivo. Para ello calculamos el valor promedio del
  //pseudocontinuo y anadimos una fraccion de dicho flujo a todo el espectro
  if ( fabs(biaserr) != 0.0 )
  {
    //proteccion
    if ( contperc >= 0 )
    {
      cout << "ERROR: biaserr and contperc cannot be used simultaneously"
           << endl;
      exit(1);
    }
    //indice atomico o molecular
    if ( (myindex.gettype() == 1) || (myindex.gettype() == 2) )
    {
      //cuentas promedio en el continuo azul
      double sb=0.0;
      for (long j=j1[0]; j<=j2[0]+1; j++)
      {
        double f;
        if (j == j1[0])
          f=1.0-d1[0];
        else if (j == j2[0]+1)
          f=d2[0];
        else
          f=1.0;
        sb+=f*s[j-1];
      }
      sb*=cdelt1;
      sb/=rl[0];
      //cuentas promedio en la banda roja
      double sr=0.0;
      for (long j=j1[2]; j<=j2[2]+1; j++)
      {
        double f;
        if (j == j1[2])
          f=1.0-d1[2];
        else if (j == j2[2]+1)
          f=d2[2];
        else
          f=1.0;
        sr+=f*s[j-1];
      }
      sr*=cdelt1;
      sr/=rl[2];
      //calculamos pseudo-continuo en el centro de la banda central
      double mwb = (myindex.getldo1(0)+myindex.getldo2(0))/2.0;
      mwb*=rcvel1;
      double mwr = (myindex.getldo1(2)+myindex.getldo2(2))/2.0;
      mwr*=rcvel1;
      long j = (j1[1]+(j2[1]+1))/2; //pixel central de la banda central
      double wla=static_cast<double>(j-1)*cdelt1+crval1-(crpix1-1.0)*cdelt1;
      double sc = (sb*(mwr-wla)+sr*(wla-mwb))/(mwr-mwb);
      //anadimos el efecto sistematico al espectro de datos (el espectro de
      //errores no se modifica)
      for (long j=1; j <= naxis1; j++)
      {
        s[j-1]+=sc*biaserr/100.0;
      }
    }
    //D4000
    else if ( myindex.gettype() == 3)
    {
      //cuentas promedio en la banda azul
      double sb=0.0;
      for (long j=j1[0]; j<=j2[0]+1; j++)
      {
        double f;
        if (j == j1[0])
          f=1.0-d1[0];
        else if (j == j2[0]+1)
          f=d2[0];
        else
          f=1.0;
        sb+=f*s[j-1];
      }
      sb*=cdelt1;
      sb/=rl[0];
      //cuentas promedio en la banda roja
      double sr=0.0;
      for (long j=j1[1]; j<=j2[1]+1; j++)
      {
        double f;
        if (j == j1[1])
          f=1.0-d1[1];
        else if (j == j2[1]+1)
          f=d2[1];
        else
          f=1.0;
        sr+=f*s[j-1];
      }
      sr*=cdelt1;
      sr/=rl[1];
      //valor promedio
      double sc = (sb+sr)/2.0;
      //anadimos el efecto sistematico al espectro de datos (el espectro de
      //errores no se modifica)
      for (long j=1; j <= naxis1; j++)
      {
        s[j-1]+=sc*biaserr/100.0;
      }
    }
    //indices para los cuales no se ha incluido el efecto de biaserr
    else
    {
      cout << "FATAL ERROR: biaserr=" << biaserr
           << " cannot be handled for this type of index." << endl;
      exit(1);
    }
  }

  //---------------------------------------------------------------------------
  //Si se ha solicitado, introducimos un error de linealidad de la forma:
  //flux_real=flux_observed^(1+linearerr). Modificamos asimismo el espectro 
  //de errores. Nota: para evitar "NaN", forzamos utilizar una senal positiva.
  if ( fabs(linearerr) != 0.0 )
  {
    //proteccion
    if ( contperc >= 0 )
    {
      cout << "ERROR: linearerr and contperc cannot be used simultaneously"
           << endl;
      exit(1);
    }
    double scale_factor;
    for (long j=1; j <= naxis1; j++)
    {
      if (s[j-1] >= 0.0 )
      {
        scale_factor=pow(s[j-1],linearerr);
      }
      else
      {
        scale_factor=pow(-s[j-1],linearerr);
      }
      s[j-1]*=scale_factor;
      es[j-1]*=scale_factor;
    }
  }

  //---------------------------------------------------------------------------
  //Indices atomicos y moleculares
  //---------------------------------------------------------------------------
  if ( (myindex.gettype() == 1) || (myindex.gettype() == 2) )
  {
    //proteccion
    if ( contperc >= 0 )
    {
      cout << "WARNING: contperc is being implemented for this index"
           << endl;
    }
    //cuentas promedio en la banda azul
    double sb=0.0;
    double esb2=0.0;
    if (contperc >= 0) //......................................usamos percentil
    {
      if (j2[0]-j1[0] < 2)
      {
        cout << "ERROR: number of pixels in blue continuum bandpass too low "
             << "to use contperc"
             << endl;
        exit(1);
      }
      else
      {
        vector <GenericPixel> fluxpix;
        for (long j=j1[0]; j<=j2[0]+1; j++)
        {
          double f;
          if (j == j1[0])
            f=1.0-d1[0];
          else if (j == j2[0]+1)
            f=d2[0];
          else
            f=1.0;
          double wave=static_cast<double>(j-1)*cdelt1+
                      crval1-(crpix1-1.0)*cdelt1;
          GenericPixel temppix(wave,s[j-1],es[j-1],f);
          fluxpix.push_back(temppix);
        }
        if(!fpercent(fluxpix,contperc,lerr,&sb,&esb2))
        {
          cout << "ERROR: while computing percentile" << endl;
          exit(1);
        }
      }
    }
    else if ( boundfit > 0 ) //.............................usamos boundary fit
    {
      if(boundfit == 1)
      {
        vector <GenericPixel> fluxpix;
        for (long j=j1[0]; j<=j2[0]+1; j++)
        {
          double f;
          if (j == j1[0])
            f=1.0-d1[0];
          else if (j == j2[0]+1)
            f=d2[0];
          else
            f=1.0;
          double wave=static_cast<double>(j-1)*cdelt1+
                      crval1-(crpix1-1.0)*cdelt1;
          GenericPixel temppix(wave,s[j-1],es[j-1],f);
          fluxpix.push_back(temppix);
        }
        if(!boundaryfit(fluxpix,lerr,&sb,&esb2))
        {
          cout << "ERROR: while computing percentile" << endl;
          exit(1);
        }
      }
      else
      {
        cout << "ERROR: invalid boundfit value" << endl;
        cout << "boundfit=" << boundfit << endl;
        exit(1);
      }
    }
    else //...............................................usamos metodo clasico
    {
      for (long j=j1[0]; j<=j2[0]+1; j++)
      {
        double f;
        if (j == j1[0])
          f=1.0-d1[0];
        else if (j == j2[0]+1)
          f=d2[0];
        else
          f=1.0;
        sb+=f*s[j-1];
        if(lerr) esb2+=f*f*es[j-1]*es[j-1];
      }
      sb*=cdelt1;
      sb/=rl[0];
      if(lerr)
      {
        esb2*=cdelt1*cdelt1;
        esb2/=(rl[0]*rl[0]);
      }
    }
    //cuentas promedio en la banda roja
    double sr=0.0;
    double esr2=0.0;
    if (contperc >= 0) //......................................usamos percentil
    {
      if (j2[2]-j1[2] < 2)
      {
        cout << "ERROR: number of pixels in red continuum bandpass too low "
             << "to use contperc"
             << endl;
        exit(1);
      }
      else
      {
        vector <GenericPixel> fluxpix;
        for (long j=j1[2]; j<=j2[2]+1; j++)
        {
          double f;
          if (j == j1[2])
            f=1.0-d1[2];
          else if (j == j2[2]+1)
            f=d2[2];
          else
            f=1.0;
          double wave=static_cast<double>(j-1)*cdelt1+
                      crval1-(crpix1-1.0)*cdelt1;
          GenericPixel temppix(wave,s[j-1],es[j-1],f);
          fluxpix.push_back(temppix);
        }
        if(!fpercent(fluxpix,contperc,lerr,&sr,&esr2))
        {
          cout << "ERROR: while computing boundary fit" << endl;
          exit(1);
        }
      }
    }
    else if ( boundfit > 0 ) //.............................usamos boundary fit
    {
      if(boundfit == 1)
      {
        vector <GenericPixel> fluxpix;
        for (long j=j1[2]; j<=j2[2]+1; j++)
        {
          double f;
          if (j == j1[2])
            f=1.0-d1[2];
          else if (j == j2[2]+1)
            f=d2[2];
          else
            f=1.0;
          double wave=static_cast<double>(j-1)*cdelt1+
                      crval1-(crpix1-1.0)*cdelt1;
          GenericPixel temppix(wave,s[j-1],es[j-1],f);
          fluxpix.push_back(temppix);
        }
        if(!boundaryfit(fluxpix,lerr,&sr,&esr2))
        {
          cout << "ERROR: while computing boundary fit" << endl;
          exit(1);
        }
      }
      else
      {
        cout << "ERROR: invalid boundfit value" << endl;
        cout << "boundfit=" << boundfit << endl;
        exit(1);
      }
    }
    else //...............................................usamos metodo clasico
    {
      for (long j=j1[2]; j<=j2[2]+1; j++)
      {
        double f;
        if (j == j1[2])
          f=1.0-d1[2];
        else if (j == j2[2]+1)
          f=d2[2];
        else
          f=1.0;
        sr+=f*s[j-1];
        if(lerr) esr2+=f*f*es[j-1]*es[j-1];
      }
      sr*=cdelt1;
      sr/=rl[2];
      if(lerr)
      {
        esr2*=cdelt1*cdelt1;
        esr2/=(rl[2]*rl[2]);
      }
    }
    //calculamos pseudo-continuo
    double mwb = (myindex.getldo1(0)+myindex.getldo2(0))/2.0;
    mwb*=rcvel1;
    double mwr = (myindex.getldo1(2)+myindex.getldo2(2))/2.0;
    mwr*=rcvel1;
    double *sc = new double [naxis1];
    double *esc2 = new double [naxis1];
    for (long j = j1min; j <= j2max+1; j++)
    {
      double wla=static_cast<double>(j-1)*cdelt1+crval1-(crpix1-1.0)*cdelt1;
      sc[j-1] = (sb*(mwr-wla)+sr*(wla-mwb))/(mwr-mwb);
    }
    if(lerr)
    {
      for (long j = j1min; j <= j2max+1; j++)
      {
        double wla=static_cast<double>(j-1)*cdelt1+crval1-(crpix1-1.0)*cdelt1;
        esc2[j-1] = (esb2*(mwr-wla)*(mwr-wla)+esr2*(wla-mwb)*(wla-mwb))/
                    ((mwr-mwb)*(mwr-mwb));
      }
    }
    //recorremos la banda central
    double tc=0.0;
    double etc2=0.0,etc=0.0;
    for (long j=j1[1]; j<=j2[1]+1; j++)
    {
      double f;
      if (j == j1[1])
        f=1.0-d1[1];
      else if (j == j2[1]+1)
        f=d2[1];
      else
        f=1.0;
      tc+=f*s[j-1]/sc[j-1];
      if(lerr) 
      {
        etc2+=f*f*(s[j-1]*s[j-1]*esc2[j-1]+sc[j-1]*sc[j-1]*es[j-1]*es[j-1])/
        (sc[j-1]*sc[j-1]*sc[j-1]*sc[j-1]);
        double wla1=static_cast<double>(j-1)*cdelt1+crval1-(crpix1-1.0)*cdelt1;
        for (long jj=j1[1]; jj<=j2[1]+1; jj++)
        {
          if (jj != j)
          {
            double ff;
            if (jj == j1[1])
              ff=1.0-d1[1];
            else if (j == j2[1]+1)
              ff=d2[1];
            else
              ff=1.0;
            double wla2=static_cast<double>(jj-1)*cdelt1+crval1
                        -(crpix1-1.0)*cdelt1;
            double cov=((mwr-wla1)*(mwr-wla2)*esb2+(wla1-mwb)*(wla2-mwb)*esr2)/
                       ((mwr-mwb)*(mwr-mwb));
            etc2+=ff*f*s[j-1]*s[jj-1]*cov/(sc[j-1]*sc[j-1]*sc[jj-1]*sc[jj-1]);
          }
        }
      }
    }
    tc*=cdelt1;
    etc=sqrt(etc2)*cdelt1;
    if (myindex.gettype() == 1) //indice molecular
    {
      findex = -2.5*log10(tc/rl[1]);
      if(lerr) eindex = cte_log_exp/pow(10,-0.4*findex)*etc/rl[1];
    }
    else //indice atomico
    {
      if(logindex) //indice atomico medido en magnitudes
      {
        findex = -2.5*log10(tc/rl[1]);
        if(lerr) eindex = cte_log_exp/pow(10,-0.4*findex)*etc/rl[1];
      }
      else //indice atomico medido como indice atomico
      {
        findex = (rl[1]-tc)/rcvel1;
        if(lerr) eindex=etc/rcvel1;
      }
    }
    delete [] sc;
    delete [] esc2;
    //=========================================================================
    //dibujamos
    if((plotmode !=0) || (plottype == 2))
    {
      if(plottype == 2)
        cpgsci(7);
      else
        cpgsci(6);
      //dibujamos el continuo
      const double wla=wvmin*rcvel1;
      const double yduma=sb*(mwr-wla)/(mwr-mwb)+sr*(wla-mwb)/(mwr-mwb);
      const double xca=(wla-crval1)/cdelt1+crpix1;
      cpgmove_d(xca,yduma*smean);
      const double wlb=wvmax*rcvel1;
      const double ydumb=sb*(mwr-wlb)/(mwr-mwb)+sr*(wlb-mwb)/(mwr-mwb);
      const double xcb=(wlb-crval1)/cdelt1+crpix1;
      cpgdraw_d(xcb,ydumb*smean);
      cpgsci(1);
      //repetimos el dibujo del espectro si hemos utilizado biaserr o linearerr
      if ((fabs(biaserr) != 0) || (fabs(linearerr) != 0))
      {
        double *x  = new double [naxis1];
        double *s_ = new double [naxis1];
        for(long j=1; j<=naxis1; j++)
        {
          x[j-1] = static_cast<double>(j);
          s_[j-1]=s[j]*smean;
        }
        cpgsci(15);
        cpgbin_d(naxis1,x,s_,true);
        cpgsci(1);
        delete [] x;
        delete [] s_;
      }
    }//========================================================================
  }
  //---------------------------------------------------------------------------
  //D4000, B4000 y colores
  //---------------------------------------------------------------------------
  else if ( (myindex.gettype() == 3) || 
            (myindex.gettype() == 4) || 
            (myindex.gettype() == 5) )
  {
    //proteccion
    if ( contperc >= 0 )
    {
      cout << "ERROR: contperc has not been implemented yet for this index"
           << endl;
      exit(1);
    }
    //pesos para la discontinuidad
    double *wl = new double [naxis1];
    double *wl2 = new double [naxis1];
    if (myindex.gettype() == 3) //D4000
    {
      //for (long j = j1min; j <= j2max+1; j++)
      for (long j = 1; j <= naxis1; j++)
      {
        double wla=static_cast<double>(j-1)*cdelt1+crval1-(crpix1-1.0)*cdelt1;
        wla/=rcvel1;
        wla/=4000.0;
        wl[j-1] = wla*wla;
        if (lerr) wl2[j-1] = wl[j-1]*wl[j-1];
      }
    }
    else //B4000 y colores
    {
      for (long j = j1min; j <= j2max+1; j++)
      {
        wl[j-1] = 1.0;
        if (lerr) wl2[j-1] = 1.0;
      }
    }
    //calculamos las integrales
    double *fx = new double [myindex.getnbands()];
    double *efx = new double [myindex.getnbands()];
    for (long nb=0; nb < myindex.getnbands(); nb++)
    {
      double tc=0.0;
      double etc=0.0;
      for (long j=j1[nb]; j<=j2[nb]+1; j++)
      {
        double f;
        if (j == j1[nb])
          f=1.0-d1[nb];
        else if (j == j2[nb]+1)
          f=d2[nb];
        else
          f=1.0;
        tc+=f*s[j-1]*wl[j-1];
        if(lerr) etc+=f*f*es[j-1]*es[j-1]*wl2[j-1];
      }
      fx[nb]=tc;
      if(lerr) efx[nb]=etc;
    }
    findex=fx[1]/fx[0];
    if(lerr) eindex=sqrt(fx[0]*fx[0]*efx[1]+fx[1]*fx[1]*efx[0])/
                    (fx[0]*fx[0]);
    if (myindex.gettype() == 5) //colores
    {
      findex=findex*rl[0]/rl[1];
      eindex=eindex*rl[0]/rl[1];
    }
    if(logindex) //si medimos en escala logaritmica
    {
      eindex= cte_log_exp*eindex/findex;
      findex=2.5*log10(findex);
    }
    delete [] wl;
    delete [] wl2;
    delete [] fx;
    delete [] efx;
    //=========================================================================
    //dibujamos
    if(plotmode !=0)
    {
      cpgsci(6);
      //continuo en banda azul
      const double wla=myindex.getldo1(0)*rcvel1;
      double yduma=0.0;
      for (long j=j1[0];j<=j2[0]+1;j++)
      {
        yduma+=s[j-1];
      }
      yduma/=static_cast<double>(j2[0]-j1[0]+2);
      const double xca=(wla-crval1)/cdelt1+crpix1;
      cpgmove_d(xca,yduma*smean);
      const double wlb=myindex.getldo2(0)*rcvel1;
      const double xcb=(wlb-crval1)/cdelt1+crpix1;
      cpgdraw_d(xcb,yduma*smean);
      //continuo en banda roja
      const double wlc=myindex.getldo1(1)*rcvel1;
      double ydumb=0.0;
      for (long j=j1[1];j<=j2[1]+1;j++)
      {
        ydumb+=s[j-1];
      }
      ydumb/=static_cast<double>(j2[1]-j1[1]+2);
      const double xcc=(wlc-crval1)/cdelt1+crpix1;
      cpgmove_d(xcc,ydumb*smean);
      const double wld=myindex.getldo2(1)*rcvel1;
      const double xcd=(wld-crval1)/cdelt1+crpix1;
      cpgdraw_d(xcd,ydumb*smean);
      cpgsci(1);
    }//========================================================================
  }
  //---------------------------------------------------------------------------
  //Lineas de emision (se ajusta el continuo a una recta)
  //---------------------------------------------------------------------------
  else if (myindex.gettype() == 10)
  {
    //proteccion
    if ( contperc >= 0 )
    {
      cout << "ERROR: contperc has not been implemented yet for this index"
           << endl;
      exit(1);
    }
    long nbands = myindex.getnbands();
    //si no hay errores, hacemos todos iguales a uno para utilizar las mismas
    //formulas
    if(!lerr)
      for (long j = j1min; j <= j2max+1; j++)
        es[j-1]=1.0;
    //calculamos la recta del continuo mediante minimos cuadrados (y=amc*x+bmc)
    //(para la variable x usamos el numero de pixel en lugar de la longitud
    //de onda porque, en principio, seran numeros mas pequenos)
    double sigma2;
    double sum0=0.0;
    double sumx=0.0;
    double sumy=0.0;
    double sumxy=0.0;
    double sumxx=0.0;
    for (long nb=0; nb < nbands; nb++)//recorremos todas las bandas porque
    {                                 //los continuos pueden estar intercalados
      double factor = myindex.getfactor_el(nb);
      if( factor == 0.0 ) //es una banda de continuo
      {
        for (long j=j1[nb]; j<=j2[nb]+1; j++)
        {
          double f;
          if (j == j1[nb])
            f=1.0-d1[nb];
          else if (j == j2[nb]+1)
            f=d2[nb];
          else
            f=1.0;
          sigma2=es[j-1]*es[j-1];
          sum0+=f/sigma2;
          sumx+=f*static_cast<double>(j)/sigma2;
          sumy+=f*s[j-1]/sigma2;
          sumxy+=f*static_cast<double>(j)*s[j-1]/sigma2;
          sumxx+=f*static_cast<double>(j)*static_cast<double>(j)/sigma2;
        }
      }
    }
    double deter=sum0*sumxx-sumx*sumx;
    double amc=(sum0*sumxy-sumx*sumy)/deter;
    double bmc=(sumxx*sumy-sumx*sumxy)/deter;
    //calculamos el pseudo-continuo
    double *sc = new double [naxis1];
    double *esc2 = new double [naxis1];
    for (long j = j1min; j <= j2max+1; j++)
    {
      sc[j-1] = amc*static_cast<double>(j)+bmc;
    }
    //=======================================================================
    //dibujamos continuo
    if (plotmode !=0)
    {
      cpgsci(6);
      double fdum = sc[j1min-1]*smean;
      cpgmove_d(static_cast<double>(j1min),fdum);
      for (long j = j1min; j <= j2max+1; j++)
      {
        fdum = sc[j-1]*smean;
        cpgdraw_d(static_cast<double>(j),fdum);
      }
      cpgsci(1);
    }
    //=======================================================================
    if(lerr)
    {
      for (long j = j1min; j <= j2max+1; j++)
      {
        esc2[j-1] = 0.0;
        for (long nb=0; nb < nbands; nb++)
        {
          double factor = myindex.getfactor_el(nb);
          if( factor == 0.0 ) //es una banda de continuo
          {
            for (long jj=j1[nb]; jj<=j2[nb]+1; jj++)
            {
              sigma2=es[jj-1]*es[jj-1];
              double fdum=(sum0*static_cast<double>(jj)/sigma2-sumx/sigma2)*
                          static_cast<double>(j)/deter+
                          (sumxx/sigma2-
                           sumx*static_cast<double>(jj)/sigma2)/deter;
              esc2[j-1]+=fdum*fdum*sigma2;
            }
          }
        }
      }
      //=======================================================================
      //dibujamos continuo y su error
      if (plotmode < 0)
      {
        cpgsci(7);
        //error superior
        double fdum = (sc[j1min-1]+sqrt(esc2[j1min-1]))*smean;
        cpgmove_d(static_cast<double>(j1min),fdum);
        for (long j = j1min; j <= j2max+1; j++)
        {
          fdum = (sc[j-1]+sqrt(esc2[j-1]))*smean;
          cpgdraw_d(static_cast<double>(j),fdum);
        }
        //error inferior
        fdum = (sc[j1min-1]-sqrt(esc2[j1min-1]))*smean;
        cpgmove_d(static_cast<double>(j1min),fdum);
        for (long j = j1min; j <= j2max+1; j++)
        {
          fdum = (sc[j-1]-sqrt(esc2[j-1]))*smean;
          cpgdraw_d(static_cast<double>(j),fdum);
        }
        cpgsci(1);
      }
      //=======================================================================
    }
    //calculamos unos sumatorios auxiliares
    double sumli=0.0; //ver notas a mano
    double sumni=0.0; //ver notas a mano
    for (long nb=0; nb < nbands; nb++)
    {
      double factor = myindex.getfactor_el(nb);
      if( factor != 0.0 ) //es una banda de linea
      {
        for (long j=j1[nb]; j<=j2[nb]+1; j++)
        {
          double f;
          if (j == j1[nb])
            f=1.0-d1[nb];
          else if (j == j2[nb]+1)
            f=d2[nb];
          else
            f=1.0;
          sumli+=f*static_cast<double>(j);
          sumni+=f;
        }
      }
    }
    //calculamos el flujo y su error
    findex=0.0;
    eindex=0.0;
    for (long nb=0; nb < nbands; nb++)
    {
      double factor = myindex.getfactor_el(nb);
      if( factor != 0.0 ) //es una banda de linea
      {
        for (long j=j1[nb]; j<=j2[nb]+1; j++)
        {
          double f;
          if (j == j1[nb])
            f=1.0-d1[nb];
          else if (j == j2[nb]+1)
            f=d2[nb];
          else
            f=1.0;
          findex+=f*(s[j-1]-sc[j-1]);
          if(lerr)
          {
            //eindex+=f*f*(es[j-1]*es[j-1]+esc2[j-1]);
            eindex+=f*f*es[j-1]*es[j-1];
          }
        }
      }
      else //es una banda de continuo
      {
        if(lerr)
        {
          for (long jj=j1[nb]; jj<=j2[nb]+1; jj++)
          {
            double f;
            if (jj == j1[nb])
              f=1.0-d1[nb];
            else if (jj == j2[nb]+1)
              f=d2[nb];
            else
              f=1.0;
            sigma2=es[jj-1]*es[jj-1];
            double fduma=(sum0*static_cast<double>(jj)/sigma2-sumx/sigma2)
                         /deter;
            double fdumb=(sumxx/sigma2-sumx*static_cast<double>(jj)/sigma2)
                         /deter;
            double fdum=sumli*fduma+sumni*fdumb;
            eindex+=f*f*fdum*fdum*sigma2;
          }
        }
      }
    }
    findex=findex*cdelt1*smean;
    eindex=sqrt(eindex)*cdelt1*smean;
    if(logindex)
    {
      //no tiene sentido
    }
    delete [] sc;
    delete [] esc2;
  }
  //---------------------------------------------------------------------------
  //Discontinuidades genericas
  //---------------------------------------------------------------------------
  else if ( (myindex.gettype() >= 11) && (myindex.gettype() <= 99) )
  {
    //proteccion
    if ( contperc >= 0 )
    {
      cout << "ERROR: contperc has not been implemented yet for this index"
           << endl;
      exit(1);
    }
    long nconti = myindex.getnconti();
    long nlines = myindex.getnlines();
    //calculamos flujo promedio en las bandas de continuo
    double fconti=0.0;
    double econti2=0.0;
    double rltot_conti=0.0;
    for (long nb=0; nb < nconti; nb++)
    {
      for (long j=j1[nb]; j<=j2[nb]+1; j++)
      {
        double f;
        if (j == j1[nb])
          f=1.0-d1[nb];
        else if (j == j2[nb]+1)
          f=d2[nb];
        else
          f=1.0;
        fconti+=f*s[j-1];
        if(lerr) econti2+=f*f*es[j-1]*es[j-1];
      }
      rltot_conti+=rl[nb];
    }
    fconti*=cdelt1;
    fconti/=rltot_conti;
    if(lerr)
    {
      econti2*=cdelt1*cdelt1;
      econti2/=(rltot_conti*rltot_conti);
    }
    //calculamos flujo promedio en las bandas de absorcion
    double flines=0.0;
    double elines2=0.0;
    double rltot_lines=0.0;
    for (long nb=0; nb < nlines; nb++)
    {
      for (long j=j1[nconti+nb]; j<=j2[nconti+nb]+1; j++)
      {
        double f;
        if (j == j1[nconti+nb])
          f=1.0-d1[nconti+nb];
        else if (j == j2[nconti+nb]+1)
          f=d2[nconti+nb];
        else
          f=1.0;
        flines+=f*s[j-1];
        if(lerr) elines2+=f*f*es[j-1]*es[j-1];
      }
      rltot_lines+=rl[nconti+nb];
    }
    flines*=cdelt1;
    flines/=rltot_lines;
    if(lerr)
    {
      elines2*=cdelt1*cdelt1;
      elines2/=(rltot_lines*rltot_lines);
    }
    //calculamos el indice
    findex=flines/fconti;
    if(lerr)
    {
      eindex=sqrt(fconti*fconti*elines2+flines*flines*econti2)/(fconti*fconti);
    }
    if(logindex) //si medimos en escala logaritmica
    {
      eindex=cte_log_exp*eindex/findex;
      findex=2.5*log10(findex);
    }
    //=========================================================================
    //dibujamos
    if(plotmode !=0)
    {
      cpgsci(6);
      //flujo medio en las bandas de continuo
      double wlmin=myindex.getldo1(0)*rcvel1;
      double wlmax=myindex.getldo2(0)*rcvel1;
      if (nconti > 1)
      {
        for (long nb=1; nb < nconti; nb++)
        {
          double wl1=myindex.getldo1(nb)*rcvel1;
          double wl2=myindex.getldo2(nb)*rcvel1;
          if(wl1 < wlmin)
            wlmin=wl1;
          if(wl2 > wlmin)
            wlmax=wl2;
        }
      }
      cpgmove_d((wlmin-crval1)/cdelt1+crpix1,fconti*smean);
      cpgdraw_d((wlmax-crval1)/cdelt1+crpix1,fconti*smean);
      //flujo medio en las bandas con lineas
      wlmin=myindex.getldo1(nconti+0)*rcvel1;
      wlmax=myindex.getldo2(nconti+0)*rcvel1;
      if (nlines > 1)
      {
        for (long nb=1; nb < nlines; nb++)
        {
          double wl1=myindex.getldo1(nconti+nb)*rcvel1;
          double wl2=myindex.getldo2(nconti+nb)*rcvel1;
          if(wl1 < wlmin)
            wlmin=wl1;
          if(wl2 > wlmin)
            wlmax=wl2;
        }
      }
      cpgmove_d((wlmin-crval1)/cdelt1+crpix1,flines*smean);
      cpgdraw_d((wlmax-crval1)/cdelt1+crpix1,flines*smean);
      cpgsci(1);
    }//========================================================================
  }
  //---------------------------------------------------------------------------
  //Indices genericos
  //---------------------------------------------------------------------------
  else if ( (myindex.gettype() >= 101) && (myindex.gettype() <= 9999) )
  {
    //proteccion
    if ( contperc >= 0 )
    {
      cout << "ERROR: contperc has not been implemented yet for this index"
           << endl;
      exit(1);
    }
    long nconti = myindex.getnconti();
    long nlines = myindex.getnlines();
    //si no hay errores, hacemos todos iguales a uno para utilizar las mismas
    //formulas
    if(!lerr)
      for (long j = j1min; j <= j2max+1; j++)
        es[j-1]=1.0;
    //calculamos la recta del continuo mediante minimos cuadrados (y=amc*x+bmc)
    //(para la variable x usamos el numero de pixel en lugar de la longitud
    //de onda porque, en principio, seran numeros mas pequenos)
    double sigma2;
    double sum0=0.0;
    double sumx=0.0;
    double sumy=0.0;
    double sumxy=0.0;
    double sumxx=0.0;
    for (long nb=0; nb < nconti; nb++)
    {
      for (long j=j1[nb]; j<=j2[nb]+1; j++)
      {
        double f;
        if (j == j1[nb])
          f=1.0-d1[nb];
        else if (j == j2[nb]+1)
          f=d2[nb];
        else
          f=1.0;
        sigma2=es[j-1]*es[j-1];
        sum0+=f/sigma2;
        sumx+=f*static_cast<double>(j)/sigma2;
        sumy+=f*s[j-1]/sigma2;
        sumxy+=f*static_cast<double>(j)*s[j-1]/sigma2;
        sumxx+=f*static_cast<double>(j)*static_cast<double>(j)/sigma2;
      }
    }
    double deter=sum0*sumxx-sumx*sumx;
    double amc=(sum0*sumxy-sumx*sumy)/deter;
    double bmc=(sumxx*sumy-sumx*sumxy)/deter;
    //calculamos el pseudo-continuo
    double *sc = new double [naxis1];
    double *esc2 = new double [naxis1];
    for (long j = j1min; j <= j2max+1; j++)
    {
      sc[j-1] = amc*static_cast<double>(j)+bmc;
    }
    if(lerr)
    {
      for (long j = j1min; j <= j2max+1; j++)
      {
        esc2[j-1] = 0.0;
        for (long nb=0; nb < nconti; nb++)
        {
          for (long jj=j1[nb]; jj<=j2[nb]+1; jj++)
          {
            sigma2=es[jj-1]*es[jj-1];
            double fdum=(sum0*static_cast<double>(jj)/sigma2-sumx/sigma2)*
                        static_cast<double>(j)/deter+
                        (sumxx/sigma2-
                         sumx*static_cast<double>(jj)/sigma2)/deter;
            esc2[j-1]+=fdum*fdum*sigma2;
          }
        }
      }
    }
    //recorremos las bandas con lineas
    double tc=0.0;
    double etc=0.0;
    double sumrl=0.0;
    for (long nb=0; nb < nlines; nb++)
    {
      double factor = myindex.getfactor(nb);
      for (long j=j1[nb+nconti]; j<=j2[nb+nconti]+1; j++)
      {
        double f;
        if (j == j1[nb+nconti])
          f=1.0-d1[nb+nconti];
        else if (j == j2[nb+nconti]+1)
          f=d2[nb+nconti];
        else
          f=1.0;
        tc+=f*factor*s[j-1]/sc[j-1];
        if(lerr)
        {
          etc+=f*f*factor*factor*(s[j-1]*s[j-1]*esc2[j-1]+
               sc[j-1]*sc[j-1]*es[j-1]*es[j-1])/
               (sc[j-1]*sc[j-1]*sc[j-1]*sc[j-1]);
          for (long nbb=0; nbb < nlines; nbb++)
          {
            double factorr = myindex.getfactor(nbb);
            for (long jj=j1[nbb+nconti]; jj<=j2[nbb+nconti]+1; jj++)
            {
              if ( (nbb != nb) || (jj != j) )
              {
                double ff;
                if (jj == j1[nbb+nconti])
                  ff=1.0-d1[nbb+nconti];
                else if (jj == j2[nbb+nconti]+1)
                  ff=d2[nbb+nconti];
                else
                  ff=1.0;
                double cov=static_cast<double>(j)*static_cast<double>(jj)*
                           (sum0*sum0*sumxx-sum0*sumx*sumx)/(deter*deter)+
                           (static_cast<double>(j)+static_cast<double>(jj))*
                           (sumx*sumx*sumx-sum0*sumx*sumxx)/(deter*deter)+
                           (sumxx*sumxx*sum0-sumxx*sumx*sumx)/(deter*deter);
                etc+=factor*factorr*ff*f*s[j-1]*s[jj-1]*cov/
                     (sc[j-1]*sc[j-1]*sc[j-1]*sc[j-1]);
              }
            }
          }
        }
      }
      sumrl+=factor*rl[nb+nconti];
    }
    if(logindex) //indice generico medido en magnitudes
    {
      findex=-2.5*log10(tc*cdelt1/sumrl);
      if(lerr) eindex=cte_log_exp/pow(10,-0.4*findex)*sqrt(etc)*cdelt1/sumrl;
    }
    else //indice generico medido como indice atomico
    {
      findex=(sumrl-tc*cdelt1)/rcvel1;
      if(lerr) eindex=sqrt(etc)*cdelt1/rcvel1;
    }
    delete [] sc;
    delete [] esc2;
    //=========================================================================
    //dibujamos
    if(plotmode !=0)
    {
      cpgsci(6);
      const double wla=wvmin*rcvel1;
      const double wlb=wvmax*rcvel1;
      const double xca=(wla-crval1)/cdelt1+crpix1;
      const double xcb=(wlb-crval1)/cdelt1+crpix1;
      const double yduma=amc*static_cast<double>(xca)+bmc; 
      const double ydumb=amc*static_cast<double>(xcb)+bmc; 
      cpgmove_d(xca,yduma*smean);
      cpgdraw_d(xcb,ydumb*smean);
      cpgsci(1);
    }//=========================================================================
  }
  //---------------------------------------------------------------------------
  //Indices pendiente
  //---------------------------------------------------------------------------
  else if ( (myindex.gettype() >= -99) && (myindex.gettype() <= -2) )
  {
    //proteccion
    if ( contperc >= 0 )
    {
      cout << "ERROR: contperc has not been implemented yet for this index"
           << endl;
      exit(1);
    }
    long nconti = myindex.getnconti();
    //si no hay errores, hacemos todos iguales a uno para utilizar las mismas
    //formulas
    if(!lerr)
      for (long j = j1min; j <= j2max+1; j++)
        es[j-1]=1.0;
    //calculamos la recta del continuo mediante minimos cuadrados (y=amc*x+bmc)
    //(para la variable x usamos el numero de pixel en lugar de la longitud
    //de onda porque, en principio, seran numeros mas pequenos)
    double sigma2;
    double sum0=0.0;
    double sumx=0.0;
    double sumy=0.0;
    double sumxy=0.0;
    double sumxx=0.0;
    for (long nb=0; nb < nconti; nb++)
    {
      for (long j=j1[nb]; j<=j2[nb]+1; j++)
      {
        double f;
        if (j == j1[nb])
          f=1.0-d1[nb];
        else if (j == j2[nb]+1)
          f=d2[nb];
        else
          f=1.0;
        sigma2=es[j-1]*es[j-1];
        sum0+=f/sigma2;
        sumx+=f*static_cast<double>(j)/sigma2;
        sumy+=f*s[j-1]/sigma2;
        sumxy+=f*static_cast<double>(j)*s[j-1]/sigma2;
        sumxx+=f*static_cast<double>(j)*static_cast<double>(j)/sigma2;
      }
    }
    double deter=sum0*sumxx-sumx*sumx;
    double amc=(sum0*sumxy-sumx*sumy)/deter;
    double bmc=(sumxx*sumy-sumx*sumxy)/deter;
    double xa=0.0,xb=0.0;
    for (long nb=0; nb < nconti; nb++)
    {
      double ca = myindex.getldo1(nb)*rcvel1;
      double cb = myindex.getldo2(nb)*rcvel1;
      double c3 = (ca-wlmin)/cdelt1+1.0;
      double c4 = (cb-wlmin)/cdelt1;
      if (nb==0)
      {
        xa=(c3+c4)/2.0;
        xb=(c3+c4)/2.0;
      }
      else
      {
        double xa_,xb_;
        xa_=(c3+c4)/2.0;
        xb_=(c3+c4)/2.0;
        if(xa_ > xa) xa=xa_;
        if(xb_ < xb) xb=xb_;
      }
    }
    double flujoa=amc*xa+bmc;
    double flujob=amc*xb+bmc;
    findex=flujoa/flujob;
    if(lerr)
    {
      double covar_aa=(sum0*sum0*sumxx-sum0*sumx*sumx)/deter/deter;
      double covar_ab=(sumx*sumx*sumx-sum0*sumx*sumxx)/deter/deter;
      double covar_bb=(sumxx*sumxx*sum0-sumxx*sumx*sumx)/deter/deter;
      eindex=sqrt(bmc*bmc*covar_aa+amc*amc*covar_bb-2.0*amc*bmc*covar_ab)*
             (xa-xb)/((amc*xb+bmc)*(amc*xb+bmc));
    }
    if(logindex) //indice pendiente medido en magnitudes
    {
      eindex=cte_log_exp*eindex/findex;
      findex=2.5*log10(findex);
    }
    //=========================================================================
    //dibujamos
    if(plotmode !=0)
    {
      cpgsci(6);
      const double wla=wvmin*rcvel1;
      const double wlb=wvmax*rcvel1;
      const double xca=(wla-crval1)/cdelt1+crpix1;
      const double xcb=(wlb-crval1)/cdelt1+crpix1;
      const double yduma=amc*static_cast<double>(xca)+bmc; 
      const double ydumb=amc*static_cast<double>(xcb)+bmc; 
      cpgmove_d(xca,yduma*smean);
      cpgdraw_d(xcb,ydumb*smean);
      cpgsci(1);
    }//========================================================================
  }
  //---------------------------------------------------------------------------
  //otros indices futuros
  //---------------------------------------------------------------------------
  else
  {
    cout << "FATAL ERROR: index type=" << myindex.gettype()
         << " is not valid" << endl;
    exit(1);
  }

  //---------------------------------------------------------------------------
  //liberamos memoria y retornamos con exito
  delete [] ca;
  delete [] cb;
  delete [] c3;
  delete [] c4;
  delete [] j1;
  delete [] j2;
  delete [] d1;
  delete [] d2;
  delete [] rl;
  delete [] rg;
  delete [] ifchan;
  delete [] s;
  delete [] es;
  return(true);
}
