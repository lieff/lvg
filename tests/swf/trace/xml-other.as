// makeswf -v 7 -r 1 -o xml-node-7.swf xml-node.as

#include "values.as"

// Test XML issues that are not handled by the other tests

function test_constructor ()
{
  // normal
  trace (new XML ("<root/>"));
  trace (new XML ());

  // weird parameters
  trace (new XML ("<root/>", "<more>stuff</more>"));

  for (var i = 0; i < values.length; i++) {
    trace ("Constructing with: " + names[i]);
    trace (new XML (values[i]));
  }
}

function test_parse ()
{
  var x = new XML ();

  // normal
  trace (x.parseXML ("
<?xml XML decl ?>
<!DOCTYPE doctype decl>
<element>
  text <!-- comment -->  more text
  <child_element attribute='value'/>
</element>
    "));
  trace (x);

  // weird parameters
  for (var i = 0; i < values.length; i++) {
    trace ("Constructing with: " + names[i]);
    trace (x.parseXML (values[i]));
    trace (x);
  }
}

function test_create ()
{
  var x = new XML ("<root/>");

  // normal
  trace (x);
  trace (x.createElement ("name"));
  trace (x);
  trace (x.createTextNode ("value"));
  trace (x);

  // weird parameters
  trace (x.createElement ());
  trace (x.createTextNode ());
  trace (x.createElement ("name", "name2"));
  trace (x.createTextNode ("value", "value2"));

  for (var i = 0; i < values.length; i++) {
    trace ("Creating with: " + names[i]);
    trace (x.createElement (values[i]));
    trace (x.createTextNode (values[i]));
  }
}

function test_properties ()
{
  // normal
  var x = new XML ("<?xml XML declaration ?><!DOCTYPE doctype declaration ><root/>");
  trace (x.contentType);
  trace (x.docTypeDecl);
  trace (x.ignoreWhite);
  trace (x.loaded);
  trace (x.status);
  trace (x.xmlDecl);
  x = new XML ("<root/>");
  trace (x.contentType);
  trace (x.docTypeDecl);
  trace (x.ignoreWhite);
  trace (x.loaded);
  trace (x.status);
  trace (x.xmlDecl);
  x = new XML ();
  trace (x.contentType);
  trace (x.docTypeDecl);
  trace (x.ignoreWhite);
  trace (x.loaded);
  trace (x.status);
  trace (x.xmlDecl);

  // write
  for (var i = 0; i < values.length; i++) {
    trace ("Testing with: " + names[i]);
    x.contentType = values[i];
    trace (x.contentType);
    x.docTypeDecl = values[i];
    trace (x.docTypeDecl);
    x.ignoreWhite = values[i];
    trace (x.ignoreWhite);
    x.loaded = values[i];
    trace (x.loaded);
    x.status = values[i];
    trace (x.status);
    x.xmlDecl = values[i];
    trace (x.xmlDecl);
  }
}

test_constructor ();
test_parse ();
test_create ();
test_properties ();

loadMovie ("FSCommand:quit", "");
