#include <iostream>

using namespace std;

void welcome()
{
  //la version esta definida como variable global
  extern const char *versionPtr;

  cout << "#\n# Using indexf version: " << versionPtr << "\n#" << endl;
}
