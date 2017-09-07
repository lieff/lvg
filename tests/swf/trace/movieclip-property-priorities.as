// makeswf -v 7 -s 200x150 -r 1 -o movieclip-property-priorities.swf movieclip-property-priorities.as

createEmptyMovieClip ("_x", 0);
trace (_x);
addProperty ("_x", function () { return "got me!"; }, null);
trace (_x);
asm {
  push "", 0
  getproperty
  trace
};

addProperty ("_y", function () { return "got me"; }, null);
trace (_y);
createEmptyMovieClip ("_y", 0);
trace (_y);

this.__proto__.addProperty ("_xscale", function () { return "hi kid"; }, null);
trace (_xscale);

getURL ("fscommand:quit", "");
