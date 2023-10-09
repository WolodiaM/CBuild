#/usr/bin/env bash

# SourceForge SFTP server
SERVER=web.sourceforge.net
USER=wolodiam
# Password in 'passwd' file
if [ -z "$1" ]; then
  echo "Error: Please provide base path as the first argument."
  exit 1
fi
BASE="$1/doxygen"
# BASE=~/dev/c++/CBuild/doxygen

# Directories
REMOTE_DIR=/home/project-web/cbuild/htdocs
WIKI_DIR=wiki
WIKI_DOXYGEN_LOGO=doxygen.svg
WIKI_TOPLEVEL_CSS=main.css
WIKI_MAIN_PAGE=index.html

# Commands
# if [ -z "$2" ]; then
#   echo "Error: Please provide the SFTP commands as the second argument."
#   exit 1
# fi
# cmd_list="$2"
cmd_list="
# Go to base dir
cd $REMOTE_DIR
# Remove old files
rm $WIKI_MAIN_PAGE
rm $WIKI_TOPLEVEL_CSS
rm $WIKI_DOXYGEN_LOGO
rm $WIKI_DIR/*
rmdir $WIKI_DIR
# Recreate dirs
mkdir $WIKI_DIR
# Upload first batch of files
cd $WIKI_DIR
put $BASE/$WIKI_DIR/*
# Upload misk files
cd ../
put $BASE/$WIKI_MAIN_PAGE
put $BASE/$WIKI_TOPLEVEL_CSS
put $BASE/$WIKI_DOXYGEN_LOGO
"
# Upload
sshpass -f "$BASE/passwd" sftp $USER@$SERVER <<EOF
$cmd_list
EOF
