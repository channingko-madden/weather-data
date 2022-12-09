# weather-data
Weather data [technical assessment](docs/Technical_Task.pdf). 

### One-time setup for vcpkg
```bash
git submodule update --init
cd extern/vcpkg
./bootstrap-vcpkg.sh -disableMetrics
```
### Use CMake to Build 
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Test Scripts
