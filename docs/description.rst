Program description
===================

Introduction
------------

**indexf** is a program written in C++ to measure line-strength indices in
fully calibrated FITS spectra. By "fully calibrated" one should understand
wavelength and relative flux-calibrated data. Note that the different types of
line-strength indices that can be measured with **indexf** (see below) do not
require absolute flux calibration. If even a relative flux-calibration is
absent (or deficient), the derived indices should be transformed to an
appropriate spectrophotometric system.

The author thanks Sergio Pascual for his invaluable help to facilitate the
installation procedure with autotools, as well as the maintenance of the source
repository.

Error estimation
----------------

The program can also compute index errors resulting from the propagation of
random errors (e.g. photon statistics, read-out noise). This option is only
available if the user provides the error spectrum as an additional input FITS
file to **indexf**. The error spectrum must contain the unbiased standard
deviation (and not the variance!) for each pixel of the data spectrum. Full
details of the formulae employed for the computation of errors in molecular and
atomic indices, as well as in the D4000-like indices, are given in
Cardiel_etal_1998_ and in chapter 2 of Cardiel's :download:`thesis ---in
Spanish---<aux/thesis_ncardiel.pdf>`; for the
generic indices see Appendix A in Cenarro_etal_2001a_. For a discussion on
the impact of random errors in line-strength indices when studying stellar
populations see Cardiel_etal_2003_.

In addition, **indexf** also estimates the effect of errors on radial velocity.
For this purpose, the program performs Monte Carlo simulations by measuring
each index using randomly drawn radial velocities (following a Gaussian
distribution of a given standard deviation).

If no error file is employed, the program can perform numerical simulations
with synthetic error spectra, the latter generated from the original data
spectra and assuming randomly generated S/N ratios.

Index definitions
-----------------

The line-strength indices that can be measured with **indexf** are those
defined in the file ``indexdef.dat``, located in the ``auxdir/`` subdirectory
of the distribution source code. The first column in this file is the
identification name of each index (maximum 8 characters), as recognized by
**indexf**. The second column, labeled as "code", is an integer number that
allows the identification of the type of line-strength feature. The code
employed in this column is the following:

===========  ====================== ===========================================
index code   type of index          examples
===========  ====================== ===========================================
1            molecular              CN1, CN2, Mg1, Mg2, TiO1, TiO2,...
2            atomic                 Ca4227, G4300, Fe4668, Hbeta, Fe5270, Fe5335,...
3            D4000-like             D4000
4            B4000-like             B4000
5            color-like             infrared CO_KH
10           emission line          OII3727e
[11..99]     generic discontinuity  D_CO,...
[101..9999]  generic index          CaT, PaT, CaT*,...
[-99..-2]    slope index            sTiO
===========  ====================== ===========================================

An example of some of the definitions that can be found in the file
indexdef.dat is the following (the list shown here is not complete!):

::
   
   Index    code    blue bandpass      central bandpass      red bandpass     >              source
   ======== ====  ==================  ==================  ==================    ======================================
   CN1         1  4080.125  4117.625  4142.125  4177.125  4244.125  4284.125  > Lick
   HdA         2  4041.600  4079.750  4083.500  4122.250  4128.500  4161.000  > Hdelta A (Worthey & Ottaviani 1997)
   D4000       3  3750.000  3950.000  4050.000  4250.000     0.000     0.000  > 4000 break (Bruzual 1983)
   B4000       4  3750.000  3950.000  4050.000  4250.000     0.000     0.000  > 4000 break (Gorgas et al. 1999)
   CO_KH       5  22872.83  22925.26  22930.52  22983.22  00000.00  00000.00  > Kleinmann & Hall (1986)
   OII3727e   10  > emission line OII 3727
                  4
                  3660.000  3666.000  0.0
                  3680.000  3688.000  0.0
                  3710.000  3735.000  1.0
                  3740.000  3750.000  0.0
   D_CO       12  > generic CO discontinuity (Marmol-Queralto et al. 2008)
                  22880.00  23010.00
                  22460.00  22550.00
                  22710.00  22770.00
   CaT_star  506  > CaT* index from Cenarro et al.(2001) (Paschen-corrected near-IR Ca triplet) 
                  8474.000  8484.000
                  8563.000  8577.000
                  8619.000  8642.000 
                  8700.000  8725.000
                  8776.000  8792.000
                  8461.000  8474.000  -0.93
                  8484.000  8513.000  1.0
                  8522.000  8562.000  1.0
                  8577.000  8619.000  -0.93
                  8642.000  8682.000  1.0                   
                  8730.000  8772.000  -0.93
   sTiO       -5  > Near-IR spectral slope (Cenarro et al. in preparation)
                  8474.000  8484.000
                  8563.000  8577.000
                  8619.000  8642.000 
                  8700.000  8725.000
                  8776.000  8792.000



.. _Cardiel_etal_1998: http://cdsads.u-strasbg.fr/abs/1998A%26AS..127..597C
.. _Cardiel_etal_2003: http://cdsads.u-strasbg.fr/abs/2003A%26A...409..511C
.. _Cenarro_etal_2001a: http://cdsads.u-strasbg.fr/abs/2001MNRAS.326..959C
