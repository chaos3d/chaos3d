#!/usr/bin/env bash

APP=cHaos3d.app
CONTENTS=$APP/Contents
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd $DIR

mkdir -p $CONTENTS/MacOS
plutil -convert binary1 -o $CONTENTS/Info.plist Info.plist 
cp chaos3d $CONTENTS/MacOS/.

touch $APP
