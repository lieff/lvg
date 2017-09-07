// makeswf -v 7 -r 1 -o xml-parse-7.swf xml-parse.as

function my_escape (value)
{
  if (String (value).indexOf ("
") == -1)
    return String (value);

  return escape (value);
}

function trace_attributes_property (node, prop)
{
  var attr_list = new Array ();
  var tmp = node[prop];
  for (var attribute in tmp) {
    attr_list.push (escape (attribute) + "=" + escape(node[prop][attribute]));
  }
  attr_list.sort ();
  trace (prop + ": " + attr_list.length + ": " + attr_list.join ("&"));
}

function trace_node_property (node, prop)
{
  if (node[prop] == undefined) {
    trace (prop + ": undefined");
  } else if (node[prop].nodeType == 1) {
    trace (prop + ": TAG(1): '" + my_escape (node[prop].nodeName) + "'");
  } else {
    trace (prop + ": TEXT(" + my_escape (node[prop].nodeType) + "): '" +
	my_escape (node[prop].nodeValue) + "'");
  }
}

function trace_node_array_property (node, prop)
{
  var strings = new Array ();
  for (var i = 0; i < node[prop].length; i++) {
    if (node[prop][i] == undefined) {
      strings.push ("undefined");
    } else if (node[prop][i].nodeType == 1) {
      strings.push ("TAG(1): '" + my_escape (node[prop][i].nodeName) + "'");
    } else {
      strings.push ("TEXT(" +  my_escape (node[prop][i].nodeType) + "): '" +
	  my_escape (node[prop][i].nodeValue) + "'");
    }
  }
  trace (prop + ": " + node[prop].length + ": " + strings.join ());
}

var node_properties = [
  "parentNode",
  "firstChild",
  "lastChild",
  "nextSibling",
  "previousSibling"
];

var string_properties = [
  "localName",
  "namespaceURI",
  "nodeName",
  "nodeType",
  "nodeValue",
  "prefix"
];

var xml_properties = [
  "docTypeDecl",
  "ignoreWhite",
  "loaded",
  "status",
  "xmlDecl"
];

function print_node (node)
{
  trace_attributes_property (node, "attributes");

  for (var i = 0; i < string_properties.length; i++) {
    trace (string_properties[i] + ": " +
	my_escape (node[string_properties[i]]));
  }

  for (var i = 0; i < node_properties.length; i++) {
    trace_node_property (node, node_properties[i]);
  }

  trace_node_array_property (node, "childNodes");

  for (var i = 0; i < node.childNodes.length; i++) {
    trace ("---");
    print_node (node.childNodes[i]);
  }
}

var x = new XML ();
var num = 1;

x.onLoad = function (success) {
  trace ("Success: " + success);
  trace ("---");
  trace (this.getBytesLoaded ());
  trace (this.getBytesTotal ());
  for (var i = 0; i < xml_properties.length; i++) {
    trace (xml_properties[i] + ": " + my_escape (this[xml_properties[i]]));
  }
  trace ("---");
  print_node (this);
  trace ("---");

  if (success) {
    num++;
    trace ("Loading: xml-parse-" + num + ".xml");
    this.load ("xml-parse-" + num + ".xml");
  } else {
    if (this.ignoreWhite == false) {
      trace ("### Running tests with ignoreWhite...");
      this.ignoreWhite = true;
      num = 1;
      trace ("Loading: xml-parse-" + num + ".xml");
      this.load ("xml-parse-" + num + ".xml");
    } else {
      loadMovie ("FSCommand:quit", "");
    }
  }
};

trace ("### Running tests without ignoreWhite...");
x.ignoreWhite = true;
trace ("Loading: xml-parse-" + num + ".xml");
x.load ("xml-parse-" + num + ".xml");
