// makeswf -v 7 -s 200x150 -r 1 -o foreach-proto-simple.swf foreach-proto-simple.as

trace ("Run for in loop with properties on parent");
o = new Object ();
o.z = 42;
p = new Object;
p.y = 21;
p.__proto__ = o;
o.x = 842;
a = [];
for (i in p)
  a.push (i);
a.sort();
for (i = 0; i < a.length; i++)
  trace (a[i]);

loadMovie ("FSCommand:quit", "");
