#/usr/bin/env bash

# SourceForge SFTP server
SERVER=web.sourceforge.net
USER=wolodiam
# Password in 'passwd' file
#PPA_BASE=~/dev/c++/CBuild
if [ -z "$1" ]; then
  echo "Error: Please provide base path as the first argument."
  exit 1
fi
BASE="$1/doxygen"

# Directories
REMOTE_DIR=/home/project-web/cbuild/htdocs
DIR_BASE=download
FILE=latest.tar.gz

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
rm $DIR_BASE/$FILE
rmdir $DIR_BASE
# Recreate dirs
mkdir $DIR_BASE
cd $DIR_BASE
# Upload files
put $BASE/latest.tar.gz
"
# Upload
sshpass -f "$BASE/passwd" sftp $USER@$SERVER <<EOF
$cmd_list
EOF
