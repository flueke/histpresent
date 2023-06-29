add_library(gutils SHARED
    Save2FileDialog.cxx
    TableOfLabels.cxx
    TGMrbButtonFrame.cxx
    TGMrbCheckButton.cxx
    TGMrbFileEntry.cxx
    TGMrbFileObject.cxx
    TGMrbFocusList.cxx
    TGMrbGeometry.cxx
    TGMrbGetTextAlignment.cxx
    TGMrbHelpWindow.cxx
    TGMrbInputDialog.cxx
    TGMrbLabelCombo.cxx
    TGMrbLabelEntry.cxx
    TGMrbLayout.cxx
    TGMrbLofKeyBindings.cxx
    TGMrbMacroBrowser.cxx
    TGMrbMessageViewer.cxx
    TGMrbObject.cxx
    TGMrbPictureButton.cxx
    TGMrbProfile.cxx
    TGMrbProgressBar.cxx
    TGMrbRadioButton.cxx
    TGMrbSliders.cxx
    TGMrbTableFrame.cxx
    TGMrbTextButton.cxx
    TGMrbValuesAndText.cxx
)

ROOT_GENERATE_DICTIONARY(gutils_dict MODULE gutils LINKDEF LinkDef.h
    Save2FileDialog.h
    TableOfLabels.h
    TGMrbButtonFrame.h
    TGMrbCheckButton.h
    TGMrbFileEntry.h
    TGMrbFileObject.h
    TGMrbFocusList.h
    TGMrbGeometry.h
    TGMrbGetTextAlignment.h
    TGMrbHelpWindow.h
    TGMrbInputDialog.h
    TGMrbLabelCombo.h
    TGMrbLabelEntry.h
    TGMrbLayout.h
    TGMrbLofKeyBindings.h
    TGMrbMacroBrowser.h
    TGMrbMessageViewer.h
    TGMrbObjectCombo.h
    TGMrbObject.h
    TGMrbPictureButton.h
    TGMrbProfile.h
    TGMrbProgressBar.h
    TGMrbRadioButton.h
    TGMrbSliders.h
    TGMrbTableFrame.h
    TGMrbTextButton.h
    TGMrbValuesAndText.h
)

target_link_libraries(gutils PUBLIC ${HISTPRESENT_ROOT_COMPONENTS})

