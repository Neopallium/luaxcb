#!/bin/sh
#
XML=`ls xproto/*.xml`

for header in $XML; do
	lua ./c_client.lua -o c_bindings $header
done
