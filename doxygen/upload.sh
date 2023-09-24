#/usr/bin/env bash

# SourceForge SFTP server
SERVER=web.sourceforge.net
USER=wolodiam
# Password in 'passwd' file

# Directories
REMOTE_DIR=/home/project-web/cbuild/htdocs
DOXYGEN_DIR=html
WIKI_DIR=wiki
DIR1=html
DIR2=wiki
FILE1=index.html
FILE2=doxygen.svg
FILE3=main.css

# Upload
sshpass -f passwd sftp $USER@$SERVER <<EOF
cd $REMOTE_DIR
rm $FILE1
rm $FILE2
rm $FILE3
rmdir $DIR1
rmdir $DIR2
mkdir $DIR1
mkdir $DIR2
put $FILE1
put $FILE2
put $FILE3
cd $DIR1
put $DIR1/*
cd ../
cd $DIR2
put $DIR2/*
exit
EOF
