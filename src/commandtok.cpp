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

//Definición de funciones miembro de la clase CommandToken, declarada en 
//commandtok.h
#include <iostream>
#include <string.h>
#include "commandtok.h"

using namespace std;

//-----------------------------------------------------------------------------
//constructor por defecto
CommandToken::CommandToken()
{
  label[0] = '\0';
  value[0] = '\0';
}

//constructor con 2 parámetros
CommandToken::CommandToken(char *keylabel, char *keyvalue)
{
  setlabel(keylabel);
  setvalue(keyvalue);
}

//-----------------------------------------------------------------------------
void CommandToken::setlabel(const char *newlabel)
{
  long length = strlen(newlabel);
  length = (length < 11 ? length : 10);
  strncpy(label,newlabel,length);
  label[length] = '\0';
}

//-----------------------------------------------------------------------------
void CommandToken::setvalue(const char *newvalue)
{
  long length = strlen(newvalue);
  length = (length < 256 ? length : 255);
  strncpy(value,newvalue,length);
  value[length] = '\0';
}

//-----------------------------------------------------------------------------
char *CommandToken::getlabel() { return label ; }

//-----------------------------------------------------------------------------
char *CommandToken::getvalue() { return value; }
