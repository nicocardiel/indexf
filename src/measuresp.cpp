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
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <time.h>
#include "indexparam.h"
#include "indexdef.h"
#include "scidata.h"
#include "cpgplot.h"
#include "cpgplot_d.h"

using namespace std;

bool mideindex(const bool &, const double *, const double *, 
               const long &,
               const double &, const double &, const double &,
               const IndexDef &,
               const long &,
               const long &,
               const bool &,
               const double &, const double &,
               const double &, const double &,
               const long &, const long &,
               bool &, bool &,
               double &, double &, double &);

bool fmean(const long, const double *, const bool *, double *, double *);

void outmeasurement(const long &,
                    const double &, const double &, 
                    const double &,
                    const double &, const double &,
                    const double &, const double &,
                    const char *,
                    const bool &,
                    const bool &, const bool &,
                    const bool &, const bool &);

bool measuresp(SciData *imagePtr, IndexParam param, IndexDef myindex)
{
  const long nseed = param.get_nseed();
  if(nseed == 0)
  {
    srand(time(0)); //aleatorizamos la semilla de los numeros aleatorios
  }
  else
  {
    srand(nseed); //usamos semilla proporcionada por el usuario
  }
  const double pi2 = 4.*acos(0.0);
  const double sqrt2 = sqrt(2.0);
  const bool lerr = ( strcmp(imagePtr->getfilename_error(),"undef") != 0 );
  const double *const my2Ddata = imagePtr->getdata();
  const double *my2Derror = NULL;
  //my2Derror no puede declararse como "const double *const" porque de lo
  //contrario no puede modificarse en la siguiente linea
  if ( lerr ) my2Derror = imagePtr->geterror();
  //OJO: la inicializacion a cero no funciona con el compilador de Solaris;
  //     por eso estan comentados los finales de las siguientes 2 lineas:
  double *sp_data = new double [imagePtr->getnaxis1()];// = { 0 };
  double *sp_error = new double [imagePtr->getnaxis1()];// = { 0 };
  const double crval1 = imagePtr->getcrval1();
  const double cdelt1 = imagePtr->getcdelt1();
  const double crpix1 = imagePtr->getcrpix1();
  //definimos parametros adicionales
  const long contperc = param.get_contperc();
  const long boundfit = param.get_boundfit();
  const long plotmode = param.get_plotmode();
  const long plottype = param.get_plottype();
  //recorremos, extraemos y medimos los espectros requeridos
  double findex, eindex, sn, findex_rv, eindex_rv, findex_sn, eindex_sn;
  if (plotmode != 0)
  {
    const long nwinx=param.get_nwinx();
    const long nwiny=param.get_nwiny();
    cpgbeg(0,param.get_grdev(),nwinx,nwiny);
    /*//inicio colores para el poster de la SEA2006
    cpgscr(0,230./255.,225./255.,207./255.);
    cpgscr(1,000./255.,000./255.,000./255.);
    cpgscr(2,230./255.,161./255.,115./255.);
    cpgscr(3,128./255.,115./255.,064./255.);
    cpgscr(4,092./255.,115./255.,184./255.);
    cpgscr(5,064./255.,080./255.,128./255.);
    cpgscr(6,191./255.,163./255.,048./255.);
    cpgscr(7,230./255.,216./255.,207./255.);
    cpgscr(15,230./255.,207./255.,115./255.);
    //fin colores para el poster de la SEA2006 */
    cpgask(false);
    cpgsch(1.4);
    if((nwinx == 1) && (nwiny == 1))
    {
      cpgslw(2);
    }
  }
  for (long ns = param.get_ns1(); ns <= param.get_ns2(); ns++)
  {
    bool out_of_limits,negative_error;
    long i1=(ns-1)*imagePtr->getnaxis1()+1;
    long i2=i1+imagePtr->getnaxis1()-1;
    for ( long i = i1; i <= i2; i++ )
      sp_data[i-i1] = my2Ddata[i-1];
    if ( lerr )
    {
      for ( long i = i1; i <= i2; i++ )
        sp_error[i-i1] = my2Derror[i-1];
    }
    else
    {
    }
    const double rvel = imagePtr->getrvel()[ns-1];
    const double rvelerr = imagePtr->getrvelerr()[ns-1];
    const bool logindex = param.get_logindex();
    const double biaserr = param.get_biaserr();
    const double linearerr = param.get_linearerr();
    bool lfindex = mideindex(lerr,sp_data,sp_error,imagePtr->getnaxis1(),
                             crval1,cdelt1,crpix1,myindex,
                             contperc,boundfit,
                             logindex,rvel,rvelerr,
                             biaserr,linearerr,
                             plotmode,plottype,
                             out_of_limits,negative_error,
                             findex,eindex,sn);
    if ((plotmode != 0) && (plottype >= 1))
    {
      //nombre del indice que se esta midiendo
      cpgsci(3);
      cpgmtxt("t",6.5,0.0,0.0,param.get_index());
      //numero de espectro
      cpgsci(6);
      ostringstream snumber;
      snumber << param.get_if() << " #" << ns;
      long lsize = (snumber.str()).length();
      char *snumberPtr = new char[lsize+1];
      (snumber.str()).copy(snumberPtr,lsize,0);
      snumberPtr[lsize]='\0';
      cpgmtxt("t",6.5,1.0,1.0,snumberPtr);
      cpgsci(1);
      delete [] snumberPtr;
    }
    //si hay error en velocidad radial, hacemos simulaciones numericas
    eindex_rv=0;
    bool leindex_rv = true;
    if( (lfindex) && (rvelerr > 0) && (param.get_nsimul() > 0) )
    {
      double *findex_sim = new double [param.get_nsimul()];
      bool *iffindex_sim = new bool [param.get_nsimul()];
      const double fRAND_MAX = static_cast<double>(RAND_MAX);
      for (long nsimul=1; nsimul <= param.get_nsimul(); nsimul++)
      {
        long iran; //evitamos obtener ran1=1 y ran2=1
        while ( (iran=rand()) == RAND_MAX);
        const double ran1 = static_cast<double>(iran)/fRAND_MAX;
        while ( (iran=rand()) == RAND_MAX);
        const double ran2 = static_cast<double>(iran)/fRAND_MAX;
        const double delta_rvel=sqrt2*rvelerr*
                                sqrt(-1*log(1-ran1))*cos(pi2*ran2);
        const double rvel_eff = rvel+delta_rvel;
        const bool logindex = param.get_logindex();
        double eindex_sim,sn_sim;
        bool out_of_limits_sim,negative_error_sim;
        iffindex_sim[nsimul-1]=
          mideindex(lerr,sp_data,sp_error,imagePtr->getnaxis1(),
                    crval1,cdelt1,crpix1,myindex,
                    contperc,boundfit,
                    logindex,rvel_eff,rvelerr,
                    biaserr,linearerr,
                    0,plottype, //no queremos plots (salvo continuo)
                    out_of_limits_sim,negative_error_sim,
                    findex_sim[nsimul-1],eindex_sim,sn_sim);
      }
      leindex_rv=fmean(param.get_nsimul(),findex_sim,iffindex_sim,
                       &findex_rv,&eindex_rv);
      delete [] findex_sim;
      delete [] iffindex_sim;
    }
    const char* labelsp = imagePtr->getlabelsp()[ns-1];
    outmeasurement(ns,findex,eindex,sn,
                   rvel,rvelerr,findex_rv,eindex_rv,labelsp,
                   lfindex,lerr,leindex_rv,out_of_limits,negative_error);
    //si se ha solicitado, se realizan las simulaciones con S/N variable
    //(usamos escala logaritmica en S/N para tener una distribucion
    //homogenea de puntos al calcular las constantes de los errores)
    eindex_sn=0;
    bool leindex_sn = true;
    if( (lfindex) && (param.get_nsimulsn() > 0) )
    {
      const double fRAND_MAX = static_cast<double>(RAND_MAX);
      const double minsn_pixel=log10(param.get_minsn()*sqrt(cdelt1));
      const double deltasn_pixel=log10(param.get_maxsn()*sqrt(cdelt1))-
                                 minsn_pixel;
      for (long nsimulsn=1; nsimulsn <= param.get_nsimulsn(); nsimulsn++)
      {
        const double ran = static_cast<double>(rand())/fRAND_MAX;
        const double sn_pixel_simul = pow(10.0,minsn_pixel+ran*deltasn_pixel);
        const double sn_Ang_simul = sn_pixel_simul/sqrt(cdelt1);
        for ( long i = i1; i <= i2; i++ )
          sp_error[i-i1]=sp_data[i-i1]/sn_pixel_simul;
        double *findex_sim = new double [param.get_nsimul()];
        bool *iffindex_sim = new bool [param.get_nsimul()];
        for (long nsimul=1; nsimul <= param.get_nsimul(); nsimul++)
        {
          double *sp_data_eff = new double [imagePtr->getnaxis1()];
          for ( long i = i1; i <= i2; i++ )
          {
            long iran; //evitamos obtener ran1=1 y ran2=1
            while ( (iran=rand()) == RAND_MAX);
            const double ran1 = static_cast<double>(iran)/fRAND_MAX;
            while ( (iran=rand()) == RAND_MAX);
            const double ran2 = static_cast<double>(iran)/fRAND_MAX;
            const double delta_data=sqrt2*sp_error[i-i1]*
                                    sqrt(-1*log(1-ran1))*cos(pi2*ran2);
            sp_data_eff[i-i1]=sp_data[i-i1]+delta_data;
          }
          const bool logindex = param.get_logindex();
          double eindex_sim,sn_sim;
          bool out_of_limits_sim,negative_error_sim;
          iffindex_sim[nsimul-1]=
            mideindex(lerr,sp_data_eff,sp_error,imagePtr->getnaxis1(),
                      crval1,cdelt1,crpix1,myindex,
                      contperc,boundfit,
                      logindex,rvel,rvelerr,
                      biaserr,linearerr,
                      0,0, //no queremos plots
                      out_of_limits_sim,negative_error_sim,
                      findex_sim[nsimul-1],eindex_sim,sn_sim);
          delete [] sp_data_eff;
        }
        leindex_sn=fmean(param.get_nsimul(),findex_sim,iffindex_sim,
                         &findex_sn,&eindex_sn);
        const char* label_false_NULL = " ";
        cout << endl;
        outmeasurement(-nsimulsn,findex_sn,eindex_sn,sn_Ang_simul,
                       rvel,0.0,0.0,0.0,label_false_NULL,
                       lfindex,true,false,false,false);
        delete [] findex_sim;
        delete [] iffindex_sim;
      }
    }
    if ((plotmode == 1) || (plotmode == -1))
    {
      char cpause;
      cin.get(cpause);
    }
    else
    {
      cout << endl;
    }
  }
  if (plotmode != 0)
  {
    cpgend();
  }
  delete [] sp_data;
  delete [] sp_error;
  return(true);
}

//-----------------------------------------------------------------------------
void outmeasurement(const long & ns,
                    const double & findex, const double & eindex, 
                    const double & sn,
                    const double & rvel, const double & rvelerr,
                    const double & findex_rv, const double & eindex_rv,
                    const char * labelsp,
                    const bool & lfindex,
                    const bool & lerr, const bool & leindex_rv,
                    const bool & out_of_limits, const bool & negative_error)
{
  //formateamos la salida
  ostringstream srvel, srvelerr;
  srvel.setf(ios::fixed);
  srvel << setprecision(1) << setw(9) << rvel;
  srvelerr.setf(ios::fixed);
  srvelerr << setprecision(1) << setw(9) << rvelerr;
 
  ostringstream snscan;
  if(ns < 0)
  {
    snscan << setw(1) << setiosflags(ios::left) << "S" << 
              setw(4) << setiosflags(ios::right) << -ns;
  }
  else
  {
    snscan << setw(5) << ns;
  }
  
  ostringstream sfindex, seindex, sfindex_rv, seindex_rv, ssn;
  if (lfindex)
  {
    sfindex.setf(ios::fixed);
    sfindex << setprecision(4) << setw(10) << setiosflags(ios::showpoint) 
            << findex;
    if (lerr)
    {
      seindex.setf(ios::fixed);
      seindex << setprecision(4) << setw(10) << setiosflags(ios::showpoint) 
              << eindex;
      ssn.setf(ios::fixed);
      ssn << setprecision(2) << setw(8) << setiosflags(ios::showpoint) << sn;
    }
    else
    {
      seindex << setw(10) << "undef0";
      ssn << setw(8) << "undef0";
    }
    if (rvelerr > 0)
    {
      if (leindex_rv)
      {
        sfindex_rv.setf(ios::fixed);
        sfindex_rv << setprecision(4) << setw(10) << setiosflags(ios::fixed) 
                   << findex_rv;
        seindex_rv.setf(ios::fixed);
        seindex_rv << setprecision(4) << setw(10) << setiosflags(ios::fixed) 
                   << eindex_rv;
      }
      else
      {
        sfindex_rv << setw(10) << "undef1";
        seindex_rv << setw(10) << "undef1";
      }
    }
    else
    {
      sfindex_rv << setw(10) << "undef4";
      seindex_rv << setw(10) << "undef4";
    }
  }
  else
  {
    char tipo_error[7];
    if(out_of_limits)
      strcpy(tipo_error,"undef2");
    else if(negative_error)
      strcpy(tipo_error,"undef3");
    sfindex << setw(10) << tipo_error;
    seindex << setw(10) << tipo_error;
    sfindex_rv << setw(10) << tipo_error;
    seindex_rv << setw(10) << tipo_error;
    ssn << setw(8) << tipo_error;
  }
  //mostramos salida
  //cout << setw(5) << ns << " "
  cout << snscan.str() << " "
       << sfindex.str() << " "
       << seindex.str() << " "
       << ssn.str() << " "
       << srvel.str() << " " << srvelerr.str()
       << sfindex_rv.str() << " " << seindex_rv.str() << "  "
       << labelsp;
}
