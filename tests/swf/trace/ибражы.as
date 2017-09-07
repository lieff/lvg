// makeswf -v 7 -s 200x150 -r 1 -o ибражы.swf ибражы.as

if (this == _level0) {
  count = 0;
  spawn = function (name) {
    count++;
    loadMovie (name + "-" + __SWF_VERSION__ + ".swf", "_level" + count);
  };

  spawn ("ибражы");
  spawn ("%D0%B8%D0%B1%D1%80%D0%B0%D0%B6%D1%8B");
  spawn ("ибр%D0%B0%D0%B6%D1%8B");
} else {
  trace (_url.substr (_url.lastIndexOf ("/")));
  _level0.count--;
  if (_level0.count == 0) {
    getURL ("fscommand:quit", "");
  }
}
