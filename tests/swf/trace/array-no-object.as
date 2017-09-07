// makeswf -v 7 -r 1 -o array-no-object-7.swf array-no-object.as

trace ("Before deleting Array");

var a = [1, 2];
trace (a);
trace (a.__constructor__);
trace (a.length);

var b = new Array (1, 2);
trace (b);
trace (b.__constructor__);
trace (b.length);

delete Array;

trace ("After deleting Array");

var c = [1, 2];
trace (c);
trace (c.__constructor__);
trace (c.length);

var d = new Array (1, 2);
trace (d);
trace (d.__constructor__);
trace (d.length);

loadMovie ("FSCommand:quit", "");
