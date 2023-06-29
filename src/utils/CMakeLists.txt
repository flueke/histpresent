add_library(utils SHARED
    TMrbEnv.cxx
    TMrbHistory.cxx
    TMrbLofData.cxx
    TMrbLofMacros.cxx
    TMrbLofNamedX.cxx
    TMrbLofUserVars.cxx
    TMrbLogger.cxx
    TMrbNamedArray.cxx
    TMrbNamedDouble.cxx
    TMrbNamedX.cxx
    TMrbResource.cxx
    TMrbSerialComm.cxx
    TMrbStatistics.cxx
    TMrbString.cxx
    TMrbSystem.cxx
    TMrbTail.cxx
    TMrbTemplate.cxx
    TMrbVariables.cxx
    TMrbWindow2DStreamer.cxx
    TMrbWindows.cxx
)

ROOT_GENERATE_DICTIONARY(utils_dict MODULE utils LINKDEF LinkDef.h
    GlobDef.h
    SetColor.h
    TMrbEnv.h
    TMrbHistory.h
    TMrbLofData.h
    TMrbLofMacros.h
    TMrbLofNamedX.h
    TMrbLofUserVars.h
    TMrbLogger.h
    TMrbNamedArray.h
    TMrbNamedDouble.h
    TMrbNamedX.h
    TMrbResource.h
    TMrbSerialComm.h
    TMrbStatistics.h
    TMrbString.h
    TMrbSystem.h
    TMrbTail.h
    TMrbTemplate.h
    TMrbVar.h
    TMrbVarWdwCommon.h
    TMrbWdw.h
)

target_link_libraries(utils PUBLIC ${HISTPRESENT_ROOT_COMPONENTS})
