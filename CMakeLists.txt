#
# Lua bindings for XCB
#
cmake_minimum_required(VERSION 2.8)

project(luaxcb C)

set(BUILD_SHARED_LIBS TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(INSTALL_CMOD ${CMAKE_INSTALL_PREFIX}/lib/lua/5.1 CACHE PATH
				"Directory to install Lua binary modules (configure lua via LUA_CPATH)")
set(LUA_NATIVE_OBJECTS_PATH ../LuaNativeObjects CACHE PATH
				"Directory to LuaNativeObjects bindings generator.")
set(USE_PRE_GENERATED_BINDINGS TRUE CACHE BOOL
				"Set this to FALSE to re-generate bindings using LuaNativeObjects")

#
# Find system libraries
#
set(COMMON_CFLAGS "${CFLAGS}")
set(COMMON_LDFLAGS)
set(COMMON_LIBS)

include(FindPkgConfig)
include(CustomMacros)

## Lua 5.1.x
pkg_search_module(LUA REQUIRED lua lua5.1 lua-5.1)
set(COMMON_CFLAGS "${COMMON_CFLAGS} ${LUA_CFLAGS}")
set(COMMON_LDFLAGS "${COMMON_LDFLAGS} ${LUA_LDFLAGS}")

## XCB 1.5
pkg_search_module(XCB REQUIRED xcb)
set(COMMON_CFLAGS "${COMMON_CFLAGS} ${XCB_CFLAGS}")
set(COMMON_LDFLAGS "${COMMON_LDFLAGS} ${XCB_LDFLAGS}")


## LuaNativeObjects
include(LuaNativeObjects)

#
# Setup CMAKE_C_FLAGS* and CMAKE_CXX_FLAGS*
#
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -Wall -Wextra -Wshadow -W -pedantic -std=gnu99 -fgnu89-inline")
	set(CMAKE_C_FLAGS_RELEASE        "${CMAKE_C_FLAGS_RELEASE}     -O3 -march=athlon64 -g")
	set(CMAKE_C_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG}       -O0 -g")
	set(CMAKE_C_FLAGS_PROFILE        "${CMAKE_C_FLAGS_PROFILE}     -O2 -g -DNDEBUG")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_WITHDEBINFO} -O2 -g")
endif(CMAKE_COMPILER_IS_GNUCC)
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -Wall -Wextra -Wshadow -W -pedantic")
	set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}     -O3 -march=athlon64 -g")
	set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}       -O0 -g")
	set(CMAKE_CXX_FLAGS_PROFILE        "${CMAKE_CXX_FLAGS_PROFILE}     -O2 -g -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_WITHDEBINFO} -O2 -g")
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${LUA_INCLUDE_DIR})

## Lua XCB
set(LUA_XCB_NOBJ_SRC
	xcb.nobj.lua
)
set(LUA_XCB_SRC
)

if(${USE_PRE_GENERATED_BINDINGS})
	set(LUA_XCB_NOBJ_SRC pre_generated-xcb.nobj.c)
else()
	# Generate Lua bindings.
	GenLuaNativeObjects(LUA_XCB_NOBJ_SRC)
endif()
set(LUA_XCB_SRC ${LUA_XCB_SRC} ${LUA_XCB_NOBJ_SRC})

add_library(xcb MODULE ${LUA_XCB_SRC})
target_link_libraries(xcb ${COMMON_LIBS})
add_target_properties(xcb COMPILE_FLAGS "${COMMON_CFLAGS}")
add_target_properties(xcb LINK_FLAGS "${LD_FLAGS} ${COMMON_LDFLAGS}")
set_target_properties(xcb PROPERTIES PREFIX "")
set_target_properties(xcb PROPERTIES OUTPUT_NAME xcb)

install(TARGETS xcb
        DESTINATION "${INSTALL_CMOD}")

