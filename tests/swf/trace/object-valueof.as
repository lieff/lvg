// makeswf -v 7 -s 200x150 -r 1 -o object-valueof.swf object-valueof.as

trace ("Check behaviour of Object.prototype.valueOf()");

function foo () {
  trace (Object.prototype.valueOf.call (super));
};
trace (Object.prototype.valueOf ());
trace (Object.prototype.valueOf.call (foo));
x = new foo ();

loadMovie ("FSCommand:quit", "");
