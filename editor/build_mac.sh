#!/usr/bin/env bash

APP=cHaos3d.app
CONTENTS=$APP/Contents
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd $DIR

mkdir -p $CONTENTS/MacOS $CONTENTS/script $CONTENTS/Frameworks
plutil -convert binary1 -o $CONTENTS/Info.plist Info.plist 
cp chaos3d $CONTENTS/MacOS/.

LIBS="../external/libs/macosx/* ../external/libs/wx/lib*.dylib"
LIB_DIR="$CONTENTS/MacOS/."
for each in $LIBS; do
    cp -a $each $LIB_DIR
done

FRMWKS="../external/powervr32/OSX/x86/Lib/lib*.dylib"
FRM_DIR="$CONTENTS/Frameworks/."
for each in $FRMWKS; do
    cp -a $each $FRM_DIR
done
touch $APP
