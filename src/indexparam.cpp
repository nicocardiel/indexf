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

//Definicion de funciones miembro de la clase IndexParam, declarada en 
//indexparam.h
#include <iostream>
#include <string.h>
#include "indexparam.h"

using namespace std;

//-----------------------------------------------------------------------------
//constructor por defecto
IndexParam::IndexParam()
{
  ifile[0] = '\0';
  ns1 = 1;
  ns2 = 1;
  index[0] = '\0';
  iefile[0] = '\0';
  contperc = -1;
  snfile[0] = '\0';
  rv = 0.0;
  rve = 0.0;
  rvfile[0] = '\0';
  rvc = 1;
  rvce = 0;
  nsimul = 100;
  logindex = false;
  verbose = true;
  nsimulsn = 0;
  minsn = 1.0;
  maxsn = 100.0;
  ilabfile[0] = '\0';
  nchar1 = 0;
  nchar2 = 0;
  biaserr = 0.0;
  linearerr = 0.0;
  plotmode = 0;
  grdev[0] = '\0';
  nwinx = 1;
  nwiny = 1;
  plottype = 0;
  nseed = 0;
  fscale = 1.0;
}

//-----------------------------------------------------------------------------
//constructor con parametros
IndexParam::IndexParam(
  char *ifile_,long ns1_,long ns2_,//input file, first and last spectrum
  char *index_,                 //index name
  char *iefile_,                //input error file
  long contperc_,               //percentile for continuum (-1=use normal mean)
  char *snfile_,                //sn estimation file
  double rv_,double rve_,       //radial velocity and error
  char *rvfile_,long rvc_,long rvce_,//rad. vel. file name, column data & error
  long nsimul_,                 //number of simulations (rad. velocity errors)
  bool logindex_,               //logindex
  bool verbose_,                //verbose   
  long nsimulsn_,               //number of simulations (different S/N ratios)
  double minsn_,double maxsn_,  //minimum,maximum S/N ratios in simulations
  char *ilabfile_,              //input label file                          
  long nchar1_,long nchar2_,    //first and last character for labels
  double biaserr_,              //bias error
  double linearerr_,            //linearity error
  long plotmode_,               //plot mode (1:pause, 2:no pause; -:error bars)
  char *grdev_,                 //PGPLOT graphics device
  long nwinx_,long nwiny_,      //number of panels (NX,NY) in plotting device
  long plottype_,               //number of panels (NX,NY) in plotting device
  long nseed_,                  //seed for random numbers (0=use computer time)
  double fscale_)     //flux scale factor (measured spectrum = original/fscale)
{
  set_if(ifile_);
  set_ns1(ns1_);
  set_ns2(ns2_);
  set_index(index_);
  set_ief(iefile_);
  set_contperc(contperc_);
  set_snf(snfile_);
  set_rv(rv_,rve_);
  set_rvf(rvfile_,rvc_,rvce_);
  set_nsimul(nsimul_);
  set_logindex(logindex_);
  set_verbose(verbose_);
  set_nsimulsn(nsimulsn_);
  set_minmaxsn(minsn_,maxsn_);
  set_ilabfile(ilabfile_);
  set_nchar1(nchar1_);
  set_nchar2(nchar2_);
  set_biaserr(biaserr_);
  set_linearerr(linearerr_);
  set_plotmode(plotmode_);
  set_grdev(grdev_);
  set_nwindows(nwinx_,nwiny_);
  set_plottype(plottype_);
  set_nseed(nseed_);
  set_fscale(fscale_);
}

//-----------------------------------------------------------------------------
void IndexParam::set_if(const char *ifile_)
{
  strncpy(ifile,ifile_,strlen(ifile_));
  ifile[strlen(ifile_)]='\0';
}

//-----------------------------------------------------------------------------
void IndexParam::set_ns1(const long ns1_)
{
  ns1 = ns1_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_ns2(const long ns2_)
{
  ns2 = ns2_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_index(const char *index_)
{
  strncpy(index,index_,strlen(index_));
  index[strlen(index_)] = '\0';
}

//-----------------------------------------------------------------------------
void IndexParam::set_ief(const char *iefile_)
{
  strncpy(iefile,iefile_,strlen(iefile_));
  iefile[strlen(iefile_)]='\0';
}

//-----------------------------------------------------------------------------
void IndexParam::set_contperc(const long contperc_)
{
  contperc = contperc_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_snf(const char *snfile_)
{
  strncpy(snfile,snfile_,strlen(snfile_));
  snfile[strlen(snfile_)]='\0';
}

//-----------------------------------------------------------------------------
void IndexParam::set_rv(const double rv_, const double rve_)
{
  rv = rv_;
  rve = rve_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_rvf(const char *rvfile_, const long rvc_, const long rvce_)
{
  strncpy(rvfile,rvfile_,strlen(rvfile_));
  rvfile[strlen(rvfile_)]='\0';
  rvc = rvc_;
  rvce = rvce_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_nsimul(const long nsimul_)
{
  nsimul=nsimul_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_nindex(const long nindex_)
{
  nindex = nindex_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_logindex(const bool logindex_)
{
  logindex=logindex_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_verbose(const bool verbose_)
{
  verbose=verbose_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_nsimulsn(const long nsimulsn_)
{
  nsimulsn=nsimulsn_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_minmaxsn(const double minsn_, const double maxsn_)
{
  minsn = minsn_;
  maxsn = maxsn_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_ilabfile(const char *ilabfile_)
{
  strncpy(ilabfile,ilabfile_,strlen(ilabfile_));
  ilabfile[strlen(ilabfile_)]='\0';
}

//-----------------------------------------------------------------------------
void IndexParam::set_nchar1(const long nchar1_)
{
  nchar1 = nchar1_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_nchar2(const long nchar2_)
{
  nchar2 = nchar2_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_biaserr(const double biaserr_)
{
  biaserr=biaserr_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_linearerr(const double linearerr_)
{
  linearerr=linearerr_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_plotmode(const long plotmode_)
{
  plotmode=plotmode_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_grdev(const char *grdev_)
{
  strncpy(grdev,grdev_,strlen(grdev_));
  grdev[strlen(grdev_)]='\0';
}

//-----------------------------------------------------------------------------
void IndexParam::set_nwindows(const long nwinx_, const long nwiny_)
{
  nwinx=nwinx_;
  nwiny=nwiny_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_plottype(const long plottype_)
{
  plottype=plottype_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_nseed(const long nseed_)
{
  nseed=nseed_;
}

//-----------------------------------------------------------------------------
void IndexParam::set_fscale(const double fscale_)
{
  fscale=fscale_;
}

//-----------------------------------------------------------------------------
char *IndexParam::get_if() {return(ifile);}

//-----------------------------------------------------------------------------
long IndexParam::get_ns1() {return ns1;}

//-----------------------------------------------------------------------------
long IndexParam::get_ns2() {return ns2;}

//-----------------------------------------------------------------------------
char *IndexParam::get_index() {return(index);}

//-----------------------------------------------------------------------------
char *IndexParam::get_ief() {return(iefile);}

//-----------------------------------------------------------------------------
long IndexParam::get_contperc() {return(contperc);}

//-----------------------------------------------------------------------------
char *IndexParam::get_snf() {return(snfile);}

//-----------------------------------------------------------------------------
double IndexParam::get_rv() {return(rv);}

//-----------------------------------------------------------------------------
double IndexParam::get_rve() {return(rve);}

//-----------------------------------------------------------------------------
char *IndexParam::get_rvfile() {return(rvfile);}

//-----------------------------------------------------------------------------
long IndexParam::get_rvc() {return(rvc);}

//-----------------------------------------------------------------------------
long IndexParam::get_rvce() {return(rvce);}

//-----------------------------------------------------------------------------
long IndexParam::get_nsimul() {return(nsimul);}

//-----------------------------------------------------------------------------
long IndexParam::get_nindex() {return(nindex);}

//-----------------------------------------------------------------------------
bool IndexParam::get_logindex() {return(logindex);}

//-----------------------------------------------------------------------------
bool IndexParam::get_verbose() {return(verbose);}

//-----------------------------------------------------------------------------
long IndexParam::get_nsimulsn() {return(nsimulsn);}

//-----------------------------------------------------------------------------
double IndexParam::get_minsn() {return(minsn);}

//-----------------------------------------------------------------------------
double IndexParam::get_maxsn() {return(maxsn);}

//-----------------------------------------------------------------------------
char *IndexParam::get_ilabfile() {return(ilabfile);}

//-----------------------------------------------------------------------------
long IndexParam::get_nchar1() {return nchar1;}

//-----------------------------------------------------------------------------
long IndexParam::get_nchar2() {return nchar2;}

//-----------------------------------------------------------------------------
double IndexParam::get_biaserr() {return(biaserr);}

//-----------------------------------------------------------------------------
double IndexParam::get_linearerr() {return(linearerr);}

//-----------------------------------------------------------------------------
long IndexParam::get_plotmode() {return(plotmode);}

//-----------------------------------------------------------------------------
char *IndexParam::get_grdev() {return(grdev);}

//-----------------------------------------------------------------------------
long IndexParam::get_nwinx() {return(nwinx);}

//-----------------------------------------------------------------------------
long IndexParam::get_nwiny() {return(nwiny);}

//-----------------------------------------------------------------------------
long IndexParam::get_plottype() {return(plottype);}

//-----------------------------------------------------------------------------
long IndexParam::get_nseed() {return(nseed);}

//-----------------------------------------------------------------------------
double IndexParam::get_fscale() {return(fscale);}
