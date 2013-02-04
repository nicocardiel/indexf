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

//Declaración de la clase IndexParam
//Las funciones miembro se definen en indexparam.cpp

#ifndef INDEXPARAM_H
#define INDEXPARAM_H

class IndexParam{
  public:
    IndexParam(); //constructor por defecto
    IndexParam(   //constructor con parámetros
      char *,long,long, //input file, first and last spectrum (0,0=all)
      char *,           //index name
      char *,           //input error file
      long,             //percentile for continuum (-1=use mean fluxes)
      long,             //boundary fit for continuum (0=use mean fluxes)
      char *,           //sn estimation file
      double,double,    //radial velocity and error
      char *,long,long, //rad. vel. file name, column data & error
      bool,             //vacuum wavelength scale in spectra
      long,             //number of simulations (radial velocity error)
      bool,             //measure indices in logarithmic units
      bool,             //verbose
      long,             //number of simulations (different S/N ratios)
      double,double,    //minimum,maximum S/N ratios in simulations
      char *,           //input label file (with labels for each spectrum)
      long,long,        //first and last character for labels (0,0=all)
      double,           //bias error (additive % of the continuum level)
      double,           //linearity error
      long,             //plot mode (0:none, 1:pause, 2:nopause; -:error bars)
      char *,           //PGPLOT graphics device
      long,long,        //number of panels (NX,NY) in the plotting device
      long,             //plot type (0: simple plot; 1: with additional info.)
      long,             //seed for random numbers (0=use computer time)
      double);          //flux scale factor (measured spectrum=original/fscale)
    void set_if(const char *);
    void set_ns1(const long);
    void set_ns2(const long);
    void set_index(const char *);
    void set_ief(const char *);
    void set_contperc(const long);
    void set_boundfit(const long);
    void set_snf(const char *);
    void set_rv(const double, const double);
    void set_rvf(const char *,const long, const long);
    void set_vacuum(const bool);
    void set_nsimul(const long);
    void set_nindex(const long);
    void set_of(const char *);
    void set_logindex(const bool);
    void set_verbose(const bool);
    void set_nsimulsn(const long);
    void set_minmaxsn(const double, const double);
    void set_ilabfile(const char *);
    void set_nchar1(const long);
    void set_nchar2(const long);
    void set_biaserr(const double);
    void set_linearerr(const double);
    void set_plotmode(const long);
    void set_grdev(const char *);
    void set_nwindows(const long, const long);
    void set_plottype(const long);
    void set_nseed(const long);
    void set_fscale(const double);
    char *get_if();
    long get_ns1();
    long get_ns2();
    char *get_index();
    char *get_ief();
    long get_contperc();
    long get_boundfit();
    char *get_snf();
    double get_rv();
    double get_rve();
    char *get_rvfile();
    long get_rvc();
    long get_rvce();
    bool get_vacuum();
    long get_nsimul();
    long get_nindex();
    bool get_logindex();
    bool get_verbose();
    long get_nsimulsn();
    double get_minsn();
    double get_maxsn();
    char *get_ilabfile();
    long get_nchar1();
    long get_nchar2();
    double get_biaserr();
    double get_linearerr();
    long get_plotmode();
    char *get_grdev();
    long get_nwinx();
    long get_nwiny();
    long get_plottype();
    long get_nseed();
    double get_fscale();
  private:
    char ifile[256];
    char index[9];;
    long ns1,ns2;
    char iefile[256];
    long contperc;
    long boundfit;
    char snfile[256];
    double rv,rve;
    char rvfile[256];
    long rvc, rvce;
    bool vacuum;
    long nsimul;
    long nindex;
    bool logindex;
    bool verbose;
    long nsimulsn;
    double minsn,maxsn;
    char ilabfile[256];
    long nchar1,nchar2;
    double biaserr;
    double linearerr;
    long plotmode;
    char grdev[256];
    long nwinx,nwiny;
    long plottype;
    long nseed;
    double fscale;
};

#endif
