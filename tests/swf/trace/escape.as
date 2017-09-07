// makeswf -v 7 -s 200x150 -r 1 -o escape.swf escape.as

trace ("Check escape() function");
s = "";
for (i = 1; i < 1000; i++) {
  s += String.fromCharCode (i);
};
es = escape (s);
trace (s);
trace (es);
trace (unescape (es));
trace (unescape (s));

loadMovie ("FSCommand:quit", "");
