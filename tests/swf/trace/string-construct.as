// makeswf -v 7 -s 200x150 -r 1 -o string-construct.swf string-construct.as

hi = String ("Hello World");
trace (hi);
trace (typeof (hi));

hi = new String ("Hello World");
trace (hi);
trace (typeof (hi));

hi = String (this);
trace (hi);
trace (typeof (hi));

hi = new String (this);
trace (hi);
trace (typeof (hi));

loadMovie ("FSCommand:quit", "");
