# Samples Scanner

Work in progress!!

Aggregated visual presentation of the *.wav graphs of the samples.

<img width="910" height="639" alt="Screenshot From 2026-08-15 02-12-46" src="https://github.com/user-attachments/assets/eb4888a3-7d91-446d-a3e7-64c8be829562" />

Click on graph to play the sound.

Intended for use with LMMS | Plugin as Drum sample analysis.
Tip: For better graphs, remove too long or full song .wav audios from your samples folder.
Best suited for drumks, kicks, snares, clicks, shaker, pop, tom like short sounds.

The samples graph is adaptive to 1000ms playtime.
Aligned to left side of the grid.
If you click on the graph, it plays the corresponding sound.
F5 is likely to rescan the last operated folder.

There have been multiple files and dirs checks even after scanning is complete.
This is intentional and works as safety measure for the runtime.

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
```

## Building with CMake

```
mkdir build
cd build

cmake -S .. .
make

ctest

./scanner
./scanner_tests
```

## Runtime Safety

- [x] Skips too long samples (10s playtime)
- [x] Skips too large samples (100kb file size)
- [x] Skips too many samples (top 50 samples max)

## If samples are not in .wav format, convert them!

```
for i in *.ogg; do ffmpeg -i "$i" "${i%.ogg}.wav"; done

# or,
for i in *.flac; do ffmpeg -i "$i" "${i%.flac}.wav"; done
```

## ToDo

- `F5` will rescan the folder.
- `Esc` will close the window.
- ~~Pass samples directory as parameter in command line.~~
- Upgrade to Qt6, particularly using Multimedia.
