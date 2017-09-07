// makeswf -v 7 -r 1 -o fscommand-7.swf fscommand.as

trace ("Testing loadMovie's handling of FSCommand");

loadMovie ("fscommand:quit", "_level0");

function quit () {
  trace ("Quit");
  loadMovie ("FSCommand:quit", "");
}

setTimeout (quit, 250);
