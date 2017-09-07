// makeswf -v 7 -s 200x150 -r 1 -o asnative-create.swf asnative-create.as

trace ("Check ASnative creates a new functio object every time");

foo = ASnative(5, 0);
foo.bla = 42;
trace (foo);
trace (foo.bla);
foo2 = ASnative(5, 0);
trace (foo2.bla);
trace (foo == foo2);
trace (foo == Mouse.show);
trace (Mouse.show.bla);

loadMovie ("FSCommand:quit", "");
