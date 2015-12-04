-- This module contains the namespace class and the singleton module class.

local function dirname(filename)
end

local function basename(filename)
end

Namespace = oo.class()
--[[
  Contains the naming information for an extension.

  Public fields:

  header is the header attribute ("header file" name).
  is_ext is true for extensions, false for xproto.
  major_version and minor_version are extension version info.
  ext_xname is the X extension name string.
  ext_name is the XCB extension name prefix.
]]

function Namespace:__init(filename)
	local new = {}
	new.path = filename
	-- Parse filename
	new.dir = filename:gsub("(.*/).*", "%1")
	new.file = filename:gsub(".*/(.*)", "%1") or filename

	-- Prase XML
	print("Parse XML file:", filename)
	new.root = dom.parse (io.lines(filename))
	new.header = new.root.attr.header
	new.ns = new.header .. ':'

	-- Process extension attributes
	if new.root.attr['extension-xname'] ~= nil then
		new.is_ext = true
		new.major_version = new.root.attr['major-version']
		new.minor_version = new.root.attr['minor-version']
		new.ext_xname = new.root.attr['extension-xname']
		new.ext_name = new.root.attr['extension-name']
		new.prefix = {'xcb', new.ext_name}
	else
		new.is_ext = false
		new.ext_name = ''
		new.prefix = {'xcb',nil}
	end

	return oo.rawnew(self, new)
end

Module = oo.class{
	namespace = '',
	output = {},
	import_list = {},
	imports = {},
	types = {},
	events = {},
	errors = {},
	all = {}
}
--[[
  This is the grand, encompassing class that represents an entire XCB specification.
  Only gets instantiated once, in the main() routine.

  Don't need to worry about this much except to declare it and to get the namespace.

  Public fields:
  namespace contains the namespace info for the spec.
]]

function Module:__init(filename, output)
	local new = {
		namespace = Namespace(filename),
		output = output,
		open = output.open,
		close = output.close,
	}
	init_output(output)
	new = oo.rawnew(self, new)

	-- Register some common types
	new:add_type('CARD8', '', {'uint8_t'}, tcard8)
	new:add_type('CARD16', '', {'uint16_t'}, tcard16)
	new:add_type('CARD32', '', {'uint32_t'}, tcard32)
	new:add_type('CARD64', '', {'uint64_t'}, tcard64)
	new:add_type('INT8', '', {'int8_t'}, tint8)
	new:add_type('INT16', '', {'int16_t'}, tint16)
	new:add_type('INT32', '', {'int32_t'}, tint32)
	new:add_type('INT64', '', {'int64_t'}, tint64)
	new:add_type('BYTE', '', {'uint8_t'}, tcard8)
	new:add_type('BOOL', '', {'uint8_t'}, tcard8)
	new:add_type('char', '', {'char'}, tchar)
	new:add_type('float', '', {'float'}, tfloat)
	new:add_type('double', '', {'double'}, tdouble)
	new:add_type('void', '', {'void'}, tcard8)
	return new
end

function Module:register()
	execute(self, self.namespace)
end

function Module:resolve()
	for _,v in ipairs(self.all) do
		local name,item = unpack(v)
		item:resolve(self)
	end
end

function Module:generate()
	self:open()

	for _,v in ipairs(self.all) do
		local name,item = unpack(v)
		item:out(name)
	end

	self:close()
end

function Module:add_import(name, namespace)
	table.insert(self.import_list, name)
	self.imports[name] = namespace.header
end

function Module:has_import(name)
	return self.imports[name] ~= nil
end

local function test_prefix(name, prefix)
	local cnt = #name - 1
	if cnt ~= #prefix then
		return false
	end
	for i,v in ipairs(prefix) do
		if name[i] ~= v then
			return false
		end
	end
	return true;
end

function Module:add_all_type(name, item)
	if test_prefix(name, self.namespace.prefix) then
		table.insert(self.all, {name, item})
	end
end

function Module:add_type(id, ns, name, item)
	local key = ns .. id
	if self.types[key] then return end
	self.types[key] = { name, item }
	self:add_all_type(name, item)
end

function Module:get_type_impl(id, idx)
	local key = id
	if self.types[key] then
		return self.types[key][idx]
	end
	key = self.namespace.ns .. id
	if self.types[key] then
		return self.types[key][idx]
	end
	for key,type_ in pairs(self.types) do
		if key:gsub(".*:(.*)","%1") == id then
			return type_[idx]
		end
	end
	error("Type " .. id .. " not found")
end

function Module:get_type(id)
	return self:get_type_impl(id, 2)
end

function Module:get_type_name(id)
	return self:get_type_impl(id, 1)
end

-- Keeps track of request datatypes
function Module:add_request(id, name, item)
	self:add_all_type(name, item)
end

-- Keeps track of event datatypes
function Module:add_event(id, name, item)
	self.events[id] = {name, item}
	self:add_all_type(name, item)
end

function Module:get_event(id)
	return self.events[id][2]
end

-- Keeps track of error datatypes
function Module:add_error(id, name, item)
	self.errors[id] = {name, item}
	self:add_all_type(name, item)
end

function Module:get_error(id)
	return self.errors[id][2]
end

