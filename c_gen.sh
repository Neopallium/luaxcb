#!/bin/sh
#
XML=`ls xproto/*.xml`
OUT=c_bindings

mkdir -p $OUT 2>/dev/null
for header in $XML; do
	lua ./c_client.lua -o $OUT $header || exit
done
