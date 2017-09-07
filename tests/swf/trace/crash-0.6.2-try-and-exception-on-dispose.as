// makeswf -v 7 -s 200x150 -r 1 -o crash-0.6.2-try-and-exception-on-dispose.swf crash-0.6.2-try-and-exception-on-dispose.as

getURL ("fscommand:quit", "");

function boom () {
  try {
    return;
    trace ("hi");
  } catch (e) {
  };
};

boom ();
throw ("hi");
