find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Hist)

# Everything seems to include everything else across the submodules. Pull out the hammer
# and include all the subdirs here.
# TODO: refactor the code and libs to get a sane dependency hierarchy.
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
add_subdirectory(hpr)
add_subdirectory(polar)
add_subdirectory(utils)
