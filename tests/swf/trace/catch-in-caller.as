// makeswf -v 7 -s 200x150 -r 1 -o movie23.swf movie23.as

trace ("Throw inside a function and catch outside of that function");

function foo () {
  throw "hi";
};

try {
  foo ();
} catch (e) {
  trace (e);
};
trace ("done");

loadMovie ("FSCommand:quit", "");
