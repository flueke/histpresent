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

target_link_libraries(fitcal PUBLIC ROOT::RIO)
