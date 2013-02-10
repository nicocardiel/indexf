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

#include <iostream>
#include <vector>
#include "commandtok.h"
#include "installdir.h"
#include "version.h"
#include "indexparam.h"
#include "indexdef.h"
#include "scidata.h"

using namespace std;
bool pyndexf_global;

//-----------------------------------------------------------------------------
//prototipos de funciones
bool checkpynd(const char *[], const int);
int  pyexit(const int);
bool loadidef(vector< IndexDef > &);
void showindex(vector< IndexDef > &);
bool loaddpar(vector< CommandToken > &);
bool loadipar(const char *[], const int, vector< CommandToken > &);
bool checkipar(vector< CommandToken > &, IndexParam &, vector< IndexDef > &);
void welcome(bool);
void updatebands(IndexParam &, IndexDef &);
void verbose(IndexParam &, IndexDef &, SciData *);
bool measuresp(SciData *, IndexParam &, IndexDef &);

//-----------------------------------------------------------------------------
//programa principal
int main (const int argc, const char *argv[])
{
  vector< CommandToken > cl;
  vector< IndexDef > id;
  IndexParam param;

  pyndexf_global=checkpynd(argv,argc);
  
  if(!loadidef(id)) return(pyexit(1)); //................read index definitions
  if(argc == 1) {welcome(true); showindex(id); return(0);} //....show help info
  if(!loaddpar(cl)) return(pyexit(1));  //read keywords:values from inputcl.dat
  if(!loadipar(argv,argc,cl)) return(pyexit(1));  //read user's keywords:values
  if(!checkipar(cl, param, id)) return(pyexit(1)); //.....check keywords:values
  if(param.get_checkkeys()) return(0); //...check only keywords=values and exit
  welcome(param.get_verbose()); //..........welcome message with version number
  SciData image(param); //..........SciData object: spectra and associated data
  IndexDef myindex = id[param.get_nindex()-1]; //IndexDef object: spec. feature
  updatebands(param,myindex); //......correct wavelengths to vacuum if required
  if(param.get_verbose()) verbose(param,myindex,&image); //....output verbosity
  if(!measuresp(&image,param,myindex)) return(pyexit(1)); //....measure spectra
  return(0);
}
