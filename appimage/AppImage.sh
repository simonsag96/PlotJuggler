#!/bin/bash

APPDIRPATH=../build/AppDir

# Uses linuxdeploy with linuxdeploy-plugin-qt to generate an AppImage
LINUXDEPLOY_URL="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT_URL="https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"

LINUXDEPLOY_APPIMAGE="linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT_APPIMAGE="linuxdeploy-plugin-qt-x86_64.AppImage"

# Get and extract linuxdeploy AppImage. Extraction is necessary since we are in a container without FUSE support
wget ${LINUXDEPLOY_URL}
chmod +x ${LINUXDEPLOY_APPIMAGE}
mkdir linuxdeploy/
mv ${LINUXDEPLOY_APPIMAGE} linuxdeploy/
cd linuxdeploy
./${LINUXDEPLOY_APPIMAGE} --appimage-extract
cd -

# Same for the plugin
wget ${LINUXDEPLOY_QT_URL}
chmod +x ${LINUXDEPLOY_QT_APPIMAGE}
mkdir linuxdeploy-plugin-qt/
mv ${LINUXDEPLOY_QT_APPIMAGE} linuxdeploy-plugin-qt/
cd linuxdeploy-plugin-qt
./${LINUXDEPLOY_QT_APPIMAGE} --appimage-extract
# Rename the plugin to match linuxdeploy's expected format
mv squashfs-root/AppRun squashfs-root/linuxdeploy-plugin-qt
cd -

PATH=$PATH:linuxdeploy-plugin-qt/squashfs-root ./linuxdeploy/squashfs-root/AppRun --appdir $APPDIRPATH -d ../PlotJuggler.desktop -i ../plotjuggler.png --plugin qt --output appimage
