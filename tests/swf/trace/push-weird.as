// makeswf -v 7 -s 200x150 -r 1 -o push-weird.swf push-weird.as

asm {
  pool "Hello World"
  push pool 0
  push type 42, type 23, undefined
  trace
  trace
  push
  trace
  push big 0
  trace
};

getURL ("fscommand:quit", "");
