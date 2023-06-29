find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Hist Gpad Gui Net Graf)

# ROOT_GENERATE_DICTIONARY requires this to be set
set(CMAKE_INSTALL_LIBDIR lib)

# Still figuring out how to solve undefined references and missing symbols.
# Central link definition for all sublibs for now.
set(HISTPRESENT_ROOT_COMPONENTS
    ROOT::Ged
    ROOT::Gpad
    ROOT::Graf
    ROOT::Graf3d
    ROOT::Gui
    ROOT::Hist
    ROOT::Spectrum
    ROOT::Rint
    ROOT::RIO
    ROOT::Physics
    ROOT::Tree
    ROOT::Net
    ROOT::HistPainter
    ROOT::Proof
    )

# Everything seems to include everything else across the submodules. Pull out the hammer
# and include all subdirs here.
include_directories(
    fitcal
    gredit
    gutils
    helpbrowser
    hpr
    polar
    utils
)


add_subdirectory(fitcal)
add_subdirectory(gutils)
add_subdirectory(gredit)
add_subdirectory(helpbrowser)
add_subdirectory(polar)
add_subdirectory(utils)
add_subdirectory(hpr)
