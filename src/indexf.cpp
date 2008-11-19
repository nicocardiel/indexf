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
#include "commandtok.h"
#include "installdir.h"
#include "indexparam.h"
#include "indexdef.h"
#include "scidata.h"

using namespace std;

//-----------------------------------------------------------------------------
//prototipos de funciones
bool loadidef(vector< IndexDef > &);
void showindex(vector< IndexDef > &);
bool loaddpar(vector< CommandToken > &);
bool loadipar(const char *[], const int, vector< CommandToken > &);
bool checkipar(vector< CommandToken > &, IndexParam &, vector< IndexDef > &);
void verbose(IndexParam, IndexDef, SciData *);
bool measuresp(SciData *, IndexParam, IndexDef);

//-----------------------------------------------------------------------------
//programa principal
int main (const int argc, const char *argv[])
{
  vector< CommandToken > cl;
  vector< IndexDef > id;
  IndexParam param;
  
  if( !loadidef(id) ) return(1);
  if( argc == 1 ) { showindex(id); return(0); }
  if( !loaddpar(cl) ) return(1);
  if( !loadipar(argv,argc,cl) ) return(1);
  if( !checkipar(cl, param, id) ) return(1);
  SciData image(param);
  IndexDef myindex = id[param.get_nindex()-1];
  if (param.get_verbose()) verbose(param,myindex,&image);
  if( !measuresp(&image,param,myindex) ) return(1);
  return(0);
}
