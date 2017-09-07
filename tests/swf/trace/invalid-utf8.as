// vivi-compile -O 0 -o invalid-utf8.swf invalid-utf8.as

// NB: There is no compiler that compiles invalid utf-8 properly. Therefore,
// I hexedited the resultig FLash file and replaced the occurences of "xx" with
// 0xFF 0xFF bytes

asm {
  pool "", "xx"
  push pool 0, pool 1
  add2
  trace
  push "xx"
  trace
};

getURL ("fscommand:quit", "");

