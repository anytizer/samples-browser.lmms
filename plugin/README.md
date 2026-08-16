# Scanner Features

    - [x] scan .wav one-shots
    - [x] adaptive audio graphs
    - [x] skip large files
    - [x] prefer short samples
    - [x] save cpu (low latency processes by filtering samples)
    - [x] import selected sample (creates a track in Pattern Editor)
    - [x] refresh/rescan files
    - [x] keyboard shortcuts
    - [x] Standalone as well as LMMS Integrated


# Installation / Setup

Except: main.cpp, symlink individual files under logic/ folder from parent.

Then:
- Create a symlink of this folder (plugin) as the name "Scanner" under LMMS/plugins/.
- Enable "Scanner" Plugin to compile with LMMS by editing [LMMS/cmake/modules/PluginList.cmake](https://github.com/LMMS/lmms/blob/master/cmake/modules/PluginList.cmake)

```
cd samples-browser.lmms

ln -s $(pwd)/src/ plugin/logic

# or,
ln -s "$(realpath src)" plugin/logic

# cd samples-browser.lmms
# cd plugin
#
# mkdir logic
# cd logic
#
# ln -s ../../src/Configurations.h Configurations.h
# ln -s ../../src/CreateWaveformImage.h CreateWaveformImage.h
# ln -s ../../src/CustomTableWidget.h CustomTableWidget.h
# ln -s ../../src/FormatFileSize.h FormatFileSize.h
# ln -s ../../src/KeysFilter.h KeysFilter.h
# ln -s ../../src/ParseWavFile.h ParseWavFile.h
# ln -s ../../src/PCH.h PCH.h
# ln -s ../../src/PopulateWavTable.h PopulateWavTable.h
# ln -s ../../src/ProcessGUI.h ProcessGUI.h
# ln -s ../../src/WaveformDelegate.h WaveformDelegate.h 
# ln -s ../../src/WavInfo.h WavInfo.h
```

Do NOT edit any files under logic/.
This is just a copy of standalone application!
This setup lacks main.cpp only.
