add_library(polar SHARED
    TPolControl.cxx
)

ROOT_GENERATE_DICTIONARY(polar_dict MODULE polar LINKDEF LinkDef.h
    TPolControl.h
)
target_link_libraries(polar PUBLIC ${HISTPRESENT_ROOT_COMPONENTS})
