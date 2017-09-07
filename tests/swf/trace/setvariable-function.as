// makeswf -v 7 -s 200x150 -r 1 -o setvariable-function.swf setvariable-function.as

trace ("Check SetVariable inside functions sets variables on the target");

function foo () {
  bla = 42;
  trace (this.bla);
  trace (_root.bla);
};
foo ();
trace (bla);
trace (this.bla);

loadMovie ("FSCommand:quit", "");
