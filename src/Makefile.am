#

BASEFILES= boundaryfit.cpp c123.cpp checkipar.cpp commandtok.cpp \
commandtok.h fmean.cpp fpercent.cpp \
ftovacuum.cpp genericpixel.cpp genericpixel.h indexdef.cpp indexdef.h \
indexf.cpp indexparam.cpp indexparam.h issdouble.cpp isslong.cpp \
loaddpar.cpp loadidef.cpp loadipar.cpp measuresp.cpp mideindex.cpp \
scidata.cpp scidata.h showindex.cpp snregion.cpp snregion.h \
sustrae_p0.cpp sustrae_p1.cpp updatebands.cpp verbose.cpp welcome.cpp \
xydata.cpp xydata.h installdir.h

PGPLOTFILES=cpgplot_d.cpp cpgplot_d.h

bin_PROGRAMS = indexf
if WITHPGPLOT
indexf_SOURCES = $(BASEFILES) $(PGPLOTFILES) 
else
indexf_SOURCES =  $(BASEFILES)
endif

indexf_LDADD = $(CFITSIO_LIBS) $(PGPLOT_LDFLAGS)
AM_CPPFLAGS = -DAUXDIR='"$(pkgdatadir)"' $(CFITSIO_CFLAGS) $(PGPLOT_CFLAGS) -I$(top_srcdir)
