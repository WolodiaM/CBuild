#/usr/bin/env sh
# Vars
TEMP_DIR=$HOME/.CBuild_temp_install_path
DOWNLOAD_FILE=CBuild_latest.tar.gz
LINK="https://cbuild.sourceforge.io/download/latest.tar.gz"
BASE_DIR=${1:-/usr} # Need to be /usr/local on Fedora Silverblue or simmilar OSes
# Create temp dir
mkdir -p $TEMP_DIR/unpack
# Get archive
curl -o $TEMP_DIR/$DOWNLOAD_FILE $LINK 
# Unpack archive
tar -xvzf $TEMP_DIR/$DOWNLOAD_FILE -C $TEMP_DIR/unpack/
# Create basic folders
sudo mkdir -p $BASE_DIR/lib
sudo mkdir -p $BASE_DIR/bin
sudo mkdir -p $BASE_DIR/include/CBuild
sudo mkdir -p $BASE_DIR/share/man/man1
sudo mkdir -p $BASE_DIR/share/doc/libcbuild
# !Create include folders, can change!!!!!!!
sudo mkdir -p $BASE_DIR/include/CBuild/build
sudo mkdir -p $BASE_DIR/include/CBuild/generator
sudo mkdir -p $BASE_DIR/include/CBuild/task
# Copy files
sudo cp -r $TEMP_DIR/unpack/bin/* $BASE_DIR/bin/
sudo cp -r $TEMP_DIR/unpack/lib/* $BASE_DIR/lib/
sudo cp -r $TEMP_DIR/unpack/share/man/man1/* $BASE_DIR/share/man/man1/
sudo cp -r $TEMP_DIR/unpack/share/doc/libcbuild/* $BASE_DIR/share/doc/libcbuild/
sudo cp -r $TEMP_DIR/unpack/include/CBuild/* $BASE_DIR/include/CBuild/
# Cleaning
rm -rf $TEMP_DIR