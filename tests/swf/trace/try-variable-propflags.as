// makeswf -v 7 -s 200x150 -r 15 -o try-variable-propflags.swf try-variable-propflags.as

trace ("See if the error variable can be changed and deleted");

try {
  throw "Error";
} catch (e) {
  trace (e);
  e = "Error2";
  trace (e);
  delete e;
  trace (e);
};

getURL ("fscommand:quit", "");
