export TARGET_ARCH=desktop
export UI_TYPE=qt4
export TARGET_STATUS=release

#export INCLUDE=-I/usr/include/glib-2.0

if [ "$ARCH" = "i486" ]; then
  SLKCFLAGS="-O2 -march=i486 -mtune=i686"
  LIBDIRSUFFIX=""
  ARCHQUADLET=""
elif [ "$ARCH" = "i686" ]; then
  SLKCFLAGS="-O2 -march=i686 -mtune=i686"
  LIBDIRSUFFIX=""
  ARCHQUADLET=""
elif [ "$ARCH" = "x86_64" ]; then
  SLKCFLAGS="-O2 -fPIC"
  LIBDIRSUFFIX="64"
  ARCHQUADLET=""
else
  SLKCFLAGS="-O2"
  LIBDIRSUFFIX=""
  ARCHQUADLET=""
fi

export LANG=C
export ZLSHARED=no

#make clear

case "$1" in
  install)
    make install LIBDIR=/usr/lib${LIBDIRSUFFIX} ZLSHARED=no
    ;;
  *)
    make  $1  LIBDIR=/usr/lib${LIBDIRSUFFIX} ZLSHARED=no MOC=/usr/bin/moc
    ;;
esac

