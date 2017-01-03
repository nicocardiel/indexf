.. _install:

Installation
============

Before you install **indexf**, make sure that ``CFITSIO`` is already installed in your computer. If, in addition, you want the program to generate graphical output, ``PGPLOT`` must also be installed. Note that since version ``4.2.0``, the by default installation procedure will not try to install **indexf** with ``PGPLOT``. If this graphical library is available and you want **indexf** to use it, you should explicitly indicate that option during the installation procedure.

Some details about how I do typically install these packages under ``Linux`` and ``Mac OS X`` are given here for ``CFITSIO`` and ``PGPLOT``.

To install **indexf** you need to perform the following steps:


1. Download the latest distribution from github:

::

    $ git clone https://github.com/nicocardiel/indexf.git

2. Enter into the directory *indexf* and prepare the code to be compiled

::
    
    $ autoreconf -s -i -f
    $ ./configure

The above command will install the program without ``PGPLOT``. If you want **indexf** to generate graphical output with the help of that graphical library, you should use instead

::

    $ ./configure --with-pgplot

If ``PGPLOT`` is in fact installed in your computer, but ``configure`` is not able to locate it, it is possible to skip this problem by specifying the directory where this auxiliary library is installed using the parameter ``LDFLAGS``. For example:

::

    $ ./configure LDFLAGS=-L/usr/local/pgplot

By default, the executable *indexf* file is installed under */usr/local/bin/*, whereas additional auxiliary files (like *indexdef.dat* and *inputcl.dat*) are installed under */usr/local/share/indexf*. If you need a personalized directory installation (for example when you do not have write access to */usr/local/...*), you can use ``--bindir`` to specify the location of the executable file, and ``--datadir`` to indicate the directory where the auxiliary files must be installed. For example:

::

    $ ./configure --bindir=/home/userxxx/bin --datadir=/home/userxxx/share

will prepare the installation to place the binary file in */home/userxxx/bin/indexf* and the auxiliary files under */home/userxxx/share/indexf/*.

3. Compile the code:

::

    $ make

4. Install the program and auxiliary files in the appropriate directories. If you are installing the software in the default directories (*/usr/local/...*), you will need root privileges:

::

    $ sudo make install

or

::

    $ su
    # make install

Otherwise, if you are installing the software in custom defined directories for which you do not need system privileges, you can simply execute:

::

    $ make install

You can optionally clean the intermediate object files generated during the compilation procedure:

::

    $ make clean



