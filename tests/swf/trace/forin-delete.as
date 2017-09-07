// makeswf -v 7 -r 1 -o forin-delete-7.swf forin-delete.as

obj = new Object ();
for (var i = 0; i < 10; i++) {
  obj[i] = "something";
}
a = new Array();
for (var prop in obj) {
  a.push (prop);
  delete obj[prop];
}
trace (a.sort ());

loadMovie ("FSCommand:quit", "");
