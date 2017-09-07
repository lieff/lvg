// makeswf -v 7 -r 1 -o xml-node-7.swf xml-node.as

#include "values.as"

// Test XMLNode issues that are not handled by xml-parse test

function test_childNodes ()
{
  var x = new XML ("<root><a/><b/>c<d/></root>");
  var root = x.firstChild;

  // normal
  trace (root);
  trace (root.childNodes);

  // add fake elements to childNodes array and test what methods reset the array
  root.childNodes.push ("fake");
  trace (root.childNodes);
  root.firstChild.removeNode ();
  trace (root.childNodes);
  root.childNodes.push (new XMLNode (1, "fake"));
  trace (root.childNodes);
  root.appendChild (new XMLNode (1, "e"));
  trace (root.childNodes);
  ASSetPropFlags (root.childNodes, "0", 0, 4); // remove write-protection
  root.childNodes[0] = new XMLNode (3, "fake");
  trace (root.childNodes);
  root.insertBefore (new XMLNode (1, "f"), root.lastChild);
  trace (root.childNodes);
}

function test_references ()
{
  var x = new XML ("<parent><previoussibling/><root><firstchild/><middlechild/><lastchild/></root><nextsibling/></parent>");
  var root = x.firstChild.firstChild.nextSibling;

  // normal
  trace (root.parentNode.nodeName);
  trace (root.firstChild.nodeName);
  trace (root.lastChild.nodeName);
  trace (root.nextSibling.nodeName);
  trace (root.previousSibling.nodeName);
  trace (root.hasChildNodes ());

  // writing
  root.parentNode = new XMLNode (1, "test");
  root.firstChild = new XMLNode (1, "test");
  root.lastChild = new XMLNode (1, "test");
  root.nextSibling = new XMLNode (1, "test");
  root.previousSibling = new XMLNode (1, "test");
  trace (root.parentNode.nodeName);
  trace (root.firstChild.nodeName);
  trace (root.lastChild.nodeName);
  trace (root.nextSibling.nodeName);
  trace (root.previousSibling.nodeName);
  trace (root.hasChildNodes ());

  // not found
  root = new XMLNode (1, "root");
  trace (root.parentNode);
  trace (root.firstChild);
  trace (root.lastChild);
  trace (root.nextSibling);
  trace (root.previousSibling);
  trace (root.hasChildNodes ());

  // fake elements in childNodes array
  root.childNodes.push (new XMLNode (1, "fake"));
  trace (root.parentNode);
  trace (root.firstChild);
  trace (root.lastChild);
  trace (root.nextSibling);
  trace (root.previousSibling);
  trace (root.hasChildNodes ());
}

function test_cloneNode ()
{
  var x = new XML ("<root><a/><b/>c<d/></root>");
  var root = x.firstChild;

  // normal
  var root2 = root.cloneNode ();
  trace (root2.childNodes);
  root2 = root.cloneNode (false);
  trace (root2.childNodes);
  root2 = root.cloneNode (true);
  trace (root2.childNodes);

  // fake elements in childNodes array
  root.childNodes.push (new XMLNode (1, "fake"));
  root2 = root.cloneNode (true);
  trace (root2.childNodes);
}

function test_nodeName ()
{
  var a = new XMLNode (1, "a:b");

  // normal
  trace (a.nodeName);
  trace (a.prefix);
  trace (a.localName);
  a.nodeName = "c:d";
  trace (a.nodeName);
  trace (a.prefix);
  trace (a.localName);
  // these are read-only
  a.prefix = "e";
  trace (a.nodeName);
  trace (a.prefix);
  trace (a.localName);
  a.localName = "f";
  trace (a.nodeName);
  trace (a.prefix);
  trace (a.localName);

  // weird values
  var testvalues = ["", ":", ":b", "a:", "a::b", "a::", "::b", "::" ];
  testvalues = testvalues.concat (values);
  for (var i = 0; i < testvalues.length; i++) {
    a.nodeName = testvalues[i];
    trace (a.nodeName);
    trace (a.prefix);
    trace (a.localName);
  }
}

function test_namespace ()
{
  var x = new XML ();
  x.ignoreWhite = true;
  x.parseXML ("
<root:root xmlns='nsdefault' xmlns:a='nsa'>
  <child xmlns='nsdefault_at_child' xmlns:d='nsd'>
    <a:a/>
    <b:b/>
  </child>
  c
  <d:d/>
  <e:e/>
</root:root>
    ");
  var root = x.firstChild;

  // normal
  var ele = root.firstChild.firstChild;
  trace (ele.namespaceURI);
  trace (ele.getNamespaceForPrefix ());
  trace (ele.getNamespaceForPrefix (""));
  trace (ele.getNamespaceForPrefix ("a"));
  ele = ele.nextSibling;
  trace (ele.namespaceURI);
  trace (ele.getNamespaceForPrefix ());
  trace (ele.getNamespaceForPrefix (""));
  trace (ele.getNamespaceForPrefix ("a"));
  ele = root.lastChild.previousSibling;
  trace (ele.namespaceURI);
  trace (ele.getNamespaceForPrefix ());
  trace (ele.getNamespaceForPrefix (""));
  trace (ele.getNamespaceForPrefix ("a"));
  ele = ele.nextSibling;
  trace (ele.namespaceURI);
  trace (ele.getNamespaceForPrefix ());
  trace (ele.getNamespaceForPrefix (""));
  trace (ele.getNamespaceForPrefix ("a"));

  // writing
  ele = root.firstChild.firstChild;
  ele.namespaceURI = "fake";
  trace (ele.namespaceURI);
}

function trace_attributes (attrs)
{
  var props = new Array ();
  for (var prop in attrs) {
    props.push (prop);
  }
  props.sort ();
  var str = "";
  for (var i = 0; i < props.length; i++) {
    var prop = props[i];
    str += "&" + prop + "=" + attrs[prop];
  }
  trace (str.substr(1));
}

function test_attributes ()
{
  var x = new XML ("<root a='a' b='b' c='' &amp;='&amp;' g='&'/>");
  var root = x.firstChild;

  // normal
  trace_attributes (root.attributes);
  root.attributes["d"] = "d";
  trace_attributes (root.attributes);
  root.attributes["a"] = "A";
  trace_attributes (root.attributes);
  delete root.attributes["b"];
  trace_attributes (root.attributes);

  // evil characters
  root.attributes["'"] = "'";
  root.attributes["\""] = "\"";
  root.attributes["&"] = "&";
  root.attributes["&amp;"] = "&amp;";
  trace_attributes (root.attributes);
}

function test_modify ()
{
  var x = new XML ("<root><a/><b/>c<d/></root>");
  var root = x.firstChild;

  // normal
  trace (x);
  trace (root);
  root.removeNode ();
  trace (x);
  trace (root);
  x.appendChild (root);
  trace (x);
  trace (root);
  root.insertBefore (new XMLNode (1, "f"));
  trace (root);
  root.insertBefore (new XMLNode (1, "f"), root.lastChild);
  trace (root);
  root.appendChild (new XMLNode (1, "g"), new XMLNode (1, "h"));
  trace (root);

  // weird cases
  root.appendChild (root.firstChild);
  trace (root);
  root.insertBefore (root.lastChild, root.lastChild);
  trace (root);
  root.appendChild (root);
  trace (root.lastChild.nodeName);
  root.firstChild.appendChild (root);
  trace (root.firstChild.parentNode.nodeName);
  trace (root.firstChild.lastChild.nodeName);
}

function test_constructor ()
{
  // normal
  trace (new XMLNode (1, "name"));
  trace (new XMLNode (3, "value"));

  for (var i = 0; i < values.length; i++) {
    trace ("Creating with: " + names[i]);
    trace (new XMLNode (1, values[i]));
    trace (new XMLNode (3, values[i]));
    trace (new XMLNode (values[i], "value"));
  }
}

test_childNodes ();
test_references ();
test_cloneNode ();
test_nodeName ();
test_namespace ();
test_attributes ();
test_modify ();
test_constructor ();

loadMovie ("FSCommand:quit", "");
