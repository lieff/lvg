// makeswf -v 7 -r 1 -o xml-properties2-7.swf xml-properties2.as

#include "trace_properties.as"

function trace_xml_node_properties (o, prefix, identifier)
{
  trace_properties (o, prefix, identifier);
  trace_properties (o.attributes, prefix + "." + identifier, "attributes");
  trace_properties (o.childNodes, prefix + "." + identifier, "childNodes");
  trace_properties (o.firstChild, prefix + "." + identifier, "firstChild");
  trace_properties (o.lastChild, prefix + "." + identifier, "lastChild");
  trace_properties (o.localName, prefix + "." + identifier, "localName");
  trace_properties (o.namespaceURI, prefix + "." + identifier, "namespaceURI");
  trace_properties (o.nextSibling, prefix + "." + identifier, "nextSibling");
  trace_properties (o.nodeName, prefix + "." + identifier, "nodeName");
  trace_properties (o.nodeType, prefix + "." + identifier, "nodeType");
  trace_properties (o.nodeValue, prefix + "." + identifier, "nodeValue");
  trace_properties (o.parentNode, prefix + "." + identifier, "parentNode");
  trace_properties (o.prefix, prefix + "." + identifier, "prefix");
  trace_properties (o.previousSibling, prefix + "." + identifier, "previousSibling");
}

function trace_xml_properties (o, prefix, identifier)
{
  trace_xml_node_properties (o, prefix, identifier);
  trace_properties (o.contentType, prefix + "." + identifier, "contentType");
  trace_properties (o.docTypeDecl, prefix + "." + identifier, "docTypeDecl");
  trace_properties (o.ignoreWhite, prefix + "." + identifier, "ignoreWhite");
  trace_properties (o.loaded, prefix + "." + identifier, "loaded");
  trace_properties (o.status, prefix + "." + identifier, "status");
  trace_properties (o.xmlDecl, prefix + "." + identifier, "xmlDecl");
}

var a = new XMLNode (1, "element");
var b = new XMLNode (3, "text");
var c = new XMLNode ();
var d = new XML ("<element attribute='value'>text</element>");
var e = d.firstChild;
var f = new XML ();

trace_xml_node_properties (a, "local", "a");
trace_xml_node_properties (b, "local", "b");
trace_xml_node_properties (c, "local", "c");
trace_xml_properties (d, "local", "d");
trace_xml_node_properties (e, "local", "e");
trace_xml_properties (f, "local", "f");

loadMovie ("FSCommand:quit", "");

