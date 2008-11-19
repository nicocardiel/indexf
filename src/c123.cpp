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

#include <cmath>

using namespace std;

void c123(const double & ldo_b1, const double & ldo_b2,
          const double & ldo_c1, const double & ldo_c2,
          const double & ldo_r1, const double & ldo_r2,
          double & c1, double & c2, double & c3)
{
  double ldo_b=(ldo_b1+ldo_b2)/2.0;
  double ldo_c=(ldo_c1+ldo_c2)/2.0;
  double ldo_r=(ldo_r1+ldo_r2)/2.0;
  double delta_ldo_b=(ldo_b2-ldo_b1);
  double delta_ldo_c=(ldo_c2-ldo_c1);
  double delta_ldo_r=(ldo_r2-ldo_r1);
  c2=1.0/delta_ldo_c+
     ((ldo_r-ldo_c)*(ldo_r-ldo_c)/((ldo_r-ldo_b)*(ldo_r-ldo_b)))/delta_ldo_b+
     ((ldo_c-ldo_b)*(ldo_c-ldo_b)/((ldo_r-ldo_b)*(ldo_r-ldo_b)))/delta_ldo_r;
  c2=sqrt(c2);
  c1=delta_ldo_c*c2;
  c3=2.5*log10(exp(1.0))*c2;
}
