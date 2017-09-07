// makeswf -v 7 -s 200x150 -r 1 -o settarget-remove.swf settarget-remove.as

var o = {};
o.__resolve = function () {
  trace ("__resolve: " + arguments);
  return "resolved";
};

for (var i = 0; i <= 13; i++) {
  o[i] = "hello";
  ASSetPropFlags (o, i, 1 << i, 0);
  trace (o[i]);
}

for (var i = 0; i <= 13; i++) {
  ASSetPropFlags (o, "__resolve", 1 << i, 0);
  trace ("flags for resolve: " + i);
  trace (o["hello"]);
  ASSetPropFlags (o, "__resolve", 0, 1 << i);
}

getURL ("fscommand:quit", "");
