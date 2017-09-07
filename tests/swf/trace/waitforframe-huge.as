// makeswf -v 4 -s 200x150 -r 1 -o waitforframe-huge.swf waitforframe-huge.as

if (frameLoaded (10000)) {
  trace ("hi");
}

loadMovie ("FSCommand:quit", "");

