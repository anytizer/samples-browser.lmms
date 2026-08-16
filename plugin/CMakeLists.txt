# This CMake file is for compiling this application as a Plugin for LMMS.
# logic/ is a symlink to src/ folder, where original src/ files are for generating standalone application.

INCLUDE(BuildPlugin)

BUILD_PLUGIN(
    scanner
        ScannerView.cpp
        Scanner.cpp
        logic/Configurations.h
        logic/PCH.h
        logic/CustomTableWidget.h
        logic/KeysFilter.h
        logic/ParseWavFile.h
        logic/PopulateWavTable.h
        logic/WaveformDelegate.h
        logic/CreateWaveformImage.h
        logic/FormatFileSize.h
        logic/ProcessGUI.h
        logic/WavInfo.h
    MOCFILES
        ScannerView.h
        Scanner.h
    EMBEDDED_RESOURCES
        *.png
)