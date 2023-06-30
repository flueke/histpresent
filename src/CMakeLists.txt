find_package(ROOT REQUIRED)


set(CMAKE_INSTALL_LIBDIR lib) # ROOT_GENERATE_DICTIONARY requires this to be set
set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib")

# Still figuring out how to solve undefined references and missing symbols.
# Central link definition for all sublibs for now.
set(HISTPRESENT_ROOT_COMPONENTS
    ROOT::Ged
    ROOT::Gpad
    ROOT::Graf
    ROOT::Graf3d
    ROOT::Gui
    ROOT::Hist
    ROOT::HistPainter
    ROOT::Net
    ROOT::Physics
    ROOT::Proof
    ROOT::Rint
    ROOT::RIO
    ROOT::Spectrum
    ROOT::Tree
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
add_subdirectory(gredit)
add_subdirectory(gutils)
add_subdirectory(helpbrowser)
add_subdirectory(hpr)
add_subdirectory(polar)
add_subdirectory(utils)

install(TARGETS
    fitcal
    gredit
    gutils
    helpbrowser
    hpr
    polar
    utils
    histpresent)
