#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <iostream>

using namespace std;

void welcome()
{
  //la version esta definida como variable global
  extern const char *versionPtr;

  cout << "#\n# Using indexf version: " << versionPtr 
#ifdef HAVE_CPGPLOT_H
       << " (PGPLOT enabled)"
#else
       << " (PGPLOT disabled)"
#endif /* HAVE_CPGPLOT_H */
       << "\n#" << endl;
}
