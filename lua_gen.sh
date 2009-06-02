#!/bin/sh
#
XML=`ls xproto/*.xml`

for header in $XML; do
	lua ./lua_client.lua -o lua_bindings $header
done
