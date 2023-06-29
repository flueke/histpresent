add_library(helpbrowser SHARED
    TMrbHelpBrowser.cxx
    TNamedBytes.cxx
    TNamedString.cxx
)

ROOT_GENERATE_DICTIONARY(helpbrowser_dict MODULE helpbrowser LINKDEF LinkDef.h
    TMrbHelpBrowser.h
    TNamedBytes.h
    TNamedString.h
)

target_link_libraries(helpbrowser PUBLIC ${HISTPRESENT_ROOT_COMPONENTS})
