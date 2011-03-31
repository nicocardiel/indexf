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

//Declaración de la clase IndexDef
//Las funciones miembro se definen en indexdef.cpp

#ifndef INDEXDEF_H
#define INDEXDEF_H

class IndexDef{
  public:
    IndexDef(const char *, const long &); //constructor
    bool setnbands(const long &);
    bool setnconti(const long &);
    bool setnlines(const long &);
    bool setldo(const long &, const double &, const double &);
    bool setldo(const long &, const double &, const double &, const double &);
    char *getlabel();
    long gettype() const;
    long getnbands() const;
    long getnconti() const;
    long getnlines() const;
    double getldo1(const long &) const;
    double getldo2(const long &) const;
    double getfactor(const long &) const;
    double getfactor_el(const long &) const;
  private:
    char label[9]; //nombre del índice (máximo 8 caracteres)
    long type;    //tipo de índice
    long nbands;  //número total de bandas (=nconti+nlines)
    long nconti;  //número de bandas de continuo
    long nlines;  //número de bandas de líneas
    double ldo1[198];//longitud de onda izquierda de cada banda
    double ldo2[198];//longitud de onda derecha de cada banda
    double factor[99];//factores para bandas de líneas
};

#endif
