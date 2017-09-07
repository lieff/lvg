// makeswf -v 7 -r 1 -o text-field-variable-7.swf text-field-variable.as

this.createTextField ("t", 0, 0, 0, 50, 50);

// movie clip
this.createEmptyMovieClip ("m", 1);
m.test = 1;
t.variable = "m.test";
trace (t.text);
m.test = 2;
trace (t.text);
t.text = 3;
trace (m.test);

// object
o = new Object();
o.test = 4;
t.variable = "o.test";
trace (t.text);
o.test = 5;
trace (t.text);
t.text = 6;
trace (o.test);

// nothing
delete n;
n.test = 7;
t.variable = "n.test";
trace (t.text);
n.test = 8;
trace (t.text);
t.text = 9;
trace (n.test);

// creating variable afterwards, re-setting t.variable
t.variable = "o.test2";
trace (t.text);
o.test2 = 1;
trace (t.text);
t.variable = "o.test2";
trace (t.text);

getURL ("FSCommand:quit", "");
