#!/bin/bash
# Build user scripts
function build_user()
{
    echo Building userspace files
    scripts=`dir scripts/*.cpp`
    files="${scripts}"
    echo User files - ${files}
    link="-lCBuild -Wl,-z,origin -Wl,-rpath,\$ORIGIN/CBuild/CBuild"
    g++ -c ${files} -fPIC -std=c++20 -Wall -Wextra -Wno-comments ${link}
    objects=`dir *.o`
    echo User objects - ${objects}
    g++ ${objects} ${link} -o CBuild.run
    rm -r ${objects}
}
# Build CBuild backend
function build_build()
{
    echo Building core dynamic library
    files=`dir CBuild/CBuild/src/*.cpp`
    echo Core files - ${files}
    g++ -c ${files} -fPIC -std=c++20 -lcrypto -Wall -Wextra -Wno-comments -Wno-deprecated-copy -Wl,-soname,libCBuild.so
    objects=`dir *.o`
    echo Core objects - ${objects}
    g++ ${objects} -lcrypto -shared -o libCBuild.so -Wl,-soname,libCBuild.so
    rm -r ${objects}
    mv libCBuild.so CBuild/CBuild
}
# Remove CBuild core and executable
function remove()
{
    if [ $1 == "core" ]; then
        rm -r CBuild/CBuild/libCBuild.so;
        rm -r CBuild/CBuild/libCBuild.dll;
        return;
    fi
    if [ $1 == "user" ]; then
        rm -r CBuild.run;
        return;
    fi
    if [ $1 == "all" ]; then
        rm -r CBuild/CBuild/libCBuild.so;
        rm -r CBuild/CBuild/libCBuild.dll;
        rm -r CBuild.run;
        return;
    fi
    echo "Usage - $0 rm [all/core/user]"
}
# Run CBuild
function run()
{
    ./CBuild.run $1
}


# Code
if [ $1 == "help" ]; then
	echo "\"help\" - this message";
	echo "\"user\" - build user scripts";
	echo "\"core\" - build CBuild backend dynamic library";
	echo "\"all\" - build core and the user";
	echo "\"pack\" - copy lib and headers, pack deb packet"
	echo "\"mkppa\" - pack ppa"
	echo "\"incv major\" - increment major version"
	echo "\"incv major\" - increment major version"
	echo "\"decv minor\" - decrement minor version"
	echo "\"decv major\" - decrement major version"
	echo "\"getv\" - print version"
	echo "\"mkppa\" - pack ppa"
	echo "\"rm core\" - remove core library";
	echo "\"rm user\" - remove user executable";
	echo "\"rm all\" - remove core library and user executable";
	echo "\"run <arg>\" - run CBuild executable with argument";
fi
if [ $1 == "user" ]; then
    build_user;
fi
if [ $1 == "core" ]; then
    build_build;
fi
if [ $1 == "all" ]; then
	build_build;
	build_user;
fi
if [ $1 == "rm" ]; then
    remove $2;
fi
if [ $1 == "pack" ]; then
    echo Copy rebuold script;
    rm -r ./deb/libcbuild/usr/bin;
    mkdir ./deb/libcbuild/usr/bin;
    cp ./rebuild.sh ./deb/libcbuild/usr/bin/CBuild_rebuild;
    echo Remove and copy lib;
    rm -r ./deb/libcbuild/usr/lib;
    mkdir ./deb/libcbuild/usr/lib;
    cp ./CBuild/CBuild/libCBuild.so ./deb/libcbuild/usr/lib/libCBuild.so;
    strip ./deb/libcbuild/usr/lib/libCBuild.so;
    echo Remove and copy headers;
    rm -r ./deb/libcbuild/usr/include;
    mkdir ./deb/libcbuild/usr/include/;
    mkdir ./deb/libcbuild/usr/include/CBuild;
    cp -r ./CBuild/headers/* ./deb/libcbuild/usr/include/CBuild/;
    echo Recreate changelog;
    rm ./deb/libcbuild/usr/share/doc/libcbuild/changelog.gz;
    cp ./deb/changelog.Debian ./deb/changelog;
    cd deb;
    gzip --best -n changelog;
    cd ..;
    mv ./deb/changelog.gz ./deb/libcbuild/usr/share/doc/libcbuild/changelog.gz;
    echo Chmod fix for dirs
    chmod 0755 ./deb/libcbuild/DEBIAN;
    chmod 0755 ./deb/libcbuild/usr;
    chmod 0755 ./deb/libcbuild/usr/include;
    chmod 0755 ./deb/libcbuild/usr/include/CBuild;
    chmod 0755 ./deb/libcbuild/usr/include/CBuild/build;
    chmod 0755 ./deb/libcbuild/usr/include/CBuild/task;
    chmod 0755 ./deb/libcbuild/usr/lib;
    chmod 0755 ./deb/libcbuild/usr/share;
    chmod 0755 ./deb/libcbuild/usr/share/doc;
    chmod 0755 ./deb/libcbuild/usr/share/doc/libcbuild;
    chmod 0755 ./deb/libcbuild/usr/bin;
    echo Chmod fix for files
    chmod 0755 ./deb/libcbuild/usr/lib/libCBuild.so;
    chmod 0755 ./deb/libcbuild/usr/bin/CBuild_rebuild;
    chmod -x ./deb/libcbuild/usr/lib/libCBuild.so;
    chmod 0644 ./deb/libcbuild/usr/include/CBuild/*.hpp;
    chmod 0644 ./deb/libcbuild/usr/include/CBuild/task/*.hpp;
    chmod 0644 ./deb/libcbuild/usr/include/CBuild/build/*.hpp;
    chmod 0644 ./deb/libcbuild/usr/share/doc/libcbuild/changelog.gz;
    echo Building deb
    cd deb;
    dpkg-deb --root-owner-group --build libcbuild;
    cd ..;
    echo Running lintian
    lintian ./deb/libcbuild.deb;
    echo End of packaging deb
fi
if [ $1 == "mkppa" ]; then
    version=`cat ./ppa/ubuntu/version`
    echo `expr index "$version" v` > tmp
    end=`cat tmp`
    end=$((end - 1))
    version=${version:0:$end}
    rm tmp
    echo Copy deb
    gpg --import private_gpg_key.asc
    cp ./deb/libcbuild.deb ./ppa/ubuntu/libcbuild-${version}.deb;
    cd ./ppa/ubuntu/;
    echo Not forget to change version though ./build.sh incv major/minor
    echo Create Packages and Packages.gs
    dpkg-scanpackages --multiversion . > Packages;
    gzip -k -f Packages;
    echo Creare Release
    apt-ftparchive release . > Release
    gpg --default-key "w_melnyk@outlook.com" -abs -o - Release > Release.gpg
    gpg --default-key "w_melnyk@outlook.com" --clearsign -o - Release > InRelease
fi
if [ $1 == "incv" ]; then
    version_old=`cat ./ppa/ubuntu/version`
    echo `expr index "$version_old" .` > tmp
    dot=`cat tmp`
    echo `expr index "$version_old" v` > tmp
    end=`cat tmp`
    end=$((end - 1))
    dot=$((dot - 1))
    major=${version_old:0:$dot}
    dot="$dot"
    end="$end"
    dot=$((dot + 1))
    end=$((end - dot))
    minor=${version_old:$dot:$end}
    if [ $2 == "major" ]; then
        major=$((major + 1))
	minor="0"
    fi
    if [ $2 == "minor" ]; then
        minor=$((minor + 1))
    fi
    version_new=${major}.${minor}v
    rm tmp
    echo $version_new > ./ppa/ubuntu/version
    echo Vesion: $version_new
    echo Not forget to change version in.deb \(deb/libcbuild/DEBIAN/control\) and update changelog \(deb/changelog.Debian\) and in CBuild headers \(CBuild/headers/CBuild_defs.hpp\)
fi
if [ $1 == "decv" ]; then
    version_old=`cat ./ppa/ubuntu/version`
    echo `expr index "$version_old" .` > tmp
    dot=`cat tmp`
    echo `expr index "$version_old" v` > tmp
    end=`cat tmp`
    end=$((end - 1))
    dot=$((dot - 1))
    major=${version_old:0:$dot}
    dot="$dot"
    end="$end"
    dot=$((dot + 1))
    end=$((end - dot))
    minor=${version_old:$dot:$end}
    if [ $2 == "major" ]; then
        major=$((major - 1))
	minor="0"
    fi
    if [ $2 == "minor" ]; then
        minor=$((minor - 1))
    fi
    version_new=${major}.${minor}v
    rm tmp
    echo $version_new > ./ppa/ubuntu/version
    echo Vesion: $version_new
    echo Not forget to change version in.deb \(deb/libcbuild/DEBIAN/control\) and update changelog \(deb/changelog.Debian\) and in CBuild headers \(CBuild/headers/CBuild_defs.hpp\)
fi
if [ $1 == "getv" ]; then
    version=`cat ./ppa/ubuntu/version`
    echo Vesion: $version
fi
