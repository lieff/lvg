// makeswf -v 7 -s 200x150 -r 1 -o ASSetPropFlags-ignore.swf ASSetPropFlags-ignore.as

trace ("Check how referencing variables with the 5 magic version flags works");
o = {};
o.__proto__ = {};
o.__proto__.a = 42;
o.__proto__.b = 42;
o.__proto__.c = 42;
o.__proto__.d = 42;
o.__proto__.e = 42;
o.a = 21;
o.b = 21;
o.c = 21;
o.d = 21;
o.e = 21;
ASSetPropFlags (o, "a", 8192);
ASSetPropFlags (o, "b", 4096);
ASSetPropFlags (o, "c", 1024);
ASSetPropFlags (o, "d", 256);
ASSetPropFlags (o, "e", 128);
trace (o.a);
trace (o.b);
trace (o.c);
trace (o.d);
trace (o.e);

loadMovie ("FSCommand:quit", "");
