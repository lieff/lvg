#if __SWF_VERSION__ <= 5
// create a _global object, since it doesn't have one, these are ver 6 values
_global = new_empty_object ();
_global.ASSetNative = ASSetNative;
_global.ASSetNativeAccessor = ASSetNativeAccessor;
_global.ASSetPropFlags = ASSetPropFlags;
_global.ASconstructor = ASconstructor;
_global.ASnative = ASnative;
_global.Accessibility = Accessibility;
_global.Array = Array;
_global.AsBroadcaster = AsBroadcaster;
_global.AsSetupError = AsSetupError;
_global.AssetCache = AssetCache;
_global.Boolean = Boolean;
_global.Button = Button;
_global.Camera = Camera;
_global.Color = Color;
_global.ContextMenu = ContextMenu;
_global.ContextMenuItem = ContextMenuItem;
_global.Date = Date;
_global.Error = Error;
_global.Function = Function;
_global.Infinity = Infinity;
_global.Key = Key;
_global.LoadVars = LoadVars;
_global.LocalConnection = LocalConnection;
_global.Math = Math;
_global.Microphone = Microphone;
_global.Mouse = Mouse;
_global.MovieClip = MovieClip;
_global.MovieClipLoader = MovieClipLoader;
_global.NaN = NaN;
_global.NetConnection = NetConnection;
_global.NetStream = NetStream;
_global.Number = Number;
_global.Object = Object;
_global.PrintJob = PrintJob;
_global.RemoteLSOUsage = RemoteLSOUsage;
_global.Selection = Selection;
_global.SharedObject = SharedObject;
_global.Sound = Sound;
_global.Stage = Stage;
_global.String = String;
_global.System = System;
_global.TextField = TextField;
_global.TextFormat = TextFormat;
_global.TextSnapshot = TextSnapshot;
_global.Video = Video;
_global.XML = XML;
_global.XMLNode = XMLNode;
_global.XMLSocket = XMLSocket;
_global.clearInterval = clearInterval;
_global.clearTimeout = clearTimeout;
_global.enableDebugConsole = enableDebugConsole;
_global.escape = escape;
_global.flash = flash;
_global.isFinite = isFinite;
_global.isNaN = isNaN;
_global.o = o;
_global.parseFloat = parseFloat;
_global.parseInt = parseInt;
_global.setInterval = setInterval;
_global.setTimeout = setTimeout;
_global.showRedrawRegions = showRedrawRegions;
_global.textRenderer = textRenderer;
_global.trace = trace;
_global.unescape = unescape;
_global.updateAfterEvent = updateAfterEvent;
#endif

function new_empty_object () {
  var hash = new Object ();
  ASSetPropFlags (hash, null, 0, 7);
  for (var prop in hash) {
    hash[prop] = "to-be-deleted";
    delete hash[prop];
  }
  return hash;
}

function hasOwnProperty_inner (o, prop)
{
  if (o.hasOwnProperty != undefined)
    return o.hasOwnProperty (prop);

  o.hasOwnProperty = ASnative (101, 5);
  var result = o.hasOwnProperty (prop);
  delete o.hasOwnProperty;
  return result;
}

function hasOwnProperty (o, prop)
{
  var result = hasOwnProperty_inner (o, prop);
#if __SWF_VERSION__ != 6
  if (result == false) {
    ASSetPropFlags (o, prop, 0, 256);
    result = hasOwnProperty_inner (o, prop);
    if (result)
      ASSetPropFlags (o, prop, 256);
  }
#endif
  return result;
}

function new_info () {
  return new_empty_object ();
}

function set_info (info, prop, id, value) {
  info[prop + "_-_" + id] = value;
}

function get_info (info, prop, id) {
  return info[prop + "_-_" + id];
}

function is_blaclisted (o, prop)
{
  if (prop == "mySecretId" || prop == "globalSecretId")
    return true;

  if (o == _global.Camera && prop == "names")
    return true;

  if (o == _global.Microphone && prop == "names")
    return true;

  return false;
}

function trace_properties_recurse (o, prefix, identifier, level)
{
  // to collect info about different properties
  var info = new_info ();

  // calculate indentation
  var indentation = "";
  for (var j = 0; j < level; j++) {
    indentation = indentation + "  ";
  }

  // mark the ones that are not hidden
  for (var prop in o)
  {
    // only get the ones that are not only in the __proto__
    if (is_blaclisted (o, prop) == false) {
      if (hasOwnProperty (o, prop) == true)
	set_info (info, prop, "hidden", false);
    }
  }

  // unhide everything
  ASSetPropFlags (o, null, 0, 1);

  var all = new Array ();
  var hidden = new Array ();
  for (var prop in o)
  {
    // only get the ones that are not only in the __proto__
    if (is_blaclisted (o, prop) == false) {
      if (hasOwnProperty (o, prop) == true) {
	all.push (prop);
	if (get_info (info, prop, "hidden") != false) {
	  set_info (info, prop, "hidden", true);
	  hidden.push (prop);
	}
      }
    }
  }
  all.sort ();

  // hide the ones that were already hidden
  ASSetPropFlags (o, hidden, 1, 0);

  if (all.length == 0) {
    trace (indentation + "no children");
    return nextSecretId;
  }

#if __SWF_VERSION__ != 6
  for (var i = 0; i < all.length; i++)
  {
    var prop = all[i];

    // try changing value
    if (!hasOwnProperty_inner(o, prop) && hasOwnProperty(o, prop))
    {
      set_info (info, prop, "not6", true);
    }
    else
    {
      set_info (info, prop, "not6", false);
    }
  }
#endif

  for (var i = 0; i < all.length; i++)
  {
    var prop = all[i];

    if (typeof (o[prop]) == "undefined") {
      ASSetPropFlags (o, prop, 0, 13440);
      if (typeof (o[prop]) != "undefined") {
	set_info (info, prop, "newer", true);
	// don't set the flags back
      } else {
	set_info (info, prop, "newer", false);
      }
    } else {
      set_info (info, prop, "newer", false);
    }
  }

  for (var i = 0; i < all.length; i++)
  {
    var prop = all[i];

    // try changing value
    var old = o[prop];
    var val = "hello " + o[prop];
    o[prop] = val;
    if (o[prop] != val)
    {
      set_info (info, prop, "constant", true);

      // try changing value after removing constant propflag
      ASSetPropFlags (o, prop, 0, 4);
      o[prop] = val;
      if (o[prop] != val) {
	set_info (info, prop, "superconstant", true);
      } else {
	set_info (info, prop, "superconstant", false);
	o[prop] = old;
      }
      ASSetPropFlags (o, prop, 4);
    }
    else
    {
      set_info (info, prop, "constant", false);
      set_info (info, prop, "superconstant", false);
      o[prop] = old;
    }
  }

  for (var i = 0; i < all.length; i++)
  {
    var prop = all[i];

    // remove constant flag
    ASSetPropFlags (o, prop, 0, 4);

    // try deleting
    var old = o[prop];
    delete o[prop];
    if (hasOwnProperty (o, prop))
    {
      set_info (info, prop, "permanent", true);
    }
    else
    {
      set_info (info, prop, "permanent", false);
      o[prop] = old;
    }

    // put constant flag back, if it was set
    if (get_info (info, prop, "constant") == true)
      ASSetPropFlags (o, prop, 4);
  }

  for (var i = 0; i < all.length; i++) {
    var prop = all[i];

    // format propflags
    var flags = "";
    if (get_info (info, prop, "hidden") == true) {
      flags += "h";
    }
    if (get_info (info, prop, "superpermanent") == true) {
      flags += "P";
    } else if (get_info (info, prop, "permanent") == true) {
      flags += "p";
    }
    if (get_info (info, prop, "superconstant") == true) {
      flags += "C";
    } else if (get_info (info, prop, "constant") == true) {
      flags += "c";
    }
    if (get_info (info, prop, "not6") == true) {
      flags += "6";
    }
    if (get_info (info, prop, "newer") == true) {
      flags += "n";
    }
    if (flags != "")
      flags = " (" + flags + ")";

    var value = "";
    // add value depending on the type
    if (typeof (o[prop]) == "number" || typeof (o[prop]) == "boolean") {
      value += " : " + o[prop];
    } else if (typeof (o[prop]) == "string") {
      value += " : \"" + o[prop] + "\"";
    }

    // recurse if it's object or function and this is the place it has been
    // named after
    if (typeof (o[prop]) == "object" || typeof (o[prop]) == "function")
    {
      if (prefix + (prefix != "" ? "." : "") + identifier + "." + prop ==
	  o[prop]["mySecretId"])
      {
	trace (indentation + prop + flags + " = " + typeof (o[prop]));
	trace_properties_recurse (o[prop], prefix + (prefix != "" ? "." : "") +
	    identifier, prop, level + 1);
      }
      else
      {
	trace (indentation + prop + flags + " = " + o[prop]["mySecretId"]);
      }
    }
    else
    {
      trace (indentation + prop + flags + " = " + typeof (o[prop]) + value);
    }
  }
}

function generate_names (o, prefix, identifier, output)
{
  // mark the ones that are not hidden
  var nothidden = new Array ();
  for (var prop in o) {
    nothidden.push (prop);
  }

  // unhide everything
  ASSetPropFlags (o, null, 0, 1);

  var all = new Array ();
  for (var prop in o)
  {
    if (is_blaclisted (o, prop) == false) {
      // only get the ones that are not only in the __proto__
      if (hasOwnProperty (o, prop) == true) {
	all.push (prop);
      }
    }
  }
  all.sort ();

  // hide the ones that were already hidden
  ASSetPropFlags (o, null, 1, 0);
  ASSetPropFlags (o, nothidden, 0, 1);

  for (var i = 0; i < all.length; i++) {
    var newer = false;
    var prop = all[i];

    if (typeof (o[prop]) == "undefined") {
      ASSetPropFlags (o, prop, 0, 5248);
      if (typeof (o[prop]) != "undefined")
	newer = true;
    }

    if (typeof (o[prop]) == "object" || typeof (o[prop]) == "function") {
      if (hasOwnProperty (o[prop], "mySecretId")) {
	all[i] = null; // don't recurse to it again
      } else {
	o[prop]["mySecretId"] = prefix + (prefix != "" ? "." : "") +
	  identifier + "." + prop;
	if (output == true) {
	  trace ("  [\"" + prefix + (prefix != "" ? "." : "") + identifier + "\", \"" + prop + "\", " + newer + "],");
	}
      }
    }

    if (newer == true)
      ASSetPropFlags (o, prop, 5248);
  }

  for (var i = 0; i < all.length; i++) {
    var prop = all[i];

    if (prop != null) {
      var newer = false;

      if (typeof (o[prop]) == "undefined") {
	ASSetPropFlags (o, prop, 0, 13440);
	if (typeof (o[prop]) != "undefined")
	  newer = true;
      }

      if (typeof (o[prop]) == "object" || typeof (o[prop]) == "function")
	generate_names (o[prop], prefix + (prefix != "" ? "." : "") +
	    identifier, prop, output);

      if (newer == true)
	ASSetPropFlags (o, prop, 13440);
    }
  }
}

#ifdef GENERATE_NAMES

ASSetPropFlags (_global, "flash", 0, 4096);

trace ("// generated by trace_properties.as");
trace ("");
trace ("var mySecretIds = [");

// prioritize some things in the naming
_global["mySecretId"] = "_global";
_global.Object["mySecretId"] = "_global.Object";
trace ("  [\"_global\", \"Object\", false],");
_global.Function["mySecretId"] = "_global.Function";
trace ("  [\"_global\", \"Function\", false],");
_global.Function.prototype["mySecretId"] = "_global.Function.prototype";
trace ("  [\"_global.Function\", \"prototype\", false],");
_global.XMLNode["mySecretId"] = "_global.XMLNode";
trace ("  [\"_global\", \"XMLNode\", false],");
_global.flash.text.TextRenderer["mySecretId"] = "_global.flash.text.TextRenderer";
trace ("  [\"_global.flash.text\", \"TextRenderer\", false],");
_global.flash.filters.BitmapFilter["mySecretId"] = "_global.flash.filters.BitmapFilter";
trace ("  [\"_global.flash.filters\", \"BitmapFilter\", false],");

generate_names (_global.Object, "_global", "Object", true);
generate_names (_global.Function, "_global", "Function", true);
generate_names (_global.Function.prototype, "_global", "Function.prototype", true);
generate_names (_global.XMLNode, "_global", "XMLNode", true);
generate_names (_global.flash.text.TextRenderer, "_global.flash.text", "TextRenderer", true);
generate_names (_global.flash.filters.BitmapFilter, "_global.flash.filters", "BitmapFilter", true);
generate_names (_global, "", "_global", true);

trace ("];");

ASSetPropFlags (_global, "flash", 4096);

#else

#if __SWF_VERSION__ <= 5
#include "trace_properties_names_5.as"
#elif __SWF_VERSION__ == 6
#include "trace_properties_names_6.as"
#elif __SWF_VERSION__ == 7
#include "trace_properties_names_7.as"
#elif __SWF_VERSION__ == 8
#include "trace_properties_names_8.as"
#elif __SWF_VERSION__ >= 9
#include "trace_properties_names_9.as"
#endif

ASSetPropFlags (_global, "flash", 0, 4096);

for (var i = 0; i < mySecretIds.length; i++) {
  if (mySecretIds[i][2] == true)
    ASSetPropFlags (eval (mySecretIds[i][0]), mySecretIds[i][1], 0, 13440);
}

_global["mySecretId"] = "_global";

for (var i = 0; i < mySecretIds.length; i++) {
  eval (mySecretIds[i][0] + "." + mySecretIds[i][1])["mySecretId"] =
    mySecretIds[i][0] + "." + mySecretIds[i][1];
}

for (var i = 0; i < mySecretIds.length; i++) {
  if (mySecretIds[i][2] == true)
    ASSetPropFlags (eval (mySecretIds[i][0]), mySecretIds[i][1], 13440);
}

ASSetPropFlags (_global, "flash", 4096);

#endif

function trace_properties (o, prefix, identifier)
{
  if (typeof (o) == "object" || typeof (o) == "function")
  {
    if (!o.hasOwnProperty ("mySecretId")) {
      o["mySecretId"] = prefix + (prefix != "" ? "." : "") + identifier;
      generate_names (o, prefix, identifier);
    }

    if (prefix + (prefix != "" ? "." : "") + identifier == o["mySecretId"])
    {
      trace (prefix + (prefix != "" ? "." : "") + identifier + " = " +
	  typeof (o));
    }
    else
    {
      trace (prefix + (prefix != "" ? "." : "") + identifier + " = " +
	  o["mySecretId"]);
    }
    trace_properties_recurse (o, prefix, identifier, 1);
  }
  else
  {
    var value = "";
    if (typeof (o) == "number" || typeof (o) == "boolean") {
      value += " : " + o;
    } else if (typeof (o) == "string") {
      value += " : \"" + o + "\"";
    }
    trace (prefix + (prefix != "" ? "." : "") + identifier + " = " +
	typeof (o) + value);
  }
}
