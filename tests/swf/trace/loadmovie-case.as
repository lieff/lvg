// makeswf -v 7 -r 1 -o loadmovie-case-7.swf loadmovie-case.as

trace ("Testing loadMovie's handling of _level in wrong case");

loadMovie ("fscommand-7.swf", "_leVEL0");

function quit () {
  trace ("Timeout, exiting");
  loadMovie ("FSCommand:quit", "");
}

setTimeout (quit, 250);
