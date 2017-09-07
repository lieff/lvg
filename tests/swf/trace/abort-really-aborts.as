// makeswf -v 7 -s 200x150 -r 1 -o abort-really-aborts.swf abort-really-aborts.as

trace ("Check that abort really aborts script execution forever");

createEmptyMovieClip ("movie", 0);
movie.onEnterFrame = function () {
  trace ("hi");
};

function foo () {
  foo ();
};
foo ();
