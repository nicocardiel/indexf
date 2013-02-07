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

//Declaración de la clase CommandToken
//Las funciones miembro se definen en commandtok.cpp

#ifndef COMMANDTOKEN_H
#define COMMANDTOKEN_H

class CommandToken{
  public:
    CommandToken(); //constructor por defecto
    CommandToken(char *, char *); //constructor con 2 parámetros
    void setlabel(const char *);
    void setvalue(const char *);
    char *getlabel();
    char *getvalue();
  private:
    char label[11];
    char value[256];
};

#endif
