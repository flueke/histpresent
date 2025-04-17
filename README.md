HistPresent
===========

This is a standalone version of the [*Histogram
Presenter*](https://www-old.mll-muenchen.de/marabou/htmldoc/hpr/index.html) for
ROOT from the MARaBOU package (https://www-old.mll-muenchen.de/marabou/htmldoc,
https://github.com/flueke/marabou).

The code was extracted from the MARaBOU sources, CMake build scripts have been
added and compile issues have been fixed (6/2023).

Update 4/2025: ported to ROOT-6.34. This removes all PROOF based features.

Build
-----

Requires ROOT-6.28 or later. Uses ROOTSYS to locate the ROOT installation. Use
`source <path-to-root>/bin/thisroot.sh` to setup your environment.

    git clone https://github.com/flueke/histpresent
    mkdir -p histpresent/build
    cd histpresent/build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/local/histpresent ..
    make -j4 install

Authors
-------
R.Lutter, O.Schaile (LMU)

F. Lüke <f.lueke@mesytec.com> (since '23)

License
-------

Unclear for now as no license info is in the original source tree. Assuming GPL
or LGPL.
