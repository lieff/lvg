// makeswf -v 7 -s 200x150 -r 1 -o externalinterface-escape.swf externalinterface-escape.as

strings = [];
for (i = 0; i < 300; i++) {
  strings.push (String.fromCharCode (i));
}
strings = strings.concat ([ "", "\"\"", "\"", "'",
  "&nbsp;", "&amp;", "&hi;", "&", 
  "_root", "_level0"
]);

for (i = 0; i < strings.length; i++) {
  trace (">>> " + i + ": " + strings[i]);
  trace (flash.external.ExternalInterface._jsQuoteString (strings[i]));
  t = flash.external.ExternalInterface._escapeXML (strings[i]);
  trace (t);
  trace (flash.external.ExternalInterface._unescapeXML (t));
  trace (flash.external.ExternalInterface._unescapeXML (strings[i]));
}
trace (">>> no args");
trace (flash.external.ExternalInterface._jsQuoteString ());
trace (flash.external.ExternalInterface._escapeXML ());
trace (flash.external.ExternalInterface._unescapeXML ());

loadMovie ("fscommand:quit", "");
