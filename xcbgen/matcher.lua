--[[
XML parser.  One function for each top-level element in the schema.

Most functions just declare a new object and add it to the module.
For typedefs, eventcopies, xidtypes, and other aliases though,
we do not create a new type object, we just record the existing one under a new name.
]]

local function get_name_prefix(namespace, id)
	local name = { unpack(namespace.prefix) }
	table.insert(name, id)
	return name
end

local funcs = {
import = function (node, mod, namespace)
	--[[
	For imports, we load the file, create a new namespace object,
	execute recursively, then record the import (for header files, etc.)
	]]
	local new_file = namespace.dir .. node.text .. '.xml'
	if not mod:has_import(node.text) then
		local new_namespace = Namespace(new_file)
		execute(mod, new_namespace)
		mod:add_import(node.text, new_namespace)
	end
end,

typedef = function (node, mod, namespace)
	local id = node.attr['newname']
	local name = get_name_prefix(namespace, id)
	local type_ = mod:get_type(node.attr['oldname'])
	mod:add_type(id, namespace.ns, name, type_)
end,

xidtype = function (node, mod, namespace)
	local id = node.attr['name']
	local name = get_name_prefix(namespace, id)
	local type_ = mod:get_type('CARD32')
	mod:add_type(id, namespace.ns, name, type_)
end,

xidunion = function (node, mod, namespace)
	local id = node.attr['name']
	local name = get_name_prefix(namespace, id)
	local type_ = mod:get_type('CARD32')
	mod:add_type(id, namespace.ns, name, type_)
end,

enum = function (node, mod, namespace)
	local id = node.attr['name']
	local name = get_name_prefix(namespace, id)
	local type_ = Enum(name, node)
	mod:add_type(id, namespace.ns, name, type_)
end,

struct = function (node, mod, namespace)
	local id = node.attr['name']
	local name = get_name_prefix(namespace, id)
	local type_ = Struct(name, node)
	mod:add_type(id, namespace.ns, name, type_)
end,

union = function (node, mod, namespace)
	local id = node.attr['name']
	local name = get_name_prefix(namespace, id)
	local type_ = Union(name, node)
	mod:add_type(id, namespace.ns, name, type_)
end,

request = function (node, mod, namespace)
	local id = node.attr['name']
	local name = get_name_prefix(namespace, id)
	local type_ = Request(name, node)
	mod:add_request(id, name, type_)
end,

event = function (node, mod, namespace)
	local id = node.attr['name']
	local name = get_name_prefix(namespace, id)
	local event = Event(name, node)
	event:add_opcode(node.attr['number'], name, true)
	mod:add_event(id, name, event)
end,

eventcopy = function (node, mod, namespace)
	local id = node.attr['name']
	local name = get_name_prefix(namespace, id)
	local event = mod:get_event(node.attr['ref'])
	event:add_opcode(node.attr['number'], name, false)
	mod:add_event(id, name, event)
end,

error = function (node, mod, namespace)
	local id = node.attr['name']
	local name = get_name_prefix(namespace, id)
	local error_ = Error(name, node)
	error_:add_opcode(node.attr['number'], name, true)
	mod:add_error(id, name, error_)
end,

errorcopy = function (node, mod, namespace)
	local id = node.attr['name']
	local name = get_name_prefix(namespace, id)
	local error_ = mod:get_error(node.attr['ref'])
	error_:add_opcode(node.attr['number'], name, false)
	mod:add_error(id, name, error_)
end,
}

function execute(mod, namespace)
	for _,elt in ipairs(namespace.root) do
		local f = funcs[elt.tag]
		if f then
			f(elt, mod, namespace)
		else
			print('unknown element: ' .. elt.tag)
		end
	end
end

