add_library(hpr SHARED
    AddFitMenus.cxx
    Ascii2GraphDialog.cxx
    Ascii2HistDialog.cxx
    Ascii2NtupleDialog.cxx
    CmdListEntry.cxx
    EmptyHistDialog.cxx
    FhContour.cxx
    FillFormDialog.cxx
    Fit2DimDialog.cxx
    FitHist.cxx
    Fitting.cxx
    FontSelectDialog.cxx
    GeneralAttDialog.cxx
    GraphAttDialog.cxx
    GroupOfHists.cxx
    HandleMenus.cxx
    histfromascii.cxx
    HistPresent.cxx
    hprbase.cxx
    HprContours.cxx
    HprFunc2.cxx
    HprGaxis.cxx
    HprLegend.cxx
    HprStack.cxx
    HprTh3Dialog.cxx
    HTCanvas.cxx
    Rebin2DimDialog.cxx
    Set1DimOptDialog.cxx
    Set2DimGraphDialog.cxx
    Set2DimOptDialog.cxx
    Set3DimOptDialog.cxx
    SetCanvasAttDialog.cxx
    SetColorModeDialog.cxx
    SetHistOptDialog.cxx
    SetOptions.cxx
    ShiftScale.cxx
    ShowTree.cxx
    support.cxx
    TMrbArray.cxx
    TText3D.cxx
    WhatToShowDialog.cxx
    Windows.cxx
    WindowSizeDialog.cxx
)

ROOT_GENERATE_DICTIONARY(hpr_dict MODULE hpr LINKDEF LinkDef.h
    AddFitMenus.h
    Ascii2GraphDialog.h
    Ascii2HistDialog.h
    Ascii2NtupleDialog.h
    CmdListEntry.h
    defineMarabou.h
    EmptyHistDialog.h
    FHCommands.h
    FhContour.h
    FhOWFile.h
    FhToASCII.h
    FillFormDialog.h
    Fit2DimDialog.h
    FitHist.h
    FontSelectDialog.h
    GeneralAttDialog.h
    GraphAttDialog.h
    GroupOfHists.h
    HandleMenus.h
    HistPresent.h
    hprbase.h
    HprFunc2.h
    HprGaxis.h
    HprLegend.h
    HprNtuple.h
    HprStack.h
    HprTh3Dialog.h
    HTCanvas.h
    Rebin2DimDialog.h
    Set1DimOptDialog.h
    Set2DimGraphDialog.h
    Set2DimOptDialog.h
    Set3DimOptDialog.h
    SetCanvasAttDialog.h
    SetColorModeDialog.h
    SetHistOptDialog.h
    ShiftScale.h
    support.h
    TMrbArrayD.h
    TMrbArrayF.h
    TText3D.h
    WhatToShowDialog.h
    WindowSizeDialog.h
)

target_link_libraries(hpr
    PUBLIC fitcal gutils gredit helpbrowser polar utils
    #PUBLIC ROOT::Core ${ROOT_LIBRARIES}
    PUBLIC ${HISTPRESENT_ROOT_COMPONENTS}
)

add_executable(histpresent main.cxx)
target_link_libraries(histpresent
    PUBLIC hpr
    #PUBLIC ROOT::Core ${ROOT_LIBRARIES}
    PUBLIC ${HISTPRESENT_ROOT_COMPONENTS}
)
