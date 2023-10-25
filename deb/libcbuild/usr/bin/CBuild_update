#!/usr/bin/env bash
# Variables
CACHE_DIR=$HOME/.local/tmp
CACHE_VER_FILE=ver
CACHE_DEB_FILE=libcbuild.deb
# Script
if [ "$1" == "--help" ] || [ "$1" == "-h" ]; then
    echo "Usage: \"$0\" to get lastest version";
    echo "Usage: \"$0 <version>\" to get specific version";
    echo "Usage: \"$0 -h|--help\" to get this message";
    exit 0;
fi
# Create cache dir
mkdir -p $CACHE_DIR
# Pull version from repo
if [ -n "$1" ]; then
    LATEST_VERSION="$1"
else
    curl -s -o "$CACHE_DIR/$CACHE_VER_FILE" "https://cbuild.sourceforge.io/ppa/ubuntu/version"
    LATEST_VERSION=$(cat "$CACHE_DIR/$CACHE_VER_FILE" | sed -E 's/^([0-9]+\.[0-9]+)v$/\1/')
fi
echo "Selected version: ${LATEST_VERSION}"
# Get file from repo
ERR=$(curl -s -w "%{http_code}" -o "$CACHE_DIR/$CACHE_DEB_FILE" "https://cbuild.sourceforge.io/ppa/ubuntu/libcbuild-${LATEST_VERSION}.deb")
if [[ "$ERR" -ne "200" ]]; then
    echo "Error: File with version ${LATEST_VERSION} is not there";
    rm -rf $CACHE_DIR;
    exit 0;
fi
# Install package
echo "Warning: Dependency checking are disabled, you need to have glibc6 and libstdc++6 with c++20 support!"
echo "Depends line from package control: libc6 (>= 2.17), libstdc++6 (>= 4.9)"
sudo dpkg --force-depends -i "$CACHE_DIR/$CACHE_DEB_FILE"
# Clear cache dir
rm -rf $CACHE_DIR
exit 0
