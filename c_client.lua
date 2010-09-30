oo = require("loop.simple")
rex = require('rex_pcre')

require('xcbgen.dom')
require('xcbgen.expr')
require('xcbgen.matcher')
require('xcbgen.state')
require('xcbgen.xtypes')

local output_dir = nil
local mod = nil

local warn = function(...)
	io.stderr:write(table.concat({...},'\t') .. '\n')
end

-- Jump to the bottom of this file for the main routine

-- Some hacks to make the API more readable, and to keep backwards compability
_cname_re = '([A-Z0-9][a-z]+|[A-Z0-9]+(?![a-z])|[a-z]+)'
_cname_special_cases = {DECnet='decnet'}

_extension_special_cases = {
	XPrint = true, XCMisc = true, BigRequests = true
}

_cplusplus_annoyances = {
	class = '_class',
	new   = '_new',
	delete= '_delete'
}

_hlines = {}
_hlevel = 0
_clines = {}
_clevel = 0
_ns = nil

local function tabcat(tab, ...)
	local tab = {unpack(tab)}
	for _,v in ipairs(arg) do
		if type(v) == 'table' then
			for _,v2 in ipairs(v) do
				table.insert(tab, v2)
			end
		else
			table.insert(tab, v)
		end
	end
	return tab
end

local function _h(fmt, ...)
	--[[
	Writes the given line to the header file.
	]]
	table.insert(_hlines[_hlevel], fmt:format(...))
end

local function _c(fmt, ...)
	--[[
	Writes the given line to the source file.
	]]
	table.insert(_clines[_clevel], fmt:format(...))
end

local function _hc(fmt, ...)
	--[[
	Writes the given line to both the header and source files.
	]]
	_h(fmt, ...)
	_c(fmt, ...)
end

-- XXX See if this level thing is really necessary.
local function _h_setlevel(idx)
	--[[
	Changes the array that header lines are written to.
	Supports writing different sections of the header file.
	]]
	while #_hlines <= idx do
		table.insert(_hlines, {})
	end
	_hlevel = idx
end

local function _c_setlevel(idx)
	--[[
	Changes the array that source lines are written to.
	Supports writing to different sections of the source file.
	]]
	while #_clines <= idx do
		table.insert(_clines, {})
	end
	_clevel = idx
end

local function _n_item(str)
	--[[
	Does C-name conversion on a single string fragment.
	Uses a regexp with some hard-coded special cases.
	]]
	if _cname_special_cases[str] ~= nil then
		return _cname_special_cases[str]
	else
		local name_parts = {}
		for match in rex.gmatch(str, _cname_re) do
			table.insert(name_parts, match)
		end
		return table.concat(name_parts, '_')
	end
end

local function _cpp(str)
	--[[
	Checks for certain C++ reserved words and fixes them.
	]]
	if _cplusplus_annoyances[str] ~= nil then
		return _cplusplus_annoyances[str]
	else
		return str
	end
end

local function _ext(str)
	--[[
	Does C-name conversion on an extension name.
	Has some additional special cases on top of _n_item.
	]]
	if _extension_special_cases[str] then
		return _n_item(str):lower()
	else
		return str:lower()
	end
end

local function _n(list)
	--[[
	Does C-name conversion on a tuple of strings.
	Different behavior depending on length of tuple, extension/not extension, etc.
	Basically C-name converts the individual pieces, then joins with underscores.
	]]
	local parts
	if #list == 1 then
		parts = list
	elseif #list == 2 then
		parts = {list[1], _n_item(list[2])}
	else
		parts = {}
		for i,v in ipairs(list) do
			parts[i] = _n_item(v)
			if i == 2 then
				if _ns.is_ext then
					parts[i] = _ext(v)
				end
			end
		end
	end

	return table.concat(parts, '_'):lower()
end

local function _t(list)
	--[[
	Does C-name conversion on a tuple of strings representing a type.
	Same as _n but adds a "_t" on the end.
	]]
	if #list == 1 then
		return (list[1]):lower()
	end

	return _n(list) .. '_t'
end

local function c_open(self)
	--[[
	Exported function that handles module open.
	Opens the files and writes out the auto-generated comment, header file includes, etc.
	]]
	_ns = self.namespace
	_ns.c_ext_global_name = _n(tabcat(_ns.prefix, 'id'))

	-- Build the type-name collision avoidance table used by c_enum
	build_collision_table()

	_h_setlevel(1)
	_c_setlevel(1)

	_hc('/*')
	_hc(' * This file generated automatically from %s by c_client.py.', _ns.file)
	_hc(' * Edit at your peril.')
	_hc(' */')
	_hc('')

	_h('/**')
	_h(' * @defgroup XCB_%s_API XCB %s API', _ns.ext_name, _ns.ext_name)
	_h(' * @brief %s XCB Protocol Implementation.', _ns.ext_name)
	_h(' * @{')
	_h(' **/')
	_h('')
	_h('#ifndef __%s_H', _ns.header:upper())
	_h('#define __%s_H', _ns.header:upper())
	_h('')
	_h('#include "xcb.h"')

	_c('#include <string.h>')
	_c('#include <assert.h>')
	_c('#include "xcbext.h"')
	_c('#include "%s.h"', _ns.header)

	if _ns.is_ext then
		for _,h in ipairs(self.import_list) do
			_hc('#include "%s.h"', h)
		end
	end

	_h('')
	_h('#ifdef __cplusplus')
	_h('extern "C" {')
	_h('#endif')

	if _ns.is_ext then
		_h('')
		_h('#define XCB_%s_MAJOR_VERSION %s', _ns.ext_name:upper(), _ns.major_version)
		_h('#define XCB_%s_MINOR_VERSION %s', _ns.ext_name:upper(), _ns.minor_version)
		_h('  ') -- XXX
		_h('extern xcb_extension_t %s;', _ns.c_ext_global_name)

		_c('')
		_c('xcb_extension_t %s = { "%s", 0 };', _ns.c_ext_global_name, _ns.ext_xname)
	end
end

local function c_close(self)
	--[[
	Exported function that handles module close.
	Writes out all the stored content lines, then closes the files.
	]]
	_h_setlevel(3)
	_c_setlevel(3)
	_hc('')

	_h('')
	_h('#ifdef __cplusplus')
	_h('}')
	_h('#endif')

	_h('')
	_h('#endif')
	_h('')
	_h('/**')
	_h(' * @}')
	_h(' */')

	-- Write header file
	local hfile = assert(io.open(('%s/%s.h'):format(output_dir, _ns.header), 'w'))
	for i,list in ipairs(_hlines) do
		for i,line in ipairs(list) do
			hfile:write(line)
			hfile:write('\n')
		end
	end
	hfile:close()

	-- Write source file
	local cfile = assert(io.open(('%s/%s.c'):format(output_dir, _ns.header), 'w'))
	for i,list in ipairs(_clines) do
		for i,line in ipairs(list) do
			cfile:write(line)
			cfile:write('\n')
		end
	end
	cfile:close()
end

namecount = {}
function build_collision_table()
	namecount = {}
	for k,v in pairs(mod.types) do
		local name = _t(v[1])
		namecount[name] = (namecount[name] or 0) + 1
	end
end

local function c_enum(self, name)
	--[[
	Exported function that handles enum declarations.
	]]

	local tname = _t(name)
	if (namecount[tname] or 0) > 1 then
		tname = _t(tabcat(name, 'enum'))
	end

	_h_setlevel(1)
	_h('')
	_h('typedef enum %s {', tname)

	local count = #self.values

	for _,val in ipairs(self.values) do
		local enam, eval = unpack(val)
		count = count - 1
		local equals = (((eval ~= '') and ' = ') or '')
		local comma = (((count > 0) and ',') or '')
		_h('    %s%s%s%s', _n(tabcat(name, enam)):upper(), equals, eval, comma)
	end

	_h('} %s;', tname)
end

local function _c_type_setup(self, name, postfix, depth)
	--[[
	Sets up all the C-related state by adding additional data fields to
	all Field and Type objects.  Here is where we figure out most of our
	variable and function names.

	Recurses into child fields and list member types.
	]]
	depth = depth or 0
	-- Do all the various names in advance
	self.c_type = _t(tabcat(name, postfix))
	self.c_wiretype = 'char'
	if self.c_type ~= 'void' then
		self.c_wiretype = self.c_type
	end

	self.c_iterator_type = _t(tabcat(name, 'iterator'))
	self.c_next_name = _n(tabcat(name, 'next'))
	self.c_end_name = _n(tabcat(name, 'end'))

	self.c_request_name = _n(name)
	self.c_checked_name = _n(tabcat(name, 'checked'))
	self.c_unchecked_name = _n(tabcat(name, 'unchecked'))
	self.c_reply_name = _n(tabcat(name, 'reply'))
	self.c_reply_type = _t(tabcat(name, 'reply'))
	self.c_cookie_type = _t(tabcat(name, 'cookie'))

	if self.is_container then

		self.c_container = ((self.is_union and 'union') or 'struct')
		local prev_varsized_field = nil
		local prev_varsized_offset = 0
		local first_field_after_varsized = nil

		for _,field in ipairs(self.fields) do
			_c_type_setup(field.type_, field.field_type, {}, depth + 1)
			if field.type_.is_list then
				_c_type_setup(field.type_.member, field.field_type, {}, depth + 1)
			end

			field.c_field_type = _t(field.field_type)
			field.c_field_const_type = ((field.type_.nmemb == 1 and '') or 'const ') .. field.c_field_type
			field.c_field_name = _cpp(field.field_name)
			field.c_subscript = (((field.type_.nmemb > 1) and ('[%d]'):format(field.type_.nmemb)) or '')
			field.c_pointer = ((field.type_.nmemb == 1 and ' ') or '*')

			field.c_iterator_type = _t(tabcat(field.field_type, 'iterator'))	  -- xcb_fieldtype_iterator_t
			field.c_iterator_name = _n(tabcat(name, field.field_name, 'iterator')) -- xcb_container_field_iterator
			field.c_accessor_name = _n(tabcat(name, field.field_name))			-- xcb_container_field
			field.c_length_name = _n(tabcat(name, field.field_name, 'length'))	 -- xcb_container_field_length
			field.c_end_name = _n(tabcat(name, field.field_name, 'end'))		   -- xcb_container_field_end

			field.prev_varsized_field = prev_varsized_field
			field.prev_varsized_offset = prev_varsized_offset

			if prev_varsized_offset == 0 then
				first_field_after_varsized = field
			end
			field.first_field_after_varsized = first_field_after_varsized

			if field.type_:fixed_size() then
				prev_varsized_offset = prev_varsized_offset + field.type_.size
			else
				self.last_varsized_field = field
				prev_varsized_field = field
				prev_varsized_offset = 0
			end
		end
	end
end

local function _c_iterator_get_end(field, accum)
	--[[
	Figures out what C code is needed to find the end of a variable-length structure field.
	For nested structures, recurses into its last variable-sized field.
	For lists, calls the end function
	]]
	if field.type_.is_container then
		accum = field.c_accessor_name .. '(' .. accum .. ')'
		-- XXX there could be fixed-length fields at the end
		return _c_iterator_get_end(field.type_.last_varsized_field, accum)
	end
	if field.type_.is_list then
		-- XXX we can always use the first way
		if field.type_.member.is_simple then
			return field.c_end_name .. '(' .. accum .. ')'
		else
			return field.type_.member.c_end_name .. '(' .. field.c_iterator_name .. '(' .. accum .. '))'
		end
	end
end

local function _c_iterator(self, name)
	--[[
	Declares the iterator structure and next/end functions for a given type.
	]]
	_h_setlevel(1)
	_h('')
	_h('/**')
	_h(' * @brief %s', self.c_iterator_type)
	_h(' **/')
	_h('typedef struct %s {', self.c_iterator_type)
	_h('    %s *data; /**<  */', self.c_type)
	_h('    int%s rem; /**<  */', (' '):rep(#self.c_type - 2))
	_h('    int%s index; /**<  */', (' '):rep(#self.c_type - 2))
	_h('} %s;', self.c_iterator_type)

	_h_setlevel(2)
	_c_setlevel(2)
	_h('')
	_h('/**')
	_h(' * Get the next element of the iterator')
	_h(' * @param i Pointer to a %s', self.c_iterator_type)
	_h(' *')
	_h(' * Get the next element in the iterator. The member rem is')
	_h(' * decreased by one. The member data points to the next')
	_h(' * element. The member index is increased by sizeof(%s)', self.c_type)
	_h(' */')
	_c('')
	_hc('')
	_hc('/*****************************************************************************')
	_hc(' **')
	_hc(' ** void %s', self.c_next_name)
	_hc(' ** ')
	_hc(' ** @param %s *i', self.c_iterator_type)
	_hc(' ** @returns void')
	_hc(' **')
	_hc(' *****************************************************************************/')
	_hc(' ')
	_hc('void')
	_h('%s (%s *i  /**< */);', self.c_next_name, self.c_iterator_type)
	_c('%s (%s *i  /**< */)', self.c_next_name, self.c_iterator_type)
	_c('{')

	if not self:fixed_size() then
		_c('    %s *R = i->data;', self.c_type)
		_c('    xcb_generic_iterator_t child = %s;', _c_iterator_get_end(self.last_varsized_field, 'R'))
		_c('    --i->rem;')
		_c('    i->data = (%s *) child.data;', self.c_type)
		_c('    i->index = child.index;')
	else
		_c('    --i->rem;')
		_c('    ++i->data;')
		_c('    i->index += sizeof(%s);', self.c_type)
	end

	_c('}')

	_h('')
	_h('/**')
	_h(' * Return the iterator pointing to the last element')
	_h(' * @param i An %s', self.c_iterator_type)
	_h(' * @return  The iterator pointing to the last element')
	_h(' *')
	_h(' * Set the current element in the iterator to the last element.')
	_h(' * The member rem is set to 0. The member data points to the')
	_h(' * last element.')
	_h(' */')
	_c('')
	_hc('')
	_hc('/*****************************************************************************')
	_hc(' **')
	_hc(' ** xcb_generic_iterator_t %s', self.c_end_name)
	_hc(' ** ')
	_hc(' ** @param %s i', self.c_iterator_type)
	_hc(' ** @returns xcb_generic_iterator_t')
	_hc(' **')
	_hc(' *****************************************************************************/')
	_hc(' ')
	_hc('xcb_generic_iterator_t')
	_h('%s (%s i  /**< */);', self.c_end_name, self.c_iterator_type)
	_c('%s (%s i  /**< */)', self.c_end_name, self.c_iterator_type)
	_c('{')
	_c('    xcb_generic_iterator_t ret;')

	if self:fixed_size() then
		_c('    ret.data = i.data + i.rem;')
		_c('    ret.index = i.index + ((char *) ret.data - (char *) i.data);')
		_c('    ret.rem = 0;')
	else
		_c('    while(i.rem > 0)')
		_c('        %s(&i);', self.c_next_name)
		_c('    ret.data = i.data;')
		_c('    ret.rem = i.rem;')
		_c('    ret.index = i.index;')
	end

	_c('    return ret;')
	_c('}')
end

local function _c_accessor_get_length(expr, prefix)
	--[[
	Figures out what C code is needed to get a length field.
	For fields that follow a variable-length field, use the accessor.
	Otherwise, just reference the structure field directly.
	]]
	prefix = prefix or ''
	local prefarrow = ((prefix == '' and '') or prefix .. '->')

	if expr.lenfield ~= nil and expr.lenfield.prev_varsized_field ~= nil then
		return expr.lenfield.c_accessor_name .. '(' .. prefix .. ')'
	elseif expr.lenfield_name ~= nil then
		return prefarrow .. expr.lenfield_name
	else
		return tostring(expr.nmemb)
	end
end

local function _c_accessor_get_expr(expr, prefix)
	--[[
	Figures out what C code is needed to get the length of a list field.
	Recurses for math operations.
	Returns bitcount for value-mask fields.
	Otherwise, uses the value of the length field.
	]]
	prefix = prefix or ''
	local lenexp = _c_accessor_get_length(expr, prefix)

	if expr.op ~= nil then
		return '(' .. _c_accessor_get_expr(expr.lhs, prefix) .. ' ' .. expr.op .. ' ' .. _c_accessor_get_expr(expr.rhs, prefix) .. ')'
	elseif expr.bitfield then
		return 'xcb_popcount(' .. lenexp .. ')'
	else
		return lenexp
	end
end

local function _c_accessors_field(self, field)
	--[[
	Declares the accessor functions for a non-list field that follows a variable-length field.
	]]
	if field.type_.is_simple then
		_hc('')
		_hc('')
		_hc('/*****************************************************************************')
		_hc(' **')
		_hc(' ** %s %s', field.c_field_type, field.c_accessor_name)
		_hc(' ** ')
		_hc(' ** @param const %s *R', self.c_type)
		_hc(' ** @returns %s', field.c_field_type)
		_hc(' **')
		_hc(' *****************************************************************************/')
		_hc(' ')
		_hc('%s', field.c_field_type)
		_h('%s (const %s *R  /**< */);', field.c_accessor_name, self.c_type)
		_c('%s (const %s *R  /**< */)', field.c_accessor_name, self.c_type)
		_c('{')
		_c('    xcb_generic_iterator_t prev = %s;', _c_iterator_get_end(field.prev_varsized_field, 'R'))
		_c('    return * (%s *) ((char *) prev.data + XCB_TYPE_PAD(%s, prev.index) + %d);', field.c_field_type, field.first_field_after_varsized.type_.c_type, field.prev_varsized_offset)
		_c('}')
	else
		_hc('')
		_hc('')
		_hc('/*****************************************************************************')
		_hc(' **')
		_hc(' ** %s * %s', field.c_field_type, field.c_accessor_name)
		_hc(' ** ')
		_hc(' ** @param const %s *R', self.c_type)
		_hc(' ** @returns %s *', field.c_field_type)
		_hc(' **')
		_hc(' *****************************************************************************/')
		_hc(' ')
		_hc('%s *', field.c_field_type)
		_h('%s (const %s *R  /**< */);', field.c_accessor_name, self.c_type)
		_c('%s (const %s *R  /**< */)', field.c_accessor_name, self.c_type)
		_c('{')
		_c('    xcb_generic_iterator_t prev = %s;', _c_iterator_get_end(field.prev_varsized_field, 'R'))
		_c('    return (%s *) ((char *) prev.data + XCB_TYPE_PAD(%s, prev.index) + %d);', field.c_field_type, field.first_field_after_varsized.type_.c_type, field.prev_varsized_offset)
		_c('}')
	end
end

local function _c_accessors_list(self, field)
	--[[
	Declares the accessor functions for a list field.
	Declares a direct-accessor function only if the list members are fixed size.
	Declares length and get-iterator functions always.
	]]
	local list = field.type_

	_h_setlevel(2)
	_c_setlevel(2)
	if list.member:fixed_size() then
		_hc('')
		_hc('')
		_hc('/*****************************************************************************')
		_hc(' **')
		_hc(' ** %s * %s', field.c_field_type, field.c_accessor_name)
		_hc(' ** ')
		_hc(' ** @param const %s *R', self.c_type)
		_hc(' ** @returns %s *', field.c_field_type)
		_hc(' **')
		_hc(' *****************************************************************************/')
		_hc(' ')
		_hc('%s *', field.c_field_type)
		_h('%s (const %s *R  /**< */);', field.c_accessor_name, self.c_type)
		_c('%s (const %s *R  /**< */)', field.c_accessor_name, self.c_type)
		_c('{')

		if field.prev_varsized_field == nil then
			_c('    return (%s *) (R + 1);', field.c_field_type)
		else
			_c('    xcb_generic_iterator_t prev = %s;', _c_iterator_get_end(field.prev_varsized_field, 'R'))
			_c('    return (%s *) ((char *) prev.data + XCB_TYPE_PAD(%s, prev.index) + %d);', field.c_field_type, field.first_field_after_varsized.type_.c_type, field.prev_varsized_offset)
		end

		_c('}')
	end

	_hc('')
	_hc('')
	_hc('/*****************************************************************************')
	_hc(' **')
	_hc(' ** int %s', field.c_length_name)
	_hc(' ** ')
	_hc(' ** @param const %s *R', self.c_type)
	_hc(' ** @returns int')
	_hc(' **')
	_hc(' *****************************************************************************/')
	_hc(' ')
	_hc('int')
	_h('%s (const %s *R  /**< */);', field.c_length_name, self.c_type)
	_c('%s (const %s *R  /**< */)', field.c_length_name, self.c_type)
	_c('{')
	_c('    return %s;', _c_accessor_get_expr(field.type_.expr, 'R'))
	_c('}')

	if field.type_.member.is_simple then
		_hc('')
		_hc('')
		_hc('/*****************************************************************************')
		_hc(' **')
		_hc(' ** xcb_generic_iterator_t %s', field.c_end_name)
		_hc(' ** ')
		_hc(' ** @param const %s *R', self.c_type)
		_hc(' ** @returns xcb_generic_iterator_t')
		_hc(' **')
		_hc(' *****************************************************************************/')
		_hc(' ')
		_hc('xcb_generic_iterator_t')
		_h('%s (const %s *R  /**< */);', field.c_end_name, self.c_type)
		_c('%s (const %s *R  /**< */)', field.c_end_name, self.c_type)
		_c('{')
		_c('    xcb_generic_iterator_t i;')

		if field.prev_varsized_field == nil then
			_c('    i.data = ((%s *) (R + 1)) + (%s);', field.type_.c_wiretype, _c_accessor_get_expr(field.type_.expr, 'R'))
		else
			_c('    xcb_generic_iterator_t child = %s;', _c_iterator_get_end(field.prev_varsized_field, 'R'))
			_c('    i.data = ((%s *) child.data) + (%s);', field.type_.c_wiretype, _c_accessor_get_expr(field.type_.expr, 'R'))
		end

		_c('    i.rem = 0;')
		_c('    i.index = (char *) i.data - (char *) R;')
		_c('    return i;')
		_c('}')

	else
		_hc('')
		_hc('')
		_hc('/*****************************************************************************')
		_hc(' **')
		_hc(' ** %s %s', field.c_iterator_type, field.c_iterator_name)
		_hc(' ** ')
		_hc(' ** @param const %s *R', self.c_type)
		_hc(' ** @returns %s', field.c_iterator_type)
		_hc(' **')
		_hc(' *****************************************************************************/')
		_hc(' ')
		_hc('%s', field.c_iterator_type)
		_h('%s (const %s *R  /**< */);', field.c_iterator_name, self.c_type)
		_c('%s (const %s *R  /**< */)', field.c_iterator_name, self.c_type)
		_c('{')
		_c('    %s i;', field.c_iterator_type)

		if field.prev_varsized_field == nil then
			_c('    i.data = (%s *) (R + 1);', field.c_field_type)
		else
			_c('    xcb_generic_iterator_t prev = %s;', _c_iterator_get_end(field.prev_varsized_field, 'R'))
			_c('    i.data = (%s *) ((char *) prev.data + XCB_TYPE_PAD(%s, prev.index));', field.c_field_type, field.c_field_type)
		end

		_c('    i.rem = %s;', _c_accessor_get_expr(field.type_.expr, 'R'))
		_c('    i.index = (char *) i.data - (char *) R;')
		_c('    return i;')
		_c('}')
	end
end

local function _c_accessors(self, name, base)
	--[[
	Declares the accessor functions for the fields of a structure.
	]]
	for _,field in ipairs(self.fields) do
		if field.type_.is_list and not field.type_:fixed_size() then
			_c_accessors_list(self, field)
		elseif field.prev_varsized_field ~= nil then
			_c_accessors_field(self, field)
		end
	end
end

local function c_simple(self, name)
	--[[
	Exported function that handles cardinal type declarations.
	These are types which are typedef'd to one of the CARDx's, char, float, etc.
	]]
	_c_type_setup(self, name, {})

	if (self.name ~= name) then
		-- Typedef
		_h_setlevel(1)
		local my_name = _t(name)
		_h('')
		_h('typedef %s %s;', _t(self.name), my_name)

		-- Iterator
		_c_iterator(self, name)
	end
end

local function _c_complex(self)
	--[[
	Helper function for handling all structure types.
	Called for all structs, requests, replies, events, errors.
	]]
	_h_setlevel(1)
	_h('')
	_h('/**')
	_h(' * @brief %s', self.c_type)
	_h(' **/')
	_h('typedef %s %s {', self.c_container, self.c_type)

	local struct_fields = {}
	local maxtypelen = 0

	local varfield = nil
	for _,field in ipairs(self.fields) do
		if not field.type_:fixed_size() then
			varfield = field.c_field_name
		else
			if varfield ~= nil and not field.type_.is_pad and field.wire then
				warn(string.format("%s: warning: variable field %s followed by fixed field %s\n",
					self.c_type, varfield, field.c_field_name))
			end
			if field.wire then
				table.insert(struct_fields, field)
			end
		end
	end
		
	for _,field in ipairs(struct_fields) do
		if #field.c_field_type > maxtypelen then
			maxtypelen = #field.c_field_type
		end
	end

	for _,field in ipairs(struct_fields) do
		local spacing = (' '):rep(maxtypelen - #field.c_field_type)
		_h('    %s%s %s%s; /**<  */', field.c_field_type, spacing, field.c_field_name, field.c_subscript)
	end

	_h('} %s;', self.c_type)
end

local function c_struct(self, name)
	--[[
	Exported function that handles structure declarations.
	]]
	_c_type_setup(self, name, {})
	_c_complex(self)
	_c_accessors(self, name, name)
	_c_iterator(self, name)
end

local function c_union(self, name)
	--[[
	Exported function that handles union declarations.
	]]
	_c_type_setup(self, name, {})
	_c_complex(self)
	_c_iterator(self, name)
end

local function _c_request_helper(self, name, cookie_type, void, regular)
	--[[
	Declares a request function.
	]]

	-- Four stunningly confusing possibilities here:
	--
	--   Void			Non-void
	-- ------------------------------
	-- "req"			"req"
	-- 0 flag		   CHECKED flag   Normal Mode
	-- void_cookie	  req_cookie
	-- ------------------------------
	-- "req_checked"	"req_unchecked"
	-- CHECKED flag	 0 flag		 Abnormal Mode
	-- void_cookie	  req_cookie
	-- ------------------------------


	-- Whether we are _checked or _unchecked
	local checked = void and not regular
	local unchecked = not void and not regular

	-- What kind of cookie we return
	local func_cookie = (((void) and 'xcb_void_cookie_t') or self.c_cookie_type)

	-- What flag is passed to xcb_request
	local func_flags = ((((void and regular) or (not void and not regular)) and '0') or 'XCB_REQUEST_CHECKED')

	-- Global extension id variable or NULL for xproto
	local func_ext_global = (((_ns.is_ext) and '&' .. _ns.c_ext_global_name) or '0')

	-- What our function name is
	local func_name = self.c_request_name
	if checked then
		func_name = self.c_checked_name
	end
	if unchecked then
		func_name = self.c_unchecked_name
	end

	local param_fields = {}
	local wire_fields = {}
	local maxtypelen = #'xcb_connection_t'

	for _,field in ipairs(self.fields) do
		if field.visible then
			-- The field should appear as a call parameter
			table.insert(param_fields, field)
		end
		if field.wire and not field.auto then
			-- We need to set the field up in the structure
			table.insert(wire_fields, field)
		end
	end
		
	for _,field in ipairs(param_fields) do
		if #field.c_field_const_type > maxtypelen then
			maxtypelen = #field.c_field_const_type
		end
	end

	_h_setlevel(2)
	_c_setlevel(2)
	_h('')
	_h('/**')
	_h(' * Delivers a request to the X server')
	_h(' * @param c The connection')
	_h(' * @return A cookie')
	_h(' *')
	_h(' * Delivers a request to the X server.')
	_h(' * ')
	if checked then
		_h(' * This form can be used only if the request will not cause')
		_h(' * a reply to be generated. Any returned error will be')
		_h(' * saved for handling by xcb_request_check().')
	end
	if unchecked then
		_h(' * This form can be used only if the request will cause')
		_h(' * a reply to be generated. Any returned error will be')
		_h(' * placed in the event queue.')
	end
	_h(' */')
	_c('')
	_hc('')
	_hc('/*****************************************************************************')
	_hc(' **')
	_hc(' ** %s %s', cookie_type, func_name)
	_hc(' ** ')

	local spacing = (' '):rep(maxtypelen - #'xcb_connection_t')
	_hc(' ** @param xcb_connection_t%s *c', spacing)

	for _,field in ipairs(param_fields) do
		spacing = (' '):rep(maxtypelen - #field.c_field_const_type)
		_hc(' ** @param %s%s %s%s', field.c_field_const_type, spacing, field.c_pointer, field.c_field_name)
	end

	_hc(' ** @returns %s', cookie_type)
	_hc(' **')
	_hc(' *****************************************************************************/')
	_hc(' ')
	_hc('%s', cookie_type)

	spacing = (' '):rep(maxtypelen - #'xcb_connection_t')
	local comma = (((#param_fields > 0) and ',') or ');')
	_h('%s (xcb_connection_t%s *c  /**< */%s', func_name, spacing, comma)
	comma = (((#param_fields > 0) and ',') or ')')
	_c('%s (xcb_connection_t%s *c  /**< */%s', func_name, spacing, comma)

	local func_spacing = (' '):rep(#func_name + 2)
	local count = #param_fields
	for _,field in ipairs(param_fields) do
		count = count - 1
		spacing = (' '):rep(maxtypelen - #field.c_field_const_type)
		comma = (((count > 0) and ',') or ');')
		_h('%s%s%s %s%s  /**< */%s', func_spacing, field.c_field_const_type, spacing, field.c_pointer, field.c_field_name, comma)
		comma = (((count > 0) and ',') or ')')
		_c('%s%s%s %s%s  /**< */%s', func_spacing, field.c_field_const_type, spacing, field.c_pointer, field.c_field_name, comma)
	end

	count = 2
	for _,field in ipairs(param_fields) do
		if not field.type_:fixed_size() then
			count = count + 2
		end
	end

	_c('{')
	_c('    static const xcb_protocol_request_t xcb_req = {')
	_c('        /* count */ %d,', count)
	_c('        /* ext */ %s,', func_ext_global)
	_c('        /* opcode */ %s,', self.c_request_name:upper())
	_c('        /* isvoid */ %d', (void and 1) or 0)
	_c('    };')
	_c('    ')
	_c('    struct iovec xcb_parts[%d];', count + 2)
	_c('    %s xcb_ret;', func_cookie)
	_c('    %s xcb_out;', self.c_type)
	_c('    ')

	for _,field in ipairs(wire_fields) do
		if field.type_:fixed_size() then
			if field.type_.is_expr then
				_c('    xcb_out.%s = %s;', field.c_field_name, _c_accessor_get_expr(field.type_.expr))

			elseif field.type_.is_pad then
				if field.type_.nmemb == 1 then
					_c('    xcb_out.%s = 0;', field.c_field_name)
				else
					_c('    memset(xcb_out.%s, 0, %d);', field.c_field_name, field.type_.nmemb)
				end
			else
				if field.type_.nmemb == 1 then
					_c('    xcb_out.%s = %s;', field.c_field_name, field.c_field_name)
				else
					_c('    memcpy(xcb_out.%s, %s, %d);', field.c_field_name, field.c_field_name, field.type_.nmemb)
				end
			end
		end
	end

	_c('    ')
	_c('    xcb_parts[2].iov_base = (char *) &xcb_out;')
	_c('    xcb_parts[2].iov_len = sizeof(xcb_out);')
	_c('    xcb_parts[3].iov_base = 0;')
	_c('    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;')

	count = 4
	for _,field in ipairs(param_fields) do
		if not field.type_:fixed_size() then
			_c('    xcb_parts[%d].iov_base = (char *) %s;', count, field.c_field_name)
			if field.type_.is_list then
				_c('    xcb_parts[%d].iov_len = %s * sizeof(%s);', count, _c_accessor_get_expr(field.type_.expr), field.type_.member.c_wiretype)
			else
				_c('    xcb_parts[%d].iov_len = %s * sizeof(%s);', count, 'Uh oh', field.type_.c_wiretype)
			end
			_c('    xcb_parts[%d].iov_base = 0;', count + 1)
			_c('    xcb_parts[%d].iov_len = -xcb_parts[%d].iov_len & 3;', count + 1, count)
			count = count + 2
		end
	end

	_c('    xcb_ret.sequence = xcb_send_request(c, %s, xcb_parts + 2, &xcb_req);', func_flags)
	_c('    return xcb_ret;')
	_c('}')
end

local function _c_reply(self, name)
	--[[
	Declares the function that returns the reply structure.
	]]
	local spacing1 = (' '):rep(#self.c_cookie_type - #'xcb_connection_t')
	local spacing2 = (' '):rep(#self.c_cookie_type - #'xcb_generic_error_t')
	local spacing3 = (' '):rep(#self.c_reply_name + 2)

	_h('')
	_h('/**')
	_h(' * Return the reply')
	_h(' * @param c      The connection')
	_h(' * @param cookie The cookie')
	_h(' * @param e      The xcb_generic_error_t supplied')
	_h(' *')
	_h(' * Returns the reply of the request asked by')
	_h(' * ')
	_h(' * The parameter @p e supplied to this function must be NULL if')
	_h(' * %s(). is used.', self.c_unchecked_name)
	_h(' * Otherwise, it stores the error if any.')
	_h(' *')
	_h(' * The returned value must be freed by the caller using free().')
	_h(' */')
	_c('')
	_hc('')
	_hc('/*****************************************************************************')
	_hc(' **')
	_hc(' ** %s * %s', self.c_reply_type, self.c_reply_name)
	_hc(' ** ')
	_hc(' ** @param xcb_connection_t%s  *c', spacing1)
	_hc(' ** @param %s   cookie', self.c_cookie_type)
	_hc(' ** @param xcb_generic_error_t%s **e', spacing2)
	_hc(' ** @returns %s *', self.c_reply_type)
	_hc(' **')
	_hc(' *****************************************************************************/')
	_hc(' ')
	_hc('%s *', self.c_reply_type)
	_hc('%s (xcb_connection_t%s  *c  /**< */,', self.c_reply_name, spacing1)
	_hc('%s%s   cookie  /**< */,', spacing3, self.c_cookie_type)
	_h('%sxcb_generic_error_t%s **e  /**< */);', spacing3, spacing2)
	_c('%sxcb_generic_error_t%s **e  /**< */)', spacing3, spacing2)
	_c('{')
	_c('    return (%s *) xcb_wait_for_reply(c, cookie.sequence, e);', self.c_reply_type)
	_c('}')
end

local function _c_opcode(name, opcode)
	--[[
	Declares the opcode define for requests, events, and errors.
	]]
	_h_setlevel(1)
	_h('')
	_h('/** Opcode for %s. */', _n(name))
	_h('#define %s %s', _n(name):upper(), opcode)
end

local function _c_cookie(self, name)
	--[[
	Declares the cookie type for a non-void request.
	]]
	_h_setlevel(1)
	_h('')
	_h('/**')
	_h(' * @brief %s', self.c_cookie_type)
	_h(' **/')
	_h('typedef struct %s {', self.c_cookie_type)
	_h('    unsigned int sequence; /**<  */')
	_h('} %s;', self.c_cookie_type)
end

local function c_request(self, name)
	--[[
	Exported function that handles request declarations.
	]]
	_c_type_setup(self, name, {'request'})

	if self.reply then
		-- Cookie type declaration
		_c_cookie(self, name)
	end

	-- Opcode define
	_c_opcode(name, self.opcode)

	-- Request structure declaration
	_c_complex(self)

	if self.reply then
		_c_type_setup(self.reply, name, {'reply'})
		-- Reply structure definition
		_c_complex(self.reply)
		-- Request prototypes
		_c_request_helper(self, name, self.c_cookie_type, false, true)
		_c_request_helper(self, name, self.c_cookie_type, false, false)
		-- Reply accessors
		_c_accessors(self.reply, tabcat(name, 'reply'), name)
		_c_reply(self, name)
	else
		-- Request prototypes
		_c_request_helper(self, name, 'xcb_void_cookie_t', true, false)
		_c_request_helper(self, name, 'xcb_void_cookie_t', true, true)
	end
end

local function c_event(self, name)
	--[[
	Exported function that handles event declarations.
	]]
	_c_type_setup(self, name, {'event'})

	-- Opcode define
	_c_opcode(name, self.opcodes[name])

	if self.name == name then
		-- Structure definition
		_c_complex(self)
	else
		-- Typedef
		_h('')
		_h('typedef %s %s;', _t(tabcat(self.name, 'event')), _t(tabcat(name, 'event')))
	end
end

local function c_error(self, name)
	--[[
	Exported function that handles error declarations.
	]]
	_c_type_setup(self, name, {'error'})

	-- Opcode define
	_c_opcode(name, self.opcodes[name])

	if self.name == name then
		-- Structure definition
		_c_complex(self)
	else
		-- Typedef
		_h('')
		_h('typedef %s %s;', _t(tabcat(self.name, 'error')), _t(tabcat(name, 'error')))
	end
end

-- Main routine starts here

-- Must create an "output" dictionary before any xcbgen imports.
output = {
	open		= c_open,
  close   = c_close,
  simple  = c_simple,
  enum		= c_enum,
  struct  = c_struct,
  union   = c_union,
  request = c_request,
  event   = c_event,
  error   = c_error
}

-- Boilerplate below this point

-- parse args.
local xml_file = nil
for i = 1, #arg do
	local val = arg[i]
	if val:sub(1,2) == '-o' then
		output_dir = arg[i+1]
		i = i + 1
	else
		xml_file = val
	end
end

if output_dir == nil or xml_file == nil then
  print([[
Usage: lua c_client.lua -o <output_directory> <xproto_file>
]])
  return
end

-- Parse the xml header
mod = Module(xml_file, output)

mod:register()
mod:resolve()

mod:generate()

