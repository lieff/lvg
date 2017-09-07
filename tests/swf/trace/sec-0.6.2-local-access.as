// makeswf -v 7 -s 200x150 -r 1 -o sec-0.6.2-local-access.swf sec-0.6.2-local-access.as

x = new XML ();
x.onData  = function (data) {
  trace (data);
  getURL ("fscommand:quit", "");
};
x.load ("sec-0.6.2-local-access-7.swf.trace");

y = new XML ();
y.onData  = function (data) {
  trace (data);
  getURL ("fscommand:quit", "");
};
y.load (_url + ".trace");
