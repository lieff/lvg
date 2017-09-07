// makeswf -v 7 -s 200x150 -r 1 -o crash-0.5.2-cvs-return-from-block.swf crash-0.5.2-cvs-return-from-block.as

foo = function () {
  trace ("CHeck that returning from inside a block works.");
  with (this) {
    return;
    trace ("hi");
  };
};
foo ();

loadMovie ("FSCommand:quit", "");
