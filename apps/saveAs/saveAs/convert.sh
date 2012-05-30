#!/bin/bash
find bin/data/LIBRARY -name "*.png" | awk -F "." '{printf("/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool -e PVRTC --channel-weighting-perceptual --bits-per-pixel-4 -o \"%s.pvr\" -f PVR \"%s.png\";rm \"%s.png\"\n",$1,$1,$1)}' | sh
