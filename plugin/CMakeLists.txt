INCLUDE(BuildPlugin)

BUILD_PLUGIN(
    scanner
        ScannerView.cpp
        Scanner.cpp
    MOCFILES
        ScannerView.h
        Scanner.h
    EMBEDDED_RESOURCES
        *.png
)