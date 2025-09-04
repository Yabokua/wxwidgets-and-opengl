# Pre-compiled Executables

## Quick Start

### Linux (x64):
```bash
cd bin/linux-x64
chmod +x MyOpenGLApp
./MyOpenGLApp

## System Requirements

### Linux:
- Ubuntu 18.04+ / equivalent distribution
- OpenGL 3.3+ support
- GTK3 libraries (usually pre-installed)

### Installing libs
sudo apt install libwxgtk3.0-gtk3-0v5 libglew2.1 libgl1-mesa-glx

### Build with CMake
mkdir build
cd build
cmake ..
make -j$(nproc)

or use "Releases" - https://github.com/Yabokua/wxwidgets-opengl/releases
