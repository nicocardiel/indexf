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

//Funciones para llamar a PGPLOT con variables en doble precision, y otras
//funciones auxiliares

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <iostream>
#include "cpgplot.h"

using namespace std;

//-----------------------------------------------------------------------------
void cpgbin_d(const long &nbin, 
              const double *x, const double *data, const bool &center)
{
  float *x_float = new float [nbin];
  for (long i=0; i<nbin; i++)
    x_float[i]=(float) x[i];

  float *data_float = new float [nbin];
  for (long i=0; i<nbin; i++)
    data_float[i]=(float) data[i];

  cpgbin((int)nbin,x_float,data_float,center);

  delete [] x_float;
  delete [] data_float;
}
//-----------------------------------------------------------------------------
void cpgdraw_d(const double &x, const double &y)
{
  cpgdraw((float)x,(float)y);
}
//-----------------------------------------------------------------------------
void cpgenv_d(const double &xmin, const double &xmax, 
              const double &ymin, const double &ymax,
              const int &just, const int &axis)
{
  cpgenv((float)xmin,(float)xmax,(float)ymin,(float)ymax,just,axis);
}
//-----------------------------------------------------------------------------
void cpgimag_d(const double *a, const long &xdim, const long &ydim, 
               const long &x1, const long &x2, const long &y1, const long &y2,
               const double &bg, const double &fg)
{
  float tr[6]={0,0,1.0,0,1.0,0};
  float *a_float = new float [xdim*ydim];
  long k=0;
  for (long i=0; i<ydim; i++)
  {
    for (long j=0; j<xdim; j++)
    {
      a_float[j*ydim+i]=(float) a[k++];
    }
  }
  cpgimag(a_float,(int)ydim,(int)xdim,
         (int)y1,(int)y2,(int)x1,(int)x2,(float)fg,(float)bg,tr);
  delete [] a_float;
}
//-----------------------------------------------------------------------------
void cpgmove_d(const double &x, const double &y)
{
  cpgmove((float)x,(float)y);
}
//-----------------------------------------------------------------------------
void cpgpt_d(const long &n, const double *x, const double *y, 
             const int &symbol)
{
  float *x_float = new float [n];
  for (long i=0; i<n; i++)
    x_float[i]=(float) x[i];

  float *y_float = new float [n];
  for (long i=0; i<n; i++)
    y_float[i]=(float) y[i];

  cpgpt((int)n,x_float,y_float,symbol);

  delete [] x_float;
  delete [] y_float;
}
//-----------------------------------------------------------------------------
void cpgrect_d(const double &x1, const double &x2,
               const double &y1, const double &y2)
{
  cpgrect((float)x1,(float)x2,(float)y1,(float)y2);
}
//-----------------------------------------------------------------------------
void cpgswin_d(const double &xmin, const double &xmax, 
               const double &ymin, const double &ymax)
{
  cpgswin((float)xmin,(float)xmax,(float)ymin,(float)ymax);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
int palette(const int &itype, const bool &show)
{
  if (itype == 0) //return the number of defined palettes
  {
    return(7);
  }

  float contra, bright;
  contra=1.0;
  contra=-contra;
  bright=0.5;
  bright=1.0-0.5;

  float gl[2]={0.0, 1.0};
  float gr[2]={0.0, 1.0};
  float gg[2]={0.0, 1.0};
  float gb[2]={0.0, 1.0};

  float rl[9]={-0.5, 0.0, 0.17, 0.33, 0.50, 0.67, 0.83, 1.0, 1.7};
  float rr[9]={ 0.0, 0.0,  0.0,  0.0,  0.6,  1.0,  1.0, 1.0, 1.0};
  float rg[9]={ 0.0, 0.0,  0.0,  1.0,  1.0,  1.0,  0.6, 0.0, 1.0};
  float rb[9]={ 0.0, 0.3,  0.8,  1.0,  0.3,  0.0,  0.0, 0.0, 1.0};

  float hl[5]={0.0, 0.3, 0.6, 0.9, 1.0};
  float hr[5]={0.0, 0.5, 1.0, 1.0, 1.0};
  float hg[5]={0.0, 0.0, 0.5, 1.0, 1.0};
  float hb[5]={0.0, 0.0, 0.0, 0.3, 1.0};

  float cl[5]={0.0, 0.3, 0.6, 0.9, 1.0};
  float cb[5]={0.0, 0.5, 1.0, 1.0, 1.0};
  float cg[5]={0.0, 0.0, 0.5, 1.0, 1.0};
  float cr[5]={0.0, 0.0, 0.0, 0.3, 1.0};

  float zl[5]={0.0, 0.3, 0.6, 0.9, 1.0};
  float zg[5]={0.0, 0.5, 1.0, 1.0, 1.0};
  float zb[5]={0.0, 0.0, 0.5, 1.0, 1.0};
  float zr[5]={0.0, 0.0, 0.0, 0.3, 1.0};

  float wl[10]={0.0, 0.5, 0.5, 0.7, 0.7, 0.85, 0.85, 0.95, 0.95, 1.0};
  float wr[10]={0.0, 1.0, 0.0, 0.0, 0.3,  0.8,  0.3,  1.0,  1.0, 1.0};
  float wg[10]={0.0, 0.5, 0.4, 1.0, 0.0,  0.0,  0.2,  0.7,  1.0, 1.0};
  float wb[10]={0.0, 0.0, 0.0, 0.0, 0.4,  1.0,  0.0,  0.0, 0.95, 1.0};

  float al[20]={0.0, 0.1, 0.1, 0.2, 0.2, 0.3, 0.3, 0.4, 0.4, 0.5,
                0.5, 0.6, 0.6, 0.7, 0.7, 0.8, 0.8, 0.9, 0.9, 1.0};
  float ar[20]={0.0, 0.0, 0.3, 0.3, 0.5, 0.5, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  float ag[20]={0.0, 0.0, 0.3, 0.3, 0.0, 0.0, 0.0, 0.0, 0.8, 0.8,
                0.6, 0.6, 1.0, 1.0, 1.0, 1.0, 0.8, 0.8, 0.0, 0.0};
  float ab[20]={0.0, 0.0, 0.3, 0.3, 0.7, 0.7, 0.7, 0.7, 0.9, 0.9,
                0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  switch (itype)
  {
    case 1:  //gray scale
      cpgctab(gl,gr,gg,gb,2,contra,bright);
      if(show) cout << ">>> palette: gray scale" << endl;
      break;
    case 2:  //rainbow
      cpgctab(rl,rr,rg,rb,9,contra,bright);
      if(show) cout << ">>> palette: rainbow" << endl;
      break;
    case 3:  //heat
      cpgctab(hl,hr,hg,hb,5,contra,bright);
      if(show) cout << ">>> palette: heat" << endl;
      break;
    case 4:  //cold night
      cpgctab(cl,cr,cg,cb,5,contra,bright);
      if(show) cout << ">>> palette: cold night" << endl;
      break;
    case 5:  //green
      cpgctab(zl,zr,zg,zb,5,contra,bright);
      if(show) cout << ">>> palette: green" << endl;
      break;
    case 6:  //weird IRAF
      cpgctab(wl,wr,wg,wb,10,contra,bright);
      if(show) cout << ">>> palette: weird IRAF" << endl;
      break;
    case 7:  //AIPS
      cpgctab(al,ar,ag,ab,20,contra,bright);
      if(show) cout << ">>> palette: AIPS" << endl;
      break;
    default:
      cout << "==> ERROR: " << itype << " is an invalid palette number." 
           << endl;
      cpgctab(gl,gr,gg,gb,2,contra,bright);
  }
  return(itype);
}
