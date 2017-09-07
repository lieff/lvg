// makeswf -v 7 -s 200x150 -r 1 -o prototype-movie.swf movie6.as

trace ("Check that movies cannot be prototypes");

o = new Object ();
this.foo = 42;
trace (o.addProperty);
o.__proto__ = this;
trace (o.foo);
trace (o.addProperty);

loadMovie ("FSCommand:quit", "");
