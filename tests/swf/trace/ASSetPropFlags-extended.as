// makeswf -v 7 -s 200x150 -r 1 -o ASSetPropFlags-extended.swf ASSetPropFlags-extended.as

trace (">>>> " + _url.substr (_url.lastIndexOf ("/")) + " (" + _level0.count + ")");
this.check = Object;
if (_level0.count == undefined) {
  trace ("init");
  _level0.count = 0;
  _level0.o = {};
  for (var i = 0; i < 16384; i += 8) {
    _level0.o["x" + i] = "Hello World";
    ASSetPropFlags (_level0.o, "x" + i, i);
  }
  loadMovie ("ASSetPropFlags-extended-5.swf", "_level5");
  loadMovie ("ASSetPropFlags-extended-6.swf", "_level6");
  loadMovie ("ASSetPropFlags-extended-7.swf", "_level7");
  loadMovie ("ASSetPropFlags-extended-8.swf", "_level8");
  loadMovie ("ASSetPropFlags-extended-9.swf", "_level9");
} else {
  trace (_level0.o);
  for (var i = 0; i < 16384; i += 8) {
    trace (i + " => " + _level0.o["x" + i]);
  }
}

_level0.count++;
if (_level0.count == 5) {
  trace (_level5.check == _level6.check);
  trace (_level5.check == _level7.check);
  trace (_level5.check == _level8.check);
  trace (_level6.check == _level7.check);
  trace (_level6.check == _level8.check);
  trace (_level7.check == _level8.check);
  trace (_level5.check == _level9.check);
  trace (_level6.check == _level9.check);
  trace (_level7.check == _level9.check);
  trace (_level8.check == _level9.check);
  trace (_level9.check == _level9.check);
  loadMovie ("FSCommand:quit", "");
}
trace (">>>> " + _url.substr (_url.lastIndexOf ("/")) + " (" + _level0.count + ")");
