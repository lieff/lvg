// makeswf -v 7 -r 1 -o xml-errors-7.swf xml-errors.as

var tests = [
  "<a><b/></a>",	// no error
  "<a/><?xml A",	// xmlDecl not closed
  "<a/><?xml A ?><b/><?xml B",
  "<a/><!DOCTYPE A",	// docTypeDecl not closed
  "<a/><!DOCTYPE A><b/><!DOCTYPE B",
  "<a>",		// no close tag
  "<a><b><c/></a>",
  "<b><c/></a>",
  "<a><b></a><c/>",
  "<a><b></c><d/></b></a>",
  "</a>",		// no open tag
  "<a></b><c/></a>",
  "<a></b></a><c/>",
  "<a/ ></a/>",		// tag missmatch, because </a/> is parsed as </a>
  "<a>&</a>",		// unescaped entities
  "<a A='&'/>",
  "<a A/><b/>",		// invalid attributes
  "<a A=x></a><b/>",
  "<a A='x></a><b/>",
  "<a/><!-- <b/>",	// comment not closed
  "<>a",		// empty tag
  "</>a",
  "<a",			// non terminated tag
  "<a/",
  "</a",
  "</a b='c'",
  "<a b='c'",
  "<a ='b'/>"		// empty attribute
];

for (var i = 0; i < tests.length; i++) {
  trace ("Testing: " + tests[i]);
  var a = new XML (tests[i]);
  trace (a);
  trace (a.status);
}

loadMovie ("FSCommand:quit", "");
