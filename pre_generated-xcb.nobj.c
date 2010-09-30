/***********************************************************************************************
************************************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!! Warning this file was generated from a set of *.nobj.lua definition files !!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
************************************************************************************************
***********************************************************************************************/

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/* some Lua 5.0 compatibility support. */
#if !defined(lua_pushliteral)
#define lua_pushliteral(L, s) lua_pushstring(L, "" s, (sizeof(s)/sizeof(char))-1)
#endif

#if !defined(LUA_VERSION_NUM)
#define lua_pushinteger(L, n) lua_pushnumber(L, (lua_Number)n)
#define luaL_Reg luaL_reg
#endif

/* some Lua 5.1 compatibility support. */
#if !defined(LUA_VERSION_NUM) || (LUA_VERSION_NUM == 501)
/*
** Adapted from Lua 5.2.0
*/
static void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -nup);
    lua_pushstring(L, l->name);
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source)

#define lua_rawlen(L, idx) lua_objlen(L, idx)

#endif

#if LUA_VERSION_NUM == 502

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source, NULL)

static int luaL_typerror (lua_State *L, int narg, const char *tname) {
  const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                    tname, luaL_typename(L, narg));
  return luaL_argerror(L, narg, msg);
}

#endif

#define REG_PACKAGE_IS_CONSTRUCTOR 0
#define REG_MODULES_AS_GLOBALS 0
#define REG_OBJECTS_AS_GLOBALS 0
#define OBJ_DATA_HIDDEN_METATABLE 0
#define USE_FIELD_GET_SET_METHODS 0
#define LUAJIT_FFI 0


#include <string.h>
#include <stdbool.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>



#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#ifdef _MSC_VER
#define __WINDOWS__
#else
#if defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__

/* for MinGW32 compiler need to include <stdint.h> */
#ifdef __GNUC__
#include <stdint.h>
#include <stdbool.h>
#else

/* define some standard types missing on Windows. */
#ifndef __INT32_MAX__
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
#endif
#ifndef __INT64_MAX__
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
typedef int bool;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#endif

/* wrap strerror_s(). */
#ifdef __GNUC__
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) do { \
	strncpy((buf), strerror(errno), (buflen)-1); \
	(buf)[(buflen)-1] = '\0'; \
} while(0)
#endif
#else
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) strerror_s((buf), (buflen), (errno))
#endif
#endif

#define FUNC_UNUSED

#define LUA_NOBJ_API __declspec(dllexport)

#else

#define LUA_NOBJ_API LUALIB_API

#include <stdint.h>
#include <stdbool.h>

#define FUNC_UNUSED __attribute__((unused))

#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define assert_obj_type(type, obj) \
	assert(__builtin_types_compatible_p(typeof(obj), type *))
#else
#define assert_obj_type(type, obj)
#endif

void *nobj_realloc(void *ptr, size_t osize, size_t nsize);

void *nobj_realloc(void *ptr, size_t osize, size_t nsize) {
	(void)osize;
	if(0 == nsize) {
		free(ptr);
		return NULL;
	}
	return realloc(ptr, nsize);
}

#define obj_type_free(type, obj) do { \
	assert_obj_type(type, obj); \
	nobj_realloc((obj), sizeof(type), 0); \
} while(0)

#define obj_type_new(type, obj) do { \
	assert_obj_type(type, obj); \
	(obj) = nobj_realloc(NULL, 0, sizeof(type)); \
} while(0)

typedef struct obj_type obj_type;

typedef void (*base_caster_t)(void **obj);

typedef void (*dyn_caster_t)(void **obj, obj_type **type);

#define OBJ_TYPE_FLAG_WEAK_REF (1<<0)
#define OBJ_TYPE_SIMPLE (1<<1)
#define OBJ_TYPE_IMPORT (1<<2)
struct obj_type {
	dyn_caster_t    dcaster;  /**< caster to support casting to sub-objects. */
	int32_t         id;       /**< type's id. */
	uint32_t        flags;    /**< type's flags (weak refs) */
	const char      *name;    /**< type's object name. */
};

typedef struct obj_base {
	int32_t        id;
	base_caster_t  bcaster;
} obj_base;

typedef enum obj_const_type {
	CONST_UNKOWN    = 0,
	CONST_BOOLEAN   = 1,
	CONST_NUMBER    = 2,
	CONST_STRING    = 3
} obj_const_type;

typedef struct obj_const {
	const char      *name;    /**< constant's name. */
	const char      *str;
	double          num;
	obj_const_type  type;
} obj_const;

typedef enum obj_field_type {
	TYPE_UNKOWN    = 0,
	TYPE_UINT8     = 1,
	TYPE_UINT16    = 2,
	TYPE_UINT32    = 3,
	TYPE_UINT64    = 4,
	TYPE_INT8      = 5,
	TYPE_INT16     = 6,
	TYPE_INT32     = 7,
	TYPE_INT64     = 8,
	TYPE_DOUBLE    = 9,
	TYPE_FLOAT     = 10,
	TYPE_STRING    = 11
} obj_field_type;

typedef struct obj_field {
	const char      *name;  /**< field's name. */
	uint32_t        offset; /**< offset to field's data. */
	obj_field_type  type;   /**< field's data type. */
	uint32_t        flags;  /**< is_writable:1bit */
} obj_field;

typedef enum {
	REG_OBJECT,
	REG_PACKAGE,
	REG_META,
} module_reg_type;

typedef struct reg_impl {
	const char *if_name;
	const void *impl;
} reg_impl;

typedef struct reg_sub_module {
	obj_type        *type;
	module_reg_type req_type;
	const luaL_Reg  *pub_funcs;
	const luaL_Reg  *methods;
	const luaL_Reg  *metas;
	const obj_base  *bases;
	const obj_field *fields;
	const obj_const *constants;
	const reg_impl  *implements;
	int             bidirectional_consts;
} reg_sub_module;

#define OBJ_UDATA_FLAG_OWN (1<<0)
#define OBJ_UDATA_FLAG_LOOKUP (1<<1)
#define OBJ_UDATA_LAST_FLAG (OBJ_UDATA_FLAG_LOOKUP)
typedef struct obj_udata {
	void     *obj;
	uint32_t flags;  /**< lua_own:1bit */
} obj_udata;

/* use static pointer as key to weak userdata table. */
static char obj_udata_weak_ref_key[] = "obj_udata_weak_ref_key";

/* use static pointer as key to module's private table. */
static char obj_udata_private_key[] = "obj_udata_private_key";

#if LUAJIT_FFI
typedef int (*ffi_export_func_t)(void);
typedef struct ffi_export_symbol {
	const char *name;
	union {
	void               *data;
	ffi_export_func_t  func;
	} sym;
} ffi_export_symbol;
#endif





static obj_type obj_types[] = {
#define obj_type_id_xcb_char2b_t 0
#define obj_type_xcb_char2b_t (obj_types[obj_type_id_xcb_char2b_t])
  { NULL, 0, OBJ_TYPE_FLAG_WEAK_REF, "xcb_char2b_t" },
#define obj_type_id_xcb_point_t 1
#define obj_type_xcb_point_t (obj_types[obj_type_id_xcb_point_t])
  { NULL, 1, OBJ_TYPE_FLAG_WEAK_REF, "xcb_point_t" },
#define obj_type_id_xcb_rectangle_t 2
#define obj_type_xcb_rectangle_t (obj_types[obj_type_id_xcb_rectangle_t])
  { NULL, 2, OBJ_TYPE_FLAG_WEAK_REF, "xcb_rectangle_t" },
#define obj_type_id_xcb_arc_t 3
#define obj_type_xcb_arc_t (obj_types[obj_type_id_xcb_arc_t])
  { NULL, 3, OBJ_TYPE_FLAG_WEAK_REF, "xcb_arc_t" },
#define obj_type_id_xcb_format_t 4
#define obj_type_xcb_format_t (obj_types[obj_type_id_xcb_format_t])
  { NULL, 4, OBJ_TYPE_FLAG_WEAK_REF, "xcb_format_t" },
#define obj_type_id_xcb_visualtype_t 5
#define obj_type_xcb_visualtype_t (obj_types[obj_type_id_xcb_visualtype_t])
  { NULL, 5, OBJ_TYPE_FLAG_WEAK_REF, "xcb_visualtype_t" },
#define obj_type_id_xcb_depth_t 6
#define obj_type_xcb_depth_t (obj_types[obj_type_id_xcb_depth_t])
  { NULL, 6, OBJ_TYPE_FLAG_WEAK_REF, "xcb_depth_t" },
#define obj_type_id_xcb_screen_t 7
#define obj_type_xcb_screen_t (obj_types[obj_type_id_xcb_screen_t])
  { NULL, 7, OBJ_TYPE_FLAG_WEAK_REF, "xcb_screen_t" },
#define obj_type_id_xcb_setup_request_t 8
#define obj_type_xcb_setup_request_t (obj_types[obj_type_id_xcb_setup_request_t])
  { NULL, 8, OBJ_TYPE_FLAG_WEAK_REF, "xcb_setup_request_t" },
#define obj_type_id_xcb_setup_failed_t 9
#define obj_type_xcb_setup_failed_t (obj_types[obj_type_id_xcb_setup_failed_t])
  { NULL, 9, OBJ_TYPE_FLAG_WEAK_REF, "xcb_setup_failed_t" },
#define obj_type_id_xcb_setup_authenticate_t 10
#define obj_type_xcb_setup_authenticate_t (obj_types[obj_type_id_xcb_setup_authenticate_t])
  { NULL, 10, OBJ_TYPE_FLAG_WEAK_REF, "xcb_setup_authenticate_t" },
#define obj_type_id_xcb_setup_t 11
#define obj_type_xcb_setup_t (obj_types[obj_type_id_xcb_setup_t])
  { NULL, 11, OBJ_TYPE_FLAG_WEAK_REF, "xcb_setup_t" },
#define obj_type_id_xcb_key_press_event_t 12
#define obj_type_xcb_key_press_event_t (obj_types[obj_type_id_xcb_key_press_event_t])
  { NULL, 12, OBJ_TYPE_FLAG_WEAK_REF, "xcb_key_press_event_t" },
#define obj_type_id_xcb_button_press_event_t 13
#define obj_type_xcb_button_press_event_t (obj_types[obj_type_id_xcb_button_press_event_t])
  { NULL, 13, OBJ_TYPE_FLAG_WEAK_REF, "xcb_button_press_event_t" },
#define obj_type_id_xcb_motion_notify_event_t 14
#define obj_type_xcb_motion_notify_event_t (obj_types[obj_type_id_xcb_motion_notify_event_t])
  { NULL, 14, OBJ_TYPE_FLAG_WEAK_REF, "xcb_motion_notify_event_t" },
#define obj_type_id_xcb_enter_notify_event_t 15
#define obj_type_xcb_enter_notify_event_t (obj_types[obj_type_id_xcb_enter_notify_event_t])
  { NULL, 15, OBJ_TYPE_FLAG_WEAK_REF, "xcb_enter_notify_event_t" },
#define obj_type_id_xcb_focus_in_event_t 16
#define obj_type_xcb_focus_in_event_t (obj_types[obj_type_id_xcb_focus_in_event_t])
  { NULL, 16, OBJ_TYPE_FLAG_WEAK_REF, "xcb_focus_in_event_t" },
#define obj_type_id_xcb_keymap_notify_event_t 17
#define obj_type_xcb_keymap_notify_event_t (obj_types[obj_type_id_xcb_keymap_notify_event_t])
  { NULL, 17, OBJ_TYPE_FLAG_WEAK_REF, "xcb_keymap_notify_event_t" },
#define obj_type_id_xcb_expose_event_t 18
#define obj_type_xcb_expose_event_t (obj_types[obj_type_id_xcb_expose_event_t])
  { NULL, 18, OBJ_TYPE_FLAG_WEAK_REF, "xcb_expose_event_t" },
#define obj_type_id_xcb_graphics_exposure_event_t 19
#define obj_type_xcb_graphics_exposure_event_t (obj_types[obj_type_id_xcb_graphics_exposure_event_t])
  { NULL, 19, OBJ_TYPE_FLAG_WEAK_REF, "xcb_graphics_exposure_event_t" },
#define obj_type_id_xcb_no_exposure_event_t 20
#define obj_type_xcb_no_exposure_event_t (obj_types[obj_type_id_xcb_no_exposure_event_t])
  { NULL, 20, OBJ_TYPE_FLAG_WEAK_REF, "xcb_no_exposure_event_t" },
#define obj_type_id_xcb_visibility_notify_event_t 21
#define obj_type_xcb_visibility_notify_event_t (obj_types[obj_type_id_xcb_visibility_notify_event_t])
  { NULL, 21, OBJ_TYPE_FLAG_WEAK_REF, "xcb_visibility_notify_event_t" },
#define obj_type_id_xcb_create_notify_event_t 22
#define obj_type_xcb_create_notify_event_t (obj_types[obj_type_id_xcb_create_notify_event_t])
  { NULL, 22, OBJ_TYPE_FLAG_WEAK_REF, "xcb_create_notify_event_t" },
#define obj_type_id_xcb_destroy_notify_event_t 23
#define obj_type_xcb_destroy_notify_event_t (obj_types[obj_type_id_xcb_destroy_notify_event_t])
  { NULL, 23, OBJ_TYPE_FLAG_WEAK_REF, "xcb_destroy_notify_event_t" },
#define obj_type_id_xcb_unmap_notify_event_t 24
#define obj_type_xcb_unmap_notify_event_t (obj_types[obj_type_id_xcb_unmap_notify_event_t])
  { NULL, 24, OBJ_TYPE_FLAG_WEAK_REF, "xcb_unmap_notify_event_t" },
#define obj_type_id_xcb_map_notify_event_t 25
#define obj_type_xcb_map_notify_event_t (obj_types[obj_type_id_xcb_map_notify_event_t])
  { NULL, 25, OBJ_TYPE_FLAG_WEAK_REF, "xcb_map_notify_event_t" },
#define obj_type_id_xcb_map_request_event_t 26
#define obj_type_xcb_map_request_event_t (obj_types[obj_type_id_xcb_map_request_event_t])
  { NULL, 26, OBJ_TYPE_FLAG_WEAK_REF, "xcb_map_request_event_t" },
#define obj_type_id_xcb_reparent_notify_event_t 27
#define obj_type_xcb_reparent_notify_event_t (obj_types[obj_type_id_xcb_reparent_notify_event_t])
  { NULL, 27, OBJ_TYPE_FLAG_WEAK_REF, "xcb_reparent_notify_event_t" },
#define obj_type_id_xcb_configure_notify_event_t 28
#define obj_type_xcb_configure_notify_event_t (obj_types[obj_type_id_xcb_configure_notify_event_t])
  { NULL, 28, OBJ_TYPE_FLAG_WEAK_REF, "xcb_configure_notify_event_t" },
#define obj_type_id_xcb_configure_request_event_t 29
#define obj_type_xcb_configure_request_event_t (obj_types[obj_type_id_xcb_configure_request_event_t])
  { NULL, 29, OBJ_TYPE_FLAG_WEAK_REF, "xcb_configure_request_event_t" },
#define obj_type_id_xcb_gravity_notify_event_t 30
#define obj_type_xcb_gravity_notify_event_t (obj_types[obj_type_id_xcb_gravity_notify_event_t])
  { NULL, 30, OBJ_TYPE_FLAG_WEAK_REF, "xcb_gravity_notify_event_t" },
#define obj_type_id_xcb_resize_request_event_t 31
#define obj_type_xcb_resize_request_event_t (obj_types[obj_type_id_xcb_resize_request_event_t])
  { NULL, 31, OBJ_TYPE_FLAG_WEAK_REF, "xcb_resize_request_event_t" },
#define obj_type_id_xcb_circulate_notify_event_t 32
#define obj_type_xcb_circulate_notify_event_t (obj_types[obj_type_id_xcb_circulate_notify_event_t])
  { NULL, 32, OBJ_TYPE_FLAG_WEAK_REF, "xcb_circulate_notify_event_t" },
#define obj_type_id_xcb_property_notify_event_t 33
#define obj_type_xcb_property_notify_event_t (obj_types[obj_type_id_xcb_property_notify_event_t])
  { NULL, 33, OBJ_TYPE_FLAG_WEAK_REF, "xcb_property_notify_event_t" },
#define obj_type_id_xcb_selection_clear_event_t 34
#define obj_type_xcb_selection_clear_event_t (obj_types[obj_type_id_xcb_selection_clear_event_t])
  { NULL, 34, OBJ_TYPE_FLAG_WEAK_REF, "xcb_selection_clear_event_t" },
#define obj_type_id_xcb_selection_request_event_t 35
#define obj_type_xcb_selection_request_event_t (obj_types[obj_type_id_xcb_selection_request_event_t])
  { NULL, 35, OBJ_TYPE_FLAG_WEAK_REF, "xcb_selection_request_event_t" },
#define obj_type_id_xcb_selection_notify_event_t 36
#define obj_type_xcb_selection_notify_event_t (obj_types[obj_type_id_xcb_selection_notify_event_t])
  { NULL, 36, OBJ_TYPE_FLAG_WEAK_REF, "xcb_selection_notify_event_t" },
#define obj_type_id_xcb_colormap_notify_event_t 37
#define obj_type_xcb_colormap_notify_event_t (obj_types[obj_type_id_xcb_colormap_notify_event_t])
  { NULL, 37, OBJ_TYPE_FLAG_WEAK_REF, "xcb_colormap_notify_event_t" },
#define obj_type_id_xcb_client_message_data_t 38
#define obj_type_xcb_client_message_data_t (obj_types[obj_type_id_xcb_client_message_data_t])
  { NULL, 38, OBJ_TYPE_FLAG_WEAK_REF, "xcb_client_message_data_t" },
#define obj_type_id_xcb_client_message_event_t 39
#define obj_type_xcb_client_message_event_t (obj_types[obj_type_id_xcb_client_message_event_t])
  { NULL, 39, OBJ_TYPE_FLAG_WEAK_REF, "xcb_client_message_event_t" },
#define obj_type_id_xcb_mapping_notify_event_t 40
#define obj_type_xcb_mapping_notify_event_t (obj_types[obj_type_id_xcb_mapping_notify_event_t])
  { NULL, 40, OBJ_TYPE_FLAG_WEAK_REF, "xcb_mapping_notify_event_t" },
#define obj_type_id_xcb_request_error_t 41
#define obj_type_xcb_request_error_t (obj_types[obj_type_id_xcb_request_error_t])
  { NULL, 41, OBJ_TYPE_FLAG_WEAK_REF, "xcb_request_error_t" },
#define obj_type_id_xcb_value_error_t 42
#define obj_type_xcb_value_error_t (obj_types[obj_type_id_xcb_value_error_t])
  { NULL, 42, OBJ_TYPE_FLAG_WEAK_REF, "xcb_value_error_t" },
#define obj_type_id_xcb_create_window_request_t 43
#define obj_type_xcb_create_window_request_t (obj_types[obj_type_id_xcb_create_window_request_t])
  { NULL, 43, OBJ_TYPE_FLAG_WEAK_REF, "xcb_create_window_request_t" },
#define obj_type_id_xcb_change_window_attributes_request_t 44
#define obj_type_xcb_change_window_attributes_request_t (obj_types[obj_type_id_xcb_change_window_attributes_request_t])
  { NULL, 44, OBJ_TYPE_FLAG_WEAK_REF, "xcb_change_window_attributes_request_t" },
#define obj_type_id_xcb_get_window_attributes_request_t 45
#define obj_type_xcb_get_window_attributes_request_t (obj_types[obj_type_id_xcb_get_window_attributes_request_t])
  { NULL, 45, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_window_attributes_request_t" },
#define obj_type_id_xcb_get_window_attributes_reply_t 46
#define obj_type_xcb_get_window_attributes_reply_t (obj_types[obj_type_id_xcb_get_window_attributes_reply_t])
  { NULL, 46, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_window_attributes_reply_t" },
#define obj_type_id_xcb_destroy_window_request_t 47
#define obj_type_xcb_destroy_window_request_t (obj_types[obj_type_id_xcb_destroy_window_request_t])
  { NULL, 47, OBJ_TYPE_FLAG_WEAK_REF, "xcb_destroy_window_request_t" },
#define obj_type_id_xcb_destroy_subwindows_request_t 48
#define obj_type_xcb_destroy_subwindows_request_t (obj_types[obj_type_id_xcb_destroy_subwindows_request_t])
  { NULL, 48, OBJ_TYPE_FLAG_WEAK_REF, "xcb_destroy_subwindows_request_t" },
#define obj_type_id_xcb_change_save_set_request_t 49
#define obj_type_xcb_change_save_set_request_t (obj_types[obj_type_id_xcb_change_save_set_request_t])
  { NULL, 49, OBJ_TYPE_FLAG_WEAK_REF, "xcb_change_save_set_request_t" },
#define obj_type_id_xcb_reparent_window_request_t 50
#define obj_type_xcb_reparent_window_request_t (obj_types[obj_type_id_xcb_reparent_window_request_t])
  { NULL, 50, OBJ_TYPE_FLAG_WEAK_REF, "xcb_reparent_window_request_t" },
#define obj_type_id_xcb_map_window_request_t 51
#define obj_type_xcb_map_window_request_t (obj_types[obj_type_id_xcb_map_window_request_t])
  { NULL, 51, OBJ_TYPE_FLAG_WEAK_REF, "xcb_map_window_request_t" },
#define obj_type_id_xcb_map_subwindows_request_t 52
#define obj_type_xcb_map_subwindows_request_t (obj_types[obj_type_id_xcb_map_subwindows_request_t])
  { NULL, 52, OBJ_TYPE_FLAG_WEAK_REF, "xcb_map_subwindows_request_t" },
#define obj_type_id_xcb_unmap_window_request_t 53
#define obj_type_xcb_unmap_window_request_t (obj_types[obj_type_id_xcb_unmap_window_request_t])
  { NULL, 53, OBJ_TYPE_FLAG_WEAK_REF, "xcb_unmap_window_request_t" },
#define obj_type_id_xcb_unmap_subwindows_request_t 54
#define obj_type_xcb_unmap_subwindows_request_t (obj_types[obj_type_id_xcb_unmap_subwindows_request_t])
  { NULL, 54, OBJ_TYPE_FLAG_WEAK_REF, "xcb_unmap_subwindows_request_t" },
#define obj_type_id_xcb_configure_window_request_t 55
#define obj_type_xcb_configure_window_request_t (obj_types[obj_type_id_xcb_configure_window_request_t])
  { NULL, 55, OBJ_TYPE_FLAG_WEAK_REF, "xcb_configure_window_request_t" },
#define obj_type_id_xcb_circulate_window_request_t 56
#define obj_type_xcb_circulate_window_request_t (obj_types[obj_type_id_xcb_circulate_window_request_t])
  { NULL, 56, OBJ_TYPE_FLAG_WEAK_REF, "xcb_circulate_window_request_t" },
#define obj_type_id_xcb_get_geometry_request_t 57
#define obj_type_xcb_get_geometry_request_t (obj_types[obj_type_id_xcb_get_geometry_request_t])
  { NULL, 57, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_geometry_request_t" },
#define obj_type_id_xcb_get_geometry_reply_t 58
#define obj_type_xcb_get_geometry_reply_t (obj_types[obj_type_id_xcb_get_geometry_reply_t])
  { NULL, 58, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_geometry_reply_t" },
#define obj_type_id_xcb_query_tree_request_t 59
#define obj_type_xcb_query_tree_request_t (obj_types[obj_type_id_xcb_query_tree_request_t])
  { NULL, 59, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_tree_request_t" },
#define obj_type_id_xcb_query_tree_reply_t 60
#define obj_type_xcb_query_tree_reply_t (obj_types[obj_type_id_xcb_query_tree_reply_t])
  { NULL, 60, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_tree_reply_t" },
#define obj_type_id_xcb_intern_atom_request_t 61
#define obj_type_xcb_intern_atom_request_t (obj_types[obj_type_id_xcb_intern_atom_request_t])
  { NULL, 61, OBJ_TYPE_FLAG_WEAK_REF, "xcb_intern_atom_request_t" },
#define obj_type_id_xcb_intern_atom_reply_t 62
#define obj_type_xcb_intern_atom_reply_t (obj_types[obj_type_id_xcb_intern_atom_reply_t])
  { NULL, 62, OBJ_TYPE_FLAG_WEAK_REF, "xcb_intern_atom_reply_t" },
#define obj_type_id_xcb_get_atom_name_request_t 63
#define obj_type_xcb_get_atom_name_request_t (obj_types[obj_type_id_xcb_get_atom_name_request_t])
  { NULL, 63, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_atom_name_request_t" },
#define obj_type_id_xcb_get_atom_name_reply_t 64
#define obj_type_xcb_get_atom_name_reply_t (obj_types[obj_type_id_xcb_get_atom_name_reply_t])
  { NULL, 64, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_atom_name_reply_t" },
#define obj_type_id_xcb_change_property_request_t 65
#define obj_type_xcb_change_property_request_t (obj_types[obj_type_id_xcb_change_property_request_t])
  { NULL, 65, OBJ_TYPE_FLAG_WEAK_REF, "xcb_change_property_request_t" },
#define obj_type_id_xcb_delete_property_request_t 66
#define obj_type_xcb_delete_property_request_t (obj_types[obj_type_id_xcb_delete_property_request_t])
  { NULL, 66, OBJ_TYPE_FLAG_WEAK_REF, "xcb_delete_property_request_t" },
#define obj_type_id_xcb_get_property_request_t 67
#define obj_type_xcb_get_property_request_t (obj_types[obj_type_id_xcb_get_property_request_t])
  { NULL, 67, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_property_request_t" },
#define obj_type_id_xcb_get_property_reply_t 68
#define obj_type_xcb_get_property_reply_t (obj_types[obj_type_id_xcb_get_property_reply_t])
  { NULL, 68, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_property_reply_t" },
#define obj_type_id_xcb_list_properties_request_t 69
#define obj_type_xcb_list_properties_request_t (obj_types[obj_type_id_xcb_list_properties_request_t])
  { NULL, 69, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_properties_request_t" },
#define obj_type_id_xcb_list_properties_reply_t 70
#define obj_type_xcb_list_properties_reply_t (obj_types[obj_type_id_xcb_list_properties_reply_t])
  { NULL, 70, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_properties_reply_t" },
#define obj_type_id_xcb_set_selection_owner_request_t 71
#define obj_type_xcb_set_selection_owner_request_t (obj_types[obj_type_id_xcb_set_selection_owner_request_t])
  { NULL, 71, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_selection_owner_request_t" },
#define obj_type_id_xcb_get_selection_owner_request_t 72
#define obj_type_xcb_get_selection_owner_request_t (obj_types[obj_type_id_xcb_get_selection_owner_request_t])
  { NULL, 72, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_selection_owner_request_t" },
#define obj_type_id_xcb_get_selection_owner_reply_t 73
#define obj_type_xcb_get_selection_owner_reply_t (obj_types[obj_type_id_xcb_get_selection_owner_reply_t])
  { NULL, 73, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_selection_owner_reply_t" },
#define obj_type_id_xcb_convert_selection_request_t 74
#define obj_type_xcb_convert_selection_request_t (obj_types[obj_type_id_xcb_convert_selection_request_t])
  { NULL, 74, OBJ_TYPE_FLAG_WEAK_REF, "xcb_convert_selection_request_t" },
#define obj_type_id_xcb_send_event_request_t 75
#define obj_type_xcb_send_event_request_t (obj_types[obj_type_id_xcb_send_event_request_t])
  { NULL, 75, OBJ_TYPE_FLAG_WEAK_REF, "xcb_send_event_request_t" },
#define obj_type_id_xcb_grab_pointer_request_t 76
#define obj_type_xcb_grab_pointer_request_t (obj_types[obj_type_id_xcb_grab_pointer_request_t])
  { NULL, 76, OBJ_TYPE_FLAG_WEAK_REF, "xcb_grab_pointer_request_t" },
#define obj_type_id_xcb_grab_pointer_reply_t 77
#define obj_type_xcb_grab_pointer_reply_t (obj_types[obj_type_id_xcb_grab_pointer_reply_t])
  { NULL, 77, OBJ_TYPE_FLAG_WEAK_REF, "xcb_grab_pointer_reply_t" },
#define obj_type_id_xcb_ungrab_pointer_request_t 78
#define obj_type_xcb_ungrab_pointer_request_t (obj_types[obj_type_id_xcb_ungrab_pointer_request_t])
  { NULL, 78, OBJ_TYPE_FLAG_WEAK_REF, "xcb_ungrab_pointer_request_t" },
#define obj_type_id_xcb_grab_button_request_t 79
#define obj_type_xcb_grab_button_request_t (obj_types[obj_type_id_xcb_grab_button_request_t])
  { NULL, 79, OBJ_TYPE_FLAG_WEAK_REF, "xcb_grab_button_request_t" },
#define obj_type_id_xcb_ungrab_button_request_t 80
#define obj_type_xcb_ungrab_button_request_t (obj_types[obj_type_id_xcb_ungrab_button_request_t])
  { NULL, 80, OBJ_TYPE_FLAG_WEAK_REF, "xcb_ungrab_button_request_t" },
#define obj_type_id_xcb_change_active_pointer_grab_request_t 81
#define obj_type_xcb_change_active_pointer_grab_request_t (obj_types[obj_type_id_xcb_change_active_pointer_grab_request_t])
  { NULL, 81, OBJ_TYPE_FLAG_WEAK_REF, "xcb_change_active_pointer_grab_request_t" },
#define obj_type_id_xcb_grab_keyboard_request_t 82
#define obj_type_xcb_grab_keyboard_request_t (obj_types[obj_type_id_xcb_grab_keyboard_request_t])
  { NULL, 82, OBJ_TYPE_FLAG_WEAK_REF, "xcb_grab_keyboard_request_t" },
#define obj_type_id_xcb_grab_keyboard_reply_t 83
#define obj_type_xcb_grab_keyboard_reply_t (obj_types[obj_type_id_xcb_grab_keyboard_reply_t])
  { NULL, 83, OBJ_TYPE_FLAG_WEAK_REF, "xcb_grab_keyboard_reply_t" },
#define obj_type_id_xcb_ungrab_keyboard_request_t 84
#define obj_type_xcb_ungrab_keyboard_request_t (obj_types[obj_type_id_xcb_ungrab_keyboard_request_t])
  { NULL, 84, OBJ_TYPE_FLAG_WEAK_REF, "xcb_ungrab_keyboard_request_t" },
#define obj_type_id_xcb_grab_key_request_t 85
#define obj_type_xcb_grab_key_request_t (obj_types[obj_type_id_xcb_grab_key_request_t])
  { NULL, 85, OBJ_TYPE_FLAG_WEAK_REF, "xcb_grab_key_request_t" },
#define obj_type_id_xcb_ungrab_key_request_t 86
#define obj_type_xcb_ungrab_key_request_t (obj_types[obj_type_id_xcb_ungrab_key_request_t])
  { NULL, 86, OBJ_TYPE_FLAG_WEAK_REF, "xcb_ungrab_key_request_t" },
#define obj_type_id_xcb_allow_events_request_t 87
#define obj_type_xcb_allow_events_request_t (obj_types[obj_type_id_xcb_allow_events_request_t])
  { NULL, 87, OBJ_TYPE_FLAG_WEAK_REF, "xcb_allow_events_request_t" },
#define obj_type_id_xcb_grab_server_request_t 88
#define obj_type_xcb_grab_server_request_t (obj_types[obj_type_id_xcb_grab_server_request_t])
  { NULL, 88, OBJ_TYPE_FLAG_WEAK_REF, "xcb_grab_server_request_t" },
#define obj_type_id_xcb_ungrab_server_request_t 89
#define obj_type_xcb_ungrab_server_request_t (obj_types[obj_type_id_xcb_ungrab_server_request_t])
  { NULL, 89, OBJ_TYPE_FLAG_WEAK_REF, "xcb_ungrab_server_request_t" },
#define obj_type_id_xcb_query_pointer_request_t 90
#define obj_type_xcb_query_pointer_request_t (obj_types[obj_type_id_xcb_query_pointer_request_t])
  { NULL, 90, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_pointer_request_t" },
#define obj_type_id_xcb_query_pointer_reply_t 91
#define obj_type_xcb_query_pointer_reply_t (obj_types[obj_type_id_xcb_query_pointer_reply_t])
  { NULL, 91, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_pointer_reply_t" },
#define obj_type_id_xcb_timecoord_t 92
#define obj_type_xcb_timecoord_t (obj_types[obj_type_id_xcb_timecoord_t])
  { NULL, 92, OBJ_TYPE_FLAG_WEAK_REF, "xcb_timecoord_t" },
#define obj_type_id_xcb_get_motion_events_request_t 93
#define obj_type_xcb_get_motion_events_request_t (obj_types[obj_type_id_xcb_get_motion_events_request_t])
  { NULL, 93, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_motion_events_request_t" },
#define obj_type_id_xcb_get_motion_events_reply_t 94
#define obj_type_xcb_get_motion_events_reply_t (obj_types[obj_type_id_xcb_get_motion_events_reply_t])
  { NULL, 94, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_motion_events_reply_t" },
#define obj_type_id_xcb_translate_coordinates_request_t 95
#define obj_type_xcb_translate_coordinates_request_t (obj_types[obj_type_id_xcb_translate_coordinates_request_t])
  { NULL, 95, OBJ_TYPE_FLAG_WEAK_REF, "xcb_translate_coordinates_request_t" },
#define obj_type_id_xcb_translate_coordinates_reply_t 96
#define obj_type_xcb_translate_coordinates_reply_t (obj_types[obj_type_id_xcb_translate_coordinates_reply_t])
  { NULL, 96, OBJ_TYPE_FLAG_WEAK_REF, "xcb_translate_coordinates_reply_t" },
#define obj_type_id_xcb_warp_pointer_request_t 97
#define obj_type_xcb_warp_pointer_request_t (obj_types[obj_type_id_xcb_warp_pointer_request_t])
  { NULL, 97, OBJ_TYPE_FLAG_WEAK_REF, "xcb_warp_pointer_request_t" },
#define obj_type_id_xcb_set_input_focus_request_t 98
#define obj_type_xcb_set_input_focus_request_t (obj_types[obj_type_id_xcb_set_input_focus_request_t])
  { NULL, 98, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_input_focus_request_t" },
#define obj_type_id_xcb_get_input_focus_request_t 99
#define obj_type_xcb_get_input_focus_request_t (obj_types[obj_type_id_xcb_get_input_focus_request_t])
  { NULL, 99, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_input_focus_request_t" },
#define obj_type_id_xcb_get_input_focus_reply_t 100
#define obj_type_xcb_get_input_focus_reply_t (obj_types[obj_type_id_xcb_get_input_focus_reply_t])
  { NULL, 100, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_input_focus_reply_t" },
#define obj_type_id_xcb_query_keymap_request_t 101
#define obj_type_xcb_query_keymap_request_t (obj_types[obj_type_id_xcb_query_keymap_request_t])
  { NULL, 101, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_keymap_request_t" },
#define obj_type_id_xcb_query_keymap_reply_t 102
#define obj_type_xcb_query_keymap_reply_t (obj_types[obj_type_id_xcb_query_keymap_reply_t])
  { NULL, 102, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_keymap_reply_t" },
#define obj_type_id_xcb_open_font_request_t 103
#define obj_type_xcb_open_font_request_t (obj_types[obj_type_id_xcb_open_font_request_t])
  { NULL, 103, OBJ_TYPE_FLAG_WEAK_REF, "xcb_open_font_request_t" },
#define obj_type_id_xcb_close_font_request_t 104
#define obj_type_xcb_close_font_request_t (obj_types[obj_type_id_xcb_close_font_request_t])
  { NULL, 104, OBJ_TYPE_FLAG_WEAK_REF, "xcb_close_font_request_t" },
#define obj_type_id_xcb_fontprop_t 105
#define obj_type_xcb_fontprop_t (obj_types[obj_type_id_xcb_fontprop_t])
  { NULL, 105, OBJ_TYPE_FLAG_WEAK_REF, "xcb_fontprop_t" },
#define obj_type_id_xcb_charinfo_t 106
#define obj_type_xcb_charinfo_t (obj_types[obj_type_id_xcb_charinfo_t])
  { NULL, 106, OBJ_TYPE_FLAG_WEAK_REF, "xcb_charinfo_t" },
#define obj_type_id_xcb_query_font_request_t 107
#define obj_type_xcb_query_font_request_t (obj_types[obj_type_id_xcb_query_font_request_t])
  { NULL, 107, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_font_request_t" },
#define obj_type_id_xcb_query_font_reply_t 108
#define obj_type_xcb_query_font_reply_t (obj_types[obj_type_id_xcb_query_font_reply_t])
  { NULL, 108, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_font_reply_t" },
#define obj_type_id_xcb_query_text_extents_request_t 109
#define obj_type_xcb_query_text_extents_request_t (obj_types[obj_type_id_xcb_query_text_extents_request_t])
  { NULL, 109, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_text_extents_request_t" },
#define obj_type_id_xcb_query_text_extents_reply_t 110
#define obj_type_xcb_query_text_extents_reply_t (obj_types[obj_type_id_xcb_query_text_extents_reply_t])
  { NULL, 110, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_text_extents_reply_t" },
#define obj_type_id_xcb_str_t 111
#define obj_type_xcb_str_t (obj_types[obj_type_id_xcb_str_t])
  { NULL, 111, OBJ_TYPE_FLAG_WEAK_REF, "xcb_str_t" },
#define obj_type_id_xcb_list_fonts_request_t 112
#define obj_type_xcb_list_fonts_request_t (obj_types[obj_type_id_xcb_list_fonts_request_t])
  { NULL, 112, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_fonts_request_t" },
#define obj_type_id_xcb_list_fonts_reply_t 113
#define obj_type_xcb_list_fonts_reply_t (obj_types[obj_type_id_xcb_list_fonts_reply_t])
  { NULL, 113, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_fonts_reply_t" },
#define obj_type_id_xcb_list_fonts_with_info_request_t 114
#define obj_type_xcb_list_fonts_with_info_request_t (obj_types[obj_type_id_xcb_list_fonts_with_info_request_t])
  { NULL, 114, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_fonts_with_info_request_t" },
#define obj_type_id_xcb_list_fonts_with_info_reply_t 115
#define obj_type_xcb_list_fonts_with_info_reply_t (obj_types[obj_type_id_xcb_list_fonts_with_info_reply_t])
  { NULL, 115, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_fonts_with_info_reply_t" },
#define obj_type_id_xcb_set_font_path_request_t 116
#define obj_type_xcb_set_font_path_request_t (obj_types[obj_type_id_xcb_set_font_path_request_t])
  { NULL, 116, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_font_path_request_t" },
#define obj_type_id_xcb_get_font_path_request_t 117
#define obj_type_xcb_get_font_path_request_t (obj_types[obj_type_id_xcb_get_font_path_request_t])
  { NULL, 117, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_font_path_request_t" },
#define obj_type_id_xcb_get_font_path_reply_t 118
#define obj_type_xcb_get_font_path_reply_t (obj_types[obj_type_id_xcb_get_font_path_reply_t])
  { NULL, 118, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_font_path_reply_t" },
#define obj_type_id_xcb_create_pixmap_request_t 119
#define obj_type_xcb_create_pixmap_request_t (obj_types[obj_type_id_xcb_create_pixmap_request_t])
  { NULL, 119, OBJ_TYPE_FLAG_WEAK_REF, "xcb_create_pixmap_request_t" },
#define obj_type_id_xcb_free_pixmap_request_t 120
#define obj_type_xcb_free_pixmap_request_t (obj_types[obj_type_id_xcb_free_pixmap_request_t])
  { NULL, 120, OBJ_TYPE_FLAG_WEAK_REF, "xcb_free_pixmap_request_t" },
#define obj_type_id_xcb_create_gc_request_t 121
#define obj_type_xcb_create_gc_request_t (obj_types[obj_type_id_xcb_create_gc_request_t])
  { NULL, 121, OBJ_TYPE_FLAG_WEAK_REF, "xcb_create_gc_request_t" },
#define obj_type_id_xcb_change_gc_request_t 122
#define obj_type_xcb_change_gc_request_t (obj_types[obj_type_id_xcb_change_gc_request_t])
  { NULL, 122, OBJ_TYPE_FLAG_WEAK_REF, "xcb_change_gc_request_t" },
#define obj_type_id_xcb_copy_gc_request_t 123
#define obj_type_xcb_copy_gc_request_t (obj_types[obj_type_id_xcb_copy_gc_request_t])
  { NULL, 123, OBJ_TYPE_FLAG_WEAK_REF, "xcb_copy_gc_request_t" },
#define obj_type_id_xcb_set_dashes_request_t 124
#define obj_type_xcb_set_dashes_request_t (obj_types[obj_type_id_xcb_set_dashes_request_t])
  { NULL, 124, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_dashes_request_t" },
#define obj_type_id_xcb_set_clip_rectangles_request_t 125
#define obj_type_xcb_set_clip_rectangles_request_t (obj_types[obj_type_id_xcb_set_clip_rectangles_request_t])
  { NULL, 125, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_clip_rectangles_request_t" },
#define obj_type_id_xcb_free_gc_request_t 126
#define obj_type_xcb_free_gc_request_t (obj_types[obj_type_id_xcb_free_gc_request_t])
  { NULL, 126, OBJ_TYPE_FLAG_WEAK_REF, "xcb_free_gc_request_t" },
#define obj_type_id_xcb_clear_area_request_t 127
#define obj_type_xcb_clear_area_request_t (obj_types[obj_type_id_xcb_clear_area_request_t])
  { NULL, 127, OBJ_TYPE_FLAG_WEAK_REF, "xcb_clear_area_request_t" },
#define obj_type_id_xcb_copy_area_request_t 128
#define obj_type_xcb_copy_area_request_t (obj_types[obj_type_id_xcb_copy_area_request_t])
  { NULL, 128, OBJ_TYPE_FLAG_WEAK_REF, "xcb_copy_area_request_t" },
#define obj_type_id_xcb_copy_plane_request_t 129
#define obj_type_xcb_copy_plane_request_t (obj_types[obj_type_id_xcb_copy_plane_request_t])
  { NULL, 129, OBJ_TYPE_FLAG_WEAK_REF, "xcb_copy_plane_request_t" },
#define obj_type_id_xcb_poly_point_request_t 130
#define obj_type_xcb_poly_point_request_t (obj_types[obj_type_id_xcb_poly_point_request_t])
  { NULL, 130, OBJ_TYPE_FLAG_WEAK_REF, "xcb_poly_point_request_t" },
#define obj_type_id_xcb_poly_line_request_t 131
#define obj_type_xcb_poly_line_request_t (obj_types[obj_type_id_xcb_poly_line_request_t])
  { NULL, 131, OBJ_TYPE_FLAG_WEAK_REF, "xcb_poly_line_request_t" },
#define obj_type_id_xcb_segment_t 132
#define obj_type_xcb_segment_t (obj_types[obj_type_id_xcb_segment_t])
  { NULL, 132, OBJ_TYPE_FLAG_WEAK_REF, "xcb_segment_t" },
#define obj_type_id_xcb_poly_segment_request_t 133
#define obj_type_xcb_poly_segment_request_t (obj_types[obj_type_id_xcb_poly_segment_request_t])
  { NULL, 133, OBJ_TYPE_FLAG_WEAK_REF, "xcb_poly_segment_request_t" },
#define obj_type_id_xcb_poly_rectangle_request_t 134
#define obj_type_xcb_poly_rectangle_request_t (obj_types[obj_type_id_xcb_poly_rectangle_request_t])
  { NULL, 134, OBJ_TYPE_FLAG_WEAK_REF, "xcb_poly_rectangle_request_t" },
#define obj_type_id_xcb_poly_arc_request_t 135
#define obj_type_xcb_poly_arc_request_t (obj_types[obj_type_id_xcb_poly_arc_request_t])
  { NULL, 135, OBJ_TYPE_FLAG_WEAK_REF, "xcb_poly_arc_request_t" },
#define obj_type_id_xcb_fill_poly_request_t 136
#define obj_type_xcb_fill_poly_request_t (obj_types[obj_type_id_xcb_fill_poly_request_t])
  { NULL, 136, OBJ_TYPE_FLAG_WEAK_REF, "xcb_fill_poly_request_t" },
#define obj_type_id_xcb_poly_fill_rectangle_request_t 137
#define obj_type_xcb_poly_fill_rectangle_request_t (obj_types[obj_type_id_xcb_poly_fill_rectangle_request_t])
  { NULL, 137, OBJ_TYPE_FLAG_WEAK_REF, "xcb_poly_fill_rectangle_request_t" },
#define obj_type_id_xcb_poly_fill_arc_request_t 138
#define obj_type_xcb_poly_fill_arc_request_t (obj_types[obj_type_id_xcb_poly_fill_arc_request_t])
  { NULL, 138, OBJ_TYPE_FLAG_WEAK_REF, "xcb_poly_fill_arc_request_t" },
#define obj_type_id_xcb_put_image_request_t 139
#define obj_type_xcb_put_image_request_t (obj_types[obj_type_id_xcb_put_image_request_t])
  { NULL, 139, OBJ_TYPE_FLAG_WEAK_REF, "xcb_put_image_request_t" },
#define obj_type_id_xcb_get_image_request_t 140
#define obj_type_xcb_get_image_request_t (obj_types[obj_type_id_xcb_get_image_request_t])
  { NULL, 140, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_image_request_t" },
#define obj_type_id_xcb_get_image_reply_t 141
#define obj_type_xcb_get_image_reply_t (obj_types[obj_type_id_xcb_get_image_reply_t])
  { NULL, 141, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_image_reply_t" },
#define obj_type_id_xcb_poly_text_8_request_t 142
#define obj_type_xcb_poly_text_8_request_t (obj_types[obj_type_id_xcb_poly_text_8_request_t])
  { NULL, 142, OBJ_TYPE_FLAG_WEAK_REF, "xcb_poly_text_8_request_t" },
#define obj_type_id_xcb_poly_text_16_request_t 143
#define obj_type_xcb_poly_text_16_request_t (obj_types[obj_type_id_xcb_poly_text_16_request_t])
  { NULL, 143, OBJ_TYPE_FLAG_WEAK_REF, "xcb_poly_text_16_request_t" },
#define obj_type_id_xcb_image_text_8_request_t 144
#define obj_type_xcb_image_text_8_request_t (obj_types[obj_type_id_xcb_image_text_8_request_t])
  { NULL, 144, OBJ_TYPE_FLAG_WEAK_REF, "xcb_image_text_8_request_t" },
#define obj_type_id_xcb_image_text_16_request_t 145
#define obj_type_xcb_image_text_16_request_t (obj_types[obj_type_id_xcb_image_text_16_request_t])
  { NULL, 145, OBJ_TYPE_FLAG_WEAK_REF, "xcb_image_text_16_request_t" },
#define obj_type_id_xcb_create_colormap_request_t 146
#define obj_type_xcb_create_colormap_request_t (obj_types[obj_type_id_xcb_create_colormap_request_t])
  { NULL, 146, OBJ_TYPE_FLAG_WEAK_REF, "xcb_create_colormap_request_t" },
#define obj_type_id_xcb_free_colormap_request_t 147
#define obj_type_xcb_free_colormap_request_t (obj_types[obj_type_id_xcb_free_colormap_request_t])
  { NULL, 147, OBJ_TYPE_FLAG_WEAK_REF, "xcb_free_colormap_request_t" },
#define obj_type_id_xcb_copy_colormap_and_free_request_t 148
#define obj_type_xcb_copy_colormap_and_free_request_t (obj_types[obj_type_id_xcb_copy_colormap_and_free_request_t])
  { NULL, 148, OBJ_TYPE_FLAG_WEAK_REF, "xcb_copy_colormap_and_free_request_t" },
#define obj_type_id_xcb_install_colormap_request_t 149
#define obj_type_xcb_install_colormap_request_t (obj_types[obj_type_id_xcb_install_colormap_request_t])
  { NULL, 149, OBJ_TYPE_FLAG_WEAK_REF, "xcb_install_colormap_request_t" },
#define obj_type_id_xcb_uninstall_colormap_request_t 150
#define obj_type_xcb_uninstall_colormap_request_t (obj_types[obj_type_id_xcb_uninstall_colormap_request_t])
  { NULL, 150, OBJ_TYPE_FLAG_WEAK_REF, "xcb_uninstall_colormap_request_t" },
#define obj_type_id_xcb_list_installed_colormaps_request_t 151
#define obj_type_xcb_list_installed_colormaps_request_t (obj_types[obj_type_id_xcb_list_installed_colormaps_request_t])
  { NULL, 151, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_installed_colormaps_request_t" },
#define obj_type_id_xcb_list_installed_colormaps_reply_t 152
#define obj_type_xcb_list_installed_colormaps_reply_t (obj_types[obj_type_id_xcb_list_installed_colormaps_reply_t])
  { NULL, 152, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_installed_colormaps_reply_t" },
#define obj_type_id_xcb_alloc_color_request_t 153
#define obj_type_xcb_alloc_color_request_t (obj_types[obj_type_id_xcb_alloc_color_request_t])
  { NULL, 153, OBJ_TYPE_FLAG_WEAK_REF, "xcb_alloc_color_request_t" },
#define obj_type_id_xcb_alloc_color_reply_t 154
#define obj_type_xcb_alloc_color_reply_t (obj_types[obj_type_id_xcb_alloc_color_reply_t])
  { NULL, 154, OBJ_TYPE_FLAG_WEAK_REF, "xcb_alloc_color_reply_t" },
#define obj_type_id_xcb_alloc_named_color_request_t 155
#define obj_type_xcb_alloc_named_color_request_t (obj_types[obj_type_id_xcb_alloc_named_color_request_t])
  { NULL, 155, OBJ_TYPE_FLAG_WEAK_REF, "xcb_alloc_named_color_request_t" },
#define obj_type_id_xcb_alloc_named_color_reply_t 156
#define obj_type_xcb_alloc_named_color_reply_t (obj_types[obj_type_id_xcb_alloc_named_color_reply_t])
  { NULL, 156, OBJ_TYPE_FLAG_WEAK_REF, "xcb_alloc_named_color_reply_t" },
#define obj_type_id_xcb_alloc_color_cells_request_t 157
#define obj_type_xcb_alloc_color_cells_request_t (obj_types[obj_type_id_xcb_alloc_color_cells_request_t])
  { NULL, 157, OBJ_TYPE_FLAG_WEAK_REF, "xcb_alloc_color_cells_request_t" },
#define obj_type_id_xcb_alloc_color_cells_reply_t 158
#define obj_type_xcb_alloc_color_cells_reply_t (obj_types[obj_type_id_xcb_alloc_color_cells_reply_t])
  { NULL, 158, OBJ_TYPE_FLAG_WEAK_REF, "xcb_alloc_color_cells_reply_t" },
#define obj_type_id_xcb_alloc_color_planes_request_t 159
#define obj_type_xcb_alloc_color_planes_request_t (obj_types[obj_type_id_xcb_alloc_color_planes_request_t])
  { NULL, 159, OBJ_TYPE_FLAG_WEAK_REF, "xcb_alloc_color_planes_request_t" },
#define obj_type_id_xcb_alloc_color_planes_reply_t 160
#define obj_type_xcb_alloc_color_planes_reply_t (obj_types[obj_type_id_xcb_alloc_color_planes_reply_t])
  { NULL, 160, OBJ_TYPE_FLAG_WEAK_REF, "xcb_alloc_color_planes_reply_t" },
#define obj_type_id_xcb_free_colors_request_t 161
#define obj_type_xcb_free_colors_request_t (obj_types[obj_type_id_xcb_free_colors_request_t])
  { NULL, 161, OBJ_TYPE_FLAG_WEAK_REF, "xcb_free_colors_request_t" },
#define obj_type_id_xcb_coloritem_t 162
#define obj_type_xcb_coloritem_t (obj_types[obj_type_id_xcb_coloritem_t])
  { NULL, 162, OBJ_TYPE_FLAG_WEAK_REF, "xcb_coloritem_t" },
#define obj_type_id_xcb_store_colors_request_t 163
#define obj_type_xcb_store_colors_request_t (obj_types[obj_type_id_xcb_store_colors_request_t])
  { NULL, 163, OBJ_TYPE_FLAG_WEAK_REF, "xcb_store_colors_request_t" },
#define obj_type_id_xcb_store_named_color_request_t 164
#define obj_type_xcb_store_named_color_request_t (obj_types[obj_type_id_xcb_store_named_color_request_t])
  { NULL, 164, OBJ_TYPE_FLAG_WEAK_REF, "xcb_store_named_color_request_t" },
#define obj_type_id_xcb_rgb_t 165
#define obj_type_xcb_rgb_t (obj_types[obj_type_id_xcb_rgb_t])
  { NULL, 165, OBJ_TYPE_FLAG_WEAK_REF, "xcb_rgb_t" },
#define obj_type_id_xcb_query_colors_request_t 166
#define obj_type_xcb_query_colors_request_t (obj_types[obj_type_id_xcb_query_colors_request_t])
  { NULL, 166, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_colors_request_t" },
#define obj_type_id_xcb_query_colors_reply_t 167
#define obj_type_xcb_query_colors_reply_t (obj_types[obj_type_id_xcb_query_colors_reply_t])
  { NULL, 167, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_colors_reply_t" },
#define obj_type_id_xcb_lookup_color_request_t 168
#define obj_type_xcb_lookup_color_request_t (obj_types[obj_type_id_xcb_lookup_color_request_t])
  { NULL, 168, OBJ_TYPE_FLAG_WEAK_REF, "xcb_lookup_color_request_t" },
#define obj_type_id_xcb_lookup_color_reply_t 169
#define obj_type_xcb_lookup_color_reply_t (obj_types[obj_type_id_xcb_lookup_color_reply_t])
  { NULL, 169, OBJ_TYPE_FLAG_WEAK_REF, "xcb_lookup_color_reply_t" },
#define obj_type_id_xcb_create_cursor_request_t 170
#define obj_type_xcb_create_cursor_request_t (obj_types[obj_type_id_xcb_create_cursor_request_t])
  { NULL, 170, OBJ_TYPE_FLAG_WEAK_REF, "xcb_create_cursor_request_t" },
#define obj_type_id_xcb_create_glyph_cursor_request_t 171
#define obj_type_xcb_create_glyph_cursor_request_t (obj_types[obj_type_id_xcb_create_glyph_cursor_request_t])
  { NULL, 171, OBJ_TYPE_FLAG_WEAK_REF, "xcb_create_glyph_cursor_request_t" },
#define obj_type_id_xcb_free_cursor_request_t 172
#define obj_type_xcb_free_cursor_request_t (obj_types[obj_type_id_xcb_free_cursor_request_t])
  { NULL, 172, OBJ_TYPE_FLAG_WEAK_REF, "xcb_free_cursor_request_t" },
#define obj_type_id_xcb_recolor_cursor_request_t 173
#define obj_type_xcb_recolor_cursor_request_t (obj_types[obj_type_id_xcb_recolor_cursor_request_t])
  { NULL, 173, OBJ_TYPE_FLAG_WEAK_REF, "xcb_recolor_cursor_request_t" },
#define obj_type_id_xcb_query_best_size_request_t 174
#define obj_type_xcb_query_best_size_request_t (obj_types[obj_type_id_xcb_query_best_size_request_t])
  { NULL, 174, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_best_size_request_t" },
#define obj_type_id_xcb_query_best_size_reply_t 175
#define obj_type_xcb_query_best_size_reply_t (obj_types[obj_type_id_xcb_query_best_size_reply_t])
  { NULL, 175, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_best_size_reply_t" },
#define obj_type_id_xcb_query_extension_request_t 176
#define obj_type_xcb_query_extension_request_t (obj_types[obj_type_id_xcb_query_extension_request_t])
  { NULL, 176, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_extension_request_t" },
#define obj_type_id_xcb_query_extension_reply_t 177
#define obj_type_xcb_query_extension_reply_t (obj_types[obj_type_id_xcb_query_extension_reply_t])
  { NULL, 177, OBJ_TYPE_FLAG_WEAK_REF, "xcb_query_extension_reply_t" },
#define obj_type_id_xcb_list_extensions_request_t 178
#define obj_type_xcb_list_extensions_request_t (obj_types[obj_type_id_xcb_list_extensions_request_t])
  { NULL, 178, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_extensions_request_t" },
#define obj_type_id_xcb_list_extensions_reply_t 179
#define obj_type_xcb_list_extensions_reply_t (obj_types[obj_type_id_xcb_list_extensions_reply_t])
  { NULL, 179, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_extensions_reply_t" },
#define obj_type_id_xcb_change_keyboard_mapping_request_t 180
#define obj_type_xcb_change_keyboard_mapping_request_t (obj_types[obj_type_id_xcb_change_keyboard_mapping_request_t])
  { NULL, 180, OBJ_TYPE_FLAG_WEAK_REF, "xcb_change_keyboard_mapping_request_t" },
#define obj_type_id_xcb_get_keyboard_mapping_request_t 181
#define obj_type_xcb_get_keyboard_mapping_request_t (obj_types[obj_type_id_xcb_get_keyboard_mapping_request_t])
  { NULL, 181, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_keyboard_mapping_request_t" },
#define obj_type_id_xcb_get_keyboard_mapping_reply_t 182
#define obj_type_xcb_get_keyboard_mapping_reply_t (obj_types[obj_type_id_xcb_get_keyboard_mapping_reply_t])
  { NULL, 182, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_keyboard_mapping_reply_t" },
#define obj_type_id_xcb_change_keyboard_control_request_t 183
#define obj_type_xcb_change_keyboard_control_request_t (obj_types[obj_type_id_xcb_change_keyboard_control_request_t])
  { NULL, 183, OBJ_TYPE_FLAG_WEAK_REF, "xcb_change_keyboard_control_request_t" },
#define obj_type_id_xcb_get_keyboard_control_request_t 184
#define obj_type_xcb_get_keyboard_control_request_t (obj_types[obj_type_id_xcb_get_keyboard_control_request_t])
  { NULL, 184, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_keyboard_control_request_t" },
#define obj_type_id_xcb_get_keyboard_control_reply_t 185
#define obj_type_xcb_get_keyboard_control_reply_t (obj_types[obj_type_id_xcb_get_keyboard_control_reply_t])
  { NULL, 185, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_keyboard_control_reply_t" },
#define obj_type_id_xcb_bell_request_t 186
#define obj_type_xcb_bell_request_t (obj_types[obj_type_id_xcb_bell_request_t])
  { NULL, 186, OBJ_TYPE_FLAG_WEAK_REF, "xcb_bell_request_t" },
#define obj_type_id_xcb_change_pointer_control_request_t 187
#define obj_type_xcb_change_pointer_control_request_t (obj_types[obj_type_id_xcb_change_pointer_control_request_t])
  { NULL, 187, OBJ_TYPE_FLAG_WEAK_REF, "xcb_change_pointer_control_request_t" },
#define obj_type_id_xcb_get_pointer_control_request_t 188
#define obj_type_xcb_get_pointer_control_request_t (obj_types[obj_type_id_xcb_get_pointer_control_request_t])
  { NULL, 188, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_pointer_control_request_t" },
#define obj_type_id_xcb_get_pointer_control_reply_t 189
#define obj_type_xcb_get_pointer_control_reply_t (obj_types[obj_type_id_xcb_get_pointer_control_reply_t])
  { NULL, 189, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_pointer_control_reply_t" },
#define obj_type_id_xcb_set_screen_saver_request_t 190
#define obj_type_xcb_set_screen_saver_request_t (obj_types[obj_type_id_xcb_set_screen_saver_request_t])
  { NULL, 190, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_screen_saver_request_t" },
#define obj_type_id_xcb_get_screen_saver_request_t 191
#define obj_type_xcb_get_screen_saver_request_t (obj_types[obj_type_id_xcb_get_screen_saver_request_t])
  { NULL, 191, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_screen_saver_request_t" },
#define obj_type_id_xcb_get_screen_saver_reply_t 192
#define obj_type_xcb_get_screen_saver_reply_t (obj_types[obj_type_id_xcb_get_screen_saver_reply_t])
  { NULL, 192, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_screen_saver_reply_t" },
#define obj_type_id_xcb_change_hosts_request_t 193
#define obj_type_xcb_change_hosts_request_t (obj_types[obj_type_id_xcb_change_hosts_request_t])
  { NULL, 193, OBJ_TYPE_FLAG_WEAK_REF, "xcb_change_hosts_request_t" },
#define obj_type_id_xcb_host_t 194
#define obj_type_xcb_host_t (obj_types[obj_type_id_xcb_host_t])
  { NULL, 194, OBJ_TYPE_FLAG_WEAK_REF, "xcb_host_t" },
#define obj_type_id_xcb_list_hosts_request_t 195
#define obj_type_xcb_list_hosts_request_t (obj_types[obj_type_id_xcb_list_hosts_request_t])
  { NULL, 195, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_hosts_request_t" },
#define obj_type_id_xcb_list_hosts_reply_t 196
#define obj_type_xcb_list_hosts_reply_t (obj_types[obj_type_id_xcb_list_hosts_reply_t])
  { NULL, 196, OBJ_TYPE_FLAG_WEAK_REF, "xcb_list_hosts_reply_t" },
#define obj_type_id_xcb_set_access_control_request_t 197
#define obj_type_xcb_set_access_control_request_t (obj_types[obj_type_id_xcb_set_access_control_request_t])
  { NULL, 197, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_access_control_request_t" },
#define obj_type_id_xcb_set_close_down_mode_request_t 198
#define obj_type_xcb_set_close_down_mode_request_t (obj_types[obj_type_id_xcb_set_close_down_mode_request_t])
  { NULL, 198, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_close_down_mode_request_t" },
#define obj_type_id_xcb_kill_client_request_t 199
#define obj_type_xcb_kill_client_request_t (obj_types[obj_type_id_xcb_kill_client_request_t])
  { NULL, 199, OBJ_TYPE_FLAG_WEAK_REF, "xcb_kill_client_request_t" },
#define obj_type_id_xcb_rotate_properties_request_t 200
#define obj_type_xcb_rotate_properties_request_t (obj_types[obj_type_id_xcb_rotate_properties_request_t])
  { NULL, 200, OBJ_TYPE_FLAG_WEAK_REF, "xcb_rotate_properties_request_t" },
#define obj_type_id_xcb_force_screen_saver_request_t 201
#define obj_type_xcb_force_screen_saver_request_t (obj_types[obj_type_id_xcb_force_screen_saver_request_t])
  { NULL, 201, OBJ_TYPE_FLAG_WEAK_REF, "xcb_force_screen_saver_request_t" },
#define obj_type_id_xcb_set_pointer_mapping_request_t 202
#define obj_type_xcb_set_pointer_mapping_request_t (obj_types[obj_type_id_xcb_set_pointer_mapping_request_t])
  { NULL, 202, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_pointer_mapping_request_t" },
#define obj_type_id_xcb_set_pointer_mapping_reply_t 203
#define obj_type_xcb_set_pointer_mapping_reply_t (obj_types[obj_type_id_xcb_set_pointer_mapping_reply_t])
  { NULL, 203, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_pointer_mapping_reply_t" },
#define obj_type_id_xcb_get_pointer_mapping_request_t 204
#define obj_type_xcb_get_pointer_mapping_request_t (obj_types[obj_type_id_xcb_get_pointer_mapping_request_t])
  { NULL, 204, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_pointer_mapping_request_t" },
#define obj_type_id_xcb_get_pointer_mapping_reply_t 205
#define obj_type_xcb_get_pointer_mapping_reply_t (obj_types[obj_type_id_xcb_get_pointer_mapping_reply_t])
  { NULL, 205, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_pointer_mapping_reply_t" },
#define obj_type_id_xcb_set_modifier_mapping_request_t 206
#define obj_type_xcb_set_modifier_mapping_request_t (obj_types[obj_type_id_xcb_set_modifier_mapping_request_t])
  { NULL, 206, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_modifier_mapping_request_t" },
#define obj_type_id_xcb_set_modifier_mapping_reply_t 207
#define obj_type_xcb_set_modifier_mapping_reply_t (obj_types[obj_type_id_xcb_set_modifier_mapping_reply_t])
  { NULL, 207, OBJ_TYPE_FLAG_WEAK_REF, "xcb_set_modifier_mapping_reply_t" },
#define obj_type_id_xcb_get_modifier_mapping_request_t 208
#define obj_type_xcb_get_modifier_mapping_request_t (obj_types[obj_type_id_xcb_get_modifier_mapping_request_t])
  { NULL, 208, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_modifier_mapping_request_t" },
#define obj_type_id_xcb_get_modifier_mapping_reply_t 209
#define obj_type_xcb_get_modifier_mapping_reply_t (obj_types[obj_type_id_xcb_get_modifier_mapping_reply_t])
  { NULL, 209, OBJ_TYPE_FLAG_WEAK_REF, "xcb_get_modifier_mapping_reply_t" },
#define obj_type_id_xcb_no_operation_request_t 210
#define obj_type_xcb_no_operation_request_t (obj_types[obj_type_id_xcb_no_operation_request_t])
  { NULL, 210, OBJ_TYPE_FLAG_WEAK_REF, "xcb_no_operation_request_t" },
#define obj_type_id_xcb_connection_t 211
#define obj_type_xcb_connection_t (obj_types[obj_type_id_xcb_connection_t])
  { NULL, 211, OBJ_TYPE_FLAG_WEAK_REF, "xcb_connection_t" },
  {NULL, -1, 0, NULL},
};


#if LUAJIT_FFI

/* nobj_ffi_support_enabled_hint should be set to 1 when FFI support is enabled in at-least one
 * instance of a LuaJIT state.  It should never be set back to 0. */
static int nobj_ffi_support_enabled_hint = 0;
static const char nobj_ffi_support_key[] = "LuaNativeObject_FFI_SUPPORT";
static const char nobj_check_ffi_support_code[] =
"local stat, ffi=pcall(require,\"ffi\")\n" /* try loading LuaJIT`s FFI module. */
"if not stat then return false end\n"
"return true\n";

static int nobj_check_ffi_support(lua_State *L) {
	int rc;
	int err;

	/* check if ffi test has already been done. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	if(!lua_isnil(L, -1)) {
		rc = lua_toboolean(L, -1);
		lua_pop(L, 1);
		/* use results of previous check. */
		goto finished;
	}
	lua_pop(L, 1); /* pop nil. */

	err = luaL_loadbuffer(L, nobj_check_ffi_support_code,
		sizeof(nobj_check_ffi_support_code) - 1, nobj_ffi_support_key);
	if(0 == err) {
		err = lua_pcall(L, 0, 1, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Error when checking for FFI-support: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
		return 0;
	}
	/* check results of test. */
	rc = lua_toboolean(L, -1);
	lua_pop(L, 1); /* pop results. */
		/* cache results. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_pushboolean(L, rc);
	lua_rawset(L, LUA_REGISTRYINDEX);

finished:
	/* turn-on hint that there is FFI code enabled. */
	if(rc) {
		nobj_ffi_support_enabled_hint = 1;
	}

	return rc;
}

typedef struct {
	const char **ffi_init_code;
	int offset;
} nobj_reader_state;

static const char *nobj_lua_Reader(lua_State *L, void *data, size_t *size) {
	nobj_reader_state *state = (nobj_reader_state *)data;
	const char *ptr;

	(void)L;
	ptr = state->ffi_init_code[state->offset];
	if(ptr != NULL) {
		*size = strlen(ptr);
		state->offset++;
	} else {
		*size = 0;
	}
	return ptr;
}

static int nobj_try_loading_ffi(lua_State *L, const char *ffi_mod_name,
		const char *ffi_init_code[], const ffi_export_symbol *ffi_exports, int priv_table)
{
	nobj_reader_state state = { ffi_init_code, 0 };
	int err;

	/* export symbols to priv_table. */
	while(ffi_exports->name != NULL) {
		lua_pushstring(L, ffi_exports->name);
		lua_pushlightuserdata(L, ffi_exports->sym.data);
		lua_settable(L, priv_table);
		ffi_exports++;
	}
	err = lua_load_no_mode(L, nobj_lua_Reader, &state, ffi_mod_name);
	if(0 == err) {
		lua_pushvalue(L, -2); /* dup C module's table. */
		lua_pushvalue(L, priv_table); /* move priv_table to top of stack. */
		lua_remove(L, priv_table);
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		err = lua_pcall(L, 3, 0, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Failed to install FFI-based bindings: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
	}
	return err;
}
#endif


typedef struct {
	void *impl;
	void *obj;
} obj_implement;

static FUNC_UNUSED void *obj_implement_luaoptional(lua_State *L, int _index, void **impl, char *if_name) {
	void *ud;
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	/* get the implements table for this interface. */
	lua_pushlightuserdata(L, if_name);
	lua_rawget(L, LUA_REGISTRYINDEX);

	/* get pointer to userdata value & check if it is a userdata value. */
	ud = (obj_implement *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* get the userdata's metatable */
		if(lua_getmetatable(L, _index)) {
			/* lookup metatable in interface table for this object's implementation of the interface. */
			lua_gettable(L, -2);
		} else {
			/* no metatable. */
			goto no_interface;
		}
#if LUAJIT_FFI
	} else if(nobj_ffi_support_enabled_hint) { /* handle cdata. */
		/* get cdata interface check function from interface table. */
		lua_getfield(L, -1, "cdata");
		if(lua_isfunction(L, -1)) {
			/* pass cdata to function, return value should be an implmentation. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			/* get pointer to cdata. */
			ud = (void *)lua_topointer(L, _index);
		} else {
			lua_pop(L, 1); /* pop non-function. */
			goto no_interface;
		}
#endif
	} else {
		goto no_interface;
	}

	if(!lua_isnil(L, -1)) {
		*impl = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop interface table & implementation. */
		/* object implements interface. */
		return ud;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
no_interface:
	lua_pop(L, 1); /* pop interface table. */
	return NULL;
}

static FUNC_UNUSED void *obj_implement_luacheck(lua_State *L, int _index, void **impl, char *type) {
	void *ud = obj_implement_luaoptional(L, _index, impl, type);
	if(ud == NULL) {
#define ERROR_BUFFER_SIZE 256
		char buf[ERROR_BUFFER_SIZE];
		snprintf(buf, ERROR_BUFFER_SIZE-1,"Expected object with %s interface", type);
		/* value doesn't implement this interface. */
		luaL_argerror(L, _index, buf);
	}
	return ud;
}

/* use static pointer as key to interfaces table. (version 1.0) */
static char obj_interfaces_table_key[] = "obj_interfaces<1.0>_table_key";

static void obj_get_global_interfaces_table(lua_State *L) {
	/* get global interfaces table. */
	lua_getfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	if(lua_isnil(L, -1)) {
		/* Need to create global interfaces table. */
		lua_pop(L, 1); /* pop nil */
		lua_createtable(L, 0, 4); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		/* store interfaces table in Lua registery. */
		lua_setfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	}
}

static void obj_get_interface(lua_State *L, const char *name, int global_if_tab) {
	/* get a interface's implementation table */
	lua_getfield(L, global_if_tab, name);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil */
		/* new interface. (i.e. no object implement it yet.)
		 *
		 * create an empty table for this interface that will be used when an
		 * implementation is registered for this interface.
		 */
		lua_createtable(L, 0, 2); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		lua_setfield(L, global_if_tab, name); /* store interface in global interfaces table. */
	}
}

static int obj_get_userdata_interface(lua_State *L) {
	/* get the userdata's metatable */
	if(lua_getmetatable(L, 2)) {
		/* lookup metatable in interface table for the userdata's implementation of the interface. */
		lua_gettable(L, 1);
		if(!lua_isnil(L, -1)) {
			/* return the implementation. */
			return 1;
		}
	}
	/* no metatable or no implementation. */
	return 0;
}

static void obj_interface_register(lua_State *L, char *name, int global_if_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, name, global_if_tab);

	/* check for 'userdata' function. */
	lua_getfield(L, -1, "userdata");
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* add C function for getting a userdata's implementation. */
		lua_pushcfunction(L, obj_get_userdata_interface);
		lua_setfield(L, -2, "userdata");
	} else {
		/* already have function. */
		lua_pop(L, 1); /* pop C function. */
	}
	/* we are going to use a lightuserdata pointer for fast lookup of the interface's impl. table. */
	lua_pushlightuserdata(L, name);
	lua_insert(L, -2);
	lua_settable(L, LUA_REGISTRYINDEX);
}

static void obj_register_interfaces(lua_State *L, char *interfaces[]) {
	int i;
	int if_tab;
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(i = 0; interfaces[i] != NULL ; i++) {
		obj_interface_register(L, interfaces[i], if_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

static void obj_type_register_implement(lua_State *L, const reg_impl *impl, int global_if_tab, int mt_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, impl->if_name, global_if_tab);

	/* register object's implement in the interface table. */
	lua_pushvalue(L, mt_tab);
	lua_pushlightuserdata(L, (void *)impl->impl);
	lua_settable(L, -3);

	lua_pop(L, 1); /* pop inteface table. */
}

static void obj_type_register_implements(lua_State *L, const reg_impl *impls) {
	int if_tab;
	int mt_tab;
	/* get absolute position of object's metatable. */
	mt_tab = lua_gettop(L);
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(; impls->if_name != NULL ; impls++) {
		obj_type_register_implement(L, impls, if_tab, mt_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

#ifndef REG_PACKAGE_IS_CONSTRUCTOR
#define REG_PACKAGE_IS_CONSTRUCTOR 1
#endif

#ifndef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

/* For Lua 5.2 don't register modules as globals. */
#if LUA_VERSION_NUM == 502
#undef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

#ifndef REG_OBJECTS_AS_GLOBALS
#define REG_OBJECTS_AS_GLOBALS 0
#endif

#ifndef OBJ_DATA_HIDDEN_METATABLE
#define OBJ_DATA_HIDDEN_METATABLE 1
#endif

static FUNC_UNUSED int obj_import_external_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the metatable. */
		/* REGISTERY[lightuserdata<type>] = REGISTERY[type->name] */
		lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
		lua_pushvalue(L, -2); /* dup. type's metatable. */
		lua_rawset(L, LUA_REGISTRYINDEX); /* save external type's metatable. */
		/* NOTE: top of Lua stack still has the type's metatable. */
		return 1;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED int obj_import_external_ffi_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the C check function. */
		/* _priv_table[lightuserdata<type>] = REGISTERY[type->name].c_check */
		lua_getfield(L, -1, "c_check");
		lua_remove(L, -2); /* remove metatable. */
		if(lua_isfunction(L, -1)) {
			lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
			lua_pushvalue(L, -2); /* dup. check function */
			lua_rawset(L, -4); /* save check function to module's private table. */
			/* NOTE: top of Lua stack still has the type's C check function. */
			return 1;
		} else {
			lua_pop(L, 1); /* pop non function value. */
		}
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_toobj(lua_State *L, int _index) {
	obj_udata *ud;
	size_t len;

	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	/* verify userdata size. */
	len = lua_rawlen(L, _index);
	if(len != sizeof(obj_udata)) {
		/* This shouldn't be possible */
		luaL_error(L, "invalid userdata size: size=%d, expected=%d", len, sizeof(obj_udata));
	}
	return ud;
}

static FUNC_UNUSED int obj_udata_is_compatible(lua_State *L, obj_udata *ud, void **obj, base_caster_t *caster, obj_type *type) {
	obj_base *base;
	obj_type *ud_type;
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
	if(lua_rawequal(L, -1, -2)) {
		*obj = ud->obj;
		/* same type no casting needed. */
		return 1;
	} else if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		if((type->flags & OBJ_TYPE_IMPORT) == 0) {
			/* can't resolve internal type. */
			luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
		}
		/* try to import external type. */
		if(obj_import_external_type(L, type)) {
			/* imported type, re-try metatable check. */
			goto recheck_metatable;
		}
		/* External type not yet available, so the object can't be compatible. */
	} else {
		/* Different types see if we can cast to the required type. */
		lua_rawgeti(L, -2, type->id);
		base = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop obj_base or nil */
		if(base != NULL) {
			*caster = base->bcaster;
			/* get the obj_type for this userdata. */
			lua_pushliteral(L, ".type");
			lua_rawget(L, -3); /* type's metatable. */
			ud_type = lua_touserdata(L, -1);
			lua_pop(L, 1); /* pop obj_type or nil */
			if(base == NULL) {
				luaL_error(L, "bad userdata, missing type info.");
				return 0;
			}
			/* check if userdata is a simple object. */
			if(ud_type->flags & OBJ_TYPE_SIMPLE) {
				*obj = ud;
			} else {
				*obj = ud->obj;
			}
			return 1;
		}
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_luacheck_internal(lua_State *L, int _index, void **obj, obj_type *type, int not_delete) {
	obj_udata *ud;
	base_caster_t caster = NULL;
	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			if(obj_udata_is_compatible(L, ud, obj, &(caster), type)) {
				lua_pop(L, 2); /* pop both metatables. */
				/* apply caster function if needed. */
				if(caster != NULL && *obj != NULL) {
					caster(obj);
				}
				/* check object pointer. */
				if(*obj == NULL) {
					if(not_delete) {
						luaL_error(L, "null %s", type->name); /* object was garbage collected? */
					}
					return NULL;
				}
				return ud;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				*obj = *(void **)lua_topointer(L, _index);
				return ud;
			}
			lua_pop(L, 2);
		} else {
			lua_pop(L, 1); /* pop nil. */
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	if(not_delete) {
		luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	}
	return NULL;
}

static FUNC_UNUSED void *obj_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	if(lua_isnoneornil(L, _index)) {
		return obj;
	}
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;
	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);
}

static FUNC_UNUSED void *obj_udata_luadelete_weak(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* remove object from weak table. */
	lua_pushlightuserdata(L, obj);
	lua_pushnil(L);
	lua_rawset(L, -3);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush_weak(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;

	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* lookup userdata instance from pointer. */
	lua_pushlightuserdata(L, obj);
	lua_rawget(L, -2);
	if(!lua_isnil(L, -1)) {
		lua_remove(L, -2);     /* remove objects table. */
		return;
	}
	lua_pop(L, 1);  /* pop nil. */

#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		lua_remove(L, -2);
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));

	/* init. obj_udata. */
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	/* add weak reference to object. */
	lua_pushlightuserdata(L, obj); /* push object pointer as the 'key' */
	lua_pushvalue(L, -2);          /* push object's udata */
	lua_rawset(L, -4);             /* add weak reference to object. */
	lua_remove(L, -2);     /* remove objects table. */
}

/* default object equal method. */
static FUNC_UNUSED int obj_udata_default_equal(lua_State *L) {
	obj_udata *ud1 = obj_udata_toobj(L, 1);
	obj_udata *ud2 = obj_udata_toobj(L, 2);

	lua_pushboolean(L, (ud1->obj == ud2->obj));
	return 1;
}

/* default object tostring method. */
static FUNC_UNUSED int obj_udata_default_tostring(lua_State *L) {
	obj_udata *ud = obj_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p, flags=%d", ud->obj, ud->flags);
	lua_concat(L, 2);

	return 1;
}

/*
 * Simple userdata objects.
 */
static FUNC_UNUSED void *obj_simple_udata_toobj(lua_State *L, int _index) {
	void *ud;

	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	return ud;
}

static FUNC_UNUSED void * obj_simple_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *ud;
	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			lua_pushlightuserdata(L, type);
			lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
			if(lua_rawequal(L, -1, -2)) {
				lua_pop(L, 2); /* pop both metatables. */
				return ud;
			} else if(lua_isnil(L, -1)) {
				lua_pop(L, 1); /* pop nil. */
				if((type->flags & OBJ_TYPE_IMPORT) == 0) {
					/* can't resolve internal type. */
					luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
				}
				/* try to import external type. */
				if(obj_import_external_type(L, type)) {
					/* imported type, re-try metatable check. */
					goto recheck_metatable;
				}
				/* External type not yet available, so the object can't be compatible. */
				return 0;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				return (void *)lua_topointer(L, _index);
			}
		} else {
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void * obj_simple_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	return obj_simple_udata_luacheck(L, _index, type);
}

static FUNC_UNUSED void * obj_simple_udata_luadelete(lua_State *L, int _index, obj_type *type) {
	void *obj;
	obj = obj_simple_udata_luacheck(L, _index, type);
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void *obj_simple_udata_luapush(lua_State *L, void *obj, int size, obj_type *type)
{
	void *ud;
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_call(L, 1, 1);
		return obj;
	}
#endif
	/* create new userdata. */
	ud = lua_newuserdata(L, size);
	memcpy(ud, obj, size);
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	return ud;
}

/* default simple object equal method. */
static FUNC_UNUSED int obj_simple_udata_default_equal(lua_State *L) {
	void *ud1 = obj_simple_udata_toobj(L, 1);
	size_t len1 = lua_rawlen(L, 1);
	void *ud2 = obj_simple_udata_toobj(L, 2);
	size_t len2 = lua_rawlen(L, 2);

	if(len1 == len2) {
		lua_pushboolean(L, (memcmp(ud1, ud2, len1) == 0));
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

/* default simple object tostring method. */
static FUNC_UNUSED int obj_simple_udata_default_tostring(lua_State *L) {
	void *ud = obj_simple_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p", ud);
	lua_concat(L, 2);

	return 1;
}

static int obj_constructor_call_wrapper(lua_State *L) {
	/* replace '__call' table with constructor function. */
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_replace(L, 1);

	/* call constructor function with all parameters after the '__call' table. */
	lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
	/* return all results from constructor. */
	return lua_gettop(L);
}

static void obj_type_register_constants(lua_State *L, const obj_const *constants, int tab_idx,
		int bidirectional) {
	/* register constants. */
	while(constants->name != NULL) {
		lua_pushstring(L, constants->name);
		switch(constants->type) {
		case CONST_BOOLEAN:
			lua_pushboolean(L, constants->num != 0.0);
			break;
		case CONST_NUMBER:
			lua_pushnumber(L, constants->num);
			break;
		case CONST_STRING:
			lua_pushstring(L, constants->str);
			break;
		default:
			lua_pushnil(L);
			break;
		}
		/* map values back to keys. */
		if(bidirectional) {
			/* check if value already exists. */
			lua_pushvalue(L, -1);
			lua_rawget(L, tab_idx - 3);
			if(lua_isnil(L, -1)) {
				lua_pop(L, 1);
				/* add value->key mapping. */
				lua_pushvalue(L, -1);
				lua_pushvalue(L, -3);
				lua_rawset(L, tab_idx - 4);
			} else {
				/* value already exists. */
				lua_pop(L, 1);
			}
		}
		lua_rawset(L, tab_idx - 2);
		constants++;
	}
}

static void obj_type_register_package(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list = type_reg->pub_funcs;

	/* create public functions table. */
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	lua_pop(L, 1);  /* drop package table */
}

static void obj_type_register_meta(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list;

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	/* register methods. */
	luaL_setfuncs(L, type_reg->methods, 0);

	/* create metatable table. */
	lua_newtable(L);
	luaL_setfuncs(L, type_reg->metas, 0); /* fill metatable */
	/* setmetatable on meta-object. */
	lua_setmetatable(L, -2);

	lua_pop(L, 1);  /* drop meta-object */
}

static void obj_type_register(lua_State *L, const reg_sub_module *type_reg, int priv_table) {
	const luaL_Reg *reg_list;
	obj_type *type = type_reg->type;
	const obj_base *base = type_reg->bases;

	if(type_reg->req_type == REG_PACKAGE) {
		obj_type_register_package(L, type_reg);
		return;
	}
	if(type_reg->req_type == REG_META) {
		obj_type_register_meta(L, type_reg);
		return;
	}

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register "constructors" as to object's public API */
		luaL_setfuncs(L, reg_list, 0); /* fill public API table. */

		/* make public API table callable as the default constructor. */
		lua_newtable(L); /* create metatable */
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, reg_list[0].func); /* push first constructor function. */
		lua_pushcclosure(L, obj_constructor_call_wrapper, 1); /* make __call wrapper. */
		lua_rawset(L, -3);         /* metatable.__call = <default constructor> */

#if OBJ_DATA_HIDDEN_METATABLE
		lua_pushliteral(L, "__metatable");
		lua_pushboolean(L, 0);
		lua_rawset(L, -3);         /* metatable.__metatable = false */
#endif

		/* setmetatable on public API table. */
		lua_setmetatable(L, -2);

		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
	} else {
		/* register all methods as public functions. */
#if OBJ_DATA_HIDDEN_METATABLE
		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
#endif
	}

	luaL_setfuncs(L, type_reg->methods, 0); /* fill methods table. */

	luaL_newmetatable(L, type->name); /* create metatable */
	lua_pushliteral(L, ".name");
	lua_pushstring(L, type->name);
	lua_rawset(L, -3);    /* metatable['.name'] = "<object_name>" */
	lua_pushliteral(L, ".type");
	lua_pushlightuserdata(L, type);
	lua_rawset(L, -3);    /* metatable['.type'] = lightuserdata -> obj_type */
	lua_pushlightuserdata(L, type);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, LUA_REGISTRYINDEX);    /* REGISTRY[type] = metatable */

	/* add metatable to 'priv_table' */
	lua_pushstring(L, type->name);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, priv_table);    /* priv_table["<object_name>"] = metatable */

	luaL_setfuncs(L, type_reg->metas, 0); /* fill metatable */

	/* add obj_bases to metatable. */
	while(base->id >= 0) {
		lua_pushlightuserdata(L, (void *)base);
		lua_rawseti(L, -2, base->id);
		base++;
	}

	obj_type_register_constants(L, type_reg->constants, -2, type_reg->bidirectional_consts);

	obj_type_register_implements(L, type_reg->implements);

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);               /* dup methods table */
	lua_rawset(L, -3);                  /* metatable.__index = methods */
#if OBJ_DATA_HIDDEN_METATABLE
	lua_pushliteral(L, "__metatable");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);                  /* hide metatable:
	                                       metatable.__metatable = false */
#endif
	lua_pop(L, 2);                      /* drop metatable & methods */
}

static FUNC_UNUSED int lua_checktype_ref(lua_State *L, int _index, int _type) {
	luaL_checktype(L,_index,_type);
	lua_pushvalue(L,_index);
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

/* use static pointer as key to weak callback_state table. */
static char obj_callback_state_weak_ref_key[] = "obj_callback_state_weak_ref_key";

static FUNC_UNUSED void *nobj_get_callback_state(lua_State *L, int owner_idx, int size) {
	void *cb_state;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* create weak table for callback_state */
		lua_newtable(L);               /* weak table. */
		lua_newtable(L);               /* metatable for weak table. */
		lua_pushliteral(L, "__mode");
		lua_pushliteral(L, "k");
		lua_rawset(L, -3);             /* metatable.__mode = 'k'  weak keys. */
		lua_setmetatable(L, -2);       /* add metatable to weak table. */
		lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
		lua_pushvalue(L, -2);          /* dup weak table. */
		lua_rawset(L, LUA_REGISTRYINDEX);  /* add weak table to registry. */
	}

	/* check weak table for callback_state. */
	lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
		/* create new callback state. */
		cb_state = lua_newuserdata(L, size);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	} else {
		/* got existing callback state. */
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop <weak table>, <callback_state> */
	}

	return cb_state;
}

static FUNC_UNUSED void *nobj_delete_callback_state(lua_State *L, int owner_idx) {
	void *cb_state = NULL;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil.  no weak table, so there is no callback state. */
		return NULL;
	}
	/* get callback state. */
	lua_pushvalue(L, owner_idx); /* dup. owner */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 2); /* pop <weak table>, nil.  No callback state for the owner. */
	} else {
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop <state> */
		/* remove callback state. */
		lua_pushvalue(L, owner_idx); /* dup. owner */
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	}

	return cb_state;
}



static char *obj_interfaces[] = {
  NULL,
};



#define obj_type_xcb_char2b_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_char2b_t))
#define obj_type_xcb_char2b_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_char2b_t))
#define obj_type_xcb_char2b_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_char2b_t), flags)
#define obj_type_xcb_char2b_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_char2b_t), flags)

#define obj_type_xcb_point_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_point_t))
#define obj_type_xcb_point_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_point_t))
#define obj_type_xcb_point_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_point_t), flags)
#define obj_type_xcb_point_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_point_t), flags)

#define obj_type_xcb_rectangle_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_rectangle_t))
#define obj_type_xcb_rectangle_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_rectangle_t))
#define obj_type_xcb_rectangle_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_rectangle_t), flags)
#define obj_type_xcb_rectangle_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_rectangle_t), flags)

#define obj_type_xcb_arc_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_arc_t))
#define obj_type_xcb_arc_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_arc_t))
#define obj_type_xcb_arc_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_arc_t), flags)
#define obj_type_xcb_arc_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_arc_t), flags)

#define obj_type_xcb_format_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_format_t))
#define obj_type_xcb_format_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_format_t))
#define obj_type_xcb_format_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_format_t), flags)
#define obj_type_xcb_format_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_format_t), flags)

#define obj_type_xcb_visualtype_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_visualtype_t))
#define obj_type_xcb_visualtype_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_visualtype_t))
#define obj_type_xcb_visualtype_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_visualtype_t), flags)
#define obj_type_xcb_visualtype_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_visualtype_t), flags)

#define obj_type_xcb_depth_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_depth_t))
#define obj_type_xcb_depth_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_depth_t))
#define obj_type_xcb_depth_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_depth_t), flags)
#define obj_type_xcb_depth_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_depth_t), flags)

#define obj_type_xcb_screen_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_screen_t))
#define obj_type_xcb_screen_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_screen_t))
#define obj_type_xcb_screen_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_screen_t), flags)
#define obj_type_xcb_screen_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_screen_t), flags)

#define obj_type_xcb_setup_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_setup_request_t))
#define obj_type_xcb_setup_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_setup_request_t))
#define obj_type_xcb_setup_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_setup_request_t), flags)
#define obj_type_xcb_setup_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_setup_request_t), flags)

#define obj_type_xcb_setup_failed_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_setup_failed_t))
#define obj_type_xcb_setup_failed_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_setup_failed_t))
#define obj_type_xcb_setup_failed_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_setup_failed_t), flags)
#define obj_type_xcb_setup_failed_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_setup_failed_t), flags)

#define obj_type_xcb_setup_authenticate_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_setup_authenticate_t))
#define obj_type_xcb_setup_authenticate_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_setup_authenticate_t))
#define obj_type_xcb_setup_authenticate_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_setup_authenticate_t), flags)
#define obj_type_xcb_setup_authenticate_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_setup_authenticate_t), flags)

#define obj_type_xcb_setup_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_setup_t))
#define obj_type_xcb_setup_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_setup_t))
#define obj_type_xcb_setup_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_setup_t), flags)
#define obj_type_xcb_setup_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_setup_t), flags)

#define obj_type_xcb_key_press_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_key_press_event_t))
#define obj_type_xcb_key_press_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_key_press_event_t))
#define obj_type_xcb_key_press_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_key_press_event_t), flags)
#define obj_type_xcb_key_press_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_key_press_event_t), flags)

#define obj_type_xcb_button_press_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_button_press_event_t))
#define obj_type_xcb_button_press_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_button_press_event_t))
#define obj_type_xcb_button_press_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_button_press_event_t), flags)
#define obj_type_xcb_button_press_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_button_press_event_t), flags)

#define obj_type_xcb_motion_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_motion_notify_event_t))
#define obj_type_xcb_motion_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_motion_notify_event_t))
#define obj_type_xcb_motion_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_motion_notify_event_t), flags)
#define obj_type_xcb_motion_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_motion_notify_event_t), flags)

#define obj_type_xcb_enter_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_enter_notify_event_t))
#define obj_type_xcb_enter_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_enter_notify_event_t))
#define obj_type_xcb_enter_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_enter_notify_event_t), flags)
#define obj_type_xcb_enter_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_enter_notify_event_t), flags)

#define obj_type_xcb_focus_in_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_focus_in_event_t))
#define obj_type_xcb_focus_in_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_focus_in_event_t))
#define obj_type_xcb_focus_in_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_focus_in_event_t), flags)
#define obj_type_xcb_focus_in_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_focus_in_event_t), flags)

#define obj_type_xcb_keymap_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_keymap_notify_event_t))
#define obj_type_xcb_keymap_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_keymap_notify_event_t))
#define obj_type_xcb_keymap_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_keymap_notify_event_t), flags)
#define obj_type_xcb_keymap_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_keymap_notify_event_t), flags)

#define obj_type_xcb_expose_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_expose_event_t))
#define obj_type_xcb_expose_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_expose_event_t))
#define obj_type_xcb_expose_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_expose_event_t), flags)
#define obj_type_xcb_expose_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_expose_event_t), flags)

#define obj_type_xcb_graphics_exposure_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_graphics_exposure_event_t))
#define obj_type_xcb_graphics_exposure_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_graphics_exposure_event_t))
#define obj_type_xcb_graphics_exposure_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_graphics_exposure_event_t), flags)
#define obj_type_xcb_graphics_exposure_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_graphics_exposure_event_t), flags)

#define obj_type_xcb_no_exposure_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_no_exposure_event_t))
#define obj_type_xcb_no_exposure_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_no_exposure_event_t))
#define obj_type_xcb_no_exposure_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_no_exposure_event_t), flags)
#define obj_type_xcb_no_exposure_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_no_exposure_event_t), flags)

#define obj_type_xcb_visibility_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_visibility_notify_event_t))
#define obj_type_xcb_visibility_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_visibility_notify_event_t))
#define obj_type_xcb_visibility_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_visibility_notify_event_t), flags)
#define obj_type_xcb_visibility_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_visibility_notify_event_t), flags)

#define obj_type_xcb_create_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_create_notify_event_t))
#define obj_type_xcb_create_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_create_notify_event_t))
#define obj_type_xcb_create_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_create_notify_event_t), flags)
#define obj_type_xcb_create_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_create_notify_event_t), flags)

#define obj_type_xcb_destroy_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_destroy_notify_event_t))
#define obj_type_xcb_destroy_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_destroy_notify_event_t))
#define obj_type_xcb_destroy_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_destroy_notify_event_t), flags)
#define obj_type_xcb_destroy_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_destroy_notify_event_t), flags)

#define obj_type_xcb_unmap_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_unmap_notify_event_t))
#define obj_type_xcb_unmap_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_unmap_notify_event_t))
#define obj_type_xcb_unmap_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_unmap_notify_event_t), flags)
#define obj_type_xcb_unmap_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_unmap_notify_event_t), flags)

#define obj_type_xcb_map_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_map_notify_event_t))
#define obj_type_xcb_map_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_map_notify_event_t))
#define obj_type_xcb_map_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_map_notify_event_t), flags)
#define obj_type_xcb_map_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_map_notify_event_t), flags)

#define obj_type_xcb_map_request_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_map_request_event_t))
#define obj_type_xcb_map_request_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_map_request_event_t))
#define obj_type_xcb_map_request_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_map_request_event_t), flags)
#define obj_type_xcb_map_request_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_map_request_event_t), flags)

#define obj_type_xcb_reparent_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_reparent_notify_event_t))
#define obj_type_xcb_reparent_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_reparent_notify_event_t))
#define obj_type_xcb_reparent_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_reparent_notify_event_t), flags)
#define obj_type_xcb_reparent_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_reparent_notify_event_t), flags)

#define obj_type_xcb_configure_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_configure_notify_event_t))
#define obj_type_xcb_configure_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_configure_notify_event_t))
#define obj_type_xcb_configure_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_configure_notify_event_t), flags)
#define obj_type_xcb_configure_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_configure_notify_event_t), flags)

#define obj_type_xcb_configure_request_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_configure_request_event_t))
#define obj_type_xcb_configure_request_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_configure_request_event_t))
#define obj_type_xcb_configure_request_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_configure_request_event_t), flags)
#define obj_type_xcb_configure_request_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_configure_request_event_t), flags)

#define obj_type_xcb_gravity_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_gravity_notify_event_t))
#define obj_type_xcb_gravity_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_gravity_notify_event_t))
#define obj_type_xcb_gravity_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_gravity_notify_event_t), flags)
#define obj_type_xcb_gravity_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_gravity_notify_event_t), flags)

#define obj_type_xcb_resize_request_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_resize_request_event_t))
#define obj_type_xcb_resize_request_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_resize_request_event_t))
#define obj_type_xcb_resize_request_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_resize_request_event_t), flags)
#define obj_type_xcb_resize_request_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_resize_request_event_t), flags)

#define obj_type_xcb_circulate_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_circulate_notify_event_t))
#define obj_type_xcb_circulate_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_circulate_notify_event_t))
#define obj_type_xcb_circulate_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_circulate_notify_event_t), flags)
#define obj_type_xcb_circulate_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_circulate_notify_event_t), flags)

#define obj_type_xcb_property_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_property_notify_event_t))
#define obj_type_xcb_property_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_property_notify_event_t))
#define obj_type_xcb_property_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_property_notify_event_t), flags)
#define obj_type_xcb_property_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_property_notify_event_t), flags)

#define obj_type_xcb_selection_clear_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_selection_clear_event_t))
#define obj_type_xcb_selection_clear_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_selection_clear_event_t))
#define obj_type_xcb_selection_clear_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_selection_clear_event_t), flags)
#define obj_type_xcb_selection_clear_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_selection_clear_event_t), flags)

#define obj_type_xcb_selection_request_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_selection_request_event_t))
#define obj_type_xcb_selection_request_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_selection_request_event_t))
#define obj_type_xcb_selection_request_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_selection_request_event_t), flags)
#define obj_type_xcb_selection_request_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_selection_request_event_t), flags)

#define obj_type_xcb_selection_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_selection_notify_event_t))
#define obj_type_xcb_selection_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_selection_notify_event_t))
#define obj_type_xcb_selection_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_selection_notify_event_t), flags)
#define obj_type_xcb_selection_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_selection_notify_event_t), flags)

#define obj_type_xcb_colormap_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_colormap_notify_event_t))
#define obj_type_xcb_colormap_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_colormap_notify_event_t))
#define obj_type_xcb_colormap_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_colormap_notify_event_t), flags)
#define obj_type_xcb_colormap_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_colormap_notify_event_t), flags)

#define obj_type_xcb_client_message_data_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_client_message_data_t))
#define obj_type_xcb_client_message_data_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_client_message_data_t))
#define obj_type_xcb_client_message_data_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_client_message_data_t), flags)
#define obj_type_xcb_client_message_data_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_client_message_data_t), flags)

#define obj_type_xcb_client_message_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_client_message_event_t))
#define obj_type_xcb_client_message_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_client_message_event_t))
#define obj_type_xcb_client_message_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_client_message_event_t), flags)
#define obj_type_xcb_client_message_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_client_message_event_t), flags)

#define obj_type_xcb_mapping_notify_event_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_mapping_notify_event_t))
#define obj_type_xcb_mapping_notify_event_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_mapping_notify_event_t))
#define obj_type_xcb_mapping_notify_event_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_mapping_notify_event_t), flags)
#define obj_type_xcb_mapping_notify_event_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_mapping_notify_event_t), flags)

#define obj_type_xcb_request_error_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_request_error_t))
#define obj_type_xcb_request_error_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_request_error_t))
#define obj_type_xcb_request_error_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_request_error_t), flags)
#define obj_type_xcb_request_error_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_request_error_t), flags)

#define obj_type_xcb_value_error_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_value_error_t))
#define obj_type_xcb_value_error_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_value_error_t))
#define obj_type_xcb_value_error_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_value_error_t), flags)
#define obj_type_xcb_value_error_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_value_error_t), flags)

#define obj_type_xcb_create_window_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_create_window_request_t))
#define obj_type_xcb_create_window_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_create_window_request_t))
#define obj_type_xcb_create_window_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_create_window_request_t), flags)
#define obj_type_xcb_create_window_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_create_window_request_t), flags)

#define obj_type_xcb_change_window_attributes_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_change_window_attributes_request_t))
#define obj_type_xcb_change_window_attributes_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_change_window_attributes_request_t))
#define obj_type_xcb_change_window_attributes_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_change_window_attributes_request_t), flags)
#define obj_type_xcb_change_window_attributes_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_change_window_attributes_request_t), flags)

#define obj_type_xcb_get_window_attributes_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_window_attributes_request_t))
#define obj_type_xcb_get_window_attributes_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_window_attributes_request_t))
#define obj_type_xcb_get_window_attributes_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_window_attributes_request_t), flags)
#define obj_type_xcb_get_window_attributes_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_window_attributes_request_t), flags)

#define obj_type_xcb_get_window_attributes_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_window_attributes_reply_t))
#define obj_type_xcb_get_window_attributes_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_window_attributes_reply_t))
#define obj_type_xcb_get_window_attributes_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_window_attributes_reply_t), flags)
#define obj_type_xcb_get_window_attributes_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_window_attributes_reply_t), flags)

#define obj_type_xcb_destroy_window_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_destroy_window_request_t))
#define obj_type_xcb_destroy_window_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_destroy_window_request_t))
#define obj_type_xcb_destroy_window_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_destroy_window_request_t), flags)
#define obj_type_xcb_destroy_window_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_destroy_window_request_t), flags)

#define obj_type_xcb_destroy_subwindows_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_destroy_subwindows_request_t))
#define obj_type_xcb_destroy_subwindows_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_destroy_subwindows_request_t))
#define obj_type_xcb_destroy_subwindows_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_destroy_subwindows_request_t), flags)
#define obj_type_xcb_destroy_subwindows_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_destroy_subwindows_request_t), flags)

#define obj_type_xcb_change_save_set_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_change_save_set_request_t))
#define obj_type_xcb_change_save_set_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_change_save_set_request_t))
#define obj_type_xcb_change_save_set_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_change_save_set_request_t), flags)
#define obj_type_xcb_change_save_set_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_change_save_set_request_t), flags)

#define obj_type_xcb_reparent_window_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_reparent_window_request_t))
#define obj_type_xcb_reparent_window_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_reparent_window_request_t))
#define obj_type_xcb_reparent_window_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_reparent_window_request_t), flags)
#define obj_type_xcb_reparent_window_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_reparent_window_request_t), flags)

#define obj_type_xcb_map_window_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_map_window_request_t))
#define obj_type_xcb_map_window_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_map_window_request_t))
#define obj_type_xcb_map_window_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_map_window_request_t), flags)
#define obj_type_xcb_map_window_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_map_window_request_t), flags)

#define obj_type_xcb_map_subwindows_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_map_subwindows_request_t))
#define obj_type_xcb_map_subwindows_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_map_subwindows_request_t))
#define obj_type_xcb_map_subwindows_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_map_subwindows_request_t), flags)
#define obj_type_xcb_map_subwindows_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_map_subwindows_request_t), flags)

#define obj_type_xcb_unmap_window_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_unmap_window_request_t))
#define obj_type_xcb_unmap_window_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_unmap_window_request_t))
#define obj_type_xcb_unmap_window_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_unmap_window_request_t), flags)
#define obj_type_xcb_unmap_window_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_unmap_window_request_t), flags)

#define obj_type_xcb_unmap_subwindows_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_unmap_subwindows_request_t))
#define obj_type_xcb_unmap_subwindows_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_unmap_subwindows_request_t))
#define obj_type_xcb_unmap_subwindows_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_unmap_subwindows_request_t), flags)
#define obj_type_xcb_unmap_subwindows_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_unmap_subwindows_request_t), flags)

#define obj_type_xcb_configure_window_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_configure_window_request_t))
#define obj_type_xcb_configure_window_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_configure_window_request_t))
#define obj_type_xcb_configure_window_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_configure_window_request_t), flags)
#define obj_type_xcb_configure_window_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_configure_window_request_t), flags)

#define obj_type_xcb_circulate_window_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_circulate_window_request_t))
#define obj_type_xcb_circulate_window_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_circulate_window_request_t))
#define obj_type_xcb_circulate_window_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_circulate_window_request_t), flags)
#define obj_type_xcb_circulate_window_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_circulate_window_request_t), flags)

#define obj_type_xcb_get_geometry_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_geometry_request_t))
#define obj_type_xcb_get_geometry_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_geometry_request_t))
#define obj_type_xcb_get_geometry_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_geometry_request_t), flags)
#define obj_type_xcb_get_geometry_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_geometry_request_t), flags)

#define obj_type_xcb_get_geometry_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_geometry_reply_t))
#define obj_type_xcb_get_geometry_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_geometry_reply_t))
#define obj_type_xcb_get_geometry_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_geometry_reply_t), flags)
#define obj_type_xcb_get_geometry_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_geometry_reply_t), flags)

#define obj_type_xcb_query_tree_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_tree_request_t))
#define obj_type_xcb_query_tree_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_tree_request_t))
#define obj_type_xcb_query_tree_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_tree_request_t), flags)
#define obj_type_xcb_query_tree_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_tree_request_t), flags)

#define obj_type_xcb_query_tree_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_tree_reply_t))
#define obj_type_xcb_query_tree_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_tree_reply_t))
#define obj_type_xcb_query_tree_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_tree_reply_t), flags)
#define obj_type_xcb_query_tree_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_tree_reply_t), flags)

#define obj_type_xcb_intern_atom_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_intern_atom_request_t))
#define obj_type_xcb_intern_atom_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_intern_atom_request_t))
#define obj_type_xcb_intern_atom_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_intern_atom_request_t), flags)
#define obj_type_xcb_intern_atom_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_intern_atom_request_t), flags)

#define obj_type_xcb_intern_atom_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_intern_atom_reply_t))
#define obj_type_xcb_intern_atom_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_intern_atom_reply_t))
#define obj_type_xcb_intern_atom_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_intern_atom_reply_t), flags)
#define obj_type_xcb_intern_atom_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_intern_atom_reply_t), flags)

#define obj_type_xcb_get_atom_name_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_atom_name_request_t))
#define obj_type_xcb_get_atom_name_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_atom_name_request_t))
#define obj_type_xcb_get_atom_name_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_atom_name_request_t), flags)
#define obj_type_xcb_get_atom_name_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_atom_name_request_t), flags)

#define obj_type_xcb_get_atom_name_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_atom_name_reply_t))
#define obj_type_xcb_get_atom_name_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_atom_name_reply_t))
#define obj_type_xcb_get_atom_name_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_atom_name_reply_t), flags)
#define obj_type_xcb_get_atom_name_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_atom_name_reply_t), flags)

#define obj_type_xcb_change_property_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_change_property_request_t))
#define obj_type_xcb_change_property_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_change_property_request_t))
#define obj_type_xcb_change_property_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_change_property_request_t), flags)
#define obj_type_xcb_change_property_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_change_property_request_t), flags)

#define obj_type_xcb_delete_property_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_delete_property_request_t))
#define obj_type_xcb_delete_property_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_delete_property_request_t))
#define obj_type_xcb_delete_property_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_delete_property_request_t), flags)
#define obj_type_xcb_delete_property_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_delete_property_request_t), flags)

#define obj_type_xcb_get_property_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_property_request_t))
#define obj_type_xcb_get_property_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_property_request_t))
#define obj_type_xcb_get_property_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_property_request_t), flags)
#define obj_type_xcb_get_property_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_property_request_t), flags)

#define obj_type_xcb_get_property_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_property_reply_t))
#define obj_type_xcb_get_property_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_property_reply_t))
#define obj_type_xcb_get_property_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_property_reply_t), flags)
#define obj_type_xcb_get_property_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_property_reply_t), flags)

#define obj_type_xcb_list_properties_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_properties_request_t))
#define obj_type_xcb_list_properties_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_properties_request_t))
#define obj_type_xcb_list_properties_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_properties_request_t), flags)
#define obj_type_xcb_list_properties_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_properties_request_t), flags)

#define obj_type_xcb_list_properties_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_properties_reply_t))
#define obj_type_xcb_list_properties_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_properties_reply_t))
#define obj_type_xcb_list_properties_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_properties_reply_t), flags)
#define obj_type_xcb_list_properties_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_properties_reply_t), flags)

#define obj_type_xcb_set_selection_owner_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_selection_owner_request_t))
#define obj_type_xcb_set_selection_owner_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_selection_owner_request_t))
#define obj_type_xcb_set_selection_owner_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_selection_owner_request_t), flags)
#define obj_type_xcb_set_selection_owner_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_selection_owner_request_t), flags)

#define obj_type_xcb_get_selection_owner_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_selection_owner_request_t))
#define obj_type_xcb_get_selection_owner_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_selection_owner_request_t))
#define obj_type_xcb_get_selection_owner_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_selection_owner_request_t), flags)
#define obj_type_xcb_get_selection_owner_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_selection_owner_request_t), flags)

#define obj_type_xcb_get_selection_owner_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_selection_owner_reply_t))
#define obj_type_xcb_get_selection_owner_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_selection_owner_reply_t))
#define obj_type_xcb_get_selection_owner_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_selection_owner_reply_t), flags)
#define obj_type_xcb_get_selection_owner_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_selection_owner_reply_t), flags)

#define obj_type_xcb_convert_selection_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_convert_selection_request_t))
#define obj_type_xcb_convert_selection_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_convert_selection_request_t))
#define obj_type_xcb_convert_selection_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_convert_selection_request_t), flags)
#define obj_type_xcb_convert_selection_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_convert_selection_request_t), flags)

#define obj_type_xcb_send_event_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_send_event_request_t))
#define obj_type_xcb_send_event_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_send_event_request_t))
#define obj_type_xcb_send_event_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_send_event_request_t), flags)
#define obj_type_xcb_send_event_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_send_event_request_t), flags)

#define obj_type_xcb_grab_pointer_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_grab_pointer_request_t))
#define obj_type_xcb_grab_pointer_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_grab_pointer_request_t))
#define obj_type_xcb_grab_pointer_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_grab_pointer_request_t), flags)
#define obj_type_xcb_grab_pointer_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_grab_pointer_request_t), flags)

#define obj_type_xcb_grab_pointer_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_grab_pointer_reply_t))
#define obj_type_xcb_grab_pointer_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_grab_pointer_reply_t))
#define obj_type_xcb_grab_pointer_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_grab_pointer_reply_t), flags)
#define obj_type_xcb_grab_pointer_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_grab_pointer_reply_t), flags)

#define obj_type_xcb_ungrab_pointer_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_ungrab_pointer_request_t))
#define obj_type_xcb_ungrab_pointer_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_ungrab_pointer_request_t))
#define obj_type_xcb_ungrab_pointer_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_ungrab_pointer_request_t), flags)
#define obj_type_xcb_ungrab_pointer_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_ungrab_pointer_request_t), flags)

#define obj_type_xcb_grab_button_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_grab_button_request_t))
#define obj_type_xcb_grab_button_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_grab_button_request_t))
#define obj_type_xcb_grab_button_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_grab_button_request_t), flags)
#define obj_type_xcb_grab_button_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_grab_button_request_t), flags)

#define obj_type_xcb_ungrab_button_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_ungrab_button_request_t))
#define obj_type_xcb_ungrab_button_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_ungrab_button_request_t))
#define obj_type_xcb_ungrab_button_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_ungrab_button_request_t), flags)
#define obj_type_xcb_ungrab_button_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_ungrab_button_request_t), flags)

#define obj_type_xcb_change_active_pointer_grab_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_change_active_pointer_grab_request_t))
#define obj_type_xcb_change_active_pointer_grab_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_change_active_pointer_grab_request_t))
#define obj_type_xcb_change_active_pointer_grab_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_change_active_pointer_grab_request_t), flags)
#define obj_type_xcb_change_active_pointer_grab_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_change_active_pointer_grab_request_t), flags)

#define obj_type_xcb_grab_keyboard_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_grab_keyboard_request_t))
#define obj_type_xcb_grab_keyboard_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_grab_keyboard_request_t))
#define obj_type_xcb_grab_keyboard_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_grab_keyboard_request_t), flags)
#define obj_type_xcb_grab_keyboard_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_grab_keyboard_request_t), flags)

#define obj_type_xcb_grab_keyboard_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_grab_keyboard_reply_t))
#define obj_type_xcb_grab_keyboard_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_grab_keyboard_reply_t))
#define obj_type_xcb_grab_keyboard_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_grab_keyboard_reply_t), flags)
#define obj_type_xcb_grab_keyboard_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_grab_keyboard_reply_t), flags)

#define obj_type_xcb_ungrab_keyboard_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_ungrab_keyboard_request_t))
#define obj_type_xcb_ungrab_keyboard_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_ungrab_keyboard_request_t))
#define obj_type_xcb_ungrab_keyboard_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_ungrab_keyboard_request_t), flags)
#define obj_type_xcb_ungrab_keyboard_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_ungrab_keyboard_request_t), flags)

#define obj_type_xcb_grab_key_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_grab_key_request_t))
#define obj_type_xcb_grab_key_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_grab_key_request_t))
#define obj_type_xcb_grab_key_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_grab_key_request_t), flags)
#define obj_type_xcb_grab_key_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_grab_key_request_t), flags)

#define obj_type_xcb_ungrab_key_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_ungrab_key_request_t))
#define obj_type_xcb_ungrab_key_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_ungrab_key_request_t))
#define obj_type_xcb_ungrab_key_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_ungrab_key_request_t), flags)
#define obj_type_xcb_ungrab_key_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_ungrab_key_request_t), flags)

#define obj_type_xcb_allow_events_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_allow_events_request_t))
#define obj_type_xcb_allow_events_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_allow_events_request_t))
#define obj_type_xcb_allow_events_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_allow_events_request_t), flags)
#define obj_type_xcb_allow_events_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_allow_events_request_t), flags)

#define obj_type_xcb_grab_server_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_grab_server_request_t))
#define obj_type_xcb_grab_server_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_grab_server_request_t))
#define obj_type_xcb_grab_server_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_grab_server_request_t), flags)
#define obj_type_xcb_grab_server_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_grab_server_request_t), flags)

#define obj_type_xcb_ungrab_server_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_ungrab_server_request_t))
#define obj_type_xcb_ungrab_server_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_ungrab_server_request_t))
#define obj_type_xcb_ungrab_server_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_ungrab_server_request_t), flags)
#define obj_type_xcb_ungrab_server_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_ungrab_server_request_t), flags)

#define obj_type_xcb_query_pointer_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_pointer_request_t))
#define obj_type_xcb_query_pointer_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_pointer_request_t))
#define obj_type_xcb_query_pointer_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_pointer_request_t), flags)
#define obj_type_xcb_query_pointer_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_pointer_request_t), flags)

#define obj_type_xcb_query_pointer_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_pointer_reply_t))
#define obj_type_xcb_query_pointer_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_pointer_reply_t))
#define obj_type_xcb_query_pointer_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_pointer_reply_t), flags)
#define obj_type_xcb_query_pointer_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_pointer_reply_t), flags)

#define obj_type_xcb_timecoord_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_timecoord_t))
#define obj_type_xcb_timecoord_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_timecoord_t))
#define obj_type_xcb_timecoord_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_timecoord_t), flags)
#define obj_type_xcb_timecoord_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_timecoord_t), flags)

#define obj_type_xcb_get_motion_events_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_motion_events_request_t))
#define obj_type_xcb_get_motion_events_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_motion_events_request_t))
#define obj_type_xcb_get_motion_events_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_motion_events_request_t), flags)
#define obj_type_xcb_get_motion_events_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_motion_events_request_t), flags)

#define obj_type_xcb_get_motion_events_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_motion_events_reply_t))
#define obj_type_xcb_get_motion_events_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_motion_events_reply_t))
#define obj_type_xcb_get_motion_events_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_motion_events_reply_t), flags)
#define obj_type_xcb_get_motion_events_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_motion_events_reply_t), flags)

#define obj_type_xcb_translate_coordinates_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_translate_coordinates_request_t))
#define obj_type_xcb_translate_coordinates_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_translate_coordinates_request_t))
#define obj_type_xcb_translate_coordinates_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_translate_coordinates_request_t), flags)
#define obj_type_xcb_translate_coordinates_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_translate_coordinates_request_t), flags)

#define obj_type_xcb_translate_coordinates_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_translate_coordinates_reply_t))
#define obj_type_xcb_translate_coordinates_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_translate_coordinates_reply_t))
#define obj_type_xcb_translate_coordinates_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_translate_coordinates_reply_t), flags)
#define obj_type_xcb_translate_coordinates_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_translate_coordinates_reply_t), flags)

#define obj_type_xcb_warp_pointer_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_warp_pointer_request_t))
#define obj_type_xcb_warp_pointer_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_warp_pointer_request_t))
#define obj_type_xcb_warp_pointer_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_warp_pointer_request_t), flags)
#define obj_type_xcb_warp_pointer_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_warp_pointer_request_t), flags)

#define obj_type_xcb_set_input_focus_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_input_focus_request_t))
#define obj_type_xcb_set_input_focus_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_input_focus_request_t))
#define obj_type_xcb_set_input_focus_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_input_focus_request_t), flags)
#define obj_type_xcb_set_input_focus_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_input_focus_request_t), flags)

#define obj_type_xcb_get_input_focus_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_input_focus_request_t))
#define obj_type_xcb_get_input_focus_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_input_focus_request_t))
#define obj_type_xcb_get_input_focus_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_input_focus_request_t), flags)
#define obj_type_xcb_get_input_focus_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_input_focus_request_t), flags)

#define obj_type_xcb_get_input_focus_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_input_focus_reply_t))
#define obj_type_xcb_get_input_focus_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_input_focus_reply_t))
#define obj_type_xcb_get_input_focus_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_input_focus_reply_t), flags)
#define obj_type_xcb_get_input_focus_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_input_focus_reply_t), flags)

#define obj_type_xcb_query_keymap_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_keymap_request_t))
#define obj_type_xcb_query_keymap_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_keymap_request_t))
#define obj_type_xcb_query_keymap_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_keymap_request_t), flags)
#define obj_type_xcb_query_keymap_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_keymap_request_t), flags)

#define obj_type_xcb_query_keymap_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_keymap_reply_t))
#define obj_type_xcb_query_keymap_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_keymap_reply_t))
#define obj_type_xcb_query_keymap_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_keymap_reply_t), flags)
#define obj_type_xcb_query_keymap_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_keymap_reply_t), flags)

#define obj_type_xcb_open_font_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_open_font_request_t))
#define obj_type_xcb_open_font_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_open_font_request_t))
#define obj_type_xcb_open_font_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_open_font_request_t), flags)
#define obj_type_xcb_open_font_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_open_font_request_t), flags)

#define obj_type_xcb_close_font_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_close_font_request_t))
#define obj_type_xcb_close_font_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_close_font_request_t))
#define obj_type_xcb_close_font_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_close_font_request_t), flags)
#define obj_type_xcb_close_font_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_close_font_request_t), flags)

#define obj_type_xcb_fontprop_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_fontprop_t))
#define obj_type_xcb_fontprop_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_fontprop_t))
#define obj_type_xcb_fontprop_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_fontprop_t), flags)
#define obj_type_xcb_fontprop_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_fontprop_t), flags)

#define obj_type_xcb_charinfo_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_charinfo_t))
#define obj_type_xcb_charinfo_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_charinfo_t))
#define obj_type_xcb_charinfo_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_charinfo_t), flags)
#define obj_type_xcb_charinfo_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_charinfo_t), flags)

#define obj_type_xcb_query_font_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_font_request_t))
#define obj_type_xcb_query_font_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_font_request_t))
#define obj_type_xcb_query_font_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_font_request_t), flags)
#define obj_type_xcb_query_font_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_font_request_t), flags)

#define obj_type_xcb_query_font_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_font_reply_t))
#define obj_type_xcb_query_font_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_font_reply_t))
#define obj_type_xcb_query_font_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_font_reply_t), flags)
#define obj_type_xcb_query_font_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_font_reply_t), flags)

#define obj_type_xcb_query_text_extents_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_text_extents_request_t))
#define obj_type_xcb_query_text_extents_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_text_extents_request_t))
#define obj_type_xcb_query_text_extents_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_text_extents_request_t), flags)
#define obj_type_xcb_query_text_extents_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_text_extents_request_t), flags)

#define obj_type_xcb_query_text_extents_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_text_extents_reply_t))
#define obj_type_xcb_query_text_extents_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_text_extents_reply_t))
#define obj_type_xcb_query_text_extents_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_text_extents_reply_t), flags)
#define obj_type_xcb_query_text_extents_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_text_extents_reply_t), flags)

#define obj_type_xcb_str_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_str_t))
#define obj_type_xcb_str_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_str_t))
#define obj_type_xcb_str_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_str_t), flags)
#define obj_type_xcb_str_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_str_t), flags)

#define obj_type_xcb_list_fonts_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_fonts_request_t))
#define obj_type_xcb_list_fonts_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_fonts_request_t))
#define obj_type_xcb_list_fonts_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_fonts_request_t), flags)
#define obj_type_xcb_list_fonts_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_fonts_request_t), flags)

#define obj_type_xcb_list_fonts_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_fonts_reply_t))
#define obj_type_xcb_list_fonts_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_fonts_reply_t))
#define obj_type_xcb_list_fonts_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_fonts_reply_t), flags)
#define obj_type_xcb_list_fonts_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_fonts_reply_t), flags)

#define obj_type_xcb_list_fonts_with_info_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_fonts_with_info_request_t))
#define obj_type_xcb_list_fonts_with_info_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_fonts_with_info_request_t))
#define obj_type_xcb_list_fonts_with_info_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_fonts_with_info_request_t), flags)
#define obj_type_xcb_list_fonts_with_info_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_fonts_with_info_request_t), flags)

#define obj_type_xcb_list_fonts_with_info_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_fonts_with_info_reply_t))
#define obj_type_xcb_list_fonts_with_info_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_fonts_with_info_reply_t))
#define obj_type_xcb_list_fonts_with_info_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_fonts_with_info_reply_t), flags)
#define obj_type_xcb_list_fonts_with_info_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_fonts_with_info_reply_t), flags)

#define obj_type_xcb_set_font_path_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_font_path_request_t))
#define obj_type_xcb_set_font_path_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_font_path_request_t))
#define obj_type_xcb_set_font_path_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_font_path_request_t), flags)
#define obj_type_xcb_set_font_path_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_font_path_request_t), flags)

#define obj_type_xcb_get_font_path_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_font_path_request_t))
#define obj_type_xcb_get_font_path_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_font_path_request_t))
#define obj_type_xcb_get_font_path_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_font_path_request_t), flags)
#define obj_type_xcb_get_font_path_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_font_path_request_t), flags)

#define obj_type_xcb_get_font_path_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_font_path_reply_t))
#define obj_type_xcb_get_font_path_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_font_path_reply_t))
#define obj_type_xcb_get_font_path_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_font_path_reply_t), flags)
#define obj_type_xcb_get_font_path_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_font_path_reply_t), flags)

#define obj_type_xcb_create_pixmap_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_create_pixmap_request_t))
#define obj_type_xcb_create_pixmap_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_create_pixmap_request_t))
#define obj_type_xcb_create_pixmap_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_create_pixmap_request_t), flags)
#define obj_type_xcb_create_pixmap_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_create_pixmap_request_t), flags)

#define obj_type_xcb_free_pixmap_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_free_pixmap_request_t))
#define obj_type_xcb_free_pixmap_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_free_pixmap_request_t))
#define obj_type_xcb_free_pixmap_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_free_pixmap_request_t), flags)
#define obj_type_xcb_free_pixmap_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_free_pixmap_request_t), flags)

#define obj_type_xcb_create_gc_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_create_gc_request_t))
#define obj_type_xcb_create_gc_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_create_gc_request_t))
#define obj_type_xcb_create_gc_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_create_gc_request_t), flags)
#define obj_type_xcb_create_gc_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_create_gc_request_t), flags)

#define obj_type_xcb_change_gc_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_change_gc_request_t))
#define obj_type_xcb_change_gc_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_change_gc_request_t))
#define obj_type_xcb_change_gc_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_change_gc_request_t), flags)
#define obj_type_xcb_change_gc_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_change_gc_request_t), flags)

#define obj_type_xcb_copy_gc_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_copy_gc_request_t))
#define obj_type_xcb_copy_gc_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_copy_gc_request_t))
#define obj_type_xcb_copy_gc_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_copy_gc_request_t), flags)
#define obj_type_xcb_copy_gc_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_copy_gc_request_t), flags)

#define obj_type_xcb_set_dashes_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_dashes_request_t))
#define obj_type_xcb_set_dashes_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_dashes_request_t))
#define obj_type_xcb_set_dashes_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_dashes_request_t), flags)
#define obj_type_xcb_set_dashes_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_dashes_request_t), flags)

#define obj_type_xcb_set_clip_rectangles_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_clip_rectangles_request_t))
#define obj_type_xcb_set_clip_rectangles_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_clip_rectangles_request_t))
#define obj_type_xcb_set_clip_rectangles_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_clip_rectangles_request_t), flags)
#define obj_type_xcb_set_clip_rectangles_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_clip_rectangles_request_t), flags)

#define obj_type_xcb_free_gc_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_free_gc_request_t))
#define obj_type_xcb_free_gc_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_free_gc_request_t))
#define obj_type_xcb_free_gc_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_free_gc_request_t), flags)
#define obj_type_xcb_free_gc_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_free_gc_request_t), flags)

#define obj_type_xcb_clear_area_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_clear_area_request_t))
#define obj_type_xcb_clear_area_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_clear_area_request_t))
#define obj_type_xcb_clear_area_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_clear_area_request_t), flags)
#define obj_type_xcb_clear_area_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_clear_area_request_t), flags)

#define obj_type_xcb_copy_area_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_copy_area_request_t))
#define obj_type_xcb_copy_area_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_copy_area_request_t))
#define obj_type_xcb_copy_area_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_copy_area_request_t), flags)
#define obj_type_xcb_copy_area_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_copy_area_request_t), flags)

#define obj_type_xcb_copy_plane_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_copy_plane_request_t))
#define obj_type_xcb_copy_plane_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_copy_plane_request_t))
#define obj_type_xcb_copy_plane_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_copy_plane_request_t), flags)
#define obj_type_xcb_copy_plane_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_copy_plane_request_t), flags)

#define obj_type_xcb_poly_point_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_poly_point_request_t))
#define obj_type_xcb_poly_point_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_poly_point_request_t))
#define obj_type_xcb_poly_point_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_poly_point_request_t), flags)
#define obj_type_xcb_poly_point_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_poly_point_request_t), flags)

#define obj_type_xcb_poly_line_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_poly_line_request_t))
#define obj_type_xcb_poly_line_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_poly_line_request_t))
#define obj_type_xcb_poly_line_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_poly_line_request_t), flags)
#define obj_type_xcb_poly_line_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_poly_line_request_t), flags)

#define obj_type_xcb_segment_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_segment_t))
#define obj_type_xcb_segment_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_segment_t))
#define obj_type_xcb_segment_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_segment_t), flags)
#define obj_type_xcb_segment_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_segment_t), flags)

#define obj_type_xcb_poly_segment_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_poly_segment_request_t))
#define obj_type_xcb_poly_segment_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_poly_segment_request_t))
#define obj_type_xcb_poly_segment_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_poly_segment_request_t), flags)
#define obj_type_xcb_poly_segment_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_poly_segment_request_t), flags)

#define obj_type_xcb_poly_rectangle_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_poly_rectangle_request_t))
#define obj_type_xcb_poly_rectangle_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_poly_rectangle_request_t))
#define obj_type_xcb_poly_rectangle_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_poly_rectangle_request_t), flags)
#define obj_type_xcb_poly_rectangle_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_poly_rectangle_request_t), flags)

#define obj_type_xcb_poly_arc_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_poly_arc_request_t))
#define obj_type_xcb_poly_arc_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_poly_arc_request_t))
#define obj_type_xcb_poly_arc_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_poly_arc_request_t), flags)
#define obj_type_xcb_poly_arc_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_poly_arc_request_t), flags)

#define obj_type_xcb_fill_poly_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_fill_poly_request_t))
#define obj_type_xcb_fill_poly_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_fill_poly_request_t))
#define obj_type_xcb_fill_poly_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_fill_poly_request_t), flags)
#define obj_type_xcb_fill_poly_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_fill_poly_request_t), flags)

#define obj_type_xcb_poly_fill_rectangle_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_poly_fill_rectangle_request_t))
#define obj_type_xcb_poly_fill_rectangle_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_poly_fill_rectangle_request_t))
#define obj_type_xcb_poly_fill_rectangle_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_poly_fill_rectangle_request_t), flags)
#define obj_type_xcb_poly_fill_rectangle_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_poly_fill_rectangle_request_t), flags)

#define obj_type_xcb_poly_fill_arc_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_poly_fill_arc_request_t))
#define obj_type_xcb_poly_fill_arc_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_poly_fill_arc_request_t))
#define obj_type_xcb_poly_fill_arc_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_poly_fill_arc_request_t), flags)
#define obj_type_xcb_poly_fill_arc_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_poly_fill_arc_request_t), flags)

#define obj_type_xcb_put_image_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_put_image_request_t))
#define obj_type_xcb_put_image_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_put_image_request_t))
#define obj_type_xcb_put_image_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_put_image_request_t), flags)
#define obj_type_xcb_put_image_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_put_image_request_t), flags)

#define obj_type_xcb_get_image_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_image_request_t))
#define obj_type_xcb_get_image_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_image_request_t))
#define obj_type_xcb_get_image_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_image_request_t), flags)
#define obj_type_xcb_get_image_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_image_request_t), flags)

#define obj_type_xcb_get_image_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_image_reply_t))
#define obj_type_xcb_get_image_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_image_reply_t))
#define obj_type_xcb_get_image_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_image_reply_t), flags)
#define obj_type_xcb_get_image_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_image_reply_t), flags)

#define obj_type_xcb_poly_text_8_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_poly_text_8_request_t))
#define obj_type_xcb_poly_text_8_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_poly_text_8_request_t))
#define obj_type_xcb_poly_text_8_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_poly_text_8_request_t), flags)
#define obj_type_xcb_poly_text_8_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_poly_text_8_request_t), flags)

#define obj_type_xcb_poly_text_16_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_poly_text_16_request_t))
#define obj_type_xcb_poly_text_16_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_poly_text_16_request_t))
#define obj_type_xcb_poly_text_16_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_poly_text_16_request_t), flags)
#define obj_type_xcb_poly_text_16_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_poly_text_16_request_t), flags)

#define obj_type_xcb_image_text_8_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_image_text_8_request_t))
#define obj_type_xcb_image_text_8_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_image_text_8_request_t))
#define obj_type_xcb_image_text_8_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_image_text_8_request_t), flags)
#define obj_type_xcb_image_text_8_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_image_text_8_request_t), flags)

#define obj_type_xcb_image_text_16_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_image_text_16_request_t))
#define obj_type_xcb_image_text_16_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_image_text_16_request_t))
#define obj_type_xcb_image_text_16_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_image_text_16_request_t), flags)
#define obj_type_xcb_image_text_16_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_image_text_16_request_t), flags)

#define obj_type_xcb_create_colormap_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_create_colormap_request_t))
#define obj_type_xcb_create_colormap_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_create_colormap_request_t))
#define obj_type_xcb_create_colormap_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_create_colormap_request_t), flags)
#define obj_type_xcb_create_colormap_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_create_colormap_request_t), flags)

#define obj_type_xcb_free_colormap_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_free_colormap_request_t))
#define obj_type_xcb_free_colormap_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_free_colormap_request_t))
#define obj_type_xcb_free_colormap_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_free_colormap_request_t), flags)
#define obj_type_xcb_free_colormap_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_free_colormap_request_t), flags)

#define obj_type_xcb_copy_colormap_and_free_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_copy_colormap_and_free_request_t))
#define obj_type_xcb_copy_colormap_and_free_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_copy_colormap_and_free_request_t))
#define obj_type_xcb_copy_colormap_and_free_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_copy_colormap_and_free_request_t), flags)
#define obj_type_xcb_copy_colormap_and_free_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_copy_colormap_and_free_request_t), flags)

#define obj_type_xcb_install_colormap_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_install_colormap_request_t))
#define obj_type_xcb_install_colormap_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_install_colormap_request_t))
#define obj_type_xcb_install_colormap_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_install_colormap_request_t), flags)
#define obj_type_xcb_install_colormap_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_install_colormap_request_t), flags)

#define obj_type_xcb_uninstall_colormap_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_uninstall_colormap_request_t))
#define obj_type_xcb_uninstall_colormap_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_uninstall_colormap_request_t))
#define obj_type_xcb_uninstall_colormap_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_uninstall_colormap_request_t), flags)
#define obj_type_xcb_uninstall_colormap_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_uninstall_colormap_request_t), flags)

#define obj_type_xcb_list_installed_colormaps_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_installed_colormaps_request_t))
#define obj_type_xcb_list_installed_colormaps_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_installed_colormaps_request_t))
#define obj_type_xcb_list_installed_colormaps_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_installed_colormaps_request_t), flags)
#define obj_type_xcb_list_installed_colormaps_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_installed_colormaps_request_t), flags)

#define obj_type_xcb_list_installed_colormaps_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_installed_colormaps_reply_t))
#define obj_type_xcb_list_installed_colormaps_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_installed_colormaps_reply_t))
#define obj_type_xcb_list_installed_colormaps_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_installed_colormaps_reply_t), flags)
#define obj_type_xcb_list_installed_colormaps_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_installed_colormaps_reply_t), flags)

#define obj_type_xcb_alloc_color_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_alloc_color_request_t))
#define obj_type_xcb_alloc_color_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_alloc_color_request_t))
#define obj_type_xcb_alloc_color_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_alloc_color_request_t), flags)
#define obj_type_xcb_alloc_color_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_alloc_color_request_t), flags)

#define obj_type_xcb_alloc_color_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_alloc_color_reply_t))
#define obj_type_xcb_alloc_color_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_alloc_color_reply_t))
#define obj_type_xcb_alloc_color_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_alloc_color_reply_t), flags)
#define obj_type_xcb_alloc_color_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_alloc_color_reply_t), flags)

#define obj_type_xcb_alloc_named_color_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_alloc_named_color_request_t))
#define obj_type_xcb_alloc_named_color_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_alloc_named_color_request_t))
#define obj_type_xcb_alloc_named_color_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_alloc_named_color_request_t), flags)
#define obj_type_xcb_alloc_named_color_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_alloc_named_color_request_t), flags)

#define obj_type_xcb_alloc_named_color_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_alloc_named_color_reply_t))
#define obj_type_xcb_alloc_named_color_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_alloc_named_color_reply_t))
#define obj_type_xcb_alloc_named_color_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_alloc_named_color_reply_t), flags)
#define obj_type_xcb_alloc_named_color_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_alloc_named_color_reply_t), flags)

#define obj_type_xcb_alloc_color_cells_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_alloc_color_cells_request_t))
#define obj_type_xcb_alloc_color_cells_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_alloc_color_cells_request_t))
#define obj_type_xcb_alloc_color_cells_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_alloc_color_cells_request_t), flags)
#define obj_type_xcb_alloc_color_cells_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_alloc_color_cells_request_t), flags)

#define obj_type_xcb_alloc_color_cells_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_alloc_color_cells_reply_t))
#define obj_type_xcb_alloc_color_cells_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_alloc_color_cells_reply_t))
#define obj_type_xcb_alloc_color_cells_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_alloc_color_cells_reply_t), flags)
#define obj_type_xcb_alloc_color_cells_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_alloc_color_cells_reply_t), flags)

#define obj_type_xcb_alloc_color_planes_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_alloc_color_planes_request_t))
#define obj_type_xcb_alloc_color_planes_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_alloc_color_planes_request_t))
#define obj_type_xcb_alloc_color_planes_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_alloc_color_planes_request_t), flags)
#define obj_type_xcb_alloc_color_planes_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_alloc_color_planes_request_t), flags)

#define obj_type_xcb_alloc_color_planes_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_alloc_color_planes_reply_t))
#define obj_type_xcb_alloc_color_planes_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_alloc_color_planes_reply_t))
#define obj_type_xcb_alloc_color_planes_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_alloc_color_planes_reply_t), flags)
#define obj_type_xcb_alloc_color_planes_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_alloc_color_planes_reply_t), flags)

#define obj_type_xcb_free_colors_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_free_colors_request_t))
#define obj_type_xcb_free_colors_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_free_colors_request_t))
#define obj_type_xcb_free_colors_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_free_colors_request_t), flags)
#define obj_type_xcb_free_colors_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_free_colors_request_t), flags)

#define obj_type_xcb_coloritem_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_coloritem_t))
#define obj_type_xcb_coloritem_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_coloritem_t))
#define obj_type_xcb_coloritem_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_coloritem_t), flags)
#define obj_type_xcb_coloritem_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_coloritem_t), flags)

#define obj_type_xcb_store_colors_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_store_colors_request_t))
#define obj_type_xcb_store_colors_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_store_colors_request_t))
#define obj_type_xcb_store_colors_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_store_colors_request_t), flags)
#define obj_type_xcb_store_colors_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_store_colors_request_t), flags)

#define obj_type_xcb_store_named_color_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_store_named_color_request_t))
#define obj_type_xcb_store_named_color_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_store_named_color_request_t))
#define obj_type_xcb_store_named_color_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_store_named_color_request_t), flags)
#define obj_type_xcb_store_named_color_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_store_named_color_request_t), flags)

#define obj_type_xcb_rgb_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_rgb_t))
#define obj_type_xcb_rgb_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_rgb_t))
#define obj_type_xcb_rgb_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_rgb_t), flags)
#define obj_type_xcb_rgb_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_rgb_t), flags)

#define obj_type_xcb_query_colors_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_colors_request_t))
#define obj_type_xcb_query_colors_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_colors_request_t))
#define obj_type_xcb_query_colors_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_colors_request_t), flags)
#define obj_type_xcb_query_colors_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_colors_request_t), flags)

#define obj_type_xcb_query_colors_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_colors_reply_t))
#define obj_type_xcb_query_colors_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_colors_reply_t))
#define obj_type_xcb_query_colors_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_colors_reply_t), flags)
#define obj_type_xcb_query_colors_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_colors_reply_t), flags)

#define obj_type_xcb_lookup_color_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_lookup_color_request_t))
#define obj_type_xcb_lookup_color_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_lookup_color_request_t))
#define obj_type_xcb_lookup_color_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_lookup_color_request_t), flags)
#define obj_type_xcb_lookup_color_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_lookup_color_request_t), flags)

#define obj_type_xcb_lookup_color_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_lookup_color_reply_t))
#define obj_type_xcb_lookup_color_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_lookup_color_reply_t))
#define obj_type_xcb_lookup_color_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_lookup_color_reply_t), flags)
#define obj_type_xcb_lookup_color_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_lookup_color_reply_t), flags)

#define obj_type_xcb_create_cursor_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_create_cursor_request_t))
#define obj_type_xcb_create_cursor_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_create_cursor_request_t))
#define obj_type_xcb_create_cursor_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_create_cursor_request_t), flags)
#define obj_type_xcb_create_cursor_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_create_cursor_request_t), flags)

#define obj_type_xcb_create_glyph_cursor_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_create_glyph_cursor_request_t))
#define obj_type_xcb_create_glyph_cursor_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_create_glyph_cursor_request_t))
#define obj_type_xcb_create_glyph_cursor_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_create_glyph_cursor_request_t), flags)
#define obj_type_xcb_create_glyph_cursor_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_create_glyph_cursor_request_t), flags)

#define obj_type_xcb_free_cursor_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_free_cursor_request_t))
#define obj_type_xcb_free_cursor_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_free_cursor_request_t))
#define obj_type_xcb_free_cursor_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_free_cursor_request_t), flags)
#define obj_type_xcb_free_cursor_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_free_cursor_request_t), flags)

#define obj_type_xcb_recolor_cursor_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_recolor_cursor_request_t))
#define obj_type_xcb_recolor_cursor_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_recolor_cursor_request_t))
#define obj_type_xcb_recolor_cursor_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_recolor_cursor_request_t), flags)
#define obj_type_xcb_recolor_cursor_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_recolor_cursor_request_t), flags)

#define obj_type_xcb_query_best_size_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_best_size_request_t))
#define obj_type_xcb_query_best_size_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_best_size_request_t))
#define obj_type_xcb_query_best_size_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_best_size_request_t), flags)
#define obj_type_xcb_query_best_size_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_best_size_request_t), flags)

#define obj_type_xcb_query_best_size_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_best_size_reply_t))
#define obj_type_xcb_query_best_size_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_best_size_reply_t))
#define obj_type_xcb_query_best_size_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_best_size_reply_t), flags)
#define obj_type_xcb_query_best_size_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_best_size_reply_t), flags)

#define obj_type_xcb_query_extension_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_extension_request_t))
#define obj_type_xcb_query_extension_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_extension_request_t))
#define obj_type_xcb_query_extension_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_extension_request_t), flags)
#define obj_type_xcb_query_extension_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_extension_request_t), flags)

#define obj_type_xcb_query_extension_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_query_extension_reply_t))
#define obj_type_xcb_query_extension_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_query_extension_reply_t))
#define obj_type_xcb_query_extension_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_query_extension_reply_t), flags)
#define obj_type_xcb_query_extension_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_query_extension_reply_t), flags)

#define obj_type_xcb_list_extensions_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_extensions_request_t))
#define obj_type_xcb_list_extensions_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_extensions_request_t))
#define obj_type_xcb_list_extensions_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_extensions_request_t), flags)
#define obj_type_xcb_list_extensions_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_extensions_request_t), flags)

#define obj_type_xcb_list_extensions_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_extensions_reply_t))
#define obj_type_xcb_list_extensions_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_extensions_reply_t))
#define obj_type_xcb_list_extensions_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_extensions_reply_t), flags)
#define obj_type_xcb_list_extensions_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_extensions_reply_t), flags)

#define obj_type_xcb_change_keyboard_mapping_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_change_keyboard_mapping_request_t))
#define obj_type_xcb_change_keyboard_mapping_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_change_keyboard_mapping_request_t))
#define obj_type_xcb_change_keyboard_mapping_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_change_keyboard_mapping_request_t), flags)
#define obj_type_xcb_change_keyboard_mapping_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_change_keyboard_mapping_request_t), flags)

#define obj_type_xcb_get_keyboard_mapping_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_keyboard_mapping_request_t))
#define obj_type_xcb_get_keyboard_mapping_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_keyboard_mapping_request_t))
#define obj_type_xcb_get_keyboard_mapping_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_keyboard_mapping_request_t), flags)
#define obj_type_xcb_get_keyboard_mapping_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_keyboard_mapping_request_t), flags)

#define obj_type_xcb_get_keyboard_mapping_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_keyboard_mapping_reply_t))
#define obj_type_xcb_get_keyboard_mapping_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_keyboard_mapping_reply_t))
#define obj_type_xcb_get_keyboard_mapping_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_keyboard_mapping_reply_t), flags)
#define obj_type_xcb_get_keyboard_mapping_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_keyboard_mapping_reply_t), flags)

#define obj_type_xcb_change_keyboard_control_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_change_keyboard_control_request_t))
#define obj_type_xcb_change_keyboard_control_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_change_keyboard_control_request_t))
#define obj_type_xcb_change_keyboard_control_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_change_keyboard_control_request_t), flags)
#define obj_type_xcb_change_keyboard_control_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_change_keyboard_control_request_t), flags)

#define obj_type_xcb_get_keyboard_control_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_keyboard_control_request_t))
#define obj_type_xcb_get_keyboard_control_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_keyboard_control_request_t))
#define obj_type_xcb_get_keyboard_control_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_keyboard_control_request_t), flags)
#define obj_type_xcb_get_keyboard_control_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_keyboard_control_request_t), flags)

#define obj_type_xcb_get_keyboard_control_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_keyboard_control_reply_t))
#define obj_type_xcb_get_keyboard_control_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_keyboard_control_reply_t))
#define obj_type_xcb_get_keyboard_control_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_keyboard_control_reply_t), flags)
#define obj_type_xcb_get_keyboard_control_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_keyboard_control_reply_t), flags)

#define obj_type_xcb_bell_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_bell_request_t))
#define obj_type_xcb_bell_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_bell_request_t))
#define obj_type_xcb_bell_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_bell_request_t), flags)
#define obj_type_xcb_bell_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_bell_request_t), flags)

#define obj_type_xcb_change_pointer_control_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_change_pointer_control_request_t))
#define obj_type_xcb_change_pointer_control_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_change_pointer_control_request_t))
#define obj_type_xcb_change_pointer_control_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_change_pointer_control_request_t), flags)
#define obj_type_xcb_change_pointer_control_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_change_pointer_control_request_t), flags)

#define obj_type_xcb_get_pointer_control_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_pointer_control_request_t))
#define obj_type_xcb_get_pointer_control_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_pointer_control_request_t))
#define obj_type_xcb_get_pointer_control_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_pointer_control_request_t), flags)
#define obj_type_xcb_get_pointer_control_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_pointer_control_request_t), flags)

#define obj_type_xcb_get_pointer_control_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_pointer_control_reply_t))
#define obj_type_xcb_get_pointer_control_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_pointer_control_reply_t))
#define obj_type_xcb_get_pointer_control_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_pointer_control_reply_t), flags)
#define obj_type_xcb_get_pointer_control_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_pointer_control_reply_t), flags)

#define obj_type_xcb_set_screen_saver_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_screen_saver_request_t))
#define obj_type_xcb_set_screen_saver_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_screen_saver_request_t))
#define obj_type_xcb_set_screen_saver_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_screen_saver_request_t), flags)
#define obj_type_xcb_set_screen_saver_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_screen_saver_request_t), flags)

#define obj_type_xcb_get_screen_saver_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_screen_saver_request_t))
#define obj_type_xcb_get_screen_saver_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_screen_saver_request_t))
#define obj_type_xcb_get_screen_saver_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_screen_saver_request_t), flags)
#define obj_type_xcb_get_screen_saver_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_screen_saver_request_t), flags)

#define obj_type_xcb_get_screen_saver_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_screen_saver_reply_t))
#define obj_type_xcb_get_screen_saver_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_screen_saver_reply_t))
#define obj_type_xcb_get_screen_saver_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_screen_saver_reply_t), flags)
#define obj_type_xcb_get_screen_saver_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_screen_saver_reply_t), flags)

#define obj_type_xcb_change_hosts_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_change_hosts_request_t))
#define obj_type_xcb_change_hosts_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_change_hosts_request_t))
#define obj_type_xcb_change_hosts_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_change_hosts_request_t), flags)
#define obj_type_xcb_change_hosts_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_change_hosts_request_t), flags)

#define obj_type_xcb_host_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_host_t))
#define obj_type_xcb_host_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_host_t))
#define obj_type_xcb_host_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_host_t), flags)
#define obj_type_xcb_host_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_host_t), flags)

#define obj_type_xcb_list_hosts_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_hosts_request_t))
#define obj_type_xcb_list_hosts_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_hosts_request_t))
#define obj_type_xcb_list_hosts_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_hosts_request_t), flags)
#define obj_type_xcb_list_hosts_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_hosts_request_t), flags)

#define obj_type_xcb_list_hosts_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_list_hosts_reply_t))
#define obj_type_xcb_list_hosts_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_list_hosts_reply_t))
#define obj_type_xcb_list_hosts_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_list_hosts_reply_t), flags)
#define obj_type_xcb_list_hosts_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_list_hosts_reply_t), flags)

#define obj_type_xcb_set_access_control_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_access_control_request_t))
#define obj_type_xcb_set_access_control_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_access_control_request_t))
#define obj_type_xcb_set_access_control_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_access_control_request_t), flags)
#define obj_type_xcb_set_access_control_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_access_control_request_t), flags)

#define obj_type_xcb_set_close_down_mode_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_close_down_mode_request_t))
#define obj_type_xcb_set_close_down_mode_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_close_down_mode_request_t))
#define obj_type_xcb_set_close_down_mode_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_close_down_mode_request_t), flags)
#define obj_type_xcb_set_close_down_mode_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_close_down_mode_request_t), flags)

#define obj_type_xcb_kill_client_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_kill_client_request_t))
#define obj_type_xcb_kill_client_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_kill_client_request_t))
#define obj_type_xcb_kill_client_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_kill_client_request_t), flags)
#define obj_type_xcb_kill_client_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_kill_client_request_t), flags)

#define obj_type_xcb_rotate_properties_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_rotate_properties_request_t))
#define obj_type_xcb_rotate_properties_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_rotate_properties_request_t))
#define obj_type_xcb_rotate_properties_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_rotate_properties_request_t), flags)
#define obj_type_xcb_rotate_properties_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_rotate_properties_request_t), flags)

#define obj_type_xcb_force_screen_saver_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_force_screen_saver_request_t))
#define obj_type_xcb_force_screen_saver_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_force_screen_saver_request_t))
#define obj_type_xcb_force_screen_saver_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_force_screen_saver_request_t), flags)
#define obj_type_xcb_force_screen_saver_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_force_screen_saver_request_t), flags)

#define obj_type_xcb_set_pointer_mapping_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_pointer_mapping_request_t))
#define obj_type_xcb_set_pointer_mapping_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_pointer_mapping_request_t))
#define obj_type_xcb_set_pointer_mapping_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_pointer_mapping_request_t), flags)
#define obj_type_xcb_set_pointer_mapping_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_pointer_mapping_request_t), flags)

#define obj_type_xcb_set_pointer_mapping_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_pointer_mapping_reply_t))
#define obj_type_xcb_set_pointer_mapping_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_pointer_mapping_reply_t))
#define obj_type_xcb_set_pointer_mapping_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_pointer_mapping_reply_t), flags)
#define obj_type_xcb_set_pointer_mapping_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_pointer_mapping_reply_t), flags)

#define obj_type_xcb_get_pointer_mapping_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_pointer_mapping_request_t))
#define obj_type_xcb_get_pointer_mapping_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_pointer_mapping_request_t))
#define obj_type_xcb_get_pointer_mapping_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_pointer_mapping_request_t), flags)
#define obj_type_xcb_get_pointer_mapping_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_pointer_mapping_request_t), flags)

#define obj_type_xcb_get_pointer_mapping_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_pointer_mapping_reply_t))
#define obj_type_xcb_get_pointer_mapping_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_pointer_mapping_reply_t))
#define obj_type_xcb_get_pointer_mapping_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_pointer_mapping_reply_t), flags)
#define obj_type_xcb_get_pointer_mapping_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_pointer_mapping_reply_t), flags)

#define obj_type_xcb_set_modifier_mapping_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_modifier_mapping_request_t))
#define obj_type_xcb_set_modifier_mapping_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_modifier_mapping_request_t))
#define obj_type_xcb_set_modifier_mapping_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_modifier_mapping_request_t), flags)
#define obj_type_xcb_set_modifier_mapping_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_modifier_mapping_request_t), flags)

#define obj_type_xcb_set_modifier_mapping_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_set_modifier_mapping_reply_t))
#define obj_type_xcb_set_modifier_mapping_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_set_modifier_mapping_reply_t))
#define obj_type_xcb_set_modifier_mapping_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_set_modifier_mapping_reply_t), flags)
#define obj_type_xcb_set_modifier_mapping_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_set_modifier_mapping_reply_t), flags)

#define obj_type_xcb_get_modifier_mapping_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_modifier_mapping_request_t))
#define obj_type_xcb_get_modifier_mapping_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_modifier_mapping_request_t))
#define obj_type_xcb_get_modifier_mapping_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_modifier_mapping_request_t), flags)
#define obj_type_xcb_get_modifier_mapping_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_modifier_mapping_request_t), flags)

#define obj_type_xcb_get_modifier_mapping_reply_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_get_modifier_mapping_reply_t))
#define obj_type_xcb_get_modifier_mapping_reply_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_get_modifier_mapping_reply_t))
#define obj_type_xcb_get_modifier_mapping_reply_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_get_modifier_mapping_reply_t), flags)
#define obj_type_xcb_get_modifier_mapping_reply_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_get_modifier_mapping_reply_t), flags)

#define obj_type_xcb_no_operation_request_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_no_operation_request_t))
#define obj_type_xcb_no_operation_request_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_no_operation_request_t))
#define obj_type_xcb_no_operation_request_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_no_operation_request_t), flags)
#define obj_type_xcb_no_operation_request_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_no_operation_request_t), flags)

#define obj_type_xcb_connection_t_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_xcb_connection_t))
#define obj_type_xcb_connection_t_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_xcb_connection_t))
#define obj_type_xcb_connection_t_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_xcb_connection_t), flags)
#define obj_type_xcb_connection_t_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_xcb_connection_t), flags)





/* method: visuals */
static int xcb_depth_t__visuals__meth(lua_State *L) {
  xcb_depth_t * this_idx1;
  int idx_idx2;
  xcb_visualtype_t * ret_idx1;
  xcb_visualtype_t * list_idx2;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_depth_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_depth_visuals(this_idx1);
  list_len_idx3 = xcb_depth_visuals_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = &(list_idx2[idx_idx2]);
    
  obj_type_xcb_visualtype_t_push(L, ret_idx1, 0);
  return 1;
}

/* method: visuals_length */
static int xcb_depth_t__visuals_length__meth(lua_State *L) {
  xcb_depth_t * this_idx1;
  int rc_xcb_depth_visuals_length_idx1 = 0;
  this_idx1 = obj_type_xcb_depth_t_check(L,1);
  rc_xcb_depth_visuals_length_idx1 = xcb_depth_visuals_length(this_idx1);
  lua_pushinteger(L, rc_xcb_depth_visuals_length_idx1);
  return 1;
}

/* method: allowed_depths_length */
static int xcb_screen_t__allowed_depths_length__meth(lua_State *L) {
  xcb_screen_t * this_idx1;
  int rc_xcb_screen_allowed_depths_length_idx1 = 0;
  this_idx1 = obj_type_xcb_screen_t_check(L,1);
  rc_xcb_screen_allowed_depths_length_idx1 = xcb_screen_allowed_depths_length(this_idx1);
  lua_pushinteger(L, rc_xcb_screen_allowed_depths_length_idx1);
  return 1;
}

/* method: authorization_protocol_name */
static int xcb_setup_request_t__authorization_protocol_name__meth(lua_State *L) {
  xcb_setup_request_t * this_idx1;
  char * rc_xcb_setup_request_authorization_protocol_name_idx1 = NULL;
  this_idx1 = obj_type_xcb_setup_request_t_check(L,1);
  rc_xcb_setup_request_authorization_protocol_name_idx1 = xcb_setup_request_authorization_protocol_name(this_idx1);
  lua_pushstring(L, rc_xcb_setup_request_authorization_protocol_name_idx1);
  return 1;
}

/* method: authorization_protocol_data */
static int xcb_setup_request_t__authorization_protocol_data__meth(lua_State *L) {
  xcb_setup_request_t * this_idx1;
  char * rc_xcb_setup_request_authorization_protocol_data_idx1 = NULL;
  this_idx1 = obj_type_xcb_setup_request_t_check(L,1);
  rc_xcb_setup_request_authorization_protocol_data_idx1 = xcb_setup_request_authorization_protocol_data(this_idx1);
  lua_pushstring(L, rc_xcb_setup_request_authorization_protocol_data_idx1);
  return 1;
}

/* method: reason */
static int xcb_setup_failed_t__reason__meth(lua_State *L) {
  xcb_setup_failed_t * this_idx1;
  char * rc_xcb_setup_failed_reason_idx1 = NULL;
  this_idx1 = obj_type_xcb_setup_failed_t_check(L,1);
  rc_xcb_setup_failed_reason_idx1 = xcb_setup_failed_reason(this_idx1);
  lua_pushstring(L, rc_xcb_setup_failed_reason_idx1);
  return 1;
}

/* method: reason */
static int xcb_setup_authenticate_t__reason__meth(lua_State *L) {
  xcb_setup_authenticate_t * this_idx1;
  char * rc_xcb_setup_authenticate_reason_idx1 = NULL;
  this_idx1 = obj_type_xcb_setup_authenticate_t_check(L,1);
  rc_xcb_setup_authenticate_reason_idx1 = xcb_setup_authenticate_reason(this_idx1);
  lua_pushstring(L, rc_xcb_setup_authenticate_reason_idx1);
  return 1;
}

/* method: vendor */
static int xcb_setup_t__vendor__meth(lua_State *L) {
  xcb_setup_t * this_idx1;
  char * rc_xcb_setup_vendor_idx1 = NULL;
  this_idx1 = obj_type_xcb_setup_t_check(L,1);
  rc_xcb_setup_vendor_idx1 = xcb_setup_vendor(this_idx1);
  lua_pushstring(L, rc_xcb_setup_vendor_idx1);
  return 1;
}

/* method: pixmap_formats */
static int xcb_setup_t__pixmap_formats__meth(lua_State *L) {
  xcb_setup_t * this_idx1;
  int idx_idx2;
  xcb_format_t * ret_idx1;
  xcb_format_t * list_idx2;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_setup_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_setup_pixmap_formats(this_idx1);
  list_len_idx3 = xcb_setup_pixmap_formats_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = &(list_idx2[idx_idx2]);
    
  obj_type_xcb_format_t_push(L, ret_idx1, 0);
  return 1;
}

/* method: pixmap_formats_length */
static int xcb_setup_t__pixmap_formats_length__meth(lua_State *L) {
  xcb_setup_t * this_idx1;
  int rc_xcb_setup_pixmap_formats_length_idx1 = 0;
  this_idx1 = obj_type_xcb_setup_t_check(L,1);
  rc_xcb_setup_pixmap_formats_length_idx1 = xcb_setup_pixmap_formats_length(this_idx1);
  lua_pushinteger(L, rc_xcb_setup_pixmap_formats_length_idx1);
  return 1;
}

/* method: roots_length */
static int xcb_setup_t__roots_length__meth(lua_State *L) {
  xcb_setup_t * this_idx1;
  int rc_xcb_setup_roots_length_idx1 = 0;
  this_idx1 = obj_type_xcb_setup_t_check(L,1);
  rc_xcb_setup_roots_length_idx1 = xcb_setup_roots_length(this_idx1);
  lua_pushinteger(L, rc_xcb_setup_roots_length_idx1);
  return 1;
}

/* method: children */
static int xcb_query_tree_reply_t__children__meth(lua_State *L) {
  xcb_query_tree_reply_t * this_idx1;
  int idx_idx2;
  xcb_window_t ret_idx1 = 0;
  xcb_window_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_query_tree_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_query_tree_children(this_idx1);
  list_len_idx3 = xcb_query_tree_children_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: children_length */
static int xcb_query_tree_reply_t__children_length__meth(lua_State *L) {
  xcb_query_tree_reply_t * this_idx1;
  int rc_xcb_query_tree_children_length_idx1 = 0;
  this_idx1 = obj_type_xcb_query_tree_reply_t_check(L,1);
  rc_xcb_query_tree_children_length_idx1 = xcb_query_tree_children_length(this_idx1);
  lua_pushinteger(L, rc_xcb_query_tree_children_length_idx1);
  return 1;
}

/* method: name */
static int xcb_get_atom_name_reply_t__name__meth(lua_State *L) {
  xcb_get_atom_name_reply_t * this_idx1;
  char * rc_xcb_get_atom_name_name_idx1 = NULL;
  this_idx1 = obj_type_xcb_get_atom_name_reply_t_check(L,1);
  rc_xcb_get_atom_name_name_idx1 = xcb_get_atom_name_name(this_idx1);
  lua_pushstring(L, rc_xcb_get_atom_name_name_idx1);
  return 1;
}

/* method: value */
static int xcb_get_property_reply_t__value__meth(lua_State *L) {
  xcb_get_property_reply_t * this_idx1;
  void * rc_xcb_get_property_value_idx1 = NULL;
  this_idx1 = obj_type_xcb_get_property_reply_t_check(L,1);
  rc_xcb_get_property_value_idx1 = xcb_get_property_value(this_idx1);
  lua_pushlightuserdata(L, rc_xcb_get_property_value_idx1);
  return 1;
}

/* method: atoms */
static int xcb_list_properties_reply_t__atoms__meth(lua_State *L) {
  xcb_list_properties_reply_t * this_idx1;
  int idx_idx2;
  xcb_atom_t ret_idx1 = 0;
  xcb_atom_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_list_properties_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_list_properties_atoms(this_idx1);
  list_len_idx3 = xcb_list_properties_atoms_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: atoms_length */
static int xcb_list_properties_reply_t__atoms_length__meth(lua_State *L) {
  xcb_list_properties_reply_t * this_idx1;
  int rc_xcb_list_properties_atoms_length_idx1 = 0;
  this_idx1 = obj_type_xcb_list_properties_reply_t_check(L,1);
  rc_xcb_list_properties_atoms_length_idx1 = xcb_list_properties_atoms_length(this_idx1);
  lua_pushinteger(L, rc_xcb_list_properties_atoms_length_idx1);
  return 1;
}

/* method: events */
static int xcb_get_motion_events_reply_t__events__meth(lua_State *L) {
  xcb_get_motion_events_reply_t * this_idx1;
  int idx_idx2;
  xcb_timecoord_t * ret_idx1;
  xcb_timecoord_t * list_idx2;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_get_motion_events_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_get_motion_events_events(this_idx1);
  list_len_idx3 = xcb_get_motion_events_events_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = &(list_idx2[idx_idx2]);
    
  obj_type_xcb_timecoord_t_push(L, ret_idx1, 0);
  return 1;
}

/* method: events_length */
static int xcb_get_motion_events_reply_t__events_length__meth(lua_State *L) {
  xcb_get_motion_events_reply_t * this_idx1;
  int rc_xcb_get_motion_events_events_length_idx1 = 0;
  this_idx1 = obj_type_xcb_get_motion_events_reply_t_check(L,1);
  rc_xcb_get_motion_events_events_length_idx1 = xcb_get_motion_events_events_length(this_idx1);
  lua_pushinteger(L, rc_xcb_get_motion_events_events_length_idx1);
  return 1;
}

/* method: properties */
static int xcb_query_font_reply_t__properties__meth(lua_State *L) {
  xcb_query_font_reply_t * this_idx1;
  int idx_idx2;
  xcb_fontprop_t * ret_idx1;
  xcb_fontprop_t * list_idx2;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_query_font_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_query_font_properties(this_idx1);
  list_len_idx3 = xcb_query_font_properties_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = &(list_idx2[idx_idx2]);
    
  obj_type_xcb_fontprop_t_push(L, ret_idx1, 0);
  return 1;
}

/* method: properties_length */
static int xcb_query_font_reply_t__properties_length__meth(lua_State *L) {
  xcb_query_font_reply_t * this_idx1;
  int rc_xcb_query_font_properties_length_idx1 = 0;
  this_idx1 = obj_type_xcb_query_font_reply_t_check(L,1);
  rc_xcb_query_font_properties_length_idx1 = xcb_query_font_properties_length(this_idx1);
  lua_pushinteger(L, rc_xcb_query_font_properties_length_idx1);
  return 1;
}

/* method: char_infos */
static int xcb_query_font_reply_t__char_infos__meth(lua_State *L) {
  xcb_query_font_reply_t * this_idx1;
  int idx_idx2;
  xcb_charinfo_t * ret_idx1;
  xcb_charinfo_t * list_idx2;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_query_font_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_query_font_char_infos(this_idx1);
  list_len_idx3 = xcb_query_font_char_infos_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = &(list_idx2[idx_idx2]);
    
  obj_type_xcb_charinfo_t_push(L, ret_idx1, 0);
  return 1;
}

/* method: char_infos_length */
static int xcb_query_font_reply_t__char_infos_length__meth(lua_State *L) {
  xcb_query_font_reply_t * this_idx1;
  int rc_xcb_query_font_char_infos_length_idx1 = 0;
  this_idx1 = obj_type_xcb_query_font_reply_t_check(L,1);
  rc_xcb_query_font_char_infos_length_idx1 = xcb_query_font_char_infos_length(this_idx1);
  lua_pushinteger(L, rc_xcb_query_font_char_infos_length_idx1);
  return 1;
}

/* method: name */
static int xcb_str_t__name__meth(lua_State *L) {
  xcb_str_t * this_idx1;
  char * rc_xcb_str_name_idx1 = NULL;
  this_idx1 = obj_type_xcb_str_t_check(L,1);
  rc_xcb_str_name_idx1 = xcb_str_name(this_idx1);
  lua_pushstring(L, rc_xcb_str_name_idx1);
  return 1;
}

/* method: names_length */
static int xcb_list_fonts_reply_t__names_length__meth(lua_State *L) {
  xcb_list_fonts_reply_t * this_idx1;
  int rc_xcb_list_fonts_names_length_idx1 = 0;
  this_idx1 = obj_type_xcb_list_fonts_reply_t_check(L,1);
  rc_xcb_list_fonts_names_length_idx1 = xcb_list_fonts_names_length(this_idx1);
  lua_pushinteger(L, rc_xcb_list_fonts_names_length_idx1);
  return 1;
}

/* method: properties */
static int xcb_list_fonts_with_info_reply_t__properties__meth(lua_State *L) {
  xcb_list_fonts_with_info_reply_t * this_idx1;
  int idx_idx2;
  xcb_fontprop_t * ret_idx1;
  xcb_fontprop_t * list_idx2;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_list_fonts_with_info_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_list_fonts_with_info_properties(this_idx1);
  list_len_idx3 = xcb_list_fonts_with_info_properties_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = &(list_idx2[idx_idx2]);
    
  obj_type_xcb_fontprop_t_push(L, ret_idx1, 0);
  return 1;
}

/* method: properties_length */
static int xcb_list_fonts_with_info_reply_t__properties_length__meth(lua_State *L) {
  xcb_list_fonts_with_info_reply_t * this_idx1;
  int rc_xcb_list_fonts_with_info_properties_length_idx1 = 0;
  this_idx1 = obj_type_xcb_list_fonts_with_info_reply_t_check(L,1);
  rc_xcb_list_fonts_with_info_properties_length_idx1 = xcb_list_fonts_with_info_properties_length(this_idx1);
  lua_pushinteger(L, rc_xcb_list_fonts_with_info_properties_length_idx1);
  return 1;
}

/* method: name */
static int xcb_list_fonts_with_info_reply_t__name__meth(lua_State *L) {
  xcb_list_fonts_with_info_reply_t * this_idx1;
  char * rc_xcb_list_fonts_with_info_name_idx1 = NULL;
  this_idx1 = obj_type_xcb_list_fonts_with_info_reply_t_check(L,1);
  rc_xcb_list_fonts_with_info_name_idx1 = xcb_list_fonts_with_info_name(this_idx1);
  lua_pushstring(L, rc_xcb_list_fonts_with_info_name_idx1);
  return 1;
}

/* method: path_length */
static int xcb_get_font_path_reply_t__path_length__meth(lua_State *L) {
  xcb_get_font_path_reply_t * this_idx1;
  int rc_xcb_get_font_path_path_length_idx1 = 0;
  this_idx1 = obj_type_xcb_get_font_path_reply_t_check(L,1);
  rc_xcb_get_font_path_path_length_idx1 = xcb_get_font_path_path_length(this_idx1);
  lua_pushinteger(L, rc_xcb_get_font_path_path_length_idx1);
  return 1;
}

/* method: data */
static int xcb_get_image_reply_t__data__meth(lua_State *L) {
  xcb_get_image_reply_t * this_idx1;
  int idx_idx2;
  uint8_t ret_idx1 = 0;
  uint8_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_get_image_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_get_image_data(this_idx1);
  list_len_idx3 = xcb_get_image_data_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: data_length */
static int xcb_get_image_reply_t__data_length__meth(lua_State *L) {
  xcb_get_image_reply_t * this_idx1;
  int rc_xcb_get_image_data_length_idx1 = 0;
  this_idx1 = obj_type_xcb_get_image_reply_t_check(L,1);
  rc_xcb_get_image_data_length_idx1 = xcb_get_image_data_length(this_idx1);
  lua_pushinteger(L, rc_xcb_get_image_data_length_idx1);
  return 1;
}

/* method: cmaps */
static int xcb_list_installed_colormaps_reply_t__cmaps__meth(lua_State *L) {
  xcb_list_installed_colormaps_reply_t * this_idx1;
  int idx_idx2;
  xcb_colormap_t ret_idx1 = 0;
  xcb_colormap_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_list_installed_colormaps_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_list_installed_colormaps_cmaps(this_idx1);
  list_len_idx3 = xcb_list_installed_colormaps_cmaps_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: cmaps_length */
static int xcb_list_installed_colormaps_reply_t__cmaps_length__meth(lua_State *L) {
  xcb_list_installed_colormaps_reply_t * this_idx1;
  int rc_xcb_list_installed_colormaps_cmaps_length_idx1 = 0;
  this_idx1 = obj_type_xcb_list_installed_colormaps_reply_t_check(L,1);
  rc_xcb_list_installed_colormaps_cmaps_length_idx1 = xcb_list_installed_colormaps_cmaps_length(this_idx1);
  lua_pushinteger(L, rc_xcb_list_installed_colormaps_cmaps_length_idx1);
  return 1;
}

/* method: pixels */
static int xcb_alloc_color_cells_reply_t__pixels__meth(lua_State *L) {
  xcb_alloc_color_cells_reply_t * this_idx1;
  int idx_idx2;
  uint32_t ret_idx1 = 0;
  uint32_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_alloc_color_cells_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_alloc_color_cells_pixels(this_idx1);
  list_len_idx3 = xcb_alloc_color_cells_pixels_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: pixels_length */
static int xcb_alloc_color_cells_reply_t__pixels_length__meth(lua_State *L) {
  xcb_alloc_color_cells_reply_t * this_idx1;
  int rc_xcb_alloc_color_cells_pixels_length_idx1 = 0;
  this_idx1 = obj_type_xcb_alloc_color_cells_reply_t_check(L,1);
  rc_xcb_alloc_color_cells_pixels_length_idx1 = xcb_alloc_color_cells_pixels_length(this_idx1);
  lua_pushinteger(L, rc_xcb_alloc_color_cells_pixels_length_idx1);
  return 1;
}

/* method: masks */
static int xcb_alloc_color_cells_reply_t__masks__meth(lua_State *L) {
  xcb_alloc_color_cells_reply_t * this_idx1;
  int idx_idx2;
  uint32_t ret_idx1 = 0;
  uint32_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_alloc_color_cells_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_alloc_color_cells_masks(this_idx1);
  list_len_idx3 = xcb_alloc_color_cells_masks_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: masks_length */
static int xcb_alloc_color_cells_reply_t__masks_length__meth(lua_State *L) {
  xcb_alloc_color_cells_reply_t * this_idx1;
  int rc_xcb_alloc_color_cells_masks_length_idx1 = 0;
  this_idx1 = obj_type_xcb_alloc_color_cells_reply_t_check(L,1);
  rc_xcb_alloc_color_cells_masks_length_idx1 = xcb_alloc_color_cells_masks_length(this_idx1);
  lua_pushinteger(L, rc_xcb_alloc_color_cells_masks_length_idx1);
  return 1;
}

/* method: pixels */
static int xcb_alloc_color_planes_reply_t__pixels__meth(lua_State *L) {
  xcb_alloc_color_planes_reply_t * this_idx1;
  int idx_idx2;
  uint32_t ret_idx1 = 0;
  uint32_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_alloc_color_planes_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_alloc_color_planes_pixels(this_idx1);
  list_len_idx3 = xcb_alloc_color_planes_pixels_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: pixels_length */
static int xcb_alloc_color_planes_reply_t__pixels_length__meth(lua_State *L) {
  xcb_alloc_color_planes_reply_t * this_idx1;
  int rc_xcb_alloc_color_planes_pixels_length_idx1 = 0;
  this_idx1 = obj_type_xcb_alloc_color_planes_reply_t_check(L,1);
  rc_xcb_alloc_color_planes_pixels_length_idx1 = xcb_alloc_color_planes_pixels_length(this_idx1);
  lua_pushinteger(L, rc_xcb_alloc_color_planes_pixels_length_idx1);
  return 1;
}

/* method: colors */
static int xcb_query_colors_reply_t__colors__meth(lua_State *L) {
  xcb_query_colors_reply_t * this_idx1;
  int idx_idx2;
  xcb_rgb_t * ret_idx1;
  xcb_rgb_t * list_idx2;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_query_colors_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_query_colors_colors(this_idx1);
  list_len_idx3 = xcb_query_colors_colors_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = &(list_idx2[idx_idx2]);
    
  obj_type_xcb_rgb_t_push(L, ret_idx1, 0);
  return 1;
}

/* method: colors_length */
static int xcb_query_colors_reply_t__colors_length__meth(lua_State *L) {
  xcb_query_colors_reply_t * this_idx1;
  int rc_xcb_query_colors_colors_length_idx1 = 0;
  this_idx1 = obj_type_xcb_query_colors_reply_t_check(L,1);
  rc_xcb_query_colors_colors_length_idx1 = xcb_query_colors_colors_length(this_idx1);
  lua_pushinteger(L, rc_xcb_query_colors_colors_length_idx1);
  return 1;
}

/* method: names_length */
static int xcb_list_extensions_reply_t__names_length__meth(lua_State *L) {
  xcb_list_extensions_reply_t * this_idx1;
  int rc_xcb_list_extensions_names_length_idx1 = 0;
  this_idx1 = obj_type_xcb_list_extensions_reply_t_check(L,1);
  rc_xcb_list_extensions_names_length_idx1 = xcb_list_extensions_names_length(this_idx1);
  lua_pushinteger(L, rc_xcb_list_extensions_names_length_idx1);
  return 1;
}

/* method: keysyms */
static int xcb_get_keyboard_mapping_reply_t__keysyms__meth(lua_State *L) {
  xcb_get_keyboard_mapping_reply_t * this_idx1;
  int idx_idx2;
  xcb_keysym_t ret_idx1 = 0;
  xcb_keysym_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_get_keyboard_mapping_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_get_keyboard_mapping_keysyms(this_idx1);
  list_len_idx3 = xcb_get_keyboard_mapping_keysyms_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: keysyms_length */
static int xcb_get_keyboard_mapping_reply_t__keysyms_length__meth(lua_State *L) {
  xcb_get_keyboard_mapping_reply_t * this_idx1;
  int rc_xcb_get_keyboard_mapping_keysyms_length_idx1 = 0;
  this_idx1 = obj_type_xcb_get_keyboard_mapping_reply_t_check(L,1);
  rc_xcb_get_keyboard_mapping_keysyms_length_idx1 = xcb_get_keyboard_mapping_keysyms_length(this_idx1);
  lua_pushinteger(L, rc_xcb_get_keyboard_mapping_keysyms_length_idx1);
  return 1;
}

/* method: address */
static int xcb_host_t__address__meth(lua_State *L) {
  xcb_host_t * this_idx1;
  int idx_idx2;
  uint8_t ret_idx1 = 0;
  uint8_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_host_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_host_address(this_idx1);
  list_len_idx3 = xcb_host_address_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: address_length */
static int xcb_host_t__address_length__meth(lua_State *L) {
  xcb_host_t * this_idx1;
  int rc_xcb_host_address_length_idx1 = 0;
  this_idx1 = obj_type_xcb_host_t_check(L,1);
  rc_xcb_host_address_length_idx1 = xcb_host_address_length(this_idx1);
  lua_pushinteger(L, rc_xcb_host_address_length_idx1);
  return 1;
}

/* method: hosts_length */
static int xcb_list_hosts_reply_t__hosts_length__meth(lua_State *L) {
  xcb_list_hosts_reply_t * this_idx1;
  int rc_xcb_list_hosts_hosts_length_idx1 = 0;
  this_idx1 = obj_type_xcb_list_hosts_reply_t_check(L,1);
  rc_xcb_list_hosts_hosts_length_idx1 = xcb_list_hosts_hosts_length(this_idx1);
  lua_pushinteger(L, rc_xcb_list_hosts_hosts_length_idx1);
  return 1;
}

/* method: map */
static int xcb_get_pointer_mapping_reply_t__map__meth(lua_State *L) {
  xcb_get_pointer_mapping_reply_t * this_idx1;
  int idx_idx2;
  uint8_t ret_idx1 = 0;
  uint8_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_get_pointer_mapping_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_get_pointer_mapping_map(this_idx1);
  list_len_idx3 = xcb_get_pointer_mapping_map_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: map_length */
static int xcb_get_pointer_mapping_reply_t__map_length__meth(lua_State *L) {
  xcb_get_pointer_mapping_reply_t * this_idx1;
  int rc_xcb_get_pointer_mapping_map_length_idx1 = 0;
  this_idx1 = obj_type_xcb_get_pointer_mapping_reply_t_check(L,1);
  rc_xcb_get_pointer_mapping_map_length_idx1 = xcb_get_pointer_mapping_map_length(this_idx1);
  lua_pushinteger(L, rc_xcb_get_pointer_mapping_map_length_idx1);
  return 1;
}

/* method: keycodes */
static int xcb_get_modifier_mapping_reply_t__keycodes__meth(lua_State *L) {
  xcb_get_modifier_mapping_reply_t * this_idx1;
  int idx_idx2;
  xcb_keycode_t ret_idx1 = 0;
  xcb_keycode_t * list_idx2 = NULL;
  int list_len_idx3 = 0;
  this_idx1 = obj_type_xcb_get_modifier_mapping_reply_t_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  list_idx2 = xcb_get_modifier_mapping_keycodes(this_idx1);
  list_len_idx3 = xcb_get_modifier_mapping_keycodes_length(this_idx1);
      if(idx_idx2 >= list_len_idx3) {
        return luaL_error(L, "Index out of bounds (%d >= %d)", idx_idx2, list_len_idx3);
      }
      ret_idx1 = list_idx2[idx_idx2];
    
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: keycodes_length */
static int xcb_get_modifier_mapping_reply_t__keycodes_length__meth(lua_State *L) {
  xcb_get_modifier_mapping_reply_t * this_idx1;
  int rc_xcb_get_modifier_mapping_keycodes_length_idx1 = 0;
  this_idx1 = obj_type_xcb_get_modifier_mapping_reply_t_check(L,1);
  rc_xcb_get_modifier_mapping_keycodes_length_idx1 = xcb_get_modifier_mapping_keycodes_length(this_idx1);
  lua_pushinteger(L, rc_xcb_get_modifier_mapping_keycodes_length_idx1);
  return 1;
}

/* method: connect */
static int xcb__connect__func(lua_State *L) {
  size_t displayname_len_idx1;
  const char * displayname_idx1;
  xcb_connection_t * rc_xcb_connect_idx1;
  int screenp_idx2 = 0;
  displayname_idx1 = luaL_checklstring(L,1,&(displayname_len_idx1));
  rc_xcb_connect_idx1 = xcb_connect(displayname_idx1, &(screenp_idx2));
  obj_type_xcb_connection_t_push(L, rc_xcb_connect_idx1, 0);
  lua_pushinteger(L, screenp_idx2);
  return 2;
}

/* method: disconnect */
static int xcb_connection_t__disconnect__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  xcb_connection_t * this_idx1;
  this_idx1 = obj_type_xcb_connection_t_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  xcb_disconnect(this_idx1);
  return 0;
}

/* method: flush */
static int xcb_connection_t__flush__meth(lua_State *L) {
  xcb_connection_t * this_idx1;
  this_idx1 = obj_type_xcb_connection_t_check(L,1);
  xcb_flush(this_idx1);
  return 0;
}

/* method: get_setup */
static int xcb_connection_t__get_setup__meth(lua_State *L) {
  xcb_connection_t * this_idx1;
  const xcb_setup_t * rc_xcb_get_setup_idx1;
  this_idx1 = obj_type_xcb_connection_t_check(L,1);
  rc_xcb_get_setup_idx1 = xcb_get_setup(this_idx1);
  obj_type_xcb_setup_t_push(L, rc_xcb_get_setup_idx1, 0);
  return 1;
}

/* method: generate_id */
static int xcb_connection_t__generate_id__meth(lua_State *L) {
  xcb_connection_t * this_idx1;
  uint32_t rc_xcb_generate_id_idx1 = 0;
  this_idx1 = obj_type_xcb_connection_t_check(L,1);
  rc_xcb_generate_id_idx1 = xcb_generate_id(this_idx1);
  lua_pushinteger(L, rc_xcb_generate_id_idx1);
  return 1;
}

/* method: get_file_descriptor */
static int xcb_connection_t__get_file_descriptor__meth(lua_State *L) {
  xcb_connection_t * this_idx1;
  int rc_xcb_get_file_descriptor_idx1 = 0;
  this_idx1 = obj_type_xcb_connection_t_check(L,1);
  rc_xcb_get_file_descriptor_idx1 = xcb_get_file_descriptor(this_idx1);
  lua_pushinteger(L, rc_xcb_get_file_descriptor_idx1);
  return 1;
}

/* method: has_error */
static int xcb_connection_t__has_error__meth(lua_State *L) {
  xcb_connection_t * this_idx1;
  bool rc_xcb_connection_has_error_idx1 = 0;
  this_idx1 = obj_type_xcb_connection_t_check(L,1);
  rc_xcb_connection_has_error_idx1 = xcb_connection_has_error(this_idx1);
  lua_pushboolean(L, rc_xcb_connection_has_error_idx1);
  return 1;
}



static const luaL_Reg obj_xcb_char2b_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_char2b_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_char2b_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_char2b_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_char2b_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_char2b_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_char2b_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_point_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_point_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_point_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_point_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_point_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_point_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_point_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_rectangle_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_rectangle_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_rectangle_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_rectangle_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_rectangle_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_rectangle_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_rectangle_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_arc_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_arc_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_arc_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_arc_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_arc_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_arc_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_arc_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_format_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_format_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_format_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_format_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_format_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_format_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_format_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_visualtype_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_visualtype_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_visualtype_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_visualtype_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_visualtype_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_visualtype_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_visualtype_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_depth_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_depth_t_methods[] = {
  {"visuals", xcb_depth_t__visuals__meth},
  {"visuals_length", xcb_depth_t__visuals_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_depth_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_depth_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_depth_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_depth_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_depth_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_screen_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_screen_t_methods[] = {
  {"allowed_depths_length", xcb_screen_t__allowed_depths_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_screen_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_screen_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_screen_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_screen_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_screen_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_request_t_methods[] = {
  {"authorization_protocol_name", xcb_setup_request_t__authorization_protocol_name__meth},
  {"authorization_protocol_data", xcb_setup_request_t__authorization_protocol_data__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_setup_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_setup_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_setup_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_setup_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_failed_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_failed_t_methods[] = {
  {"reason", xcb_setup_failed_t__reason__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_failed_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_setup_failed_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_setup_failed_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_setup_failed_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_setup_failed_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_authenticate_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_authenticate_t_methods[] = {
  {"reason", xcb_setup_authenticate_t__reason__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_authenticate_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_setup_authenticate_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_setup_authenticate_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_setup_authenticate_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_setup_authenticate_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_t_methods[] = {
  {"vendor", xcb_setup_t__vendor__meth},
  {"pixmap_formats", xcb_setup_t__pixmap_formats__meth},
  {"pixmap_formats_length", xcb_setup_t__pixmap_formats_length__meth},
  {"roots_length", xcb_setup_t__roots_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_setup_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_setup_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_setup_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_setup_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_setup_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_key_press_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_key_press_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_key_press_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_key_press_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_key_press_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_key_press_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_key_press_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_button_press_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_button_press_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_button_press_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_button_press_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_button_press_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_button_press_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_button_press_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_motion_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_motion_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_motion_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_motion_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_motion_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_motion_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_motion_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_enter_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_enter_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_enter_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_enter_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_enter_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_enter_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_enter_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_focus_in_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_focus_in_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_focus_in_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_focus_in_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_focus_in_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_focus_in_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_focus_in_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_keymap_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_keymap_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_keymap_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_keymap_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_keymap_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_keymap_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_keymap_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_expose_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_expose_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_expose_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_expose_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_expose_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_expose_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_expose_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_graphics_exposure_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_graphics_exposure_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_graphics_exposure_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_graphics_exposure_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_graphics_exposure_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_graphics_exposure_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_graphics_exposure_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_no_exposure_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_no_exposure_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_no_exposure_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_no_exposure_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_no_exposure_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_no_exposure_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_no_exposure_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_visibility_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_visibility_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_visibility_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_visibility_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_visibility_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_visibility_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_visibility_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_create_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_create_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_create_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_create_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_destroy_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_destroy_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_destroy_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_destroy_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_destroy_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_destroy_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_destroy_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_unmap_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_unmap_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_unmap_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_unmap_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_unmap_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_unmap_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_unmap_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_map_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_map_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_map_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_map_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_request_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_request_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_request_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_map_request_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_map_request_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_map_request_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_map_request_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_reparent_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_reparent_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_reparent_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_reparent_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_reparent_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_reparent_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_reparent_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_configure_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_configure_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_configure_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_configure_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_configure_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_configure_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_configure_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_configure_request_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_configure_request_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_configure_request_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_configure_request_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_configure_request_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_configure_request_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_configure_request_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_gravity_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_gravity_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_gravity_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_gravity_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_gravity_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_gravity_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_gravity_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_resize_request_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_resize_request_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_resize_request_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_resize_request_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_resize_request_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_resize_request_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_resize_request_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_circulate_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_circulate_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_circulate_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_circulate_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_circulate_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_circulate_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_circulate_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_property_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_property_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_property_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_property_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_property_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_property_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_property_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_selection_clear_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_selection_clear_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_selection_clear_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_selection_clear_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_selection_clear_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_selection_clear_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_selection_clear_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_selection_request_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_selection_request_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_selection_request_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_selection_request_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_selection_request_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_selection_request_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_selection_request_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_selection_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_selection_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_selection_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_selection_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_selection_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_selection_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_selection_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_colormap_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_colormap_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_colormap_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_colormap_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_colormap_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_colormap_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_colormap_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_client_message_data_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_client_message_data_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_client_message_data_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_client_message_data_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_client_message_data_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_client_message_data_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_client_message_data_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_client_message_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_client_message_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_client_message_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_client_message_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_client_message_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_client_message_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_client_message_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_mapping_notify_event_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_mapping_notify_event_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_mapping_notify_event_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_mapping_notify_event_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_mapping_notify_event_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_mapping_notify_event_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_mapping_notify_event_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_request_error_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_request_error_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_request_error_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_request_error_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_request_error_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_request_error_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_request_error_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_value_error_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_value_error_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_value_error_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_value_error_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_value_error_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_value_error_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_value_error_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_window_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_window_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_window_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_create_window_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_create_window_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_create_window_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_create_window_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_window_attributes_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_window_attributes_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_window_attributes_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_change_window_attributes_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_change_window_attributes_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_change_window_attributes_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_change_window_attributes_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_window_attributes_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_window_attributes_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_window_attributes_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_window_attributes_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_window_attributes_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_window_attributes_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_window_attributes_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_window_attributes_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_window_attributes_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_window_attributes_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_window_attributes_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_window_attributes_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_window_attributes_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_window_attributes_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_destroy_window_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_destroy_window_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_destroy_window_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_destroy_window_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_destroy_window_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_destroy_window_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_destroy_window_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_destroy_subwindows_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_destroy_subwindows_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_destroy_subwindows_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_destroy_subwindows_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_destroy_subwindows_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_destroy_subwindows_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_destroy_subwindows_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_save_set_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_save_set_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_save_set_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_change_save_set_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_change_save_set_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_change_save_set_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_change_save_set_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_reparent_window_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_reparent_window_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_reparent_window_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_reparent_window_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_reparent_window_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_reparent_window_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_reparent_window_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_window_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_window_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_window_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_map_window_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_map_window_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_map_window_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_map_window_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_subwindows_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_subwindows_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_map_subwindows_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_map_subwindows_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_map_subwindows_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_map_subwindows_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_map_subwindows_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_unmap_window_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_unmap_window_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_unmap_window_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_unmap_window_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_unmap_window_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_unmap_window_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_unmap_window_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_unmap_subwindows_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_unmap_subwindows_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_unmap_subwindows_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_unmap_subwindows_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_unmap_subwindows_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_unmap_subwindows_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_unmap_subwindows_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_configure_window_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_configure_window_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_configure_window_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_configure_window_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_configure_window_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_configure_window_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_configure_window_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_circulate_window_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_circulate_window_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_circulate_window_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_circulate_window_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_circulate_window_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_circulate_window_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_circulate_window_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_geometry_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_geometry_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_geometry_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_geometry_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_geometry_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_geometry_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_geometry_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_geometry_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_geometry_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_geometry_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_geometry_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_geometry_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_geometry_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_geometry_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_tree_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_tree_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_tree_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_tree_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_tree_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_tree_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_tree_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_tree_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_tree_reply_t_methods[] = {
  {"children", xcb_query_tree_reply_t__children__meth},
  {"children_length", xcb_query_tree_reply_t__children_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_tree_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_tree_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_tree_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_tree_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_tree_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_intern_atom_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_intern_atom_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_intern_atom_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_intern_atom_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_intern_atom_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_intern_atom_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_intern_atom_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_intern_atom_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_intern_atom_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_intern_atom_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_intern_atom_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_intern_atom_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_intern_atom_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_intern_atom_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_atom_name_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_atom_name_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_atom_name_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_atom_name_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_atom_name_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_atom_name_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_atom_name_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_atom_name_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_atom_name_reply_t_methods[] = {
  {"name", xcb_get_atom_name_reply_t__name__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_atom_name_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_atom_name_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_atom_name_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_atom_name_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_atom_name_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_property_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_property_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_property_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_change_property_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_change_property_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_change_property_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_change_property_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_delete_property_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_delete_property_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_delete_property_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_delete_property_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_delete_property_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_delete_property_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_delete_property_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_property_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_property_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_property_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_property_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_property_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_property_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_property_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_property_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_property_reply_t_methods[] = {
  {"value", xcb_get_property_reply_t__value__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_property_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_property_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_property_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_property_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_property_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_properties_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_properties_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_properties_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_properties_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_properties_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_properties_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_properties_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_properties_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_properties_reply_t_methods[] = {
  {"atoms", xcb_list_properties_reply_t__atoms__meth},
  {"atoms_length", xcb_list_properties_reply_t__atoms_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_properties_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_properties_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_properties_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_properties_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_properties_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_selection_owner_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_selection_owner_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_selection_owner_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_selection_owner_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_selection_owner_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_selection_owner_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_selection_owner_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_selection_owner_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_selection_owner_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_selection_owner_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_selection_owner_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_selection_owner_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_selection_owner_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_selection_owner_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_selection_owner_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_selection_owner_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_selection_owner_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_selection_owner_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_selection_owner_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_selection_owner_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_selection_owner_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_convert_selection_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_convert_selection_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_convert_selection_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_convert_selection_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_convert_selection_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_convert_selection_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_convert_selection_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_send_event_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_send_event_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_send_event_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_send_event_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_send_event_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_send_event_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_send_event_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_pointer_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_pointer_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_pointer_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_grab_pointer_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_grab_pointer_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_grab_pointer_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_grab_pointer_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_pointer_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_pointer_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_pointer_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_grab_pointer_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_grab_pointer_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_grab_pointer_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_grab_pointer_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_pointer_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_pointer_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_pointer_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_ungrab_pointer_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_ungrab_pointer_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_ungrab_pointer_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_ungrab_pointer_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_button_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_button_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_button_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_grab_button_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_grab_button_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_grab_button_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_grab_button_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_button_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_button_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_button_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_ungrab_button_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_ungrab_button_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_ungrab_button_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_ungrab_button_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_active_pointer_grab_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_active_pointer_grab_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_active_pointer_grab_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_change_active_pointer_grab_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_change_active_pointer_grab_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_change_active_pointer_grab_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_change_active_pointer_grab_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_keyboard_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_keyboard_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_keyboard_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_grab_keyboard_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_grab_keyboard_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_grab_keyboard_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_grab_keyboard_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_keyboard_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_keyboard_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_keyboard_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_grab_keyboard_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_grab_keyboard_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_grab_keyboard_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_grab_keyboard_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_keyboard_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_keyboard_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_keyboard_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_ungrab_keyboard_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_ungrab_keyboard_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_ungrab_keyboard_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_ungrab_keyboard_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_key_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_key_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_key_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_grab_key_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_grab_key_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_grab_key_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_grab_key_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_key_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_key_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_key_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_ungrab_key_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_ungrab_key_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_ungrab_key_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_ungrab_key_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_allow_events_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_allow_events_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_allow_events_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_allow_events_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_allow_events_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_allow_events_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_allow_events_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_server_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_server_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_grab_server_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_grab_server_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_grab_server_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_grab_server_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_grab_server_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_server_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_server_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_ungrab_server_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_ungrab_server_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_ungrab_server_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_ungrab_server_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_ungrab_server_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_pointer_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_pointer_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_pointer_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_pointer_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_pointer_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_pointer_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_pointer_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_pointer_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_pointer_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_pointer_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_pointer_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_pointer_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_pointer_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_pointer_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_timecoord_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_timecoord_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_timecoord_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_timecoord_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_timecoord_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_timecoord_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_timecoord_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_motion_events_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_motion_events_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_motion_events_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_motion_events_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_motion_events_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_motion_events_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_motion_events_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_motion_events_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_motion_events_reply_t_methods[] = {
  {"events", xcb_get_motion_events_reply_t__events__meth},
  {"events_length", xcb_get_motion_events_reply_t__events_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_motion_events_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_motion_events_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_motion_events_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_motion_events_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_motion_events_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_translate_coordinates_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_translate_coordinates_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_translate_coordinates_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_translate_coordinates_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_translate_coordinates_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_translate_coordinates_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_translate_coordinates_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_translate_coordinates_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_translate_coordinates_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_translate_coordinates_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_translate_coordinates_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_translate_coordinates_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_translate_coordinates_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_translate_coordinates_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_warp_pointer_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_warp_pointer_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_warp_pointer_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_warp_pointer_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_warp_pointer_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_warp_pointer_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_warp_pointer_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_input_focus_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_input_focus_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_input_focus_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_input_focus_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_input_focus_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_input_focus_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_input_focus_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_input_focus_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_input_focus_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_input_focus_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_input_focus_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_input_focus_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_input_focus_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_input_focus_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_input_focus_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_input_focus_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_input_focus_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_input_focus_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_input_focus_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_input_focus_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_input_focus_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_keymap_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_keymap_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_keymap_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_keymap_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_keymap_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_keymap_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_keymap_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_keymap_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_keymap_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_keymap_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_keymap_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_keymap_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_keymap_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_keymap_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_open_font_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_open_font_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_open_font_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_open_font_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_open_font_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_open_font_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_open_font_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_close_font_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_close_font_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_close_font_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_close_font_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_close_font_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_close_font_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_close_font_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_fontprop_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_fontprop_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_fontprop_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_fontprop_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_fontprop_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_fontprop_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_fontprop_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_charinfo_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_charinfo_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_charinfo_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_charinfo_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_charinfo_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_charinfo_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_charinfo_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_font_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_font_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_font_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_font_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_font_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_font_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_font_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_font_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_font_reply_t_methods[] = {
  {"properties", xcb_query_font_reply_t__properties__meth},
  {"properties_length", xcb_query_font_reply_t__properties_length__meth},
  {"char_infos", xcb_query_font_reply_t__char_infos__meth},
  {"char_infos_length", xcb_query_font_reply_t__char_infos_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_font_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_font_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_font_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_font_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_font_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_text_extents_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_text_extents_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_text_extents_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_text_extents_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_text_extents_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_text_extents_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_text_extents_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_text_extents_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_text_extents_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_text_extents_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_text_extents_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_text_extents_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_text_extents_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_text_extents_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_str_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_str_t_methods[] = {
  {"name", xcb_str_t__name__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_str_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_str_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_str_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_str_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_str_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_fonts_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_fonts_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_fonts_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_fonts_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_reply_t_methods[] = {
  {"names_length", xcb_list_fonts_reply_t__names_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_fonts_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_fonts_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_fonts_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_fonts_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_with_info_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_with_info_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_with_info_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_fonts_with_info_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_fonts_with_info_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_fonts_with_info_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_fonts_with_info_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_with_info_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_with_info_reply_t_methods[] = {
  {"properties", xcb_list_fonts_with_info_reply_t__properties__meth},
  {"properties_length", xcb_list_fonts_with_info_reply_t__properties_length__meth},
  {"name", xcb_list_fonts_with_info_reply_t__name__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_fonts_with_info_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_fonts_with_info_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_fonts_with_info_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_fonts_with_info_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_fonts_with_info_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_font_path_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_font_path_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_font_path_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_font_path_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_font_path_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_font_path_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_font_path_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_font_path_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_font_path_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_font_path_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_font_path_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_font_path_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_font_path_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_font_path_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_font_path_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_font_path_reply_t_methods[] = {
  {"path_length", xcb_get_font_path_reply_t__path_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_font_path_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_font_path_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_font_path_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_font_path_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_font_path_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_pixmap_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_pixmap_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_pixmap_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_create_pixmap_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_create_pixmap_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_create_pixmap_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_create_pixmap_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_pixmap_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_pixmap_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_pixmap_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_free_pixmap_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_free_pixmap_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_free_pixmap_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_free_pixmap_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_gc_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_gc_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_gc_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_create_gc_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_create_gc_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_create_gc_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_create_gc_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_gc_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_gc_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_gc_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_change_gc_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_change_gc_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_change_gc_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_change_gc_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_gc_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_gc_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_gc_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_copy_gc_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_copy_gc_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_copy_gc_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_copy_gc_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_dashes_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_dashes_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_dashes_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_dashes_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_dashes_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_dashes_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_dashes_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_clip_rectangles_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_clip_rectangles_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_clip_rectangles_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_clip_rectangles_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_clip_rectangles_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_clip_rectangles_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_clip_rectangles_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_gc_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_gc_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_gc_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_free_gc_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_free_gc_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_free_gc_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_free_gc_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_clear_area_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_clear_area_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_clear_area_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_clear_area_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_clear_area_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_clear_area_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_clear_area_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_area_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_area_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_area_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_copy_area_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_copy_area_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_copy_area_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_copy_area_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_plane_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_plane_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_plane_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_copy_plane_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_copy_plane_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_copy_plane_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_copy_plane_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_point_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_point_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_point_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_poly_point_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_poly_point_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_poly_point_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_poly_point_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_line_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_line_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_line_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_poly_line_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_poly_line_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_poly_line_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_poly_line_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_segment_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_segment_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_segment_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_segment_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_segment_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_segment_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_segment_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_segment_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_segment_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_segment_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_poly_segment_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_poly_segment_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_poly_segment_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_poly_segment_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_rectangle_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_rectangle_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_rectangle_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_poly_rectangle_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_poly_rectangle_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_poly_rectangle_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_poly_rectangle_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_arc_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_arc_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_arc_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_poly_arc_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_poly_arc_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_poly_arc_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_poly_arc_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_fill_poly_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_fill_poly_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_fill_poly_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_fill_poly_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_fill_poly_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_fill_poly_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_fill_poly_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_fill_rectangle_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_fill_rectangle_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_fill_rectangle_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_poly_fill_rectangle_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_poly_fill_rectangle_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_poly_fill_rectangle_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_poly_fill_rectangle_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_fill_arc_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_fill_arc_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_fill_arc_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_poly_fill_arc_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_poly_fill_arc_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_poly_fill_arc_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_poly_fill_arc_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_put_image_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_put_image_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_put_image_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_put_image_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_put_image_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_put_image_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_put_image_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_image_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_image_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_image_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_image_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_image_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_image_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_image_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_image_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_image_reply_t_methods[] = {
  {"data", xcb_get_image_reply_t__data__meth},
  {"data_length", xcb_get_image_reply_t__data_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_image_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_image_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_image_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_image_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_image_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_text_8_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_text_8_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_text_8_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_poly_text_8_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_poly_text_8_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_poly_text_8_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_poly_text_8_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_text_16_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_text_16_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_poly_text_16_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_poly_text_16_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_poly_text_16_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_poly_text_16_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_poly_text_16_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_image_text_8_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_image_text_8_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_image_text_8_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_image_text_8_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_image_text_8_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_image_text_8_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_image_text_8_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_image_text_16_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_image_text_16_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_image_text_16_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_image_text_16_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_image_text_16_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_image_text_16_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_image_text_16_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_colormap_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_colormap_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_colormap_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_create_colormap_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_create_colormap_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_create_colormap_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_create_colormap_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_colormap_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_colormap_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_colormap_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_free_colormap_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_free_colormap_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_free_colormap_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_free_colormap_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_colormap_and_free_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_colormap_and_free_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_copy_colormap_and_free_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_copy_colormap_and_free_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_copy_colormap_and_free_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_copy_colormap_and_free_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_copy_colormap_and_free_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_install_colormap_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_install_colormap_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_install_colormap_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_install_colormap_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_install_colormap_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_install_colormap_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_install_colormap_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_uninstall_colormap_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_uninstall_colormap_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_uninstall_colormap_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_uninstall_colormap_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_uninstall_colormap_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_uninstall_colormap_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_uninstall_colormap_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_installed_colormaps_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_installed_colormaps_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_installed_colormaps_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_installed_colormaps_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_installed_colormaps_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_installed_colormaps_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_installed_colormaps_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_installed_colormaps_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_installed_colormaps_reply_t_methods[] = {
  {"cmaps", xcb_list_installed_colormaps_reply_t__cmaps__meth},
  {"cmaps_length", xcb_list_installed_colormaps_reply_t__cmaps_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_installed_colormaps_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_installed_colormaps_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_installed_colormaps_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_installed_colormaps_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_installed_colormaps_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_alloc_color_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_alloc_color_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_alloc_color_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_alloc_color_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_alloc_color_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_alloc_color_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_alloc_color_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_alloc_color_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_named_color_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_named_color_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_named_color_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_alloc_named_color_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_alloc_named_color_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_alloc_named_color_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_alloc_named_color_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_named_color_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_named_color_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_named_color_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_alloc_named_color_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_alloc_named_color_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_alloc_named_color_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_alloc_named_color_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_cells_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_cells_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_cells_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_alloc_color_cells_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_alloc_color_cells_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_alloc_color_cells_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_alloc_color_cells_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_cells_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_cells_reply_t_methods[] = {
  {"pixels", xcb_alloc_color_cells_reply_t__pixels__meth},
  {"pixels_length", xcb_alloc_color_cells_reply_t__pixels_length__meth},
  {"masks", xcb_alloc_color_cells_reply_t__masks__meth},
  {"masks_length", xcb_alloc_color_cells_reply_t__masks_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_cells_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_alloc_color_cells_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_alloc_color_cells_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_alloc_color_cells_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_alloc_color_cells_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_planes_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_planes_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_planes_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_alloc_color_planes_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_alloc_color_planes_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_alloc_color_planes_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_alloc_color_planes_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_planes_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_planes_reply_t_methods[] = {
  {"pixels", xcb_alloc_color_planes_reply_t__pixels__meth},
  {"pixels_length", xcb_alloc_color_planes_reply_t__pixels_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_alloc_color_planes_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_alloc_color_planes_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_alloc_color_planes_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_alloc_color_planes_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_alloc_color_planes_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_colors_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_colors_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_colors_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_free_colors_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_free_colors_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_free_colors_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_free_colors_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_coloritem_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_coloritem_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_coloritem_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_coloritem_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_coloritem_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_coloritem_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_coloritem_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_store_colors_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_store_colors_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_store_colors_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_store_colors_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_store_colors_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_store_colors_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_store_colors_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_store_named_color_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_store_named_color_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_store_named_color_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_store_named_color_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_store_named_color_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_store_named_color_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_store_named_color_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_rgb_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_rgb_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_rgb_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_rgb_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_rgb_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_rgb_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_rgb_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_colors_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_colors_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_colors_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_colors_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_colors_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_colors_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_colors_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_colors_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_colors_reply_t_methods[] = {
  {"colors", xcb_query_colors_reply_t__colors__meth},
  {"colors_length", xcb_query_colors_reply_t__colors_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_colors_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_colors_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_colors_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_colors_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_colors_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_lookup_color_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_lookup_color_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_lookup_color_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_lookup_color_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_lookup_color_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_lookup_color_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_lookup_color_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_lookup_color_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_lookup_color_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_lookup_color_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_lookup_color_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_lookup_color_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_lookup_color_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_lookup_color_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_cursor_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_cursor_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_cursor_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_create_cursor_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_create_cursor_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_create_cursor_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_create_cursor_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_glyph_cursor_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_glyph_cursor_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_create_glyph_cursor_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_create_glyph_cursor_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_create_glyph_cursor_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_create_glyph_cursor_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_create_glyph_cursor_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_cursor_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_cursor_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_free_cursor_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_free_cursor_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_free_cursor_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_free_cursor_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_free_cursor_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_recolor_cursor_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_recolor_cursor_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_recolor_cursor_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_recolor_cursor_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_recolor_cursor_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_recolor_cursor_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_recolor_cursor_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_best_size_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_best_size_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_best_size_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_best_size_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_best_size_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_best_size_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_best_size_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_best_size_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_best_size_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_best_size_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_best_size_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_best_size_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_best_size_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_best_size_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_extension_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_extension_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_extension_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_extension_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_extension_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_extension_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_extension_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_extension_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_extension_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_query_extension_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_query_extension_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_query_extension_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_query_extension_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_query_extension_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_extensions_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_extensions_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_extensions_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_extensions_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_extensions_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_extensions_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_extensions_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_extensions_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_extensions_reply_t_methods[] = {
  {"names_length", xcb_list_extensions_reply_t__names_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_extensions_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_extensions_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_extensions_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_extensions_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_extensions_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_keyboard_mapping_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_keyboard_mapping_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_keyboard_mapping_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_change_keyboard_mapping_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_change_keyboard_mapping_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_change_keyboard_mapping_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_change_keyboard_mapping_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_mapping_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_mapping_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_mapping_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_keyboard_mapping_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_keyboard_mapping_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_keyboard_mapping_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_keyboard_mapping_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_mapping_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_mapping_reply_t_methods[] = {
  {"keysyms", xcb_get_keyboard_mapping_reply_t__keysyms__meth},
  {"keysyms_length", xcb_get_keyboard_mapping_reply_t__keysyms_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_mapping_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_keyboard_mapping_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_keyboard_mapping_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_keyboard_mapping_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_keyboard_mapping_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_keyboard_control_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_keyboard_control_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_keyboard_control_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_change_keyboard_control_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_change_keyboard_control_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_change_keyboard_control_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_change_keyboard_control_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_control_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_control_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_control_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_keyboard_control_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_keyboard_control_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_keyboard_control_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_keyboard_control_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_control_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_control_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_keyboard_control_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_keyboard_control_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_keyboard_control_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_keyboard_control_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_keyboard_control_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_bell_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_bell_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_bell_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_bell_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_bell_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_bell_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_bell_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_pointer_control_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_pointer_control_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_pointer_control_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_change_pointer_control_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_change_pointer_control_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_change_pointer_control_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_change_pointer_control_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_control_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_control_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_control_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_pointer_control_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_pointer_control_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_pointer_control_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_pointer_control_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_control_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_control_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_control_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_pointer_control_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_pointer_control_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_pointer_control_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_pointer_control_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_screen_saver_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_screen_saver_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_screen_saver_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_screen_saver_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_screen_saver_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_screen_saver_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_screen_saver_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_screen_saver_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_screen_saver_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_screen_saver_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_screen_saver_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_screen_saver_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_screen_saver_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_screen_saver_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_screen_saver_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_screen_saver_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_screen_saver_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_screen_saver_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_screen_saver_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_screen_saver_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_screen_saver_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_hosts_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_hosts_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_change_hosts_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_change_hosts_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_change_hosts_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_change_hosts_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_change_hosts_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_host_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_host_t_methods[] = {
  {"address", xcb_host_t__address__meth},
  {"address_length", xcb_host_t__address_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_host_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_host_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_host_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_host_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_host_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_hosts_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_hosts_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_hosts_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_hosts_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_hosts_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_hosts_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_hosts_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_hosts_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_hosts_reply_t_methods[] = {
  {"hosts_length", xcb_list_hosts_reply_t__hosts_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_list_hosts_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_list_hosts_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_list_hosts_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_list_hosts_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_list_hosts_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_access_control_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_access_control_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_access_control_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_access_control_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_access_control_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_access_control_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_access_control_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_close_down_mode_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_close_down_mode_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_close_down_mode_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_close_down_mode_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_close_down_mode_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_close_down_mode_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_close_down_mode_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_kill_client_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_kill_client_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_kill_client_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_kill_client_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_kill_client_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_kill_client_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_kill_client_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_rotate_properties_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_rotate_properties_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_rotate_properties_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_rotate_properties_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_rotate_properties_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_rotate_properties_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_rotate_properties_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_force_screen_saver_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_force_screen_saver_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_force_screen_saver_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_force_screen_saver_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_force_screen_saver_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_force_screen_saver_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_force_screen_saver_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_pointer_mapping_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_pointer_mapping_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_pointer_mapping_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_pointer_mapping_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_pointer_mapping_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_pointer_mapping_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_pointer_mapping_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_pointer_mapping_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_pointer_mapping_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_pointer_mapping_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_pointer_mapping_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_pointer_mapping_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_pointer_mapping_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_pointer_mapping_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_mapping_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_mapping_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_mapping_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_pointer_mapping_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_pointer_mapping_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_pointer_mapping_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_pointer_mapping_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_mapping_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_mapping_reply_t_methods[] = {
  {"map", xcb_get_pointer_mapping_reply_t__map__meth},
  {"map_length", xcb_get_pointer_mapping_reply_t__map_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_pointer_mapping_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_pointer_mapping_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_pointer_mapping_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_pointer_mapping_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_pointer_mapping_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_modifier_mapping_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_modifier_mapping_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_modifier_mapping_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_modifier_mapping_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_modifier_mapping_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_modifier_mapping_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_modifier_mapping_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_modifier_mapping_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_modifier_mapping_reply_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_set_modifier_mapping_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_set_modifier_mapping_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_set_modifier_mapping_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_set_modifier_mapping_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_set_modifier_mapping_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_modifier_mapping_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_modifier_mapping_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_modifier_mapping_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_modifier_mapping_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_modifier_mapping_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_modifier_mapping_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_modifier_mapping_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_modifier_mapping_reply_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_modifier_mapping_reply_t_methods[] = {
  {"keycodes", xcb_get_modifier_mapping_reply_t__keycodes__meth},
  {"keycodes_length", xcb_get_modifier_mapping_reply_t__keycodes_length__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_get_modifier_mapping_reply_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_get_modifier_mapping_reply_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_get_modifier_mapping_reply_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_get_modifier_mapping_reply_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_get_modifier_mapping_reply_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_no_operation_request_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_no_operation_request_t_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_no_operation_request_t_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_no_operation_request_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_no_operation_request_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_no_operation_request_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_no_operation_request_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_connection_t_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_connection_t_methods[] = {
  {"disconnect", xcb_connection_t__disconnect__meth},
  {"flush", xcb_connection_t__flush__meth},
  {"get_setup", xcb_connection_t__get_setup__meth},
  {"generate_id", xcb_connection_t__generate_id__meth},
  {"get_file_descriptor", xcb_connection_t__get_file_descriptor__meth},
  {"has_error", xcb_connection_t__has_error__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_xcb_connection_t_metas[] = {
  {"__gc", xcb_connection_t__disconnect__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_xcb_connection_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_xcb_connection_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_xcb_connection_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_xcb_connection_t_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg xcb_function[] = {
  {"connect", xcb__connect__func},
  {NULL, NULL}
};

static const obj_const xcb_constants[] = {
  {NULL, NULL, 0.0 , 0}
};



static const reg_sub_module reg_sub_modules[] = {
  { &(obj_type_xcb_char2b_t), REG_OBJECT, obj_xcb_char2b_t_pub_funcs, obj_xcb_char2b_t_methods, obj_xcb_char2b_t_metas, obj_xcb_char2b_t_bases, obj_xcb_char2b_t_fields, obj_xcb_char2b_t_constants, obj_xcb_char2b_t_implements, 0},
  { &(obj_type_xcb_point_t), REG_OBJECT, obj_xcb_point_t_pub_funcs, obj_xcb_point_t_methods, obj_xcb_point_t_metas, obj_xcb_point_t_bases, obj_xcb_point_t_fields, obj_xcb_point_t_constants, obj_xcb_point_t_implements, 0},
  { &(obj_type_xcb_rectangle_t), REG_OBJECT, obj_xcb_rectangle_t_pub_funcs, obj_xcb_rectangle_t_methods, obj_xcb_rectangle_t_metas, obj_xcb_rectangle_t_bases, obj_xcb_rectangle_t_fields, obj_xcb_rectangle_t_constants, obj_xcb_rectangle_t_implements, 0},
  { &(obj_type_xcb_arc_t), REG_OBJECT, obj_xcb_arc_t_pub_funcs, obj_xcb_arc_t_methods, obj_xcb_arc_t_metas, obj_xcb_arc_t_bases, obj_xcb_arc_t_fields, obj_xcb_arc_t_constants, obj_xcb_arc_t_implements, 0},
  { &(obj_type_xcb_format_t), REG_OBJECT, obj_xcb_format_t_pub_funcs, obj_xcb_format_t_methods, obj_xcb_format_t_metas, obj_xcb_format_t_bases, obj_xcb_format_t_fields, obj_xcb_format_t_constants, obj_xcb_format_t_implements, 0},
  { &(obj_type_xcb_visualtype_t), REG_OBJECT, obj_xcb_visualtype_t_pub_funcs, obj_xcb_visualtype_t_methods, obj_xcb_visualtype_t_metas, obj_xcb_visualtype_t_bases, obj_xcb_visualtype_t_fields, obj_xcb_visualtype_t_constants, obj_xcb_visualtype_t_implements, 0},
  { &(obj_type_xcb_depth_t), REG_OBJECT, obj_xcb_depth_t_pub_funcs, obj_xcb_depth_t_methods, obj_xcb_depth_t_metas, obj_xcb_depth_t_bases, obj_xcb_depth_t_fields, obj_xcb_depth_t_constants, obj_xcb_depth_t_implements, 0},
  { &(obj_type_xcb_screen_t), REG_OBJECT, obj_xcb_screen_t_pub_funcs, obj_xcb_screen_t_methods, obj_xcb_screen_t_metas, obj_xcb_screen_t_bases, obj_xcb_screen_t_fields, obj_xcb_screen_t_constants, obj_xcb_screen_t_implements, 0},
  { &(obj_type_xcb_setup_request_t), REG_OBJECT, obj_xcb_setup_request_t_pub_funcs, obj_xcb_setup_request_t_methods, obj_xcb_setup_request_t_metas, obj_xcb_setup_request_t_bases, obj_xcb_setup_request_t_fields, obj_xcb_setup_request_t_constants, obj_xcb_setup_request_t_implements, 0},
  { &(obj_type_xcb_setup_failed_t), REG_OBJECT, obj_xcb_setup_failed_t_pub_funcs, obj_xcb_setup_failed_t_methods, obj_xcb_setup_failed_t_metas, obj_xcb_setup_failed_t_bases, obj_xcb_setup_failed_t_fields, obj_xcb_setup_failed_t_constants, obj_xcb_setup_failed_t_implements, 0},
  { &(obj_type_xcb_setup_authenticate_t), REG_OBJECT, obj_xcb_setup_authenticate_t_pub_funcs, obj_xcb_setup_authenticate_t_methods, obj_xcb_setup_authenticate_t_metas, obj_xcb_setup_authenticate_t_bases, obj_xcb_setup_authenticate_t_fields, obj_xcb_setup_authenticate_t_constants, obj_xcb_setup_authenticate_t_implements, 0},
  { &(obj_type_xcb_setup_t), REG_OBJECT, obj_xcb_setup_t_pub_funcs, obj_xcb_setup_t_methods, obj_xcb_setup_t_metas, obj_xcb_setup_t_bases, obj_xcb_setup_t_fields, obj_xcb_setup_t_constants, obj_xcb_setup_t_implements, 0},
  { &(obj_type_xcb_key_press_event_t), REG_OBJECT, obj_xcb_key_press_event_t_pub_funcs, obj_xcb_key_press_event_t_methods, obj_xcb_key_press_event_t_metas, obj_xcb_key_press_event_t_bases, obj_xcb_key_press_event_t_fields, obj_xcb_key_press_event_t_constants, obj_xcb_key_press_event_t_implements, 0},
  { &(obj_type_xcb_button_press_event_t), REG_OBJECT, obj_xcb_button_press_event_t_pub_funcs, obj_xcb_button_press_event_t_methods, obj_xcb_button_press_event_t_metas, obj_xcb_button_press_event_t_bases, obj_xcb_button_press_event_t_fields, obj_xcb_button_press_event_t_constants, obj_xcb_button_press_event_t_implements, 0},
  { &(obj_type_xcb_motion_notify_event_t), REG_OBJECT, obj_xcb_motion_notify_event_t_pub_funcs, obj_xcb_motion_notify_event_t_methods, obj_xcb_motion_notify_event_t_metas, obj_xcb_motion_notify_event_t_bases, obj_xcb_motion_notify_event_t_fields, obj_xcb_motion_notify_event_t_constants, obj_xcb_motion_notify_event_t_implements, 0},
  { &(obj_type_xcb_enter_notify_event_t), REG_OBJECT, obj_xcb_enter_notify_event_t_pub_funcs, obj_xcb_enter_notify_event_t_methods, obj_xcb_enter_notify_event_t_metas, obj_xcb_enter_notify_event_t_bases, obj_xcb_enter_notify_event_t_fields, obj_xcb_enter_notify_event_t_constants, obj_xcb_enter_notify_event_t_implements, 0},
  { &(obj_type_xcb_focus_in_event_t), REG_OBJECT, obj_xcb_focus_in_event_t_pub_funcs, obj_xcb_focus_in_event_t_methods, obj_xcb_focus_in_event_t_metas, obj_xcb_focus_in_event_t_bases, obj_xcb_focus_in_event_t_fields, obj_xcb_focus_in_event_t_constants, obj_xcb_focus_in_event_t_implements, 0},
  { &(obj_type_xcb_keymap_notify_event_t), REG_OBJECT, obj_xcb_keymap_notify_event_t_pub_funcs, obj_xcb_keymap_notify_event_t_methods, obj_xcb_keymap_notify_event_t_metas, obj_xcb_keymap_notify_event_t_bases, obj_xcb_keymap_notify_event_t_fields, obj_xcb_keymap_notify_event_t_constants, obj_xcb_keymap_notify_event_t_implements, 0},
  { &(obj_type_xcb_expose_event_t), REG_OBJECT, obj_xcb_expose_event_t_pub_funcs, obj_xcb_expose_event_t_methods, obj_xcb_expose_event_t_metas, obj_xcb_expose_event_t_bases, obj_xcb_expose_event_t_fields, obj_xcb_expose_event_t_constants, obj_xcb_expose_event_t_implements, 0},
  { &(obj_type_xcb_graphics_exposure_event_t), REG_OBJECT, obj_xcb_graphics_exposure_event_t_pub_funcs, obj_xcb_graphics_exposure_event_t_methods, obj_xcb_graphics_exposure_event_t_metas, obj_xcb_graphics_exposure_event_t_bases, obj_xcb_graphics_exposure_event_t_fields, obj_xcb_graphics_exposure_event_t_constants, obj_xcb_graphics_exposure_event_t_implements, 0},
  { &(obj_type_xcb_no_exposure_event_t), REG_OBJECT, obj_xcb_no_exposure_event_t_pub_funcs, obj_xcb_no_exposure_event_t_methods, obj_xcb_no_exposure_event_t_metas, obj_xcb_no_exposure_event_t_bases, obj_xcb_no_exposure_event_t_fields, obj_xcb_no_exposure_event_t_constants, obj_xcb_no_exposure_event_t_implements, 0},
  { &(obj_type_xcb_visibility_notify_event_t), REG_OBJECT, obj_xcb_visibility_notify_event_t_pub_funcs, obj_xcb_visibility_notify_event_t_methods, obj_xcb_visibility_notify_event_t_metas, obj_xcb_visibility_notify_event_t_bases, obj_xcb_visibility_notify_event_t_fields, obj_xcb_visibility_notify_event_t_constants, obj_xcb_visibility_notify_event_t_implements, 0},
  { &(obj_type_xcb_create_notify_event_t), REG_OBJECT, obj_xcb_create_notify_event_t_pub_funcs, obj_xcb_create_notify_event_t_methods, obj_xcb_create_notify_event_t_metas, obj_xcb_create_notify_event_t_bases, obj_xcb_create_notify_event_t_fields, obj_xcb_create_notify_event_t_constants, obj_xcb_create_notify_event_t_implements, 0},
  { &(obj_type_xcb_destroy_notify_event_t), REG_OBJECT, obj_xcb_destroy_notify_event_t_pub_funcs, obj_xcb_destroy_notify_event_t_methods, obj_xcb_destroy_notify_event_t_metas, obj_xcb_destroy_notify_event_t_bases, obj_xcb_destroy_notify_event_t_fields, obj_xcb_destroy_notify_event_t_constants, obj_xcb_destroy_notify_event_t_implements, 0},
  { &(obj_type_xcb_unmap_notify_event_t), REG_OBJECT, obj_xcb_unmap_notify_event_t_pub_funcs, obj_xcb_unmap_notify_event_t_methods, obj_xcb_unmap_notify_event_t_metas, obj_xcb_unmap_notify_event_t_bases, obj_xcb_unmap_notify_event_t_fields, obj_xcb_unmap_notify_event_t_constants, obj_xcb_unmap_notify_event_t_implements, 0},
  { &(obj_type_xcb_map_notify_event_t), REG_OBJECT, obj_xcb_map_notify_event_t_pub_funcs, obj_xcb_map_notify_event_t_methods, obj_xcb_map_notify_event_t_metas, obj_xcb_map_notify_event_t_bases, obj_xcb_map_notify_event_t_fields, obj_xcb_map_notify_event_t_constants, obj_xcb_map_notify_event_t_implements, 0},
  { &(obj_type_xcb_map_request_event_t), REG_OBJECT, obj_xcb_map_request_event_t_pub_funcs, obj_xcb_map_request_event_t_methods, obj_xcb_map_request_event_t_metas, obj_xcb_map_request_event_t_bases, obj_xcb_map_request_event_t_fields, obj_xcb_map_request_event_t_constants, obj_xcb_map_request_event_t_implements, 0},
  { &(obj_type_xcb_reparent_notify_event_t), REG_OBJECT, obj_xcb_reparent_notify_event_t_pub_funcs, obj_xcb_reparent_notify_event_t_methods, obj_xcb_reparent_notify_event_t_metas, obj_xcb_reparent_notify_event_t_bases, obj_xcb_reparent_notify_event_t_fields, obj_xcb_reparent_notify_event_t_constants, obj_xcb_reparent_notify_event_t_implements, 0},
  { &(obj_type_xcb_configure_notify_event_t), REG_OBJECT, obj_xcb_configure_notify_event_t_pub_funcs, obj_xcb_configure_notify_event_t_methods, obj_xcb_configure_notify_event_t_metas, obj_xcb_configure_notify_event_t_bases, obj_xcb_configure_notify_event_t_fields, obj_xcb_configure_notify_event_t_constants, obj_xcb_configure_notify_event_t_implements, 0},
  { &(obj_type_xcb_configure_request_event_t), REG_OBJECT, obj_xcb_configure_request_event_t_pub_funcs, obj_xcb_configure_request_event_t_methods, obj_xcb_configure_request_event_t_metas, obj_xcb_configure_request_event_t_bases, obj_xcb_configure_request_event_t_fields, obj_xcb_configure_request_event_t_constants, obj_xcb_configure_request_event_t_implements, 0},
  { &(obj_type_xcb_gravity_notify_event_t), REG_OBJECT, obj_xcb_gravity_notify_event_t_pub_funcs, obj_xcb_gravity_notify_event_t_methods, obj_xcb_gravity_notify_event_t_metas, obj_xcb_gravity_notify_event_t_bases, obj_xcb_gravity_notify_event_t_fields, obj_xcb_gravity_notify_event_t_constants, obj_xcb_gravity_notify_event_t_implements, 0},
  { &(obj_type_xcb_resize_request_event_t), REG_OBJECT, obj_xcb_resize_request_event_t_pub_funcs, obj_xcb_resize_request_event_t_methods, obj_xcb_resize_request_event_t_metas, obj_xcb_resize_request_event_t_bases, obj_xcb_resize_request_event_t_fields, obj_xcb_resize_request_event_t_constants, obj_xcb_resize_request_event_t_implements, 0},
  { &(obj_type_xcb_circulate_notify_event_t), REG_OBJECT, obj_xcb_circulate_notify_event_t_pub_funcs, obj_xcb_circulate_notify_event_t_methods, obj_xcb_circulate_notify_event_t_metas, obj_xcb_circulate_notify_event_t_bases, obj_xcb_circulate_notify_event_t_fields, obj_xcb_circulate_notify_event_t_constants, obj_xcb_circulate_notify_event_t_implements, 0},
  { &(obj_type_xcb_property_notify_event_t), REG_OBJECT, obj_xcb_property_notify_event_t_pub_funcs, obj_xcb_property_notify_event_t_methods, obj_xcb_property_notify_event_t_metas, obj_xcb_property_notify_event_t_bases, obj_xcb_property_notify_event_t_fields, obj_xcb_property_notify_event_t_constants, obj_xcb_property_notify_event_t_implements, 0},
  { &(obj_type_xcb_selection_clear_event_t), REG_OBJECT, obj_xcb_selection_clear_event_t_pub_funcs, obj_xcb_selection_clear_event_t_methods, obj_xcb_selection_clear_event_t_metas, obj_xcb_selection_clear_event_t_bases, obj_xcb_selection_clear_event_t_fields, obj_xcb_selection_clear_event_t_constants, obj_xcb_selection_clear_event_t_implements, 0},
  { &(obj_type_xcb_selection_request_event_t), REG_OBJECT, obj_xcb_selection_request_event_t_pub_funcs, obj_xcb_selection_request_event_t_methods, obj_xcb_selection_request_event_t_metas, obj_xcb_selection_request_event_t_bases, obj_xcb_selection_request_event_t_fields, obj_xcb_selection_request_event_t_constants, obj_xcb_selection_request_event_t_implements, 0},
  { &(obj_type_xcb_selection_notify_event_t), REG_OBJECT, obj_xcb_selection_notify_event_t_pub_funcs, obj_xcb_selection_notify_event_t_methods, obj_xcb_selection_notify_event_t_metas, obj_xcb_selection_notify_event_t_bases, obj_xcb_selection_notify_event_t_fields, obj_xcb_selection_notify_event_t_constants, obj_xcb_selection_notify_event_t_implements, 0},
  { &(obj_type_xcb_colormap_notify_event_t), REG_OBJECT, obj_xcb_colormap_notify_event_t_pub_funcs, obj_xcb_colormap_notify_event_t_methods, obj_xcb_colormap_notify_event_t_metas, obj_xcb_colormap_notify_event_t_bases, obj_xcb_colormap_notify_event_t_fields, obj_xcb_colormap_notify_event_t_constants, obj_xcb_colormap_notify_event_t_implements, 0},
  { &(obj_type_xcb_client_message_data_t), REG_OBJECT, obj_xcb_client_message_data_t_pub_funcs, obj_xcb_client_message_data_t_methods, obj_xcb_client_message_data_t_metas, obj_xcb_client_message_data_t_bases, obj_xcb_client_message_data_t_fields, obj_xcb_client_message_data_t_constants, obj_xcb_client_message_data_t_implements, 0},
  { &(obj_type_xcb_client_message_event_t), REG_OBJECT, obj_xcb_client_message_event_t_pub_funcs, obj_xcb_client_message_event_t_methods, obj_xcb_client_message_event_t_metas, obj_xcb_client_message_event_t_bases, obj_xcb_client_message_event_t_fields, obj_xcb_client_message_event_t_constants, obj_xcb_client_message_event_t_implements, 0},
  { &(obj_type_xcb_mapping_notify_event_t), REG_OBJECT, obj_xcb_mapping_notify_event_t_pub_funcs, obj_xcb_mapping_notify_event_t_methods, obj_xcb_mapping_notify_event_t_metas, obj_xcb_mapping_notify_event_t_bases, obj_xcb_mapping_notify_event_t_fields, obj_xcb_mapping_notify_event_t_constants, obj_xcb_mapping_notify_event_t_implements, 0},
  { &(obj_type_xcb_request_error_t), REG_OBJECT, obj_xcb_request_error_t_pub_funcs, obj_xcb_request_error_t_methods, obj_xcb_request_error_t_metas, obj_xcb_request_error_t_bases, obj_xcb_request_error_t_fields, obj_xcb_request_error_t_constants, obj_xcb_request_error_t_implements, 0},
  { &(obj_type_xcb_value_error_t), REG_OBJECT, obj_xcb_value_error_t_pub_funcs, obj_xcb_value_error_t_methods, obj_xcb_value_error_t_metas, obj_xcb_value_error_t_bases, obj_xcb_value_error_t_fields, obj_xcb_value_error_t_constants, obj_xcb_value_error_t_implements, 0},
  { &(obj_type_xcb_create_window_request_t), REG_OBJECT, obj_xcb_create_window_request_t_pub_funcs, obj_xcb_create_window_request_t_methods, obj_xcb_create_window_request_t_metas, obj_xcb_create_window_request_t_bases, obj_xcb_create_window_request_t_fields, obj_xcb_create_window_request_t_constants, obj_xcb_create_window_request_t_implements, 0},
  { &(obj_type_xcb_change_window_attributes_request_t), REG_OBJECT, obj_xcb_change_window_attributes_request_t_pub_funcs, obj_xcb_change_window_attributes_request_t_methods, obj_xcb_change_window_attributes_request_t_metas, obj_xcb_change_window_attributes_request_t_bases, obj_xcb_change_window_attributes_request_t_fields, obj_xcb_change_window_attributes_request_t_constants, obj_xcb_change_window_attributes_request_t_implements, 0},
  { &(obj_type_xcb_get_window_attributes_request_t), REG_OBJECT, obj_xcb_get_window_attributes_request_t_pub_funcs, obj_xcb_get_window_attributes_request_t_methods, obj_xcb_get_window_attributes_request_t_metas, obj_xcb_get_window_attributes_request_t_bases, obj_xcb_get_window_attributes_request_t_fields, obj_xcb_get_window_attributes_request_t_constants, obj_xcb_get_window_attributes_request_t_implements, 0},
  { &(obj_type_xcb_get_window_attributes_reply_t), REG_OBJECT, obj_xcb_get_window_attributes_reply_t_pub_funcs, obj_xcb_get_window_attributes_reply_t_methods, obj_xcb_get_window_attributes_reply_t_metas, obj_xcb_get_window_attributes_reply_t_bases, obj_xcb_get_window_attributes_reply_t_fields, obj_xcb_get_window_attributes_reply_t_constants, obj_xcb_get_window_attributes_reply_t_implements, 0},
  { &(obj_type_xcb_destroy_window_request_t), REG_OBJECT, obj_xcb_destroy_window_request_t_pub_funcs, obj_xcb_destroy_window_request_t_methods, obj_xcb_destroy_window_request_t_metas, obj_xcb_destroy_window_request_t_bases, obj_xcb_destroy_window_request_t_fields, obj_xcb_destroy_window_request_t_constants, obj_xcb_destroy_window_request_t_implements, 0},
  { &(obj_type_xcb_destroy_subwindows_request_t), REG_OBJECT, obj_xcb_destroy_subwindows_request_t_pub_funcs, obj_xcb_destroy_subwindows_request_t_methods, obj_xcb_destroy_subwindows_request_t_metas, obj_xcb_destroy_subwindows_request_t_bases, obj_xcb_destroy_subwindows_request_t_fields, obj_xcb_destroy_subwindows_request_t_constants, obj_xcb_destroy_subwindows_request_t_implements, 0},
  { &(obj_type_xcb_change_save_set_request_t), REG_OBJECT, obj_xcb_change_save_set_request_t_pub_funcs, obj_xcb_change_save_set_request_t_methods, obj_xcb_change_save_set_request_t_metas, obj_xcb_change_save_set_request_t_bases, obj_xcb_change_save_set_request_t_fields, obj_xcb_change_save_set_request_t_constants, obj_xcb_change_save_set_request_t_implements, 0},
  { &(obj_type_xcb_reparent_window_request_t), REG_OBJECT, obj_xcb_reparent_window_request_t_pub_funcs, obj_xcb_reparent_window_request_t_methods, obj_xcb_reparent_window_request_t_metas, obj_xcb_reparent_window_request_t_bases, obj_xcb_reparent_window_request_t_fields, obj_xcb_reparent_window_request_t_constants, obj_xcb_reparent_window_request_t_implements, 0},
  { &(obj_type_xcb_map_window_request_t), REG_OBJECT, obj_xcb_map_window_request_t_pub_funcs, obj_xcb_map_window_request_t_methods, obj_xcb_map_window_request_t_metas, obj_xcb_map_window_request_t_bases, obj_xcb_map_window_request_t_fields, obj_xcb_map_window_request_t_constants, obj_xcb_map_window_request_t_implements, 0},
  { &(obj_type_xcb_map_subwindows_request_t), REG_OBJECT, obj_xcb_map_subwindows_request_t_pub_funcs, obj_xcb_map_subwindows_request_t_methods, obj_xcb_map_subwindows_request_t_metas, obj_xcb_map_subwindows_request_t_bases, obj_xcb_map_subwindows_request_t_fields, obj_xcb_map_subwindows_request_t_constants, obj_xcb_map_subwindows_request_t_implements, 0},
  { &(obj_type_xcb_unmap_window_request_t), REG_OBJECT, obj_xcb_unmap_window_request_t_pub_funcs, obj_xcb_unmap_window_request_t_methods, obj_xcb_unmap_window_request_t_metas, obj_xcb_unmap_window_request_t_bases, obj_xcb_unmap_window_request_t_fields, obj_xcb_unmap_window_request_t_constants, obj_xcb_unmap_window_request_t_implements, 0},
  { &(obj_type_xcb_unmap_subwindows_request_t), REG_OBJECT, obj_xcb_unmap_subwindows_request_t_pub_funcs, obj_xcb_unmap_subwindows_request_t_methods, obj_xcb_unmap_subwindows_request_t_metas, obj_xcb_unmap_subwindows_request_t_bases, obj_xcb_unmap_subwindows_request_t_fields, obj_xcb_unmap_subwindows_request_t_constants, obj_xcb_unmap_subwindows_request_t_implements, 0},
  { &(obj_type_xcb_configure_window_request_t), REG_OBJECT, obj_xcb_configure_window_request_t_pub_funcs, obj_xcb_configure_window_request_t_methods, obj_xcb_configure_window_request_t_metas, obj_xcb_configure_window_request_t_bases, obj_xcb_configure_window_request_t_fields, obj_xcb_configure_window_request_t_constants, obj_xcb_configure_window_request_t_implements, 0},
  { &(obj_type_xcb_circulate_window_request_t), REG_OBJECT, obj_xcb_circulate_window_request_t_pub_funcs, obj_xcb_circulate_window_request_t_methods, obj_xcb_circulate_window_request_t_metas, obj_xcb_circulate_window_request_t_bases, obj_xcb_circulate_window_request_t_fields, obj_xcb_circulate_window_request_t_constants, obj_xcb_circulate_window_request_t_implements, 0},
  { &(obj_type_xcb_get_geometry_request_t), REG_OBJECT, obj_xcb_get_geometry_request_t_pub_funcs, obj_xcb_get_geometry_request_t_methods, obj_xcb_get_geometry_request_t_metas, obj_xcb_get_geometry_request_t_bases, obj_xcb_get_geometry_request_t_fields, obj_xcb_get_geometry_request_t_constants, obj_xcb_get_geometry_request_t_implements, 0},
  { &(obj_type_xcb_get_geometry_reply_t), REG_OBJECT, obj_xcb_get_geometry_reply_t_pub_funcs, obj_xcb_get_geometry_reply_t_methods, obj_xcb_get_geometry_reply_t_metas, obj_xcb_get_geometry_reply_t_bases, obj_xcb_get_geometry_reply_t_fields, obj_xcb_get_geometry_reply_t_constants, obj_xcb_get_geometry_reply_t_implements, 0},
  { &(obj_type_xcb_query_tree_request_t), REG_OBJECT, obj_xcb_query_tree_request_t_pub_funcs, obj_xcb_query_tree_request_t_methods, obj_xcb_query_tree_request_t_metas, obj_xcb_query_tree_request_t_bases, obj_xcb_query_tree_request_t_fields, obj_xcb_query_tree_request_t_constants, obj_xcb_query_tree_request_t_implements, 0},
  { &(obj_type_xcb_query_tree_reply_t), REG_OBJECT, obj_xcb_query_tree_reply_t_pub_funcs, obj_xcb_query_tree_reply_t_methods, obj_xcb_query_tree_reply_t_metas, obj_xcb_query_tree_reply_t_bases, obj_xcb_query_tree_reply_t_fields, obj_xcb_query_tree_reply_t_constants, obj_xcb_query_tree_reply_t_implements, 0},
  { &(obj_type_xcb_intern_atom_request_t), REG_OBJECT, obj_xcb_intern_atom_request_t_pub_funcs, obj_xcb_intern_atom_request_t_methods, obj_xcb_intern_atom_request_t_metas, obj_xcb_intern_atom_request_t_bases, obj_xcb_intern_atom_request_t_fields, obj_xcb_intern_atom_request_t_constants, obj_xcb_intern_atom_request_t_implements, 0},
  { &(obj_type_xcb_intern_atom_reply_t), REG_OBJECT, obj_xcb_intern_atom_reply_t_pub_funcs, obj_xcb_intern_atom_reply_t_methods, obj_xcb_intern_atom_reply_t_metas, obj_xcb_intern_atom_reply_t_bases, obj_xcb_intern_atom_reply_t_fields, obj_xcb_intern_atom_reply_t_constants, obj_xcb_intern_atom_reply_t_implements, 0},
  { &(obj_type_xcb_get_atom_name_request_t), REG_OBJECT, obj_xcb_get_atom_name_request_t_pub_funcs, obj_xcb_get_atom_name_request_t_methods, obj_xcb_get_atom_name_request_t_metas, obj_xcb_get_atom_name_request_t_bases, obj_xcb_get_atom_name_request_t_fields, obj_xcb_get_atom_name_request_t_constants, obj_xcb_get_atom_name_request_t_implements, 0},
  { &(obj_type_xcb_get_atom_name_reply_t), REG_OBJECT, obj_xcb_get_atom_name_reply_t_pub_funcs, obj_xcb_get_atom_name_reply_t_methods, obj_xcb_get_atom_name_reply_t_metas, obj_xcb_get_atom_name_reply_t_bases, obj_xcb_get_atom_name_reply_t_fields, obj_xcb_get_atom_name_reply_t_constants, obj_xcb_get_atom_name_reply_t_implements, 0},
  { &(obj_type_xcb_change_property_request_t), REG_OBJECT, obj_xcb_change_property_request_t_pub_funcs, obj_xcb_change_property_request_t_methods, obj_xcb_change_property_request_t_metas, obj_xcb_change_property_request_t_bases, obj_xcb_change_property_request_t_fields, obj_xcb_change_property_request_t_constants, obj_xcb_change_property_request_t_implements, 0},
  { &(obj_type_xcb_delete_property_request_t), REG_OBJECT, obj_xcb_delete_property_request_t_pub_funcs, obj_xcb_delete_property_request_t_methods, obj_xcb_delete_property_request_t_metas, obj_xcb_delete_property_request_t_bases, obj_xcb_delete_property_request_t_fields, obj_xcb_delete_property_request_t_constants, obj_xcb_delete_property_request_t_implements, 0},
  { &(obj_type_xcb_get_property_request_t), REG_OBJECT, obj_xcb_get_property_request_t_pub_funcs, obj_xcb_get_property_request_t_methods, obj_xcb_get_property_request_t_metas, obj_xcb_get_property_request_t_bases, obj_xcb_get_property_request_t_fields, obj_xcb_get_property_request_t_constants, obj_xcb_get_property_request_t_implements, 0},
  { &(obj_type_xcb_get_property_reply_t), REG_OBJECT, obj_xcb_get_property_reply_t_pub_funcs, obj_xcb_get_property_reply_t_methods, obj_xcb_get_property_reply_t_metas, obj_xcb_get_property_reply_t_bases, obj_xcb_get_property_reply_t_fields, obj_xcb_get_property_reply_t_constants, obj_xcb_get_property_reply_t_implements, 0},
  { &(obj_type_xcb_list_properties_request_t), REG_OBJECT, obj_xcb_list_properties_request_t_pub_funcs, obj_xcb_list_properties_request_t_methods, obj_xcb_list_properties_request_t_metas, obj_xcb_list_properties_request_t_bases, obj_xcb_list_properties_request_t_fields, obj_xcb_list_properties_request_t_constants, obj_xcb_list_properties_request_t_implements, 0},
  { &(obj_type_xcb_list_properties_reply_t), REG_OBJECT, obj_xcb_list_properties_reply_t_pub_funcs, obj_xcb_list_properties_reply_t_methods, obj_xcb_list_properties_reply_t_metas, obj_xcb_list_properties_reply_t_bases, obj_xcb_list_properties_reply_t_fields, obj_xcb_list_properties_reply_t_constants, obj_xcb_list_properties_reply_t_implements, 0},
  { &(obj_type_xcb_set_selection_owner_request_t), REG_OBJECT, obj_xcb_set_selection_owner_request_t_pub_funcs, obj_xcb_set_selection_owner_request_t_methods, obj_xcb_set_selection_owner_request_t_metas, obj_xcb_set_selection_owner_request_t_bases, obj_xcb_set_selection_owner_request_t_fields, obj_xcb_set_selection_owner_request_t_constants, obj_xcb_set_selection_owner_request_t_implements, 0},
  { &(obj_type_xcb_get_selection_owner_request_t), REG_OBJECT, obj_xcb_get_selection_owner_request_t_pub_funcs, obj_xcb_get_selection_owner_request_t_methods, obj_xcb_get_selection_owner_request_t_metas, obj_xcb_get_selection_owner_request_t_bases, obj_xcb_get_selection_owner_request_t_fields, obj_xcb_get_selection_owner_request_t_constants, obj_xcb_get_selection_owner_request_t_implements, 0},
  { &(obj_type_xcb_get_selection_owner_reply_t), REG_OBJECT, obj_xcb_get_selection_owner_reply_t_pub_funcs, obj_xcb_get_selection_owner_reply_t_methods, obj_xcb_get_selection_owner_reply_t_metas, obj_xcb_get_selection_owner_reply_t_bases, obj_xcb_get_selection_owner_reply_t_fields, obj_xcb_get_selection_owner_reply_t_constants, obj_xcb_get_selection_owner_reply_t_implements, 0},
  { &(obj_type_xcb_convert_selection_request_t), REG_OBJECT, obj_xcb_convert_selection_request_t_pub_funcs, obj_xcb_convert_selection_request_t_methods, obj_xcb_convert_selection_request_t_metas, obj_xcb_convert_selection_request_t_bases, obj_xcb_convert_selection_request_t_fields, obj_xcb_convert_selection_request_t_constants, obj_xcb_convert_selection_request_t_implements, 0},
  { &(obj_type_xcb_send_event_request_t), REG_OBJECT, obj_xcb_send_event_request_t_pub_funcs, obj_xcb_send_event_request_t_methods, obj_xcb_send_event_request_t_metas, obj_xcb_send_event_request_t_bases, obj_xcb_send_event_request_t_fields, obj_xcb_send_event_request_t_constants, obj_xcb_send_event_request_t_implements, 0},
  { &(obj_type_xcb_grab_pointer_request_t), REG_OBJECT, obj_xcb_grab_pointer_request_t_pub_funcs, obj_xcb_grab_pointer_request_t_methods, obj_xcb_grab_pointer_request_t_metas, obj_xcb_grab_pointer_request_t_bases, obj_xcb_grab_pointer_request_t_fields, obj_xcb_grab_pointer_request_t_constants, obj_xcb_grab_pointer_request_t_implements, 0},
  { &(obj_type_xcb_grab_pointer_reply_t), REG_OBJECT, obj_xcb_grab_pointer_reply_t_pub_funcs, obj_xcb_grab_pointer_reply_t_methods, obj_xcb_grab_pointer_reply_t_metas, obj_xcb_grab_pointer_reply_t_bases, obj_xcb_grab_pointer_reply_t_fields, obj_xcb_grab_pointer_reply_t_constants, obj_xcb_grab_pointer_reply_t_implements, 0},
  { &(obj_type_xcb_ungrab_pointer_request_t), REG_OBJECT, obj_xcb_ungrab_pointer_request_t_pub_funcs, obj_xcb_ungrab_pointer_request_t_methods, obj_xcb_ungrab_pointer_request_t_metas, obj_xcb_ungrab_pointer_request_t_bases, obj_xcb_ungrab_pointer_request_t_fields, obj_xcb_ungrab_pointer_request_t_constants, obj_xcb_ungrab_pointer_request_t_implements, 0},
  { &(obj_type_xcb_grab_button_request_t), REG_OBJECT, obj_xcb_grab_button_request_t_pub_funcs, obj_xcb_grab_button_request_t_methods, obj_xcb_grab_button_request_t_metas, obj_xcb_grab_button_request_t_bases, obj_xcb_grab_button_request_t_fields, obj_xcb_grab_button_request_t_constants, obj_xcb_grab_button_request_t_implements, 0},
  { &(obj_type_xcb_ungrab_button_request_t), REG_OBJECT, obj_xcb_ungrab_button_request_t_pub_funcs, obj_xcb_ungrab_button_request_t_methods, obj_xcb_ungrab_button_request_t_metas, obj_xcb_ungrab_button_request_t_bases, obj_xcb_ungrab_button_request_t_fields, obj_xcb_ungrab_button_request_t_constants, obj_xcb_ungrab_button_request_t_implements, 0},
  { &(obj_type_xcb_change_active_pointer_grab_request_t), REG_OBJECT, obj_xcb_change_active_pointer_grab_request_t_pub_funcs, obj_xcb_change_active_pointer_grab_request_t_methods, obj_xcb_change_active_pointer_grab_request_t_metas, obj_xcb_change_active_pointer_grab_request_t_bases, obj_xcb_change_active_pointer_grab_request_t_fields, obj_xcb_change_active_pointer_grab_request_t_constants, obj_xcb_change_active_pointer_grab_request_t_implements, 0},
  { &(obj_type_xcb_grab_keyboard_request_t), REG_OBJECT, obj_xcb_grab_keyboard_request_t_pub_funcs, obj_xcb_grab_keyboard_request_t_methods, obj_xcb_grab_keyboard_request_t_metas, obj_xcb_grab_keyboard_request_t_bases, obj_xcb_grab_keyboard_request_t_fields, obj_xcb_grab_keyboard_request_t_constants, obj_xcb_grab_keyboard_request_t_implements, 0},
  { &(obj_type_xcb_grab_keyboard_reply_t), REG_OBJECT, obj_xcb_grab_keyboard_reply_t_pub_funcs, obj_xcb_grab_keyboard_reply_t_methods, obj_xcb_grab_keyboard_reply_t_metas, obj_xcb_grab_keyboard_reply_t_bases, obj_xcb_grab_keyboard_reply_t_fields, obj_xcb_grab_keyboard_reply_t_constants, obj_xcb_grab_keyboard_reply_t_implements, 0},
  { &(obj_type_xcb_ungrab_keyboard_request_t), REG_OBJECT, obj_xcb_ungrab_keyboard_request_t_pub_funcs, obj_xcb_ungrab_keyboard_request_t_methods, obj_xcb_ungrab_keyboard_request_t_metas, obj_xcb_ungrab_keyboard_request_t_bases, obj_xcb_ungrab_keyboard_request_t_fields, obj_xcb_ungrab_keyboard_request_t_constants, obj_xcb_ungrab_keyboard_request_t_implements, 0},
  { &(obj_type_xcb_grab_key_request_t), REG_OBJECT, obj_xcb_grab_key_request_t_pub_funcs, obj_xcb_grab_key_request_t_methods, obj_xcb_grab_key_request_t_metas, obj_xcb_grab_key_request_t_bases, obj_xcb_grab_key_request_t_fields, obj_xcb_grab_key_request_t_constants, obj_xcb_grab_key_request_t_implements, 0},
  { &(obj_type_xcb_ungrab_key_request_t), REG_OBJECT, obj_xcb_ungrab_key_request_t_pub_funcs, obj_xcb_ungrab_key_request_t_methods, obj_xcb_ungrab_key_request_t_metas, obj_xcb_ungrab_key_request_t_bases, obj_xcb_ungrab_key_request_t_fields, obj_xcb_ungrab_key_request_t_constants, obj_xcb_ungrab_key_request_t_implements, 0},
  { &(obj_type_xcb_allow_events_request_t), REG_OBJECT, obj_xcb_allow_events_request_t_pub_funcs, obj_xcb_allow_events_request_t_methods, obj_xcb_allow_events_request_t_metas, obj_xcb_allow_events_request_t_bases, obj_xcb_allow_events_request_t_fields, obj_xcb_allow_events_request_t_constants, obj_xcb_allow_events_request_t_implements, 0},
  { &(obj_type_xcb_grab_server_request_t), REG_OBJECT, obj_xcb_grab_server_request_t_pub_funcs, obj_xcb_grab_server_request_t_methods, obj_xcb_grab_server_request_t_metas, obj_xcb_grab_server_request_t_bases, obj_xcb_grab_server_request_t_fields, obj_xcb_grab_server_request_t_constants, obj_xcb_grab_server_request_t_implements, 0},
  { &(obj_type_xcb_ungrab_server_request_t), REG_OBJECT, obj_xcb_ungrab_server_request_t_pub_funcs, obj_xcb_ungrab_server_request_t_methods, obj_xcb_ungrab_server_request_t_metas, obj_xcb_ungrab_server_request_t_bases, obj_xcb_ungrab_server_request_t_fields, obj_xcb_ungrab_server_request_t_constants, obj_xcb_ungrab_server_request_t_implements, 0},
  { &(obj_type_xcb_query_pointer_request_t), REG_OBJECT, obj_xcb_query_pointer_request_t_pub_funcs, obj_xcb_query_pointer_request_t_methods, obj_xcb_query_pointer_request_t_metas, obj_xcb_query_pointer_request_t_bases, obj_xcb_query_pointer_request_t_fields, obj_xcb_query_pointer_request_t_constants, obj_xcb_query_pointer_request_t_implements, 0},
  { &(obj_type_xcb_query_pointer_reply_t), REG_OBJECT, obj_xcb_query_pointer_reply_t_pub_funcs, obj_xcb_query_pointer_reply_t_methods, obj_xcb_query_pointer_reply_t_metas, obj_xcb_query_pointer_reply_t_bases, obj_xcb_query_pointer_reply_t_fields, obj_xcb_query_pointer_reply_t_constants, obj_xcb_query_pointer_reply_t_implements, 0},
  { &(obj_type_xcb_timecoord_t), REG_OBJECT, obj_xcb_timecoord_t_pub_funcs, obj_xcb_timecoord_t_methods, obj_xcb_timecoord_t_metas, obj_xcb_timecoord_t_bases, obj_xcb_timecoord_t_fields, obj_xcb_timecoord_t_constants, obj_xcb_timecoord_t_implements, 0},
  { &(obj_type_xcb_get_motion_events_request_t), REG_OBJECT, obj_xcb_get_motion_events_request_t_pub_funcs, obj_xcb_get_motion_events_request_t_methods, obj_xcb_get_motion_events_request_t_metas, obj_xcb_get_motion_events_request_t_bases, obj_xcb_get_motion_events_request_t_fields, obj_xcb_get_motion_events_request_t_constants, obj_xcb_get_motion_events_request_t_implements, 0},
  { &(obj_type_xcb_get_motion_events_reply_t), REG_OBJECT, obj_xcb_get_motion_events_reply_t_pub_funcs, obj_xcb_get_motion_events_reply_t_methods, obj_xcb_get_motion_events_reply_t_metas, obj_xcb_get_motion_events_reply_t_bases, obj_xcb_get_motion_events_reply_t_fields, obj_xcb_get_motion_events_reply_t_constants, obj_xcb_get_motion_events_reply_t_implements, 0},
  { &(obj_type_xcb_translate_coordinates_request_t), REG_OBJECT, obj_xcb_translate_coordinates_request_t_pub_funcs, obj_xcb_translate_coordinates_request_t_methods, obj_xcb_translate_coordinates_request_t_metas, obj_xcb_translate_coordinates_request_t_bases, obj_xcb_translate_coordinates_request_t_fields, obj_xcb_translate_coordinates_request_t_constants, obj_xcb_translate_coordinates_request_t_implements, 0},
  { &(obj_type_xcb_translate_coordinates_reply_t), REG_OBJECT, obj_xcb_translate_coordinates_reply_t_pub_funcs, obj_xcb_translate_coordinates_reply_t_methods, obj_xcb_translate_coordinates_reply_t_metas, obj_xcb_translate_coordinates_reply_t_bases, obj_xcb_translate_coordinates_reply_t_fields, obj_xcb_translate_coordinates_reply_t_constants, obj_xcb_translate_coordinates_reply_t_implements, 0},
  { &(obj_type_xcb_warp_pointer_request_t), REG_OBJECT, obj_xcb_warp_pointer_request_t_pub_funcs, obj_xcb_warp_pointer_request_t_methods, obj_xcb_warp_pointer_request_t_metas, obj_xcb_warp_pointer_request_t_bases, obj_xcb_warp_pointer_request_t_fields, obj_xcb_warp_pointer_request_t_constants, obj_xcb_warp_pointer_request_t_implements, 0},
  { &(obj_type_xcb_set_input_focus_request_t), REG_OBJECT, obj_xcb_set_input_focus_request_t_pub_funcs, obj_xcb_set_input_focus_request_t_methods, obj_xcb_set_input_focus_request_t_metas, obj_xcb_set_input_focus_request_t_bases, obj_xcb_set_input_focus_request_t_fields, obj_xcb_set_input_focus_request_t_constants, obj_xcb_set_input_focus_request_t_implements, 0},
  { &(obj_type_xcb_get_input_focus_request_t), REG_OBJECT, obj_xcb_get_input_focus_request_t_pub_funcs, obj_xcb_get_input_focus_request_t_methods, obj_xcb_get_input_focus_request_t_metas, obj_xcb_get_input_focus_request_t_bases, obj_xcb_get_input_focus_request_t_fields, obj_xcb_get_input_focus_request_t_constants, obj_xcb_get_input_focus_request_t_implements, 0},
  { &(obj_type_xcb_get_input_focus_reply_t), REG_OBJECT, obj_xcb_get_input_focus_reply_t_pub_funcs, obj_xcb_get_input_focus_reply_t_methods, obj_xcb_get_input_focus_reply_t_metas, obj_xcb_get_input_focus_reply_t_bases, obj_xcb_get_input_focus_reply_t_fields, obj_xcb_get_input_focus_reply_t_constants, obj_xcb_get_input_focus_reply_t_implements, 0},
  { &(obj_type_xcb_query_keymap_request_t), REG_OBJECT, obj_xcb_query_keymap_request_t_pub_funcs, obj_xcb_query_keymap_request_t_methods, obj_xcb_query_keymap_request_t_metas, obj_xcb_query_keymap_request_t_bases, obj_xcb_query_keymap_request_t_fields, obj_xcb_query_keymap_request_t_constants, obj_xcb_query_keymap_request_t_implements, 0},
  { &(obj_type_xcb_query_keymap_reply_t), REG_OBJECT, obj_xcb_query_keymap_reply_t_pub_funcs, obj_xcb_query_keymap_reply_t_methods, obj_xcb_query_keymap_reply_t_metas, obj_xcb_query_keymap_reply_t_bases, obj_xcb_query_keymap_reply_t_fields, obj_xcb_query_keymap_reply_t_constants, obj_xcb_query_keymap_reply_t_implements, 0},
  { &(obj_type_xcb_open_font_request_t), REG_OBJECT, obj_xcb_open_font_request_t_pub_funcs, obj_xcb_open_font_request_t_methods, obj_xcb_open_font_request_t_metas, obj_xcb_open_font_request_t_bases, obj_xcb_open_font_request_t_fields, obj_xcb_open_font_request_t_constants, obj_xcb_open_font_request_t_implements, 0},
  { &(obj_type_xcb_close_font_request_t), REG_OBJECT, obj_xcb_close_font_request_t_pub_funcs, obj_xcb_close_font_request_t_methods, obj_xcb_close_font_request_t_metas, obj_xcb_close_font_request_t_bases, obj_xcb_close_font_request_t_fields, obj_xcb_close_font_request_t_constants, obj_xcb_close_font_request_t_implements, 0},
  { &(obj_type_xcb_fontprop_t), REG_OBJECT, obj_xcb_fontprop_t_pub_funcs, obj_xcb_fontprop_t_methods, obj_xcb_fontprop_t_metas, obj_xcb_fontprop_t_bases, obj_xcb_fontprop_t_fields, obj_xcb_fontprop_t_constants, obj_xcb_fontprop_t_implements, 0},
  { &(obj_type_xcb_charinfo_t), REG_OBJECT, obj_xcb_charinfo_t_pub_funcs, obj_xcb_charinfo_t_methods, obj_xcb_charinfo_t_metas, obj_xcb_charinfo_t_bases, obj_xcb_charinfo_t_fields, obj_xcb_charinfo_t_constants, obj_xcb_charinfo_t_implements, 0},
  { &(obj_type_xcb_query_font_request_t), REG_OBJECT, obj_xcb_query_font_request_t_pub_funcs, obj_xcb_query_font_request_t_methods, obj_xcb_query_font_request_t_metas, obj_xcb_query_font_request_t_bases, obj_xcb_query_font_request_t_fields, obj_xcb_query_font_request_t_constants, obj_xcb_query_font_request_t_implements, 0},
  { &(obj_type_xcb_query_font_reply_t), REG_OBJECT, obj_xcb_query_font_reply_t_pub_funcs, obj_xcb_query_font_reply_t_methods, obj_xcb_query_font_reply_t_metas, obj_xcb_query_font_reply_t_bases, obj_xcb_query_font_reply_t_fields, obj_xcb_query_font_reply_t_constants, obj_xcb_query_font_reply_t_implements, 0},
  { &(obj_type_xcb_query_text_extents_request_t), REG_OBJECT, obj_xcb_query_text_extents_request_t_pub_funcs, obj_xcb_query_text_extents_request_t_methods, obj_xcb_query_text_extents_request_t_metas, obj_xcb_query_text_extents_request_t_bases, obj_xcb_query_text_extents_request_t_fields, obj_xcb_query_text_extents_request_t_constants, obj_xcb_query_text_extents_request_t_implements, 0},
  { &(obj_type_xcb_query_text_extents_reply_t), REG_OBJECT, obj_xcb_query_text_extents_reply_t_pub_funcs, obj_xcb_query_text_extents_reply_t_methods, obj_xcb_query_text_extents_reply_t_metas, obj_xcb_query_text_extents_reply_t_bases, obj_xcb_query_text_extents_reply_t_fields, obj_xcb_query_text_extents_reply_t_constants, obj_xcb_query_text_extents_reply_t_implements, 0},
  { &(obj_type_xcb_str_t), REG_OBJECT, obj_xcb_str_t_pub_funcs, obj_xcb_str_t_methods, obj_xcb_str_t_metas, obj_xcb_str_t_bases, obj_xcb_str_t_fields, obj_xcb_str_t_constants, obj_xcb_str_t_implements, 0},
  { &(obj_type_xcb_list_fonts_request_t), REG_OBJECT, obj_xcb_list_fonts_request_t_pub_funcs, obj_xcb_list_fonts_request_t_methods, obj_xcb_list_fonts_request_t_metas, obj_xcb_list_fonts_request_t_bases, obj_xcb_list_fonts_request_t_fields, obj_xcb_list_fonts_request_t_constants, obj_xcb_list_fonts_request_t_implements, 0},
  { &(obj_type_xcb_list_fonts_reply_t), REG_OBJECT, obj_xcb_list_fonts_reply_t_pub_funcs, obj_xcb_list_fonts_reply_t_methods, obj_xcb_list_fonts_reply_t_metas, obj_xcb_list_fonts_reply_t_bases, obj_xcb_list_fonts_reply_t_fields, obj_xcb_list_fonts_reply_t_constants, obj_xcb_list_fonts_reply_t_implements, 0},
  { &(obj_type_xcb_list_fonts_with_info_request_t), REG_OBJECT, obj_xcb_list_fonts_with_info_request_t_pub_funcs, obj_xcb_list_fonts_with_info_request_t_methods, obj_xcb_list_fonts_with_info_request_t_metas, obj_xcb_list_fonts_with_info_request_t_bases, obj_xcb_list_fonts_with_info_request_t_fields, obj_xcb_list_fonts_with_info_request_t_constants, obj_xcb_list_fonts_with_info_request_t_implements, 0},
  { &(obj_type_xcb_list_fonts_with_info_reply_t), REG_OBJECT, obj_xcb_list_fonts_with_info_reply_t_pub_funcs, obj_xcb_list_fonts_with_info_reply_t_methods, obj_xcb_list_fonts_with_info_reply_t_metas, obj_xcb_list_fonts_with_info_reply_t_bases, obj_xcb_list_fonts_with_info_reply_t_fields, obj_xcb_list_fonts_with_info_reply_t_constants, obj_xcb_list_fonts_with_info_reply_t_implements, 0},
  { &(obj_type_xcb_set_font_path_request_t), REG_OBJECT, obj_xcb_set_font_path_request_t_pub_funcs, obj_xcb_set_font_path_request_t_methods, obj_xcb_set_font_path_request_t_metas, obj_xcb_set_font_path_request_t_bases, obj_xcb_set_font_path_request_t_fields, obj_xcb_set_font_path_request_t_constants, obj_xcb_set_font_path_request_t_implements, 0},
  { &(obj_type_xcb_get_font_path_request_t), REG_OBJECT, obj_xcb_get_font_path_request_t_pub_funcs, obj_xcb_get_font_path_request_t_methods, obj_xcb_get_font_path_request_t_metas, obj_xcb_get_font_path_request_t_bases, obj_xcb_get_font_path_request_t_fields, obj_xcb_get_font_path_request_t_constants, obj_xcb_get_font_path_request_t_implements, 0},
  { &(obj_type_xcb_get_font_path_reply_t), REG_OBJECT, obj_xcb_get_font_path_reply_t_pub_funcs, obj_xcb_get_font_path_reply_t_methods, obj_xcb_get_font_path_reply_t_metas, obj_xcb_get_font_path_reply_t_bases, obj_xcb_get_font_path_reply_t_fields, obj_xcb_get_font_path_reply_t_constants, obj_xcb_get_font_path_reply_t_implements, 0},
  { &(obj_type_xcb_create_pixmap_request_t), REG_OBJECT, obj_xcb_create_pixmap_request_t_pub_funcs, obj_xcb_create_pixmap_request_t_methods, obj_xcb_create_pixmap_request_t_metas, obj_xcb_create_pixmap_request_t_bases, obj_xcb_create_pixmap_request_t_fields, obj_xcb_create_pixmap_request_t_constants, obj_xcb_create_pixmap_request_t_implements, 0},
  { &(obj_type_xcb_free_pixmap_request_t), REG_OBJECT, obj_xcb_free_pixmap_request_t_pub_funcs, obj_xcb_free_pixmap_request_t_methods, obj_xcb_free_pixmap_request_t_metas, obj_xcb_free_pixmap_request_t_bases, obj_xcb_free_pixmap_request_t_fields, obj_xcb_free_pixmap_request_t_constants, obj_xcb_free_pixmap_request_t_implements, 0},
  { &(obj_type_xcb_create_gc_request_t), REG_OBJECT, obj_xcb_create_gc_request_t_pub_funcs, obj_xcb_create_gc_request_t_methods, obj_xcb_create_gc_request_t_metas, obj_xcb_create_gc_request_t_bases, obj_xcb_create_gc_request_t_fields, obj_xcb_create_gc_request_t_constants, obj_xcb_create_gc_request_t_implements, 0},
  { &(obj_type_xcb_change_gc_request_t), REG_OBJECT, obj_xcb_change_gc_request_t_pub_funcs, obj_xcb_change_gc_request_t_methods, obj_xcb_change_gc_request_t_metas, obj_xcb_change_gc_request_t_bases, obj_xcb_change_gc_request_t_fields, obj_xcb_change_gc_request_t_constants, obj_xcb_change_gc_request_t_implements, 0},
  { &(obj_type_xcb_copy_gc_request_t), REG_OBJECT, obj_xcb_copy_gc_request_t_pub_funcs, obj_xcb_copy_gc_request_t_methods, obj_xcb_copy_gc_request_t_metas, obj_xcb_copy_gc_request_t_bases, obj_xcb_copy_gc_request_t_fields, obj_xcb_copy_gc_request_t_constants, obj_xcb_copy_gc_request_t_implements, 0},
  { &(obj_type_xcb_set_dashes_request_t), REG_OBJECT, obj_xcb_set_dashes_request_t_pub_funcs, obj_xcb_set_dashes_request_t_methods, obj_xcb_set_dashes_request_t_metas, obj_xcb_set_dashes_request_t_bases, obj_xcb_set_dashes_request_t_fields, obj_xcb_set_dashes_request_t_constants, obj_xcb_set_dashes_request_t_implements, 0},
  { &(obj_type_xcb_set_clip_rectangles_request_t), REG_OBJECT, obj_xcb_set_clip_rectangles_request_t_pub_funcs, obj_xcb_set_clip_rectangles_request_t_methods, obj_xcb_set_clip_rectangles_request_t_metas, obj_xcb_set_clip_rectangles_request_t_bases, obj_xcb_set_clip_rectangles_request_t_fields, obj_xcb_set_clip_rectangles_request_t_constants, obj_xcb_set_clip_rectangles_request_t_implements, 0},
  { &(obj_type_xcb_free_gc_request_t), REG_OBJECT, obj_xcb_free_gc_request_t_pub_funcs, obj_xcb_free_gc_request_t_methods, obj_xcb_free_gc_request_t_metas, obj_xcb_free_gc_request_t_bases, obj_xcb_free_gc_request_t_fields, obj_xcb_free_gc_request_t_constants, obj_xcb_free_gc_request_t_implements, 0},
  { &(obj_type_xcb_clear_area_request_t), REG_OBJECT, obj_xcb_clear_area_request_t_pub_funcs, obj_xcb_clear_area_request_t_methods, obj_xcb_clear_area_request_t_metas, obj_xcb_clear_area_request_t_bases, obj_xcb_clear_area_request_t_fields, obj_xcb_clear_area_request_t_constants, obj_xcb_clear_area_request_t_implements, 0},
  { &(obj_type_xcb_copy_area_request_t), REG_OBJECT, obj_xcb_copy_area_request_t_pub_funcs, obj_xcb_copy_area_request_t_methods, obj_xcb_copy_area_request_t_metas, obj_xcb_copy_area_request_t_bases, obj_xcb_copy_area_request_t_fields, obj_xcb_copy_area_request_t_constants, obj_xcb_copy_area_request_t_implements, 0},
  { &(obj_type_xcb_copy_plane_request_t), REG_OBJECT, obj_xcb_copy_plane_request_t_pub_funcs, obj_xcb_copy_plane_request_t_methods, obj_xcb_copy_plane_request_t_metas, obj_xcb_copy_plane_request_t_bases, obj_xcb_copy_plane_request_t_fields, obj_xcb_copy_plane_request_t_constants, obj_xcb_copy_plane_request_t_implements, 0},
  { &(obj_type_xcb_poly_point_request_t), REG_OBJECT, obj_xcb_poly_point_request_t_pub_funcs, obj_xcb_poly_point_request_t_methods, obj_xcb_poly_point_request_t_metas, obj_xcb_poly_point_request_t_bases, obj_xcb_poly_point_request_t_fields, obj_xcb_poly_point_request_t_constants, obj_xcb_poly_point_request_t_implements, 0},
  { &(obj_type_xcb_poly_line_request_t), REG_OBJECT, obj_xcb_poly_line_request_t_pub_funcs, obj_xcb_poly_line_request_t_methods, obj_xcb_poly_line_request_t_metas, obj_xcb_poly_line_request_t_bases, obj_xcb_poly_line_request_t_fields, obj_xcb_poly_line_request_t_constants, obj_xcb_poly_line_request_t_implements, 0},
  { &(obj_type_xcb_segment_t), REG_OBJECT, obj_xcb_segment_t_pub_funcs, obj_xcb_segment_t_methods, obj_xcb_segment_t_metas, obj_xcb_segment_t_bases, obj_xcb_segment_t_fields, obj_xcb_segment_t_constants, obj_xcb_segment_t_implements, 0},
  { &(obj_type_xcb_poly_segment_request_t), REG_OBJECT, obj_xcb_poly_segment_request_t_pub_funcs, obj_xcb_poly_segment_request_t_methods, obj_xcb_poly_segment_request_t_metas, obj_xcb_poly_segment_request_t_bases, obj_xcb_poly_segment_request_t_fields, obj_xcb_poly_segment_request_t_constants, obj_xcb_poly_segment_request_t_implements, 0},
  { &(obj_type_xcb_poly_rectangle_request_t), REG_OBJECT, obj_xcb_poly_rectangle_request_t_pub_funcs, obj_xcb_poly_rectangle_request_t_methods, obj_xcb_poly_rectangle_request_t_metas, obj_xcb_poly_rectangle_request_t_bases, obj_xcb_poly_rectangle_request_t_fields, obj_xcb_poly_rectangle_request_t_constants, obj_xcb_poly_rectangle_request_t_implements, 0},
  { &(obj_type_xcb_poly_arc_request_t), REG_OBJECT, obj_xcb_poly_arc_request_t_pub_funcs, obj_xcb_poly_arc_request_t_methods, obj_xcb_poly_arc_request_t_metas, obj_xcb_poly_arc_request_t_bases, obj_xcb_poly_arc_request_t_fields, obj_xcb_poly_arc_request_t_constants, obj_xcb_poly_arc_request_t_implements, 0},
  { &(obj_type_xcb_fill_poly_request_t), REG_OBJECT, obj_xcb_fill_poly_request_t_pub_funcs, obj_xcb_fill_poly_request_t_methods, obj_xcb_fill_poly_request_t_metas, obj_xcb_fill_poly_request_t_bases, obj_xcb_fill_poly_request_t_fields, obj_xcb_fill_poly_request_t_constants, obj_xcb_fill_poly_request_t_implements, 0},
  { &(obj_type_xcb_poly_fill_rectangle_request_t), REG_OBJECT, obj_xcb_poly_fill_rectangle_request_t_pub_funcs, obj_xcb_poly_fill_rectangle_request_t_methods, obj_xcb_poly_fill_rectangle_request_t_metas, obj_xcb_poly_fill_rectangle_request_t_bases, obj_xcb_poly_fill_rectangle_request_t_fields, obj_xcb_poly_fill_rectangle_request_t_constants, obj_xcb_poly_fill_rectangle_request_t_implements, 0},
  { &(obj_type_xcb_poly_fill_arc_request_t), REG_OBJECT, obj_xcb_poly_fill_arc_request_t_pub_funcs, obj_xcb_poly_fill_arc_request_t_methods, obj_xcb_poly_fill_arc_request_t_metas, obj_xcb_poly_fill_arc_request_t_bases, obj_xcb_poly_fill_arc_request_t_fields, obj_xcb_poly_fill_arc_request_t_constants, obj_xcb_poly_fill_arc_request_t_implements, 0},
  { &(obj_type_xcb_put_image_request_t), REG_OBJECT, obj_xcb_put_image_request_t_pub_funcs, obj_xcb_put_image_request_t_methods, obj_xcb_put_image_request_t_metas, obj_xcb_put_image_request_t_bases, obj_xcb_put_image_request_t_fields, obj_xcb_put_image_request_t_constants, obj_xcb_put_image_request_t_implements, 0},
  { &(obj_type_xcb_get_image_request_t), REG_OBJECT, obj_xcb_get_image_request_t_pub_funcs, obj_xcb_get_image_request_t_methods, obj_xcb_get_image_request_t_metas, obj_xcb_get_image_request_t_bases, obj_xcb_get_image_request_t_fields, obj_xcb_get_image_request_t_constants, obj_xcb_get_image_request_t_implements, 0},
  { &(obj_type_xcb_get_image_reply_t), REG_OBJECT, obj_xcb_get_image_reply_t_pub_funcs, obj_xcb_get_image_reply_t_methods, obj_xcb_get_image_reply_t_metas, obj_xcb_get_image_reply_t_bases, obj_xcb_get_image_reply_t_fields, obj_xcb_get_image_reply_t_constants, obj_xcb_get_image_reply_t_implements, 0},
  { &(obj_type_xcb_poly_text_8_request_t), REG_OBJECT, obj_xcb_poly_text_8_request_t_pub_funcs, obj_xcb_poly_text_8_request_t_methods, obj_xcb_poly_text_8_request_t_metas, obj_xcb_poly_text_8_request_t_bases, obj_xcb_poly_text_8_request_t_fields, obj_xcb_poly_text_8_request_t_constants, obj_xcb_poly_text_8_request_t_implements, 0},
  { &(obj_type_xcb_poly_text_16_request_t), REG_OBJECT, obj_xcb_poly_text_16_request_t_pub_funcs, obj_xcb_poly_text_16_request_t_methods, obj_xcb_poly_text_16_request_t_metas, obj_xcb_poly_text_16_request_t_bases, obj_xcb_poly_text_16_request_t_fields, obj_xcb_poly_text_16_request_t_constants, obj_xcb_poly_text_16_request_t_implements, 0},
  { &(obj_type_xcb_image_text_8_request_t), REG_OBJECT, obj_xcb_image_text_8_request_t_pub_funcs, obj_xcb_image_text_8_request_t_methods, obj_xcb_image_text_8_request_t_metas, obj_xcb_image_text_8_request_t_bases, obj_xcb_image_text_8_request_t_fields, obj_xcb_image_text_8_request_t_constants, obj_xcb_image_text_8_request_t_implements, 0},
  { &(obj_type_xcb_image_text_16_request_t), REG_OBJECT, obj_xcb_image_text_16_request_t_pub_funcs, obj_xcb_image_text_16_request_t_methods, obj_xcb_image_text_16_request_t_metas, obj_xcb_image_text_16_request_t_bases, obj_xcb_image_text_16_request_t_fields, obj_xcb_image_text_16_request_t_constants, obj_xcb_image_text_16_request_t_implements, 0},
  { &(obj_type_xcb_create_colormap_request_t), REG_OBJECT, obj_xcb_create_colormap_request_t_pub_funcs, obj_xcb_create_colormap_request_t_methods, obj_xcb_create_colormap_request_t_metas, obj_xcb_create_colormap_request_t_bases, obj_xcb_create_colormap_request_t_fields, obj_xcb_create_colormap_request_t_constants, obj_xcb_create_colormap_request_t_implements, 0},
  { &(obj_type_xcb_free_colormap_request_t), REG_OBJECT, obj_xcb_free_colormap_request_t_pub_funcs, obj_xcb_free_colormap_request_t_methods, obj_xcb_free_colormap_request_t_metas, obj_xcb_free_colormap_request_t_bases, obj_xcb_free_colormap_request_t_fields, obj_xcb_free_colormap_request_t_constants, obj_xcb_free_colormap_request_t_implements, 0},
  { &(obj_type_xcb_copy_colormap_and_free_request_t), REG_OBJECT, obj_xcb_copy_colormap_and_free_request_t_pub_funcs, obj_xcb_copy_colormap_and_free_request_t_methods, obj_xcb_copy_colormap_and_free_request_t_metas, obj_xcb_copy_colormap_and_free_request_t_bases, obj_xcb_copy_colormap_and_free_request_t_fields, obj_xcb_copy_colormap_and_free_request_t_constants, obj_xcb_copy_colormap_and_free_request_t_implements, 0},
  { &(obj_type_xcb_install_colormap_request_t), REG_OBJECT, obj_xcb_install_colormap_request_t_pub_funcs, obj_xcb_install_colormap_request_t_methods, obj_xcb_install_colormap_request_t_metas, obj_xcb_install_colormap_request_t_bases, obj_xcb_install_colormap_request_t_fields, obj_xcb_install_colormap_request_t_constants, obj_xcb_install_colormap_request_t_implements, 0},
  { &(obj_type_xcb_uninstall_colormap_request_t), REG_OBJECT, obj_xcb_uninstall_colormap_request_t_pub_funcs, obj_xcb_uninstall_colormap_request_t_methods, obj_xcb_uninstall_colormap_request_t_metas, obj_xcb_uninstall_colormap_request_t_bases, obj_xcb_uninstall_colormap_request_t_fields, obj_xcb_uninstall_colormap_request_t_constants, obj_xcb_uninstall_colormap_request_t_implements, 0},
  { &(obj_type_xcb_list_installed_colormaps_request_t), REG_OBJECT, obj_xcb_list_installed_colormaps_request_t_pub_funcs, obj_xcb_list_installed_colormaps_request_t_methods, obj_xcb_list_installed_colormaps_request_t_metas, obj_xcb_list_installed_colormaps_request_t_bases, obj_xcb_list_installed_colormaps_request_t_fields, obj_xcb_list_installed_colormaps_request_t_constants, obj_xcb_list_installed_colormaps_request_t_implements, 0},
  { &(obj_type_xcb_list_installed_colormaps_reply_t), REG_OBJECT, obj_xcb_list_installed_colormaps_reply_t_pub_funcs, obj_xcb_list_installed_colormaps_reply_t_methods, obj_xcb_list_installed_colormaps_reply_t_metas, obj_xcb_list_installed_colormaps_reply_t_bases, obj_xcb_list_installed_colormaps_reply_t_fields, obj_xcb_list_installed_colormaps_reply_t_constants, obj_xcb_list_installed_colormaps_reply_t_implements, 0},
  { &(obj_type_xcb_alloc_color_request_t), REG_OBJECT, obj_xcb_alloc_color_request_t_pub_funcs, obj_xcb_alloc_color_request_t_methods, obj_xcb_alloc_color_request_t_metas, obj_xcb_alloc_color_request_t_bases, obj_xcb_alloc_color_request_t_fields, obj_xcb_alloc_color_request_t_constants, obj_xcb_alloc_color_request_t_implements, 0},
  { &(obj_type_xcb_alloc_color_reply_t), REG_OBJECT, obj_xcb_alloc_color_reply_t_pub_funcs, obj_xcb_alloc_color_reply_t_methods, obj_xcb_alloc_color_reply_t_metas, obj_xcb_alloc_color_reply_t_bases, obj_xcb_alloc_color_reply_t_fields, obj_xcb_alloc_color_reply_t_constants, obj_xcb_alloc_color_reply_t_implements, 0},
  { &(obj_type_xcb_alloc_named_color_request_t), REG_OBJECT, obj_xcb_alloc_named_color_request_t_pub_funcs, obj_xcb_alloc_named_color_request_t_methods, obj_xcb_alloc_named_color_request_t_metas, obj_xcb_alloc_named_color_request_t_bases, obj_xcb_alloc_named_color_request_t_fields, obj_xcb_alloc_named_color_request_t_constants, obj_xcb_alloc_named_color_request_t_implements, 0},
  { &(obj_type_xcb_alloc_named_color_reply_t), REG_OBJECT, obj_xcb_alloc_named_color_reply_t_pub_funcs, obj_xcb_alloc_named_color_reply_t_methods, obj_xcb_alloc_named_color_reply_t_metas, obj_xcb_alloc_named_color_reply_t_bases, obj_xcb_alloc_named_color_reply_t_fields, obj_xcb_alloc_named_color_reply_t_constants, obj_xcb_alloc_named_color_reply_t_implements, 0},
  { &(obj_type_xcb_alloc_color_cells_request_t), REG_OBJECT, obj_xcb_alloc_color_cells_request_t_pub_funcs, obj_xcb_alloc_color_cells_request_t_methods, obj_xcb_alloc_color_cells_request_t_metas, obj_xcb_alloc_color_cells_request_t_bases, obj_xcb_alloc_color_cells_request_t_fields, obj_xcb_alloc_color_cells_request_t_constants, obj_xcb_alloc_color_cells_request_t_implements, 0},
  { &(obj_type_xcb_alloc_color_cells_reply_t), REG_OBJECT, obj_xcb_alloc_color_cells_reply_t_pub_funcs, obj_xcb_alloc_color_cells_reply_t_methods, obj_xcb_alloc_color_cells_reply_t_metas, obj_xcb_alloc_color_cells_reply_t_bases, obj_xcb_alloc_color_cells_reply_t_fields, obj_xcb_alloc_color_cells_reply_t_constants, obj_xcb_alloc_color_cells_reply_t_implements, 0},
  { &(obj_type_xcb_alloc_color_planes_request_t), REG_OBJECT, obj_xcb_alloc_color_planes_request_t_pub_funcs, obj_xcb_alloc_color_planes_request_t_methods, obj_xcb_alloc_color_planes_request_t_metas, obj_xcb_alloc_color_planes_request_t_bases, obj_xcb_alloc_color_planes_request_t_fields, obj_xcb_alloc_color_planes_request_t_constants, obj_xcb_alloc_color_planes_request_t_implements, 0},
  { &(obj_type_xcb_alloc_color_planes_reply_t), REG_OBJECT, obj_xcb_alloc_color_planes_reply_t_pub_funcs, obj_xcb_alloc_color_planes_reply_t_methods, obj_xcb_alloc_color_planes_reply_t_metas, obj_xcb_alloc_color_planes_reply_t_bases, obj_xcb_alloc_color_planes_reply_t_fields, obj_xcb_alloc_color_planes_reply_t_constants, obj_xcb_alloc_color_planes_reply_t_implements, 0},
  { &(obj_type_xcb_free_colors_request_t), REG_OBJECT, obj_xcb_free_colors_request_t_pub_funcs, obj_xcb_free_colors_request_t_methods, obj_xcb_free_colors_request_t_metas, obj_xcb_free_colors_request_t_bases, obj_xcb_free_colors_request_t_fields, obj_xcb_free_colors_request_t_constants, obj_xcb_free_colors_request_t_implements, 0},
  { &(obj_type_xcb_coloritem_t), REG_OBJECT, obj_xcb_coloritem_t_pub_funcs, obj_xcb_coloritem_t_methods, obj_xcb_coloritem_t_metas, obj_xcb_coloritem_t_bases, obj_xcb_coloritem_t_fields, obj_xcb_coloritem_t_constants, obj_xcb_coloritem_t_implements, 0},
  { &(obj_type_xcb_store_colors_request_t), REG_OBJECT, obj_xcb_store_colors_request_t_pub_funcs, obj_xcb_store_colors_request_t_methods, obj_xcb_store_colors_request_t_metas, obj_xcb_store_colors_request_t_bases, obj_xcb_store_colors_request_t_fields, obj_xcb_store_colors_request_t_constants, obj_xcb_store_colors_request_t_implements, 0},
  { &(obj_type_xcb_store_named_color_request_t), REG_OBJECT, obj_xcb_store_named_color_request_t_pub_funcs, obj_xcb_store_named_color_request_t_methods, obj_xcb_store_named_color_request_t_metas, obj_xcb_store_named_color_request_t_bases, obj_xcb_store_named_color_request_t_fields, obj_xcb_store_named_color_request_t_constants, obj_xcb_store_named_color_request_t_implements, 0},
  { &(obj_type_xcb_rgb_t), REG_OBJECT, obj_xcb_rgb_t_pub_funcs, obj_xcb_rgb_t_methods, obj_xcb_rgb_t_metas, obj_xcb_rgb_t_bases, obj_xcb_rgb_t_fields, obj_xcb_rgb_t_constants, obj_xcb_rgb_t_implements, 0},
  { &(obj_type_xcb_query_colors_request_t), REG_OBJECT, obj_xcb_query_colors_request_t_pub_funcs, obj_xcb_query_colors_request_t_methods, obj_xcb_query_colors_request_t_metas, obj_xcb_query_colors_request_t_bases, obj_xcb_query_colors_request_t_fields, obj_xcb_query_colors_request_t_constants, obj_xcb_query_colors_request_t_implements, 0},
  { &(obj_type_xcb_query_colors_reply_t), REG_OBJECT, obj_xcb_query_colors_reply_t_pub_funcs, obj_xcb_query_colors_reply_t_methods, obj_xcb_query_colors_reply_t_metas, obj_xcb_query_colors_reply_t_bases, obj_xcb_query_colors_reply_t_fields, obj_xcb_query_colors_reply_t_constants, obj_xcb_query_colors_reply_t_implements, 0},
  { &(obj_type_xcb_lookup_color_request_t), REG_OBJECT, obj_xcb_lookup_color_request_t_pub_funcs, obj_xcb_lookup_color_request_t_methods, obj_xcb_lookup_color_request_t_metas, obj_xcb_lookup_color_request_t_bases, obj_xcb_lookup_color_request_t_fields, obj_xcb_lookup_color_request_t_constants, obj_xcb_lookup_color_request_t_implements, 0},
  { &(obj_type_xcb_lookup_color_reply_t), REG_OBJECT, obj_xcb_lookup_color_reply_t_pub_funcs, obj_xcb_lookup_color_reply_t_methods, obj_xcb_lookup_color_reply_t_metas, obj_xcb_lookup_color_reply_t_bases, obj_xcb_lookup_color_reply_t_fields, obj_xcb_lookup_color_reply_t_constants, obj_xcb_lookup_color_reply_t_implements, 0},
  { &(obj_type_xcb_create_cursor_request_t), REG_OBJECT, obj_xcb_create_cursor_request_t_pub_funcs, obj_xcb_create_cursor_request_t_methods, obj_xcb_create_cursor_request_t_metas, obj_xcb_create_cursor_request_t_bases, obj_xcb_create_cursor_request_t_fields, obj_xcb_create_cursor_request_t_constants, obj_xcb_create_cursor_request_t_implements, 0},
  { &(obj_type_xcb_create_glyph_cursor_request_t), REG_OBJECT, obj_xcb_create_glyph_cursor_request_t_pub_funcs, obj_xcb_create_glyph_cursor_request_t_methods, obj_xcb_create_glyph_cursor_request_t_metas, obj_xcb_create_glyph_cursor_request_t_bases, obj_xcb_create_glyph_cursor_request_t_fields, obj_xcb_create_glyph_cursor_request_t_constants, obj_xcb_create_glyph_cursor_request_t_implements, 0},
  { &(obj_type_xcb_free_cursor_request_t), REG_OBJECT, obj_xcb_free_cursor_request_t_pub_funcs, obj_xcb_free_cursor_request_t_methods, obj_xcb_free_cursor_request_t_metas, obj_xcb_free_cursor_request_t_bases, obj_xcb_free_cursor_request_t_fields, obj_xcb_free_cursor_request_t_constants, obj_xcb_free_cursor_request_t_implements, 0},
  { &(obj_type_xcb_recolor_cursor_request_t), REG_OBJECT, obj_xcb_recolor_cursor_request_t_pub_funcs, obj_xcb_recolor_cursor_request_t_methods, obj_xcb_recolor_cursor_request_t_metas, obj_xcb_recolor_cursor_request_t_bases, obj_xcb_recolor_cursor_request_t_fields, obj_xcb_recolor_cursor_request_t_constants, obj_xcb_recolor_cursor_request_t_implements, 0},
  { &(obj_type_xcb_query_best_size_request_t), REG_OBJECT, obj_xcb_query_best_size_request_t_pub_funcs, obj_xcb_query_best_size_request_t_methods, obj_xcb_query_best_size_request_t_metas, obj_xcb_query_best_size_request_t_bases, obj_xcb_query_best_size_request_t_fields, obj_xcb_query_best_size_request_t_constants, obj_xcb_query_best_size_request_t_implements, 0},
  { &(obj_type_xcb_query_best_size_reply_t), REG_OBJECT, obj_xcb_query_best_size_reply_t_pub_funcs, obj_xcb_query_best_size_reply_t_methods, obj_xcb_query_best_size_reply_t_metas, obj_xcb_query_best_size_reply_t_bases, obj_xcb_query_best_size_reply_t_fields, obj_xcb_query_best_size_reply_t_constants, obj_xcb_query_best_size_reply_t_implements, 0},
  { &(obj_type_xcb_query_extension_request_t), REG_OBJECT, obj_xcb_query_extension_request_t_pub_funcs, obj_xcb_query_extension_request_t_methods, obj_xcb_query_extension_request_t_metas, obj_xcb_query_extension_request_t_bases, obj_xcb_query_extension_request_t_fields, obj_xcb_query_extension_request_t_constants, obj_xcb_query_extension_request_t_implements, 0},
  { &(obj_type_xcb_query_extension_reply_t), REG_OBJECT, obj_xcb_query_extension_reply_t_pub_funcs, obj_xcb_query_extension_reply_t_methods, obj_xcb_query_extension_reply_t_metas, obj_xcb_query_extension_reply_t_bases, obj_xcb_query_extension_reply_t_fields, obj_xcb_query_extension_reply_t_constants, obj_xcb_query_extension_reply_t_implements, 0},
  { &(obj_type_xcb_list_extensions_request_t), REG_OBJECT, obj_xcb_list_extensions_request_t_pub_funcs, obj_xcb_list_extensions_request_t_methods, obj_xcb_list_extensions_request_t_metas, obj_xcb_list_extensions_request_t_bases, obj_xcb_list_extensions_request_t_fields, obj_xcb_list_extensions_request_t_constants, obj_xcb_list_extensions_request_t_implements, 0},
  { &(obj_type_xcb_list_extensions_reply_t), REG_OBJECT, obj_xcb_list_extensions_reply_t_pub_funcs, obj_xcb_list_extensions_reply_t_methods, obj_xcb_list_extensions_reply_t_metas, obj_xcb_list_extensions_reply_t_bases, obj_xcb_list_extensions_reply_t_fields, obj_xcb_list_extensions_reply_t_constants, obj_xcb_list_extensions_reply_t_implements, 0},
  { &(obj_type_xcb_change_keyboard_mapping_request_t), REG_OBJECT, obj_xcb_change_keyboard_mapping_request_t_pub_funcs, obj_xcb_change_keyboard_mapping_request_t_methods, obj_xcb_change_keyboard_mapping_request_t_metas, obj_xcb_change_keyboard_mapping_request_t_bases, obj_xcb_change_keyboard_mapping_request_t_fields, obj_xcb_change_keyboard_mapping_request_t_constants, obj_xcb_change_keyboard_mapping_request_t_implements, 0},
  { &(obj_type_xcb_get_keyboard_mapping_request_t), REG_OBJECT, obj_xcb_get_keyboard_mapping_request_t_pub_funcs, obj_xcb_get_keyboard_mapping_request_t_methods, obj_xcb_get_keyboard_mapping_request_t_metas, obj_xcb_get_keyboard_mapping_request_t_bases, obj_xcb_get_keyboard_mapping_request_t_fields, obj_xcb_get_keyboard_mapping_request_t_constants, obj_xcb_get_keyboard_mapping_request_t_implements, 0},
  { &(obj_type_xcb_get_keyboard_mapping_reply_t), REG_OBJECT, obj_xcb_get_keyboard_mapping_reply_t_pub_funcs, obj_xcb_get_keyboard_mapping_reply_t_methods, obj_xcb_get_keyboard_mapping_reply_t_metas, obj_xcb_get_keyboard_mapping_reply_t_bases, obj_xcb_get_keyboard_mapping_reply_t_fields, obj_xcb_get_keyboard_mapping_reply_t_constants, obj_xcb_get_keyboard_mapping_reply_t_implements, 0},
  { &(obj_type_xcb_change_keyboard_control_request_t), REG_OBJECT, obj_xcb_change_keyboard_control_request_t_pub_funcs, obj_xcb_change_keyboard_control_request_t_methods, obj_xcb_change_keyboard_control_request_t_metas, obj_xcb_change_keyboard_control_request_t_bases, obj_xcb_change_keyboard_control_request_t_fields, obj_xcb_change_keyboard_control_request_t_constants, obj_xcb_change_keyboard_control_request_t_implements, 0},
  { &(obj_type_xcb_get_keyboard_control_request_t), REG_OBJECT, obj_xcb_get_keyboard_control_request_t_pub_funcs, obj_xcb_get_keyboard_control_request_t_methods, obj_xcb_get_keyboard_control_request_t_metas, obj_xcb_get_keyboard_control_request_t_bases, obj_xcb_get_keyboard_control_request_t_fields, obj_xcb_get_keyboard_control_request_t_constants, obj_xcb_get_keyboard_control_request_t_implements, 0},
  { &(obj_type_xcb_get_keyboard_control_reply_t), REG_OBJECT, obj_xcb_get_keyboard_control_reply_t_pub_funcs, obj_xcb_get_keyboard_control_reply_t_methods, obj_xcb_get_keyboard_control_reply_t_metas, obj_xcb_get_keyboard_control_reply_t_bases, obj_xcb_get_keyboard_control_reply_t_fields, obj_xcb_get_keyboard_control_reply_t_constants, obj_xcb_get_keyboard_control_reply_t_implements, 0},
  { &(obj_type_xcb_bell_request_t), REG_OBJECT, obj_xcb_bell_request_t_pub_funcs, obj_xcb_bell_request_t_methods, obj_xcb_bell_request_t_metas, obj_xcb_bell_request_t_bases, obj_xcb_bell_request_t_fields, obj_xcb_bell_request_t_constants, obj_xcb_bell_request_t_implements, 0},
  { &(obj_type_xcb_change_pointer_control_request_t), REG_OBJECT, obj_xcb_change_pointer_control_request_t_pub_funcs, obj_xcb_change_pointer_control_request_t_methods, obj_xcb_change_pointer_control_request_t_metas, obj_xcb_change_pointer_control_request_t_bases, obj_xcb_change_pointer_control_request_t_fields, obj_xcb_change_pointer_control_request_t_constants, obj_xcb_change_pointer_control_request_t_implements, 0},
  { &(obj_type_xcb_get_pointer_control_request_t), REG_OBJECT, obj_xcb_get_pointer_control_request_t_pub_funcs, obj_xcb_get_pointer_control_request_t_methods, obj_xcb_get_pointer_control_request_t_metas, obj_xcb_get_pointer_control_request_t_bases, obj_xcb_get_pointer_control_request_t_fields, obj_xcb_get_pointer_control_request_t_constants, obj_xcb_get_pointer_control_request_t_implements, 0},
  { &(obj_type_xcb_get_pointer_control_reply_t), REG_OBJECT, obj_xcb_get_pointer_control_reply_t_pub_funcs, obj_xcb_get_pointer_control_reply_t_methods, obj_xcb_get_pointer_control_reply_t_metas, obj_xcb_get_pointer_control_reply_t_bases, obj_xcb_get_pointer_control_reply_t_fields, obj_xcb_get_pointer_control_reply_t_constants, obj_xcb_get_pointer_control_reply_t_implements, 0},
  { &(obj_type_xcb_set_screen_saver_request_t), REG_OBJECT, obj_xcb_set_screen_saver_request_t_pub_funcs, obj_xcb_set_screen_saver_request_t_methods, obj_xcb_set_screen_saver_request_t_metas, obj_xcb_set_screen_saver_request_t_bases, obj_xcb_set_screen_saver_request_t_fields, obj_xcb_set_screen_saver_request_t_constants, obj_xcb_set_screen_saver_request_t_implements, 0},
  { &(obj_type_xcb_get_screen_saver_request_t), REG_OBJECT, obj_xcb_get_screen_saver_request_t_pub_funcs, obj_xcb_get_screen_saver_request_t_methods, obj_xcb_get_screen_saver_request_t_metas, obj_xcb_get_screen_saver_request_t_bases, obj_xcb_get_screen_saver_request_t_fields, obj_xcb_get_screen_saver_request_t_constants, obj_xcb_get_screen_saver_request_t_implements, 0},
  { &(obj_type_xcb_get_screen_saver_reply_t), REG_OBJECT, obj_xcb_get_screen_saver_reply_t_pub_funcs, obj_xcb_get_screen_saver_reply_t_methods, obj_xcb_get_screen_saver_reply_t_metas, obj_xcb_get_screen_saver_reply_t_bases, obj_xcb_get_screen_saver_reply_t_fields, obj_xcb_get_screen_saver_reply_t_constants, obj_xcb_get_screen_saver_reply_t_implements, 0},
  { &(obj_type_xcb_change_hosts_request_t), REG_OBJECT, obj_xcb_change_hosts_request_t_pub_funcs, obj_xcb_change_hosts_request_t_methods, obj_xcb_change_hosts_request_t_metas, obj_xcb_change_hosts_request_t_bases, obj_xcb_change_hosts_request_t_fields, obj_xcb_change_hosts_request_t_constants, obj_xcb_change_hosts_request_t_implements, 0},
  { &(obj_type_xcb_host_t), REG_OBJECT, obj_xcb_host_t_pub_funcs, obj_xcb_host_t_methods, obj_xcb_host_t_metas, obj_xcb_host_t_bases, obj_xcb_host_t_fields, obj_xcb_host_t_constants, obj_xcb_host_t_implements, 0},
  { &(obj_type_xcb_list_hosts_request_t), REG_OBJECT, obj_xcb_list_hosts_request_t_pub_funcs, obj_xcb_list_hosts_request_t_methods, obj_xcb_list_hosts_request_t_metas, obj_xcb_list_hosts_request_t_bases, obj_xcb_list_hosts_request_t_fields, obj_xcb_list_hosts_request_t_constants, obj_xcb_list_hosts_request_t_implements, 0},
  { &(obj_type_xcb_list_hosts_reply_t), REG_OBJECT, obj_xcb_list_hosts_reply_t_pub_funcs, obj_xcb_list_hosts_reply_t_methods, obj_xcb_list_hosts_reply_t_metas, obj_xcb_list_hosts_reply_t_bases, obj_xcb_list_hosts_reply_t_fields, obj_xcb_list_hosts_reply_t_constants, obj_xcb_list_hosts_reply_t_implements, 0},
  { &(obj_type_xcb_set_access_control_request_t), REG_OBJECT, obj_xcb_set_access_control_request_t_pub_funcs, obj_xcb_set_access_control_request_t_methods, obj_xcb_set_access_control_request_t_metas, obj_xcb_set_access_control_request_t_bases, obj_xcb_set_access_control_request_t_fields, obj_xcb_set_access_control_request_t_constants, obj_xcb_set_access_control_request_t_implements, 0},
  { &(obj_type_xcb_set_close_down_mode_request_t), REG_OBJECT, obj_xcb_set_close_down_mode_request_t_pub_funcs, obj_xcb_set_close_down_mode_request_t_methods, obj_xcb_set_close_down_mode_request_t_metas, obj_xcb_set_close_down_mode_request_t_bases, obj_xcb_set_close_down_mode_request_t_fields, obj_xcb_set_close_down_mode_request_t_constants, obj_xcb_set_close_down_mode_request_t_implements, 0},
  { &(obj_type_xcb_kill_client_request_t), REG_OBJECT, obj_xcb_kill_client_request_t_pub_funcs, obj_xcb_kill_client_request_t_methods, obj_xcb_kill_client_request_t_metas, obj_xcb_kill_client_request_t_bases, obj_xcb_kill_client_request_t_fields, obj_xcb_kill_client_request_t_constants, obj_xcb_kill_client_request_t_implements, 0},
  { &(obj_type_xcb_rotate_properties_request_t), REG_OBJECT, obj_xcb_rotate_properties_request_t_pub_funcs, obj_xcb_rotate_properties_request_t_methods, obj_xcb_rotate_properties_request_t_metas, obj_xcb_rotate_properties_request_t_bases, obj_xcb_rotate_properties_request_t_fields, obj_xcb_rotate_properties_request_t_constants, obj_xcb_rotate_properties_request_t_implements, 0},
  { &(obj_type_xcb_force_screen_saver_request_t), REG_OBJECT, obj_xcb_force_screen_saver_request_t_pub_funcs, obj_xcb_force_screen_saver_request_t_methods, obj_xcb_force_screen_saver_request_t_metas, obj_xcb_force_screen_saver_request_t_bases, obj_xcb_force_screen_saver_request_t_fields, obj_xcb_force_screen_saver_request_t_constants, obj_xcb_force_screen_saver_request_t_implements, 0},
  { &(obj_type_xcb_set_pointer_mapping_request_t), REG_OBJECT, obj_xcb_set_pointer_mapping_request_t_pub_funcs, obj_xcb_set_pointer_mapping_request_t_methods, obj_xcb_set_pointer_mapping_request_t_metas, obj_xcb_set_pointer_mapping_request_t_bases, obj_xcb_set_pointer_mapping_request_t_fields, obj_xcb_set_pointer_mapping_request_t_constants, obj_xcb_set_pointer_mapping_request_t_implements, 0},
  { &(obj_type_xcb_set_pointer_mapping_reply_t), REG_OBJECT, obj_xcb_set_pointer_mapping_reply_t_pub_funcs, obj_xcb_set_pointer_mapping_reply_t_methods, obj_xcb_set_pointer_mapping_reply_t_metas, obj_xcb_set_pointer_mapping_reply_t_bases, obj_xcb_set_pointer_mapping_reply_t_fields, obj_xcb_set_pointer_mapping_reply_t_constants, obj_xcb_set_pointer_mapping_reply_t_implements, 0},
  { &(obj_type_xcb_get_pointer_mapping_request_t), REG_OBJECT, obj_xcb_get_pointer_mapping_request_t_pub_funcs, obj_xcb_get_pointer_mapping_request_t_methods, obj_xcb_get_pointer_mapping_request_t_metas, obj_xcb_get_pointer_mapping_request_t_bases, obj_xcb_get_pointer_mapping_request_t_fields, obj_xcb_get_pointer_mapping_request_t_constants, obj_xcb_get_pointer_mapping_request_t_implements, 0},
  { &(obj_type_xcb_get_pointer_mapping_reply_t), REG_OBJECT, obj_xcb_get_pointer_mapping_reply_t_pub_funcs, obj_xcb_get_pointer_mapping_reply_t_methods, obj_xcb_get_pointer_mapping_reply_t_metas, obj_xcb_get_pointer_mapping_reply_t_bases, obj_xcb_get_pointer_mapping_reply_t_fields, obj_xcb_get_pointer_mapping_reply_t_constants, obj_xcb_get_pointer_mapping_reply_t_implements, 0},
  { &(obj_type_xcb_set_modifier_mapping_request_t), REG_OBJECT, obj_xcb_set_modifier_mapping_request_t_pub_funcs, obj_xcb_set_modifier_mapping_request_t_methods, obj_xcb_set_modifier_mapping_request_t_metas, obj_xcb_set_modifier_mapping_request_t_bases, obj_xcb_set_modifier_mapping_request_t_fields, obj_xcb_set_modifier_mapping_request_t_constants, obj_xcb_set_modifier_mapping_request_t_implements, 0},
  { &(obj_type_xcb_set_modifier_mapping_reply_t), REG_OBJECT, obj_xcb_set_modifier_mapping_reply_t_pub_funcs, obj_xcb_set_modifier_mapping_reply_t_methods, obj_xcb_set_modifier_mapping_reply_t_metas, obj_xcb_set_modifier_mapping_reply_t_bases, obj_xcb_set_modifier_mapping_reply_t_fields, obj_xcb_set_modifier_mapping_reply_t_constants, obj_xcb_set_modifier_mapping_reply_t_implements, 0},
  { &(obj_type_xcb_get_modifier_mapping_request_t), REG_OBJECT, obj_xcb_get_modifier_mapping_request_t_pub_funcs, obj_xcb_get_modifier_mapping_request_t_methods, obj_xcb_get_modifier_mapping_request_t_metas, obj_xcb_get_modifier_mapping_request_t_bases, obj_xcb_get_modifier_mapping_request_t_fields, obj_xcb_get_modifier_mapping_request_t_constants, obj_xcb_get_modifier_mapping_request_t_implements, 0},
  { &(obj_type_xcb_get_modifier_mapping_reply_t), REG_OBJECT, obj_xcb_get_modifier_mapping_reply_t_pub_funcs, obj_xcb_get_modifier_mapping_reply_t_methods, obj_xcb_get_modifier_mapping_reply_t_metas, obj_xcb_get_modifier_mapping_reply_t_bases, obj_xcb_get_modifier_mapping_reply_t_fields, obj_xcb_get_modifier_mapping_reply_t_constants, obj_xcb_get_modifier_mapping_reply_t_implements, 0},
  { &(obj_type_xcb_no_operation_request_t), REG_OBJECT, obj_xcb_no_operation_request_t_pub_funcs, obj_xcb_no_operation_request_t_methods, obj_xcb_no_operation_request_t_metas, obj_xcb_no_operation_request_t_bases, obj_xcb_no_operation_request_t_fields, obj_xcb_no_operation_request_t_constants, obj_xcb_no_operation_request_t_implements, 0},
  { &(obj_type_xcb_connection_t), REG_OBJECT, obj_xcb_connection_t_pub_funcs, obj_xcb_connection_t_methods, obj_xcb_connection_t_metas, obj_xcb_connection_t_bases, obj_xcb_connection_t_fields, obj_xcb_connection_t_constants, obj_xcb_connection_t_implements, 0},
  {NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0}
};





#if LUAJIT_FFI
static const ffi_export_symbol xcb_ffi_export[] = {
  {NULL, { NULL } }
};
#endif




static const luaL_Reg submodule_libs[] = {
  {NULL, NULL}
};



static void create_object_instance_cache(lua_State *L) {
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(!lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop weak table. */
		return;
	}
	lua_pop(L, 1); /* pop nil. */
	/* create weak table for object instance references. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_newtable(L);               /* weak table. */
	lua_newtable(L);               /* metatable for weak table. */
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "v");
	lua_rawset(L, -3);             /* metatable.__mode = 'v'  weak values. */
	lua_setmetatable(L, -2);       /* add metatable to weak table. */
	lua_rawset(L, LUA_REGISTRYINDEX);  /* create reference to weak table. */
}

LUA_NOBJ_API int luaopen_xcb(lua_State *L) {
	const reg_sub_module *reg = reg_sub_modules;
	const luaL_Reg *submodules = submodule_libs;
	int priv_table = -1;

	/* register interfaces */
	obj_register_interfaces(L, obj_interfaces);

	/* private table to hold reference to object metatables. */
	lua_newtable(L);
	priv_table = lua_gettop(L);
	lua_pushlightuserdata(L, obj_udata_private_key);
	lua_pushvalue(L, priv_table);
	lua_rawset(L, LUA_REGISTRYINDEX);  /* store private table in registry. */

	/* create object cache. */
	create_object_instance_cache(L);

	/* module table. */
#if REG_MODULES_AS_GLOBALS
	luaL_register(L, "xcb", xcb_function);
#else
	lua_newtable(L);
	luaL_setfuncs(L, xcb_function, 0);
#endif

	/* register module constants. */
	obj_type_register_constants(L, xcb_constants, -1, 0);

	for(; submodules->func != NULL ; submodules++) {
		lua_pushcfunction(L, submodules->func);
		lua_pushstring(L, submodules->name);
		lua_call(L, 1, 0);
	}

	/* register objects */
	for(; reg->type != NULL ; reg++) {
		lua_newtable(L); /* create public API table for object. */
		lua_pushvalue(L, -1); /* dup. object's public API table. */
		lua_setfield(L, -3, reg->type->name); /* module["<object_name>"] = <object public API> */
#if REG_OBJECTS_AS_GLOBALS
		lua_pushvalue(L, -1);                 /* dup value. */
		lua_setglobal(L, reg->type->name);    /* global: <object_name> = <object public API> */
#endif
		obj_type_register(L, reg, priv_table);
	}

#if LUAJIT_FFI
	if(nobj_check_ffi_support(L)) {
		nobj_try_loading_ffi(L, "xcb.nobj.ffi.lua", xcb_ffi_lua_code,
			xcb_ffi_export, priv_table);
	}
#endif



	return 1;
}


