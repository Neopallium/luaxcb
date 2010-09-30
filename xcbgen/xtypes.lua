-- This module contains the classes which represent XCB data types.

local oo = require "loop.simple"

Type = oo.class()
--[[
Abstract base class for all XCB data types.
Contains default fields, and some abstract methods.
]]

--[[
Default structure initializer.  Sets up default fields.

Public fields:
name is a tuple of strings specifying the full type name.
size is the size of the datatype in bytes, or nil if variable-sized.
nmemb is 1 for non-list types, nil for variable-sized lists, otherwise number of elts.
booleans for identifying subclasses, because I can't figure out isinstance().
]]
function Type:__init(name, obj)
	local new = {
	name = name,
	size = 0,
	nmemb = 0,
	resolved = false,
	is_simple = false,
	is_list = false,
	is_expr = false,
	is_container = false,
	is_reply = false,
	is_pad = false
	}
	return oo.rawnew(self, new)
end

function Type:resolve(mod)
	--[[
	Abstract method for resolving a type.
	This should make sure any referenced types are already declared.
	]]
	error('abstract resolve method not overridden!')
end

function Type:out(name)
	--[[
	Abstract method for outputting code.
	These are declared in the language-specific modules, and
	there must be a dictionary containing them declared when this module is imported!
	]]
	error('abstract out method not overridden!')
end

function Type:fixed_size()
	--[[
	Abstract method for determining if the data type is fixed-size.
	]]
	error('abstract fixed_size method not overridden!')
end

function Type:make_member_of(mod, complex_type, field_type, field_name, visible, wire, auto)
	--[[
	Default method for making a data type a member of a structure.
	Extend this if the data type needs to add an additional length field or something.

	module is the global module object.
	complex_type is the structure object.
	see Field for the meaning of the other parameters.
	]]
	local new_field = Field(self, field_type, field_name, visible, wire, auto)

	-- We dump the _placeholder_byte if any fields are added.
	for idx, field in ipairs(complex_type.fields) do
		if field == _placeholder_byte then
			complex_type.fields[idx] = new_field
			return
		end
	end

	table.insert(complex_type.fields, new_field)
end

SimpleType = oo.class({}, Type)
--[[
Derived class which represents a cardinal type like CARD32 or char.
Any type which is typedef'ed to cardinal will be one of these.

Public fields added:
none
]]

function SimpleType:__init(name, size)
	self = Type.__init(self, name)
	self.is_simple = true
	self.size = size
	self.nmemb = 1
	return self
end

function SimpleType:resolve(mod)
	self.resolved = true
end

function SimpleType:fixed_size()
	return true
end

-- Cardinal datatype globals.  See module __init method.
tcard8 = SimpleType({'uint8_t'}, 1)
tcard16 = SimpleType({'uint16_t'}, 2)
tcard32 = SimpleType({'uint32_t'}, 4)
tint8 =  SimpleType({'int8_t'}, 1)
tint16 = SimpleType({'int16_t'}, 2)
tint32 = SimpleType({'int32_t'}, 4)
tchar =  SimpleType({'char'}, 1)
tfloat = SimpleType({'float'}, 4)
tdouble = SimpleType({'double'}, 8)

Enum = oo.class({}, SimpleType)
--[[
Derived class which represents an enum.  Fixed-size.

Public fields added:
values contains a list of (name, value) tuples.  value is empty, or a number.
]]
function Enum:__init(name, elt)
	self = SimpleType.__init(self, name, 4)
	self.values = {}
	for _,item in ipairs(elt) do
		-- First check if we're using a default value
		if #item == 0 then
			table.insert(self.values, {item.attr['name'], ''})
		else
			-- An explicit value or bit was specified.
			local value = item[1]
			if value.tag == 'value' then
				table.insert(self.values, {item.attr['name'], value.text})
			elseif value.tag == 'bit' then
				-- XXX replace this with a simple number, please.
				table.insert(self.values, {item.attr['name'], tostring(2 ^ tonumber(value.text))})
			end
		end
	end
	return self
end

function Enum:resolve(mod)
	self.resolved = true
end

function Enum:fixed_size()
	return true
end

ListType = oo.class({}, Type)
--[[
Derived class which represents a list of some other datatype.  Fixed- or variable-sized.

Public fields added:
member is the datatype of the list elements.
parent is the structure type containing the list.
expr is an Expression object containing the length information, for variable-sized lists.
]]
function ListType:__init(elt, member, parent)
	self = Type.__init(self, member.name)
	self.is_list = true
	self.member = member
	self.parent = parent

	if elt.tag == 'list' then
		self.expr = Expression(elt[1] or elt, self)
	elseif elt.tag == 'valueparam' then
		self.expr = Expression(elt, self)
	end

	if member:fixed_size() then
		self.size = member.size
	else
		self.size = 0
	end
	if self.expr:fixed_size() then
		self.nmemb = self.expr.nmemb
	else
		self.nmemb = 0
	end
	return self
end

function ListType:make_member_of(mod, complex_type, field_type, field_name, visible, wire, auto)
	if not self:fixed_size() then
		-- We need a length field.
		-- Ask our Expression object for it's name, type, and whether it's on the wire.
		local lenfid = self.expr.lenfield_type
		local lenfield_name = self.expr.lenfield_name
		local lenwire = self.expr.lenwire
		local needlen = true

		-- See if the length field is already in the structure.
		for _,field in ipairs(self.parent.fields) do
			if field.field_name == lenfield_name then
				needlen = false
			end
		end

		-- It isn't, so we need to add it to the structure ourself.
		if needlen then
			local type_ = mod:get_type(lenfid)
			local lenfield_type = mod:get_type_name(lenfid)
			type_:make_member_of(mod, complex_type, lenfield_type, lenfield_name, true, lenwire, false)
		end
	end

	-- Add ourself to the structure by calling our original method.
	Type.make_member_of(self, mod, complex_type, field_type, field_name, visible, wire, auto)
end

function ListType:resolve(mod)
	if self.resolved then
		return
	end
	self.member:resolve(mod)

	-- Find my length field again.  We need the actual Field object in the expr.
	-- This is needed because we might have added it ourself above.
	if not self:fixed_size() then
		for _,field in ipairs(self.parent.fields) do
			if field.field_name == self.expr.lenfield_name and field.wire then
				self.expr.lenfield = field
				break
			end
		end
	end
		
	self.resolved = true
end

function ListType:fixed_size()
	return self.member:fixed_size() and self.expr:fixed_size()
end

ExprType = oo.class({}, Type)
--[[
Derived class which represents an exprfield.  Fixed size.

Public fields added:
expr is an Expression object containing the value of the field.
]]
function ExprType:__init(elt, member, parent)
	self = Type.__init(self, member.name)
	self.is_expr = true
	self.member = member
	self.parent = parent

	self.expr = Expression(elt[1], self)

	self.size = member.size
	self.nmemb = 1
	return self
end

function ExprType:resolve(mod)
	if self.resolved then
		return
	end
	self.member:resolve(mod)
	self.resolved = true
end

function ExprType:fixed_size()
	return true
end

PadType = oo.class({}, Type)
--[[
Derived class which represents a padding field.
]]
function PadType:__init(elt)
	self = Type.__init(self, tcard8.name)
	self.is_pad = true
	self.size = 1
	if (elt == nil) then
		self.nmemb = 1
	else
		self.nmemb = tonumber(elt.attr['bytes'])
	end
	return self
end

function PadType:resolve(mod)
	self.resolved = true
end

function PadType:fixed_size()
	return true
end


ComplexType = oo.class({}, Type)
--[[
Derived class which represents a structure.  Base type for all structure types.

Public fields added:
fields is an array of Field objects describing the structure fields.
]]
function ComplexType:__init(name, elt)
	self = Type.__init(self, name)
	self.is_container = true
	self.elt = elt
	self.fields = {}
	self.nmemb = 1
	self.size = 0
	return self
end

function ComplexType:resolve(mod)
	if self.resolved then
		return
	end
	local pads = 0
	local type_ = nil
	local visible = nil
	local fkey = nil
	local field_name = nil

	-- Resolve all of our field datatypes.
	for _,child in ipairs(self.elt) do
		if child.tag == 'pad' then
			field_name = 'pad' .. pads
			fkey = 'CARD8'
			type_ = PadType(child)
			pads = pads + 1
			visible = false
		elseif child.tag == 'field' then
			field_name = child.attr['name']
			fkey = child.attr['type']
			type_ = mod:get_type(fkey)
			visible = true
		elseif child.tag == 'exprfield' then
			field_name = child.attr['name']
			fkey = child.attr['type']
			type_ = ExprType(child, mod:get_type(fkey), self)
			visible = false
		elseif child.tag == 'list' then
			field_name = child.attr['name']
			fkey = child.attr['type']
			type_ = ListType(child, mod:get_type(fkey), self)
			visible = true
		elseif child.tag == 'valueparam' then
			field_name = child.attr['value-list-name']
			fkey = 'CARD32'
			type_ = ListType(child, mod:get_type(fkey), self)
			visible = true
		else
			-- Hit this on Reply
			field_name = nil
		end

		if field_name ~= nil then
			-- Get the full type name for the field
			local field_type = mod:get_type_name(fkey)
			-- Add the field to ourself
			type_:make_member_of(mod, self, field_type, field_name, visible, true, false)
			-- Recursively resolve the type (could be another structure, list)
			type_:resolve(mod)
		end
	end

	self:calc_size() -- Figure out how big we are
	self.resolved = true
end

function ComplexType:calc_size()
	self.size = 0
	for _,m in ipairs(self.fields) do
		if not m.wire then
			-- continue
		end
		if m.type_:fixed_size() then
			self.size = self.size + (m.type_.size * m.type_.nmemb)
		else
			self.size = 0
			break
		end
	end
end

function ComplexType:fixed_size()
	for _,m in ipairs(self.fields) do
		if not m.type_:fixed_size() then
			return false
		end
	end
	return true
end


Struct = oo.class({}, ComplexType)
--[[
Derived class representing a struct data type.
]]


Union = oo.class({}, ComplexType)
--[[
Derived class representing a union data type.
]]
function Union:__init(name, elt)
	self = ComplexType.__init(self, name, elt)
	self.is_union = true
	return self
end


Reply = oo.class({}, ComplexType)
--[[
Derived class representing a reply.  Only found as a field of Request.
]]
function Reply:__init(name, elt)
	self = ComplexType.__init(self, name, elt)
	self.is_reply = true
	return self
end

function Reply:resolve(mod)
	if self.resolved then
		return
	end
	-- Add the automatic protocol fields
	table.insert(self.fields, Field(tcard8, tcard8.name, 'response_type', false, true, true))
	table.insert(self.fields, _placeholder_byte)
	table.insert(self.fields, Field(tcard16, tcard16.name, 'sequence', false, true, true))
	table.insert(self.fields, Field(tcard32, tcard32.name, 'length', false, true, true))
	ComplexType.resolve(self, mod)
end
	

Request = oo.class({}, ComplexType)
--[[
Derived class representing a request.

Public fields added:
reply contains the reply datatype or nil for void requests.
opcode contains the request number.
]]
function Request:__init(name, elt)
	self = ComplexType.__init(self, name, elt)
	self.reply = nil
	self.opcode = elt.attr['opcode']

	for _,child in ipairs(elt) do
		if child.tag == 'reply' then
			self.reply = Reply(name, child)
		end
	end
	return self
end

function Request:resolve(mod)
	if self.resolved then
		return
	end
	-- Add the automatic protocol fields
	if mod.namespace.is_ext then
		table.insert(self.fields, Field(tcard8, tcard8.name, 'major_opcode', false, true, true))
		table.insert(self.fields, Field(tcard8, tcard8.name, 'minor_opcode', false, true, true))
		table.insert(self.fields, Field(tcard16, tcard16.name, 'length', false, true, true))
		ComplexType.resolve(self, mod)
	else
		table.insert(self.fields, Field(tcard8, tcard8.name, 'major_opcode', false, true, true))
		table.insert(self.fields, _placeholder_byte)
		table.insert(self.fields, Field(tcard16, tcard16.name, 'length', false, true, true))
		ComplexType.resolve(self, mod)
	end

	if self.reply then
		self.reply:resolve(mod)
	end
end


Event = oo.class({}, ComplexType)
--[[
Derived class representing an event data type.

Public fields added:
opcodes is a dictionary of name -> opcode number, for eventcopies.
]]
function Event:__init(name, elt)
	self = ComplexType.__init(self, name, elt)
	self.opcodes = {}

	local tmp = elt.attr['no-sequence-number']
	self.has_seq = (tmp == nil or tmp:lower() == 'false' or tmp == '0')
	return self
end
		
function Event:add_opcode(opcode, name, main)
	self.opcodes[name] = opcode
	if main then
		self.name = name
	end
end

function Event:resolve(mod)
	if self.resolved then
		return
	end

	-- Add the automatic protocol fields
	table.insert(self.fields, Field(tcard8, tcard8.name, 'response_type', false, true, true))
	if self.has_seq then
		table.insert(self.fields, _placeholder_byte)
		table.insert(self.fields, Field(tcard16, tcard16.name, 'sequence', false, true, true))
	end
	ComplexType.resolve(self, mod)
end


Error = oo.class({}, ComplexType)
--[[
Derived class representing an error data type.

Public fields added:
opcodes is a dictionary of name -> opcode number, for errorcopies.
]]
function Error:__init(name, elt)
	self = ComplexType.__init(self, name, elt)
	self.opcodes = {}
	return self
end

function Error:add_opcode(opcode, name, main)
	self.opcodes[name] = opcode
	if main then
		self.name = name
	end
end

function Error:resolve(mod)
	if self.resolved then
		return
	end

	-- Add the automatic protocol fields
	table.insert(self.fields, Field(tcard8, tcard8.name, 'response_type', false, true, true))
	table.insert(self.fields, Field(tcard8, tcard8.name, 'error_code', false, true, true))
	table.insert(self.fields, Field(tcard16, tcard16.name, 'sequence', false, true, true))
	ComplexType.resolve(self, mod)
end

_placeholder_byte = Field(PadType(nil), tcard8.name, 'pad0', false, true, false)

function init_output(output)
	SimpleType.out = output['simple']
	Enum.out = output['enum']
	Struct.out = output['struct']
	Union.out = output['union']
	Request.out = output['request']
	Event.out = output['event']
	Error.out = output['error']
end

