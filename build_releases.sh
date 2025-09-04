#!/bin/bash
set -e

APP_NAME="MyOpenGLApp"

mkdir -p bin/linux-x64

rm -rf build-linux

echo "🔧 Собираем Linux x64..."
cmake -B build-linux -DCMAKE_BUILD_TYPE=Release
cmake --build build-linux --config Release -j$(nproc)

cp build-linux/$APP_NAME bin/linux-x64/
if [ -d "icon" ]; then
    cp -r icon bin/linux-x64/
fi

chmod +x bin/linux-x64/$APP_NAME

cd bin
tar -czf ${APP_NAME}-linux-x64.tar.gz linux-x64
cd ..

echo "Done!"
echo "Archive: bin/${APP_NAME}-linux-x64.tar.gz"

