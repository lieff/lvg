// makeswf -v 7 -r 1 -o test-7.swf test.as

// FIXME: We don't test for _level0 onData because it's called at the start by
// Flash player, but not by swfdec atm.

var a = this.createEmptyMovieClip ("a", this.getNextHighestDepth ());
var b = a.createEmptyMovieClip ("b", a.getNextHighestDepth ());

a.onData = function () {
  trace ("onData a: " + this.test);
};

b.onData = function () {
  trace ("onData b: " + this.test);
};

_level0.test = 1;
b.test = 1;

trace ("start _level0: " + _level0.test);
trace ("start a: " + a.test);
trace ("start b: " + _level0.test);

loadVariables ("loadvars.txt", "a");
loadVariablesNum ("loadvars.txt", 0);
// ming lowercases loadVariables without this
b["loadVariables"] ("loadvars.txt");

function quit () {
  trace ("quit _level0: " + _level0.test);
  trace ("quit a: " + a.test);
  trace ("quit b: " + b.test);
  loadMovie ("FSCommand:quit", "");
}

setTimeout (quit, 250);
