// makeswf -v 7 -s 200x150 -r 1 -o object-watch-segv.swf object-watch-segv.as

function foo (name, old, new, data) {
  delete this[x];
  trace ("run " + data);
  return new + 1;
};
function evil () {
  this.unwatch ("x");
  return 5;
};

trace (this.watch ("x", foo, 1));
trace (this.watch ("x", foo, 2));
this.x = 42;
trace (x);
delete this[x];
this.x = 10;
trace (x);
trace (this.hasOwnProperty ("x"));
trace (this.watch ("x", evil));
this.x = 100;
trace (this.x);

loadMovie ("FSCommand:quit", "");
