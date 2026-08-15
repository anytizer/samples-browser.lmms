Work in progress!!

## MultiMedia

```
sudo apt install qtmultimedia5-dev
```

## Building with CMake

```
mkdir build
cd build

cmake -S .. .
make

./scanner
```

## Runtime Safety

- [x] Skips too long samples (10s playtime)
- [x] Skips too large samples (100kb file size)
- [x] Skips too many samples (top 50 samples max)

## if samples are not in .wav format:

```
for i in *.ogg; do ffmpeg -i "$i" "${i%.ogg}.wav"; done
for i in *.flac; do ffmpeg -i "$i" "${i%.flac}.wav"; done
```

## ToDo

- `F5` will rescan the folder.
- `Esc` will close the window.
- Pass samples directory as parameter in command line.