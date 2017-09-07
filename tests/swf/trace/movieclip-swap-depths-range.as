// makeswf -v 7 -r 15 -o movieclip-swap-depths-range-7.swf movieclip-swap-depths-range.as

function trace_depths () {
  //trace ("this: " + this.getDepth ());
  trace ("a: " + a.getDepth ());
  trace ("b: " + b.getDepth ());
  trace ("c: " + c.getDepth ());
}

this.createEmptyMovieClip ("a", 1);
this.createEmptyMovieClip ("b", 2);
this.createEmptyMovieClip ("c", 3);

trace_depths ();

a.swapDepths (-16385);
b.swapDepths (-16384);
c.swapDepths (-16383);

trace_depths ();

a.swapDepths (-1);
b.swapDepths (0);
c.swapDepths (1);

trace_depths ();

a.swapDepths (1048575);
b.swapDepths (1048576);
c.swapDepths (1048577);

trace_depths ();

a.swapDepths (2130690045);
b.swapDepths (2130690046);
c.swapDepths (2130690047);

trace_depths ();

a.swapDepths (2130690044);
b.swapDepths (2130690043);
c.swapDepths (2130690042);

trace_depths ();

getURL ("FSCommand:quit", "");
