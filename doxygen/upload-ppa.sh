#/usr/bin/env bash

# SourceForge SFTP server
SERVER=web.sourceforge.net
USER=wolodiam
# Password in 'passwd' file
if [ -z "$1" ]; then
  echo "Error: Please provide base path as the first argument."
  exit 1
fi
PPA_BASE="$1"
#PPA_BASE=~/dev/c++/CBuild
BASE=$PPA_BASE/doxygen

# Directories
REMOTE_DIR=/home/project-web/cbuild/htdocs
CONF_FILE=.htaccess
PPA_DIR_BASE=ppa
PPA_DIR_SUB=ubuntu

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
rm $CONF_FILE
rm $PPA_DIR_BASE/$PPA_DIR_SUB/*
rmdir $PPA_DIR_BASE/$PPA_DIR_SUB
rmdir $PPA_DIR_BASE
# Recreate dirs
mkdir $PPA_DIR_BASE
mkdir $PPA_DIR_BASE/$PPA_DIR_SUB
# Upload conf file
put $BASE/$CONF_FILE
# Upload ppa content
cd $PPA_DIR_BASE/$PPA_DIR_SUB
put $PPA_BASE/$PPA_DIR_BASE/$PPA_DIR_SUB/*
"
# Upload
sshpass -f "$BASE/passwd" sftp $USER@$SERVER <<EOF
$cmd_list
EOF
