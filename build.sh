#!/usr/bin/env bash
# Environment
set -euo pipefail
# constants
CARGS="-O3 -g -std=c99 -Wall -Wextra -Wpedantic"
# Global variables
Silent="no"  # Need to be set to `yes`
Verbose="no" # Need to be set to `yes`
ScriptPath=""
#Handle Terminal
if test -t 1; then
	ncolors=$(tput colors || true)
	if test -n "$ncolors" && test $ncolors -ge 8; then
		reset="$(tput sgr0)"
		black="$(tput setaf 0)"
		red="$(tput setaf 1)"
		green="$(tput setaf 2)"
		yellow="$(tput setaf 3)"
		blue="$(tput setaf 4)"
		magenta="$(tput setaf 5)"
		gray="$(tput setaf 7)"
		cyan="$(tput setaf 6)"
	else
		reset=""
		red=""
		green=""
		yellow=""
		blue=""
		magenta=""
		cyan=""
	fi
fi
# pack subcommand
pack_header_strip() {
	cat "src/$1" | tr "\n" "$" | sed "s/#include \"[^\"]*\"\\\$//g" | sed "s/\\/\\/ Project includes\\\$//g" | tr "$" "\n" >>cbuild.h
}
pack_nostrip() {
	cat "src/$1" >>"cbuild.h"
}
pack_ifdef() {
	echo "#ifdef CBUILD_IMPLEMENTATION" >>cbuild.h
}
pack_endif() {
	echo "#endif // CBUILD_IMPLEMENTATION" >>cbuild.h
}
pack() {
	call_cmd rm cbuild.h
	call_cmd_ns pack_nostrip "common.h"
	call_cmd_ns pack_header_strip	"Term.h"
	call_cmd_ns pack_header_strip "DynArray.h"
	call_cmd_ns pack_header_strip "StringView.h"
	call_cmd_ns pack_header_strip "StringBuilder.h"
	call_cmd_ns pack_header_strip "Log.h"
	# call_cmd_ns pack_header_strip "Proc.h"
	# call_cmd_ns pack_header_strip "Command.h"
	# call_cmd_ns pack_header_strip "FS.h"
	# call_cmd_ns pack_header_strip "Compile.h"
	call_cmd_ns pack_ifdef
	call_cmd_ns pack_header_strip "impl.c"
	call_cmd_ns pack_endif
	call_cmd clang-format --style file -i "cbuild.h"
	return
}
# docs subcommand
docs() {
	if [ "$#" -lt 1 ]; then
		help
		return
	fi
	operation="$1"
	shift
	case "$operation" in
		"wiki") docs_wiki "$@" ;;
		"doxygen") docs_doxygen "$@" ;;
		"serve") docs_serve "$@" ;;
		*) help "$@" ;;
	esac
}
docs_wiki() {
	call_cmd ln -fsrT "wiki/doxygen/html" "wiki/mkdocs/docs/doxygen"
	call_cmd mkdocs build
}
docs_doxygen() {
	call_cmd doxygen doxygen.conf
}
docs_serve() {
	call_cmd mkdocs serve
}
# test subcommand
test_cmd() {
	if [ "$#" -lt 1 ]; then
		test_run_all
	else
		test_run "$1"
	fi
}
test_run() {
	call_cmd rm -rf "build/$1.*"
	printf "${green}Building test \"${red}$1${green}\" into executable.${reset}"
	if [ "$Silent" == "no" ]; then
		printf "${red} GCC output will be shown.${reset}\n"
	else
		printf "\n"
	fi
	if [ "$Silent" == "no" ]; then
		printf "${cyan}%s${reset}\n" "---------- Begin of compiler output ----------"
	fi
	NEWCARGS="$CARGS"
	if [ -f "tests/$1.args" ]; then
		NEWCARGS="$CARGS $(cat "tests/$1.args")"
	fi
	call_cmd gcc $NEWCARGS tests/"$1".c src/impl.c -o build/test_"$1".run
	ERR=$?
	if [ "$Silent" == "no" ]; then
		printf "${cyan}%s${reset}\n" "----------  End of compiler output  ----------"
	fi
	if [ "$ERR" -ne 0 ]; then
		printf "${red}Error, test \"${green}$1${red}\" failed to build!${reset}\n"
	else
		echo "Test output will be saved in \"build/test_${1}_out.txt\""
		call_cmd_ns ./build/test_"$1".run >build/test_"$1"_out.txt || true
		ERR=$?
		printf "${cyan}%s${reset}\n" "----------   Begin of test output   ----------"
		cat build/test_"$1"_out.txt
		printf "${cyan}%s${reset}\n" "----------    End of test output    ----------"
	fi
	return $ERR
}
test_run_all() {
	for file in tests/*.c; do
		file="$(basename -- "$file")"
		file="${file%.*}"
		test_run "$file"
		ERR=$?
		if [ "$ERR" -ne 0 ]; then
			exit 1
		fi
	done
}
# clean subcommand
clean() {
	call_cmd rm -rf wiki/mkdocs/site/
	call_cmd rm -rf CBuild.h
	call_cmd rm -rf build
}
# help subcommand
help() {
	printf "Usage:\n"

	printf "\t./build.sh [global arguments] <subcomand> [additional args]\n"

	printf "\n"

	printf "Global arguments:\n"

	printf "\t-V/--verbose - Print all used commands\n"
	printf "\t-v/--version - Display build-script version\n"
	printf "\t-h/--help - Display this message\n"
	printf "\t-s/--silent - Don't display compiler/linker output\n"

	printf "\n"

	printf "Subcommands:\n"

	printf "\tpack - Pack all lib headers into \"CBuild.h\"\n"
	printf "\t\tUsage: ./build.sh pack\n"

	printf "\tdocs - Manage documentation\n"
	printf "\t\tUsage: ./build.sh docs <operation>\n"
	printf "\t\tList of operations:\n"
	printf "\t\t\twiki - build custom wiki\n"
	printf "\t\t\tdoxygen - build doxygen\n"
	printf "\t\t\tserve - run webserver for a wiki\n"

	printf "\ttest - Run tests\n"
	printf "\t\tUsage: ./build.sh test [test_id]\n"

	printf "\tclean - Clean all build artifacts\n"
	printf "\t\tUsage: ./build.sh clean\n"

	printf "\ttags - Generate CTags\n"

	printf "\thelp - Display this message\n"
	printf "\t\tUsage: ./build.sh help\n"

	printf "\tversion - Display build-script version\n"
	printf "\t\tUsage: ./build.sh version\n"
}
# Version
version() {
	printf "CBuild's bash buildscript version v1.1.\n"
	printf "\n"
	printf "Licensed under MIT license.\n"
	printf "\n"
	printf "Created by WolodiaM\n"
}
# Handle global flags
handle_silent() {
	Silent="yes"
	parse_args "$@"
}
handle_verbose() {
	Verbose="yes"
	parse_args "$@"
}
# Ignore silent flag. Needed for commands that already use redirect
call_cmd_ns() {
	if [ "$Verbose" == "yes" ]; then
		printf "${gray}CMD: "
		printf "%s " "$@"
		printf "${reset}\n"
	fi
	$@
}
call_cmd() {
	if [ "$Verbose" == "yes" ]; then
		printf "${gray}CMD: "
		printf "%s " "$@"
		printf "${reset}\n"
	fi
	if [ "$Silent" == "yes" ]; then
		$@ 2>/dev/null 1>/dev/null
	else
		$@
	fi
}
# Generate CTags
tags() {
	ctags -eR .
}
# Cli parser
parse_args() {
	if [ "$#" -lt 1 ]; then
		help "$@"
		return 1
	fi
	arg="${1:-}"
	shift
	case "$arg" in
		"pack") pack "$@" ;;
		"docs") docs "$@" ;;
		"test") test_cmd "$@" ;;
		"clean") clean "$@" ;;
		"tags") tags "$@" ;;
		"help") help "$@" ;;
		"-h") help "$@" ;;
		"--help") help "$@" ;;
		"version") version "$@" ;;
		"-v") version "$@" ;;
		"--version") version "$@" ;;
		"-s") handle_silent "$@" ;;
		"--silent") handle_silent "$@" ;;
		"-V") handle_verbose "$@" ;;
		"--verbose") handle_verbose "$@" ;;
		*) help "$@" ;;
	esac
}
# Main
ScriptPath=$(cd $(dirname "$0") && pwd)
if [ "$#" -lt 1 ]; then
	help "$@"
	exit 1
fi
call_cmd mkdir -p build
parse_args "$@"
