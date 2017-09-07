// makeswf -v 7 -s 200x150 -r 1 -o movie2.swf movie2.as

trace ("Check how with behaves when called on non-object values");

with ("hi") {
  trace (indexOf ("i"));
};
with (undefined) {
  trace ("undefined");
};
trace ("...");
with (null) {
  trace ("null");
};
trace ("done");

loadMovie ("FSCommand:quit", "");
