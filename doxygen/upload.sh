#/usr/bin/env bash
# Get script dir
scripts_dir=$(dirname "$(readlink -f "$0")")
# Call all scripts
$scripts_dir/upload-doxygen.sh
$scripts_dir/upload-wiki.sh
$scripts_dir/upload-doxygen.sh
$scripts_dir/upload-latest.sh
