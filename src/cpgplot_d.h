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

//Prototipo de las funciones declaradas en el fichero cpgplot_d.cpp
void cpgbin_d(const long &, const double *, const double *, const bool &);
void cpgdraw_d(const double &, const double &);
void cpgenv_d(const double &, const double &,const double &, const double &,
              const int &, const int &);
void cpgimag_d(const double *, const long &, const long &, 
               const long &, const long &, const long &, const long &, 
               const double &, const double &);
void cpgmove_d(const double &, const double &);
void cpgpt_d(const long &, const double *, const double *, const int &);
void cpgrect_d(const double &, const double &,
               const double &, const double &);
void cpgswin_d(const double &, const double &,const double &, const double &);
//-----------------------------------------------------------------------------
//funciones auxiliares
int palette(const int &, const bool &);
