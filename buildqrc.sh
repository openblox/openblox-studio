#!/bin/bash
# Do not invoke this directly

SRCDIR=$1
RCC=$2

cd $SRCDIR/../rc
ls -lah class_icons
sleep 3000
$RCC -o $SRCDIR/qrc_resources.cpp resources.qrc
