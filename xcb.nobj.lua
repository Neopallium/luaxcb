--***************************************************************************
--* Copyright (C) 2009-2010 by Robert G. Jakabosky <bobby@sharedrealm.com>  *
--***************************************************************************

-- some standard basetypes
basetype "uint32_t *" "nil" "NULL"

c_module "xcb" {
hide_meta_info = false,

sys_include "string.h",
sys_include "stdbool.h",
sys_include "xcb/xcb.h",

subfiles {
	"lua_bindings/xproto.lua",
},

c_function "connect" {
  c_call "xcb_connection_t *" "xcb_connect" {
	  "const char *", "displayname", "int", "&screenp>2"
	},
},

object "xcb_connection_t" {
	destructor "disconnect" {
		c_method_call "void" "xcb_disconnect" {},
	},
	method "flush" {
		c_method_call "void" "xcb_flush" {},
	},
	method "get_setup" {
		c_method_call "const xcb_setup_t *" "xcb_get_setup" {},
	},
	method "generate_id" {
		c_method_call "uint32_t" "xcb_generate_id" {},
	},
	method "get_file_descriptor" {
		c_method_call "int" "xcb_get_file_descriptor" {},
	},
	method "has_error" {
		c_method_call "bool" "xcb_connection_has_error" {},
	},
},
}
