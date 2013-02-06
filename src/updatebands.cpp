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

#include "indexparam.h"
#include "indexdef.h"

double ftovacuum(const long, const double);

// Verifica si el usuario ha indicado que la calibracion en longitud de onda
// del espectro ha sido realizada en vacio. Si es asi, se modifican las
// longitudes de onda de todas las bandas a medir.
void updatebands(IndexParam &param, IndexDef &myindex)
{
  const long vacuum = param.get_vacuum();
  if (vacuum == 0) return; // trabajamos en aire
  // corregimos a vacio
  const long nbands = myindex.getnbands();
  double ldo1, ldo2;
  for (long i=1; i <= nbands; i++)
  {
    ldo1 = myindex.getldo1(i-1);
    ldo2 = myindex.getldo2(i-1);
    myindex.setldo(i-1, ftovacuum(vacuum,ldo1), ftovacuum(vacuum,ldo2));
  }
}
