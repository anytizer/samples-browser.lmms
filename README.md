# Samples Scanner and Importer

Work in progress!!

It aggregates visual presentation of the uncompressed *.wav samples graphs.
The graph shows loudness and duration of the sample in the list.
Pre-Delays can be determined by looking into mis-aligned sample.
You can then decide which samples to include in your Pattern Editor track.

<img width="910" height="639" alt="Screenshot" src="https://github.com/user-attachments/assets/eb4888a3-7d91-446d-a3e7-64c8be829562" />

Click on sound graph to play/preview the sound.

Intended for:
    - use with [LMMS](https://github.com/LMMS/lmms/) DAW.
    - stand alone application with limited feaures

Tip: For better graphs, remove too long or full song .wav audios from your samples folder.
Best suited for drums, kicks, snares, clicks, shaker, pop, tom like short and one-shot sounds.

- Graph is aligned to left side of the grid.
- F5 to rescan the last operated folder.

There have been multiple files and dirs checks even after scanning is complete.
This is intentional and works as safety measure during the runtime.

This piece of software has 2 operating modes:
- Standalone Qt5 application.
- Qt6 based plugin for LMMS

Each of them handles the audio play in their own manners, using callbacks to common codebase.

## Personalized workflow

- Collect only necessary samples in your project folder.
- Pre-edit them.
- Keep small file sizes, playtimes and total count.
- Under LMMS, open Samples Scanner and browse these samples.
- Preview the sound by graph, audio play, and double click the row to import them in tracks.

Usage:

```
./scanner /PATH/TO/wavs/
```

eg:

```
./scanner ./samples/
```

## Install MultiMedia Dependency

```
sudo apt install qtmultimedia5-dev

# For Qt6
sudo apt install libqt6multimedia6 qt6-multimedia-dev
```

## Building with CMake

```
mkdir build
cd build

cmake -S .. .
make

ctest

./scanner_tests
./scanner

# Optionally
sudo make install
```

## Runtime Safety

- [x] Skips too long samples (10s playtime)
- [x] Skips too large samples (100kb file size)
- [x] Skips too many samples (top 50 samples max)

## If samples are not in .wav format, convert them!

Alert: Not recommended convert and depend on the files. Only for references.

### Linux Shell

```
for i in *.ogg; do ffmpeg -loglevel quiet -i "$i" "${i%.ogg}.wav"; done

# or,
for i in *.flac; do ffmpeg -loglevel quiet -i "$i" "${i%.flac}.wav"; done

# move the .wav files into wavs/ directory
mkdir wavs
mv *.wav wavs/

scanner wavs
```

### PowerShell

```
Get-ChildItem *.ogg | ForEach-Object {
    $newName = $_.BaseName + ".wav"
    ffmpeg -loglevel quiet -i $_.FullName $newName
}
```

Optionally pass parameter: `-c:a pcm_f32le`.

You MUST own that "folder"; or have read/write permissions.
DO NOT use root account.

# For LMMS

@see [Plugin's ReadMe](plugin/README.md)

## ToDo

- Show durations in ms.
- Inform that CMake for LMMS was modified (added Qt6::Multimedia)
    - ~~almost unuseful now;~~
    - ~~using SamplePlayHandle().~~
- Too long file names are trimmed and accessed in alternative manner.
- Qt6 version is non-functional at the time:
    - Upgrade to Qt6, particularly using Qt6::Multimedia.
    - Windows: `cmake -S . build -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1"`
    - Linux Qt6: `cmake -S . build DCMAKE_PREFIX_PATH=/home/USER/Qt/6.11.1/gcc_64`
- From an array, populate column widths, names, resizabilities.
- Handle key presses:
    - ~~`Esc` will hide the window. - Properly close when ESC.~~
    - ~~`F5` will rescan the folder.~~
    - ~~Deleting layout is not efficient.~~
- ~~Pass samples directory as parameter in command line.~~
- ~~Set max col width of wav graphs to 300.~~
- ~~Pass a function pointer to send a sample to LMMS Pattern Editor.~~
- ~~QSound/Effect to repalce with LMMS Native features (Sample Play Handle)~~
- ~~using callbacks.~~
- When a tombstone track created for sample, do not add again. May be, track samples by index?
    - ~~Sometimes, the sample tracks are not painted.~~
    - ~~Add Pattern track manually, once to ease. => Using signal: ps->trackUpdated()~~
- ~~Change plguin's window title from within the code~~
- Fix threading realated wanrings.
- Limit the #define'ed constants within application/plugin's namespace.
- Reduce console logs.
- Disconnect player immediately if mouse moved away; not waiting to finish the long sample.
    - eg. when playing longer samples, thread stuck in playing full length.
- called Track::getClip( 0 ), but Clip 0 doesn't exist
