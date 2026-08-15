## MultiMedia
```
sudo apt install qtmultimedia5-dev
```

## Installation
```
mkdir build
cd build

cmake -S ../src build
make

./scanner
```

# # Runtime Safety
- [x] Skips too long samples (10s playtime)
- [x] Skips too large samples (100kb file size)
- [x] Skips too many samples (top 50 samples max)
