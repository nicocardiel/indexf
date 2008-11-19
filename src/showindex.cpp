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
#include "indexdef.h"
#include <string.h>
#include <stdlib.h>

using namespace std;

void showindex(vector< IndexDef > &id)
{
  long space;
  const char *label;

  //el directorio de instalación está definido como variable global
  extern const char *installdirPtr;

  cout << "\n* Running indexf from: ";
  cout << installdirPtr << "indexf" << endl;

  cout << "\nUsage:\n"
  "% indexf keyword1=keyval1 keyword2=keyval2 ...\n\nwhere:\n";

  long lsize = strlen("cat ") + strlen(installdirPtr) + strlen("inputcl.dat");
  char *instructionPtr = new char[lsize+1];
  strcpy(instructionPtr,"cat ");
  strcat(instructionPtr,installdirPtr);
  strcat(instructionPtr,"inputcl.dat");
  system (instructionPtr);
  delete [] instructionPtr;

  space=strlen("> Molecular indices: ");
  cout << "\n> Molecular indices: ";
  for (unsigned long i=1; i <= id.size(); i++)
  {
    if(id[i-1].gettype() == 1)
    {
      label=id[i-1].getlabel();
      space = space + strlen(label)+1;
      if(space > 80)
      {
        cout << "\n  ";
        space = strlen(label)+1+2;
      }
      cout << label << " ";
    }
  }
  cout << endl;
 
  space=strlen("> Atomic indices: ");
  cout << "\n> Atomic indices: ";
  for (unsigned long i=1; i <= id.size(); i++)
  {
    if(id[i-1].gettype() == 2)
    {
      label=id[i-1].getlabel();
      space = space + strlen(label)+1;
      if(space > 80)
      {
        cout << "\n  ";
        space = strlen(label)+1+2;
      }
      cout << label << " ";
    }
  }
  cout << endl;

  space=strlen("> D4000 like indices: ");
  cout << "\n> D4000 like indices: ";
  for (unsigned long i=1; i <= id.size(); i++)
  {
    if(id[i-1].gettype() == 3)
    {
      label=id[i-1].getlabel();
      space = space + strlen(label)+1;
      if(space > 80)
      {
        cout << "\n  ";
        space = strlen(label)+1+2;
      }
      cout << label << " ";
    }
  }
  cout << endl;

  space=strlen("> B4000 like indices: ");
  cout << "\n> B4000 like indices: ";
  for (unsigned long i=1; i <= id.size(); i++)
  {
    if(id[i-1].gettype() == 4)
    {
      label=id[i-1].getlabel();
      space = space + strlen(label)+1;
      if(space > 80)
      {
        cout << "\n  ";
        space = strlen(label)+1+2;
      }
      cout << label << " ";
    }
  }
  cout << endl;

  space=strlen("> color like indices: ");
  cout << "\n> color like indices: ";
  for (unsigned long i=1; i <= id.size(); i++)
  {
    if(id[i-1].gettype() == 5)
    {
      label=id[i-1].getlabel();
      space = space + strlen(label)+1;
      if(space > 80)
      {
        cout << "\n  ";
        space = strlen(label)+1+2;
      }
      cout << label << " ";
    }
  }
  cout << endl;

  space=strlen("> Generic indices: ");
  cout << "\n> Generic indices: ";
  for (unsigned long i=1; i <= id.size(); i++)
  {
    if( (id[i-1].gettype() >= 101) && (id[i-1].gettype() <= 9999) )
    {
      label=id[i-1].getlabel();
      space = space + strlen(label)+1;
      if(space > 80)
      {
        cout << "\n  ";
        space = strlen(label)+1+2;
      }
      cout << label << " ";
    }
  }
  cout << endl;

  space=strlen("> Slope indices: ");
  cout << "\n> Slope indices: ";
  for (unsigned long i=1; i <= id.size(); i++)
  {
    if( (id[i-1].gettype() >= -99) && (id[i-1].gettype() <= -2) )
    {
      label=id[i-1].getlabel();
      space = space + strlen(label)+1;
      if(space > 80)
      {
        cout << "\n  ";
        space = strlen(label)+1+2;
      }
      cout << label << " ";
    }
  }
  cout << endl;

  cout << endl;
}
