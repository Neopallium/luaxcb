-- This module contains helper classes for structure fields and length expressions.

Field = oo.class()
--[[
  Represents a field of a structure.

  type is the datatype object for the field.
  field_type is the name of the type (string tuple)
  field_name is the name of the structure field.
  visible is true iff the field should be in the request API.
  wire is true iff the field should be in the request structure.
  auto is true iff the field is on the wire but not in the request API (e.g. opcode)
]]

function Field:__init(type_, field_type, field_name, visible, wire, auto)
	return oo.rawnew(self, {
		type_ = type_,
		field_type = field_type,
		field_name = field_name,
		visible = visible,
		wire = wire,
		auto = auto
	})
end

Expression = oo.class()
--[[
  Represents a mathematical expression for a list length or exprfield.

  Public fields:
  op is the operation (text +,*,/,<<,~) or nil.
  lhs and rhs are the sub-Expressions if op is set.
  lenfield_name is the name of the length field, or nil for request lists.
  lenfield is the Field object for the length field, or nil.
  bitfield is true if the length field is a bitmask instead of a number.
  nmemb is the fixed size (value)of the expression, or nil
]]

function Expression:__init(elt, parent)
	local new = {
		parent = parent,
		nmemb = nil,
		lenfield_name = nil,
		lenfield_type = nil,
		lenfield = nil,
		lenwire = false,
		bitfield = false,

		op = nil,
		lhs = nil,
		rhs = nil
	}
	if elt.tag == 'list' then
		-- List going into a request, which has no length field (inferred by server)
		new.lenfield_name = elt.attr['name'] .. '_len'
		new.lenfield_type = 'CARD32'

	elseif elt.tag == 'fieldref' then
		-- Standard list with a fieldref
		new.lenfield_name = elt.text

	elseif elt.tag == 'valueparam' then
		-- Value-mask.  The length bitmask is described by attributes.
		new.lenfield_name = elt.attr['value-mask-name']
		new.lenfield_type = elt.attr['value-mask-type']
		new.lenwire = true
		new.bitfield = true

	elseif elt.tag == 'op' then
		-- Op field.  Need to recurse.
		new.op = elt.attr['op']
		new.lhs = Expression(elt[1], parent)
		new.rhs = Expression(elt[2], parent)

		-- Hopefully we don't have two separate length fields...
		new.lenfield_name = new.lhs.lenfield_name
		if new.lenfield_name == nil then
			new.lenfield_name = new.rhs.lenfield_name
		end

	elseif elt.tag == 'unop' then
		-- Unop field.  Need to recurse.
		new.op = elt.attr['op']
		new.rhs = Expression(elt[1], parent)

		new.lenfield_name = new.rhs.lenfield_name

	elseif elt.tag == 'value' then
		-- Constant expression
		new.nmemb = tonumber(elt.text)
	else
		-- Notreached
		error("undefined tag '"..elt.tag.."'")
	end

	return oo.rawnew(self, new)
end

function Expression:fixed_size()
	return self.nmemb ~= nil
end

