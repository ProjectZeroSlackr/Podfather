#!/bin/sh
#set -vx
#exec >> /opt/Media/Podfather/Misc/Launch.log 2>&1

killall -15 ZeroLauncher >> /dev/null 2>&1
cd /opt/Media/Podfather
exec /opt/Media/Podfather/Podfather
