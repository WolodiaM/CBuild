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
#BASE=$HOME/dev/c++/CBuild/doxygen

# Directories
REMOTE_DIR=/home/project-web/cbuild/htdocs
DOXYGEN_DIR_MAIN=html
DOXYGEN_DIR_SEARCH=search

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
rm $DOXYGEN_DIR_MAIN/$DOXYGEN_DIR_SEARCH/*
rm $DOXYGEN_DIR_MAIN/*
rmdir $DOXYGEN_DIR_MAIN/$DOXYGEN_DIR_SEARCH
rmdir $DOXYGEN_DIR_MAIN
# Recreate dirs
mkdir $DOXYGEN_DIR_MAIN
mkdir $DOXYGEN_DIR_MAIN/$DOXYGEN_DIR_SEARCH
# Upload first batch of files
cd $DOXYGEN_DIR_MAIN
put $BASE/$DOXYGEN_DIR_MAIN/*
# Upload second batch of files
cd $DOXYGEN_DIR_SEARCH
put $BASE/$DOXYGEN_DIR_MAIN/$DOXYGEN_DIR_SEARCH/*
"

# Upload
sshpass -f "$BASE/passwd" sftp $USER@$SERVER <<EOF
$cmd_list
EOF
