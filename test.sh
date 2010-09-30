#!/bin/sh
#

LUA_CPATH="$LUA_CPATH;./build/?.so;;" LUA_PATH="$LUA_PATH;./build/?.lua;;" lua -i ./test.lua

