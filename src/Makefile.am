#
bin_PROGRAMS = indexf
if WITHPGPLOT
indexf_SOURCES =  \
boundaryfit.cpp \
c123.cpp \
checkipar.cpp \
commandtok.cpp \
commandtok.h \
cpgplot_d.cpp \
cpgplot_d.h \
fmean.cpp \
fpercent.cpp \
ftovacuum.cpp \
genericpixel.cpp \
genericpixel.h \
indexdef.cpp \
indexdef.h \
indexf.cpp \
indexparam.cpp \
indexparam.h \
issdouble.cpp \
isslong.cpp \
loaddpar.cpp \
loadidef.cpp \
loadipar.cpp \
measuresp.cpp \
mideindex.cpp \
scidata.cpp \
scidata.h \
showindex.cpp \
snregion.cpp \
snregion.h \
sustrae_p0.cpp \
sustrae_p1.cpp \
updatebands.cpp \
verbose.cpp \
welcome.cpp \
xydata.cpp \
xydata.h
else
indexf_SOURCES =  \
boundaryfit.cpp \
c123.cpp \
checkipar.cpp \
commandtok.cpp \
commandtok.h \
fmean.cpp \
fpercent.cpp \
ftovacuum.cpp \
genericpixel.cpp \
genericpixel.h \
indexdef.cpp \
indexdef.h \
indexf.cpp \
indexparam.cpp \
indexparam.h \
issdouble.cpp \
isslong.cpp \
loaddpar.cpp \
loadidef.cpp \
loadipar.cpp \
measuresp.cpp \
mideindex.cpp \
scidata.cpp \
scidata.h \
showindex.cpp \
snregion.cpp \
snregion.h \
sustrae_p0.cpp \
sustrae_p1.cpp \
updatebands.cpp \
verbose.cpp \
welcome.cpp \
xydata.cpp \
xydata.h
endif

indexf_LDADD = $(CFITSIO_LIBS) 
AM_CPPFLAGS = $(CFITSIO_CFLAGS) -I$(top_srcdir)
