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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <iostream>

using namespace std;

void welcome()
{
  //la version esta definida como variable global
  extern const char *versionPtr;

  cout << "#\n#Using indexf version: " << versionPtr 
       << " (pyndexf compatible, "
#ifdef HAVE_CPGPLOT_H
       << "PGPLOT enabled)"
#else
       << "PGPLOT disabled)"
#endif /* HAVE_CPGPLOT_H */
       << "\n#" << endl;
}
