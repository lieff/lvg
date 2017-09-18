#pragma once

typedef enum
{
    // AS3 Actions go below here.
    ABC_ACTION_END                  = 0x00,
    // Do: Enter the debugger if one has been invoked.
    ABC_ACTION_BKPT                 = 0x01,
    // Do: Nothing.
    ABC_ACTION_NOP                  = 0x02,
    // Stack In:
    //  obj -- an object
    // Stack Out:
    //  .
    // Do: Throw obj as an exception
    // Equivalent: ACTIONTHROW
    ABC_ACTION_THROW                = 0X03,
    // Stream:
    //  name_id -- V32 index to multiname 'name'
    // Stack In:
    //  obj -- an object
    //  [ns [n]] -- Namespace stuff.
    // Stack Out:
    //  super -- if obj.name is a method, the super of that method.
    //        -- if obj.name is write-only data, throw ReferenceError.
    //        -- if obj.name is readable, the return of the getter of super.
    ABC_ACTION_GETSUPER             = 0X04,
    // Stream: UV32 index to multiname 'name'
    // Stack In:
    //  obj -- an object
    //  val -- an object
    //  [ns [n]] -- Namespace stuff.
    // Stack Out:
    //  .
    // Do: Make val the super of obj.name ; throw ReferenceError if obj.name is
    //  not writable.
    ABC_ACTION_SETSUPER             = 0X05,
    // Dynamically eXtend Name Space?
    // Stream: UV32 index to string pool 'nsname'
    // Do: Create a new public namespace with name nsname, enter the namespace.
    ABC_ACTION_DXNS                 = 0X06,
    // Stack In:
    //  nsname -- a string object
    // Stack Out:
    //  .
    // Do: Create a new public namespace with name nsname, but don't enter it.
    ABC_ACTION_DXNSLATE             = 0X07,
    // Stream: UV32 frame pointer offset 'offset'
    // Frame: 
    //  Kill at offset
    // Equivalent: ACTION_DELETE
    ABC_ACTION_KILL                 = 0X08,
    // Do: Unknown purpose, Tamarin does nothing.
    ABC_ACTION_LABEL                = 0X09,
    ABC_ACTION_0x0A                 = 0X0A,
    ABC_ACTION_0X0B                 = 0X0B,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If !(a < b) move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFNLT                = 0X0C,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If !(a <= b) move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFNLE                = 0X0D,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If !(a > b) move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFNGT                = 0X0E,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If !(a >= b) move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFNGE                = 0X0F,
    // Stream: S24 jump offset 'jump'
    // Do: If jump is negative, check for interrupts. Move by jump in stream.
    // Equivalent: ACTION_BRANCHALWAYS
    ABC_ACTION_JUMP                 = 0X10,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    // Stack Out:
    //  .
    // Do: If a is 'true', move by jump in stream, as ABC_ACTION_JUMP does.
    // Equivalent: ACTION_BRANCHIFTRUE
    ABC_ACTION_IFTRUE               = 0X11,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    // Stack Out:
    //  .
    // Do: If a is 'false', move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFFALSE              = 0X12,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If a == b (weakly), move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFEQ                 = 0x13,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If a != b (weakly), move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFNE                 = 0X14,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If a < b move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFLT                 = 0X15,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If a <= b move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFLE                 = 0x16,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If a > b move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFGT                 = 0x17,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If a >= b move by jump in stream, as ABC_ACTION_JUMP does.
    ABC_ACTION_IFGE                 = 0x18,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If a == b (strictly), move by jump in stream, as ABC_ACTION_JUMP
    ABC_ACTION_IFSTRICTEQ           = 0x19,
    // Stream: S24 jump offset 'jump'
    // Stack In:
    //  a -- an object
    //  b -- an object
    // Stack Out:
    //  .
    // Do: If a != b (strongly), move by jump in stream, as ABC_ACTION_JUMP
    ABC_ACTION_IFSTRICTNE           = 0x1A,
    // Stream: 3 bytes | V32 count as 'case_count - 1' | case_count of S24
    //  as 'cases'
    // Stack In:
    //  index -- an integer object
    // Stack Out:
    //  .
    // Do: If index >= case_count, reset stream to position on op entry.
    //  Otherwise, move by cases[index] - 1 from stream position on op entry.
    ABC_ACTION_LOOKUPSWITCH         = 0x1B,
    // Stack In:
    //  scope -- a scope
    // Stack Out:
    //  .
    // Do: Enter scope with previous scope as its base, unless it already had
    //  a base, in which case leave that alone.
    ABC_ACTION_PUSHWITH             = 0x1C,
    // Do: exit current scope. Clear the base if the depth is now 
    //  shallower than the base's depth.
    ABC_ACTION_POPSCOPE             = 0x1D,
    // Stack In:
    //  obj -- an object
    //  index -- an integer object
    // Stack Out:
    //  name -- the key name at index in obj (not next after index).
    // Do: If obj is a namespace index 1 is the uri of the namespace, 
    //  index 2 is the prefix of the namespace, and any other value is a
    //  null string object. If obj is an immutable object, like a number,
    //  operate on its prototype instead. If there is no such index,
    //  name is null.
    ABC_ACTION_NEXTNAME             = 0x1E,
    // Stack In:
    //  obj -- an object
    //  index -- an integer object
    // Stack Out:
    //  next_index -- next index after index in obj, or 0 if none.
    // Do: If there is a key/val pair after index, make next_index as it.
    //  Otherwise, make next_index 0.
    ABC_ACTION_HASNEXT              = 0x1F,
    // Stack Out:
    //  n -- a Null object.
    ABC_ACTION_PUSHNULL             = 0x20,
    //  n -- an Undefined object.
    ABC_ACTION_PUSHUNDEFINED        = 0x21,
    ABC_ACTION_0x22                 = 0x22,
    // Stack In:
    //  obj -- an object (namespaces okay)
    //  index -- an integer object
    // Stack Out:
    //  value -- the value of the key value pair in obj at index.
    ABC_ACTION_NEXTVALUE            = 0x23,
    // Stream: S8 as 'byte'
    // Stack Out:
    //  byte -- as a raw byte
    ABC_ACTION_PUSHBYTE             = 0x24,
    // Stream: V32 as 'value'
    // Stack Out:
    //  value -- as a raw integer
    ABC_ACTION_PUSHSHORT            = 0x25,
    // Stack Out:
    //  true -- the True object
    ABC_ACTION_PUSHTRUE             = 0x26,
    // Stack Out:
    //  false -- the False object
    ABC_ACTION_PUSHFALSE            = 0x27,
    // Stack Out:
    //  NaN -- the NaN object
    ABC_ACTION_PUSHNAN              = 0x28,
    // Stack In:
    //  a -- anything
    // Stack Out:
    //  .
    ABC_ACTION_POP                  = 0x29,
    // Stack In:
    //  a -- anything
    // Stack Out:
    //  a
    //  a
    ABC_ACTION_DUP                  = 0x2A,
    // Stack In:
    //  a -- anything
    //  b -- anything
    // Stack Out:
    //  b
    //  a
    ABC_ACTION_SWAP                 = 0x2B,
    // Stream: V32 string pool index 'index'
    // Stack Out:
    //  value -- String object from string_pool[index]
    ABC_ACTION_PUSHSTRING           = 0x2C,
    // Stream: V32 int pool index 'index'
    // Stack Out:
    //  value -- Integer object from integer_pool[index]
    ABC_ACTION_PUSHINT              = 0x2D,
    // Stream: V32 uint pool index 'index'
    // Stack Out:
    //  value -- Unsigned Integer object from unsigned_integer_pool[index]
    ABC_ACTION_PUSHUINT             = 0x2E,
    // Stream: V32 double pool index 'index'
    // Stack Out:
    //  value -- Double object from double_pool[index]
    ABC_ACTION_PUSHDOUBLE           = 0x2F,
    // Stack In:
    //  scope -- a scope
    // Stack Out:
    //  .
    // Do: Enter scope without altering base.
    ABC_ACTION_PUSHSCOPE            = 0x30,
    // Stream: V32 namespace pool index 'index'
    // Stack Out:
    //  ns -- Namespace object from namespace_pool[index]
    ABC_ACTION_PUSHNAMESPACE        = 0x31,
    // Stream: V32 frame location 'objloc' | V32 frame location 'indexloc'
    // Stack Out:
    //  truth -- True if frame[objloc] has key/val pair after frame[indexloc],
    //   following delagates (__proto__) objects if needed. False, otherwise.
    // Frame:
    //  Change at objloc to object which possessed next value.
    //  Change at indexloc to index (as object) of the next value.
    ABC_ACTION_HASNEXT2             = 0x32,
    ABC_ACTION_0x33                 = 0x33,
    ABC_ACTION_0x34                 = 0x34,
    ABC_ACTION_0x35                 = 0x35,
    ABC_ACTION_0x36                 = 0x36,
    ABC_ACTION_0x37                 = 0x37,
    ABC_ACTION_0x38                 = 0x38,
    ABC_ACTION_0x39                 = 0x39,
    ABC_ACTION_0x3A                 = 0x3A,
    ABC_ACTION_0x3B                 = 0x3B,
    ABC_ACTION_0x3C                 = 0x3C,
    ABC_ACTION_0x3D                 = 0x3D,
    ABC_ACTION_0x3E                 = 0x3E,
    ABC_ACTION_0x3F                 = 0x3F,
    // Stream: V32 'index'
    // Stack Out:
    //  func -- the function object
    // Do: Information about function is in the pool at index. Construct the
    //  function from this information, current scope, and base of the scope.
    ABC_ACTION_NEWFUNCTION          = 0x40,
    // Stream: V32 'arg_count'
    // Stack In:
    //  func -- the function to be called
    //  obj -- the object to which the function belongs
    //  arg1 ... argN -- the arg_count arguments to pass
    // Stack Out:
    //  value -- the value returned by obj->func(arg1, ...., argN)
    ABC_ACTION_CALL                 = 0x41,
    // Stream: V32 'arg_count'
    // Stack In:
    //  obj -- the object to be constructed
    //  arg1 ... argN -- the arg_count arguments to pass
    // Stack Out:
    //  value -- obj after it has been constructed as obj(arg1, ..., argN)
    ABC_ACTION_CONSTRUCT            = 0x42,
    // Stream: V32 'method_id + 1' | V32 'arg_count'
    // Stack In:
    //  obj -- the object to be called
    //  arg1 ... argN -- the arg_count arguments to pass
    // Stack Out:
    //  value -- the value returned by obj::'method_id'(arg1, ..., argN)
    ABC_ACTION_CALLMETHOD           = 0x43,
    // Stream: V32 'method_id' | V32 'arg_count'
    // Stack In:
    //  obj -- the object to act as a receiver for the static call
    //  arg1 ... argN -- the arg_count arguments to pass
    // Stack Out:
    //  value -- the value returned by obj->ABC::'method_id'(arg1, ..., argN)
    ABC_ACTION_CALLSTATIC           = 0x44,
    // Stream: V32 'name_offset' | V32 'arg_count'
    // Stack In:
    //  obj -- the object whose super is to be called
    //  [ns [n]] -- Namespace stuff
    //  arg1 ... argN -- the arg_count arguments to pass
    // Stack Out:
    //  value -- the value returned by obj::(resolve)'name_offset'::super(arg1,
    //   ..., argN)
    ABC_ACTION_CALLSUPER            = 0x45,
    // Stream: V32 'name_offset' | V32 'arg_count'
    // Stack In:
    //  obj -- The object whose property is to be accessed.
    //  [ns [n]] -- Namespace stuff
    //  arg1 ... argN -- the arg_count arguments to pass
    // Stack Out:
    //  value -- the value from obj::(resolve)'name_offset'(arg1, ..., argN)
    // NB: Calls getter/setter if they exist.
    ABC_ACTION_CALLPROPERTY         = 0x46,
    // Do: Return an Undefined object up the callstack.
    ABC_ACTION_RETURNVOID           = 0x47,
    // Stack In:
    //  value -- value to be returned
    // Stack Out:
    //  .
    // Do: Return value up the callstack.
    ABC_ACTION_RETURNVALUE          = 0x48,
    // Stream: V32 'arg_count'
    // Stack In:
    //  obj -- the object whose super's constructor should be invoked
    //  arg1 ... argN -- the arg_count arguments
    // Stack Out:
    //  value -- obj after obj::super(arg1, ..., argN) has been invoked
    ABC_ACTION_CONSTRUCTSUPER       = 0x49,
    // Stream: V32 'name_offset' | V32 'arg_count'
    // Stack In:
    //  obj -- the object whose property should be constructed
    //  [ns [n]] -- Namespace stuff
    //  arg1 ... argN -- the arg_count arguments to pass
    // Stack Out:
    //  value -- the newly constructed prop from obj::(resolve)
    //   'name_offset'(arg1, ..., argN)
    ABC_ACTION_CONSTRUCTPROP        = 0x4A,
    // Do: Nothing, this is an intermediate code op, should not appear in ABC.
    ABC_ACTION_CALLSUPERID          = 0x4B,
    // Stream: V32 'name_offset' | V32 'arg_count'
    // Stack In:
    //  obj -- The object whose property is to be accessed.
    //  [ns [n]] -- Namespace stuff
    //  arg1 ... argN -- the arg_count arguments to pass
    // Stack Out:
    //  value -- the value from by obj::(resolve)'name_offset'(arg1, ..., argN)
    // NB: It seems like this does _not_ call getter/setter if they exist,
    //  but is otherwise identical to ABC_ACTION_CALLPROPERTY
    ABC_ACTION_CALLPROPLEX          = 0x4C,
    // Do: Nothing, this is an intermediate code op, should not appear in ABC.
    ABC_ACTION_CALLINTERFACE        = 0x4D,
    // See: 0x45 (ABC_ACTION_CALLSUPER), but
    // Stack Out:
    //  .
    ABC_ACTION_CALLSUPERVOID        = 0x4E,
    // See: 0x46 (ABC_ACTION_CALLPROPERTY), but
    // Stack Out:
    //  .
    ABC_ACTION_CALLPROPVOID         = 0x4F,
    ABC_ACTION_0x50                 = 0x50,
    ABC_ACTION_0x51                 = 0x51,
    ABC_ACTION_0x52                 = 0x52,
    ABC_ACTION_0x53                 = 0x53,
    ABC_ACTION_0x54                 = 0x54,
    // Stream: V32 'arg_count'
    // Stack In:
    //  prop_name_1 -- a string
    //  prop_value_1 -- a value object
    //  .
    //  . (arg_count name/value pairs in all)
    //  .
    //  prop_name_n -- a string
    //  prop_value_n -- a value object
    // Stack Out:
    //  obj -- A new object which contains all of the given properties.
    // NB: This builds an object from its properties, it's not a constructor.
    ABC_ACTION_NEWOBJECT            = 0x55,
    // Stream: V32 'array_size'
    // Stack In:
    //  value_1 -- a value
    //  .
    //  . (array_size of these)
    //  .
    //  value_n -- a value
    // Stack Out:
    //  array -- an array { value_1, value_2, ..., value_n }
    ABC_ACTION_NEWARRAY             = 0x56,
    // Stack Out:
    //  vtable -- A new virtual table, which has the previous one as a parent.
    ABC_ACTION_NEWACTIVATION        = 0x57,
    // Stream: V32 'class_id'
    // Stack In:
    //  obj -- An object to be turned into a class
    // Stack Out:
    //  class -- The newly made class, made from obj and the information at
    //   cinits_pool[class_id]
    // NB: This depends on scope and scope base (to determine base class)
    ABC_ACTION_NEWCLASS             = 0x58,
    // Stream: V32 'name_id'
    // Stack In:
    //  value -- Whose descendants to get
    //  [ns [n]] -- Namespace stuff
    // Stack Out:
    //  ?
    // NB: This op seems to always throw a TypeError in Tamarin, though I
    // assume that it ought to do something to yield a list of
    // descendants of a class.
    ABC_ACTION_GETDESCENDANTS       = 0x59,
    // Stream: V32 'catch_id'
    // Stack Out:
    //  vtable -- vtable suitable to catch an exception of type in catch_id.
    // NB: Need more information on how exceptions are set up.
    ABC_ACTION_NEWCATCH             = 0x5A,
    ABC_ACTION_0x5B                 = 0x5B,
    ABC_ACTION_0x5C                 = 0x5C,
    // Stream: V32 'name_id'
    // Stack In:
    //  [ns [n]] -- Namespace stuff
    // Stack Out:
    //  owner -- object which owns property given by looking up the name_id,
    //   or throw a ReferenceError if none exists.
    ABC_ACTION_FINDPROPSTRICT       = 0x5D,
    // Stream: V32 'name_id'
    // Stack In:
    //  [ns [n]] -- Namespace stuff
    // Stack Out:
    //  owner -- object which owns property given by looking up the name_id,
    //   or an Undefined object if none exists.
    ABC_ACTION_FINDPROPERTY         = 0x5E,
    // Stream: V32 'name_id' (no ns expansion)
    // Stack Out:
    //  def -- The definition of the name at name_id.
    ABC_ACTION_FINDDEF              = 0x5F,
    // Stream: V32 'name_id' (no ns expansion)
    // Stack Out:
    //  property -- The result of 0x5D (ABC_ACTION_FINDPROPSTRICT)
    //   + 0x66 (ABC_ACTION_GETPROPERTY)
    ABC_ACTION_GETLEX               = 0x60,
    // Stream: V32 'name_id'
    // Stack In:
    //  obj -- The object whose property is to be set
    //  [ns [n]] -- Namespace stuff
    //  [key] -- Key name for property. Will not have both Namespace and key.
    //  value -- The value to be used
    // Stack Out:
    //  .
    // Do: Set obj::(resolve)'name_id' to value, or set obj::key to value.
    // NB: I'm not yet clear which should be used when. (Chad)
    ABC_ACTION_SETPROPERTY          = 0x61,
    // Stream: V32 'frame_index'
    // Frame: value at frame_index is needed
    // Stack Out:
    //  value
    ABC_ACTION_GETLOCAL             = 0x62,
    // Stream: V32 'frame_index'
    // Frame: obj at frame_index is set to value
    // Stack In:
    //  value
    // Stack Out:
    //  .
    ABC_ACTION_SETLOCAL             = 0x63,
    // Stack Out:
    //  global -- The global scope object
    ABC_ACTION_GETGLOBALSCOPE       = 0x64,
    // Stream: S8 'depth'
    // Stack Out:
    //  scope -- The scope object at depth
    ABC_ACTION_GETSCOPEOBJECT       = 0x65,
    // Stream: V32 'name_id'
    // Stack In:
    //  obj -- The object whose property is to be retrieved
    //  [ns [n]] -- Namespace stuff
    //  [key] -- Key name for property. Will not have both Namespace and key.
    // Stack Out:
    //  prop -- The requested property.
    // NB: As with 0x61 (ABC_ACTION_SETPROPERTY) it's unclear to me when
    // key gets used and when the namespace (or nothing) is used.
    ABC_ACTION_GETPROPERTY          = 0x66,
    ABC_ACTION_0x67                 = 0x67,
    // Stream V32 'name_id'
    // Stack In:
    //  obj -- The object whose property is to be initialized
    //  [ns [n]] -- Namespace stuff
    //  value -- The value to be put into the property.
    // Stack Out:
    //  .
    // Do:
    //  Set obj::(resolve)'name_id' to value, set bindings from the context.
    ABC_ACTION_INITPROPERTY         = 0x68,
    ABC_ACTION_0x69                 = 0x69,
    // Stream: V32 'name_id'
    // Stack In:
    //  obj -- The object whose property should be deleted.
    //  [ns [n]] -- Namespace stuff
    // Stack Out:
    //  truth -- True if property was deleted or did not exist, else False.
    ABC_ACTION_DELETEPROPERTY       = 0x6A,
    ABC_ACTION_0x6B                 = 0x6B,
    // Stream: V32 'slot_index + 1'
    // Stack In:
    //  obj -- The object which owns the desired slot.
    // Stack Out:
    //  slot -- obj.slots[slot_index]
    ABC_ACTION_GETSLOT              = 0x6C,
    // Stream: V32 'slot_index + 1'
    // Stack In:
    //  obj -- The object whose slot should be set.
    //  value -- The value intended for the slot.
    // Stack Out:
    //  .
    // Do: obj.slots[slot_index] = value
    ABC_ACTION_SETSLOT              = 0x6D,
    // Stream: V32 'slot_index + 1'
    // Stack In:
    //  .
    // Stack Out:
    //  slot -- globals.slots[slot_index]
    // NB: Deprecated
    ABC_ACTION_GETGLOBALSLOT        = 0x6E,
    // Stream: V32 'slot_index + 1'
    // Stack In:
    //  value -- The value to be placed into the slot.
    // Stack Out:
    //  .
    // Do: globals[slot_index] = value
    // NB: Deprecated
    ABC_ACTION_SETGLOBALSLOT        = 0x6F,
    // Stack In:
    //  value -- An object
    // Stack Out:
    //  str_value -- value as a string
    ABC_ACTION_CONVERT_S            = 0x70,
    // Stack In:
    //  value -- An object to be escaped
    // Stack Out:
    //  str_value -- value as a string, escaped suitably for an XML element.
    ABC_ACTION_ESC_XELEM            = 0x71,
    // Stack In:
    //  value -- An object to be escaped
    // Stack Out:
    //  str_value -- value as a string, escaped suitably for an XML attribute.
    ABC_ACTION_ESC_XATTR            = 0x72,
    // Stack In:
    //  value -- An object to be converted to Integer
    // Stack Out:
    //  int_value -- value as an integer object
    ABC_ACTION_CONVERT_I            = 0x73,
    // Stack In:
    //  value -- An object to be converted to unsigned integer
    // Stack Out:
    //  int_value -- value as an unsigned integer object
    ABC_ACTION_CONVERT_U            = 0X74,
    // Stack In:
    //  value -- An object to be converted to a double
    // Stack Out:
    //  double_value -- value as a double object
    ABC_ACTION_CONVERT_D            = 0X75,
    // Stack In:
    //  value -- An object to be converted to a boolean
    // Stack Out:
    //  bool_value -- value as a boolean object
    ABC_ACTION_CONVERT_B            = 0X76,
    // Stack In:
    //  obj -- An object
    // Stack Out:
    //  obj -- An object
    // Do: If obj is Undefined or Null, throw TypeError
    ABC_ACTION_CONVERT_O            = 0X77,
    // Stack In:
    //  obj -- An object
    // Stack Out:
    //  obj -- An object
    // Do: If obj is not XML based, throw TypeError
    ABC_ACTION_CHECKFILTER          = 0x78,
    ABC_ACTION_0x79                 = 0x79,
    ABC_ACTION_0x7A                 = 0x7A,
    ABC_ACTION_0x7B                 = 0x7B,
    ABC_ACTION_0x7C                 = 0x7C,
    ABC_ACTION_0x7D                 = 0x7D,
    ABC_ACTION_0x7E                 = 0x7E,
    ABC_ACTION_0x7F                 = 0x7F,
    // Stream: V32 'name_index'
    // Stack In:
    //  obj -- An object to be converted
    // Stack Out:
    //  coerced_obj -- The object as the desired (resolve)'name_index' type.
    ABC_ACTION_COERCE               = 0x80,
    // See: 0x76 (ABC_ACTION_CONVERT_B)
    // NB: Deprecated
    ABC_ACTION_COERCE_B             = 0x81,
    // Stack In:
    //  obj -- An object to be converted
    // Stack Out:
    //  obj
    // Do: Nothing. (The 'a' is for atom, and this already is.)
    ABC_ACTION_COERCE_A             = 0x82,
    // See: 0x73 ABC_ACTION_CONVERT_I
    // NB: Deprecated
    // NOMING 20 Aug 2007
    ABC_ACTION_COERCE_I             = 0x83,
    // See: 0x75 ABC_ACTION_CONVERT_D
    // NB: Deprecated
    // NOMING 20 Aug 2007
    ABC_ACTION_COERCE_D             = 0x84,
    // Stack In:
    //  obj -- An object to be converted
    // Stack Out:
    //  str_obj -- obj as string. nullString object if obj is Null or Undefined
    ABC_ACTION_COERCE_S             = 0x85,
    // Stream: V32 'name_index' (no namespace)
    // Stack In:
    //  obj -- An object to be checked
    // Stack Out:
    //  cobj -- obj if obj is of type (resolve)'name_index', otherwise Null
    ABC_ACTION_ASTYPE               = 0x86,
    // Stack In:
    //  obj -- An object to be checked
    //  valid -- The object whose type is to be matched
    // Stack Out:
    //  cobj -- obj if type of obj is type of valid, otherwise Null
    ABC_ACTION_ASTYPELATE           = 0x87,
    // See: 0x74 (ABC_ACTION_CONVERT_U)
    // NB: Deprecated
    ABC_ACTION_COERCE_U             = 0x88,
    // Stack In:
    //  obj -- An object
    // Stack Out:
    //  cobj -- obj if obj is not Undefined, otherwise Null
    ABC_ACTION_COERCE_O             = 0x89,
    ABC_ACTION_0x8A                 = 0x8A,
    ABC_ACTION_0x8B                 = 0x8B,
    ABC_ACTION_0x8C                 = 0x8C,
    ABC_ACTION_0x8D                 = 0x8D,
    ABC_ACTION_0x8E                 = 0x8E,
    ABC_ACTION_0x8F                 = 0x8F,
    // Stack In:
    //  obj -- An object
    // Stack Out:
    //  negdouble -- -1.0 * (double) obj
    ABC_ACTION_NEGATE               = 0x90,
    // Stack In:
    //  num -- A number, integer or double
    // Stack Out:
    //  num2 -- The same value, but new object.
    // Do:
    //  num = num + 1 (post-increment)
    ABC_ACTION_INCREMENT            = 0x91,
    // Stream: V32 'frame_addr'
    // Frame: Load i from frame_addr. Store clone(i) at frame_addr. i = i + 1
    //  (post-increment in frame)
    ABC_ACTION_INCLOCAL             = 0x92,
    // Stack In:
    //  num -- A number, integer or double
    // Stack Out:
    //  num2 -- The same value, but new object.
    // Do:
    //  num = num - 1 (post-decrement)
    ABC_ACTION_DECREMENT            = 0x93,
    // Stream: V32 'frame_addr'
    // Frame: Load i from frame_addr. Store clone(i) at frame_addr. i = i - 1
    //  (post-decrement in frame)
    ABC_ACTION_DECLOCAL             = 0x94,
    // Stack In:
    //  obj -- An object
    // Stack Out:
    //  type -- typeof(obj)
    ABC_ACTION_ABC_TYPEOF           = 0x95,
    // Stack In:
    //  obj -- An object
    // Stack Out:
    //  nobj -- A truth object with value !((Boolean) obj)
    ABC_ACTION_NOT                  = 0x96,
    // Stack In:
    //  obj -- An object
    // Stack Out:
    //  nint -- ~((Int) obj)
    ABC_ACTION_BITNOT               = 0x97,
    ABC_ACTION_0x98                 = 0x98,
    ABC_ACTION_0x99                 = 0x99,
    // NB: It is an error for this to appear.
    ABC_ACTION_CONCAT               = 0x9A,
    // NB: It is an error for this to appear.
    ABC_ACTION_ADD_D                = 0x9B,
    ABC_ACTION_0x9C                 = 0x9C,
    ABC_ACTION_0x9D                 = 0x9D,
    ABC_ACTION_0x9E                 = 0x9E,
    ABC_ACTION_0x9F                 = 0x9F,
    // Stack In:
    // a
    // b
    // Stack Out:
    // a + b (double if numeric)
    ABC_ACTION_ADD                  = 0xA0,
    // Stack In:    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  a - b (double)
    ABC_ACTION_SUBTRACT             = 0xA1,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  a * b (double)
    ABC_ACTION_MULTIPLY             = 0xA2,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  a / b (double)
    ABC_ACTION_DIVIDE               = 0xA3,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  a % b
    ABC_ACTION_MODULO               = 0xA4,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  a << b
    ABC_ACTION_LSHIFT               = 0xA5,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  a >> b
    ABC_ACTION_RSHIFT               = 0xA6,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  ((unsigned) a) >> b
    ABC_ACTION_URSHIFT              = 0xA7,
    //  a
    //  b
    // Stack Out:
    //  a & b
    ABC_ACTION_BITAND               = 0xA8,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  a | b
    ABC_ACTION_BITOR                = 0xA9,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  a ^ b
    ABC_ACTION_BITXOR               = 0xAA,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  truth -- Truth of (a == b) (weakly)
    ABC_ACTION_EQUALS               = 0xAB,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  truth -- Truth of (a == b) (strongly, as in 
    //   0x19 (ABC_ACTION_IFSTRICTEQ))
    ABC_ACTION_STRICTEQUALS         = 0xAC,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  truth -- Truth of (a < b)
    ABC_ACTION_LESSTHAN             = 0xAD,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  truth -- Truth of (a <= b)
    ABC_ACTION_LESSEQUALS           = 0xAE,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  truth -- Truth of (a > b)
    ABC_ACTION_GREATERTHAN          = 0xAF,
    // Stack In:
    //  a
    //  b
    // Stack Out:
    //  truth -- Truth of (a >= b)
    ABC_ACTION_GREATEREQUALS        = 0xB0,
    // Stack In:
    //  val -- An object
    //  super -- An object
    // Stack Out:
    //  truth -- Truth of "val is an instance of super"
    ABC_ACTION_INSTANCEOF           = 0xB1,
    // Stream: V32 'name_id'
    // Stack In:
    //  obj -- An object
    // Stack Out:
    //  truth -- Truth of "obj is of the type given in (resolve)'name_id'"
    ABC_ACTION_ISTYPE               = 0xB2,
    // Stack In:
    //  obj -- An object
    //  type -- A type to match
    // Stack Out:
    //  truth -- Truth of "obj is of type"
    ABC_ACTION_ISTYPELATE           = 0xB3,
    // Stack In:
    //  name -- The name to find
    //  obj -- The object to search for it
    // Stack Out:
    //  truth -- True if name is in current namespace or anywhere in object.
    //   Don't look in the namespace if obj is a dictionary.
    ABC_ACTION_IN                   = 0xB4,
    ABC_ACTION_0xB5                 = 0xB5,
    ABC_ACTION_0xB6                 = 0xB6,
    ABC_ACTION_0xB7                 = 0xB7,
    ABC_ACTION_0xB8                 = 0xB8,
    ABC_ACTION_0xB9                 = 0xB9,
    ABC_ACTION_0xBA                 = 0xBA,
    ABC_ACTION_0xBB                 = 0xBB,
    ABC_ACTION_0xBC                 = 0xBC,
    ABC_ACTION_0xBD                 = 0xBD,
    ABC_ACTION_0xBE                 = 0xBE,
    ABC_ACTION_0xBF                 = 0xBF,
    // See: 0x91 (ABC_ACTION_INCREMENT), but forces types to int, not double
    ABC_ACTION_INCREMENT_I          = 0xC0,
    // See: 0x93 (ABC_ACTION_DECREMENT), but forces types to int, not double
    ABC_ACTION_DECREMENT_I          = 0xC1,
    // See: 0x92 (ABC_ACTION_INCLOCAL), but forces types to int, not double
    ABC_ACTION_INCLOCAL_I           = 0xC2,
    // See: 0x94 (ABC_ACTION_DECLOCAL), but forces types to int, not double
    ABC_ACTION_DECLOCAL_I           = 0xC3,
    // See: 0x90 (ABC_ACTION_NEGATE), but forces type to int, not double
    ABC_ACTION_NEGATE_I             = 0xC4,
    // See: 0xA0 (ABC_ACTION_ADD), but forces type to int
    ABC_ACTION_ADD_I                = 0xC5,
    // See: 0xA1 (ABC_ACTION_SUBTRACT), but forces type to int
    ABC_ACTION_SUBTRACT_I           = 0xC6,
    // See: 0xA2 (ABC_ACTION_MULTIPLY), but forces type to int
    ABC_ACTION_MULTIPLY_I           = 0xC7,
    ABC_ACTION_0xC8                 = 0xC8,
    ABC_ACTION_0xC9                 = 0xC9,
    ABC_ACTION_0xCA                 = 0xCA,
    ABC_ACTION_0xCB                 = 0xCB,
    ABC_ACTION_0xCC                 = 0xCC,
    ABC_ACTION_0xCD                 = 0xCD,
    ABC_ACTION_0xCE                 = 0xCE,
    ABC_ACTION_0xCF                 = 0xCF,
    // Frame: Load frame[0] as val
    // Stack Out:
    //  val
    ABC_ACTION_GETLOCAL0            = 0xD0,
    // Frame: Load frame[1] as val
    // Stack Out:
    //  val
    ABC_ACTION_GETLOCAL1            = 0xD1,
    // Frame: Load frame[2] as val
    // Stack Out:
    //  val
    ABC_ACTION_GETLOCAL2            = 0xD2,
    // Frame: Load frame[3] as val
    // Stack Out:
    //  val
    ABC_ACTION_GETLOCAL3            = 0xD3,
    // Frame: Store val as frame[0]
    // Stack In:
    //  val
    // Stack Out:
    //  .
    ABC_ACTION_SETLOCAL0            = 0xD4,
    // Frame: Store val as frame[1]
    // Stack In:
    //  val
    // Stack Out:
    //  .
    ABC_ACTION_SETLOCAL1            = 0xD5,
    // Frame: Store val as frame[2]
    // Stack In:
    //  val
    // Stack Out:
    //  .
    ABC_ACTION_SETLOCAL2            = 0xD6,
    // Frame: Store val as frame[3]
    // Stack In:
    //  val
    // Stack Out:
    //  .
    ABC_ACTION_SETLOCAL3            = 0xD7,
    ABC_ACTION_0xD8                 = 0xD8,
    ABC_ACTION_0xD9                 = 0xD9,
    ABC_ACTION_0xDA                 = 0xDA,
    ABC_ACTION_0xDB                 = 0xDB,
    ABC_ACTION_0xDC                 = 0xDC,
    ABC_ACTION_0xDD                 = 0xDD,
    ABC_ACTION_0xDE                 = 0xDE,
    ABC_ACTION_0xDF                 = 0xDF,
    ABC_ACTION_0xE0                 = 0xE0,
    ABC_ACTION_0xE1                 = 0xE1,
    ABC_ACTION_0xE2                 = 0xE2,
    ABC_ACTION_0xE3                 = 0xE3,
    ABC_ACTION_0xE4                 = 0xE4,
    ABC_ACTION_0xE5                 = 0xE5,
    ABC_ACTION_0xE6                 = 0xE6,
    ABC_ACTION_0xE7                 = 0xE7,
    ABC_ACTION_0xE8                 = 0xE8,
    ABC_ACTION_0xE9                 = 0xE9,
    ABC_ACTION_0xEA                 = 0xEA,
    ABC_ACTION_0xEB                 = 0xEB,
    ABC_ACTION_0xEC                 = 0xEC,
    ABC_ACTION_0xED                 = 0xED,
    // NB: It is an error for this to appear.
    ABC_ACTION_ABS_JUMP             = 0xEE,
    // Stream: 7 bytes of unknown stuff to be skipped
    // Do: skip ahead 7 bytes in stream
    ABC_ACTION_DEBUG                = 0xEF,
    // Stream: V32 'line_number'
    // Do: Nothing, but line_number is for the debugger if wanted.
    ABC_ACTION_DEBUGLINE            = 0xF0,
    // Stream: V32 'name_offset'
    // Do: Nothing. 'name_offset' into string pool is the file name if wanted.
    ABC_ACTION_DEBUGFILE            = 0xF1,
    // Stream: V32 'line_number'
    // Do: Enter debugger if present, line_number is the line number in source.
    ABC_ACTION_BKPTLINE             = 0xF2,
    // Do: Nothing.
    ABC_ACTION_TIMESTAMP            = 0xF3,
    ABC_ACTION_0xF4                 = 0xF4,
    // NB: It seems an error for this to appear.
    ABC_ACTION_VERIFYPASS           = 0xF5,
    // NB: Error to appear in ABC
    ABC_ACTION_ALLOC                = 0xF6,
    // NB: Error to appear in ABC
    ABC_ACTION_MARK                 = 0xF7,
    // NB: Error to appear in ABC
    ABC_ACTION_WB                   = 0xF8,
    // NB: Error to appear in ABC
    ABC_ACTION_PROLOGUE             = 0xF9,
    // NB: Error to appear in ABC
    ABC_ACTION_SENDENTER            = 0xFA,
    // NB: Error to appear in ABC
    ABC_ACTION_DOUBLETOATOM         = 0xFB,
    // NB: Error to appear in ABC
    ABC_ACTION_SWEEP                = 0xFC,
    // NB: Error to appear in ABC
    ABC_ACTION_CODEGENOP            = 0xFD,
    // NB: Error to appear in ABC
    ABC_ACTION_VERIFYOP             = 0xFE
} Actions2;
