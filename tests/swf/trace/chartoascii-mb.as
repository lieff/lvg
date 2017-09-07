// makeswf -v 7 -s 200x150 -r 1 -o chartoascii-mb-7.swf chartoascii-mb.as

trace("MBCharToAscii testcase");

#if __SWF_VERSION__ > 4
for (i=0; i<1000; i++) {
  x = chr (i);
  trace (i + " - "  + x + ":");
  asm {
    push "x"
    getvariable
    mbord
    trace
  };
}
#endif

asm {
  push "å"
  mbord
  trace
  push "ä"
  mbord
  trace
  push "ö"
  mbord
  trace
  push "Å"
  mbord
  trace
  push "Ä"
  mbord
  trace
  push "Ö"
  mbord
  trace
};

getURL("FSCommand:quit", "");
