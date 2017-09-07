// makeswf -v 7 -s 200x150 -r 15 -o text-field-type-of.swf text-field-type-of.as

createTextField ("a", 0, 0, 0, 100, 100);
var b = a;
a.a = 1;
b.b = 2;
trace ("Text: " + typeof (a) + ", " + typeof (b) + " : " + a.b + ", " + b.a);

a.removeTextField ();
trace ("Remove: " + typeof (a) + ", " + typeof (b) + " : " + a.b + ", " + b.a);

createEmptyMovieClip ("a", 0);
a.a = 3;
b.b = 4;
trace ("Movie: " + typeof (a) + ", " + typeof (b) + " : " + a.b + ", " + b.a);

a.removeMovieClip();
trace ("Remove: " + typeof (a) + ", " + typeof (b) + " : " + a.b + ", " + b.a);

createTextField ("a", 0, 0, 0, 100, 100);
a.a = 5;
b.b = 6;
trace ("Text: " + typeof (a) + ", " + typeof (b) + " : " + a.b + ", " + b.a);

getURL ("fscommand:quit", "");
