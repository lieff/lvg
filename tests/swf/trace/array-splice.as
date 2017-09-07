// makeswf -v 7 -s 200x150 -r 1 -o array-splice.swf array-splice.as

function check (a) {
  trace ("checking...");
  for (i in a)
    trace (i);
};

a = ["x"];
a.splice (0, 1);
check (a);

a = [1, 2, 3, 4];
a.splice (0, 3);
check (a);

getURL ("fscommand:quit", "");
