add_library(fitcal SHARED
    CalibrationDialog.cxx
    FhMarker.cxx
    FhPeak.cxx
    FindPeakDialog2D.cxx
    FindPeakDialog.cxx
    FitOneDimDialog.cxx
    PeakFinder.cxx
    TF1Range.cxx
)

ROOT_GENERATE_DICTIONARY(fitcal_dict MODULE fitcal LINKDEF LinkDef.h
    CalibrationDialog.h
    FhMarker.h
    FhPeak.h
    FindPeakDialog2D.h
    FindPeakDialog.h
    FitOneDimDialog.h
    PeakFinder.h
    TF1Range.h
)

target_link_libraries(fitcal PUBLIC ${HISTPRESENT_ROOT_COMPONENTS})
