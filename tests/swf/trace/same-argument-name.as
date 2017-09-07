// makeswf -v 7 -s 200x150 -r 1 -o same-argument-name.swf same-argument-name.as

function foo (x, x) {
  trace (x);
};
foo (1, 2);

getURL ("fscommand:quit", "");
