#!/bin/sh
#
XML=`ls xproto/*.xml`
OUT=lua_bindings

mkdir -p $OUT 2>/dev/null
for header in $XML; do
	lua ./lua_client.lua -o $OUT $header || exit
done

touch *.nobj.lua ; cd build ; make

