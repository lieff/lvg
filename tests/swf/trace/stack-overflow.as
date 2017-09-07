// makeswf -v 7 -s 200x150 -r 1 -o stack-overflow.swf stack-overflow.as

trace ("Check the stack overflows correctly");
i = 1;
function foo () {
  trace (i);
  i++;
  foo ();
};
foo ();
trace ("done @ " + i);

loadMovie ("FSCommand:quit", "");
