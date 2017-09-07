// makeswf -v 7 -s 200x150 -r 1 -o asciitochar.swf asciitochar.as
                                                                     
trace("CharToAscii testcase");

#if __SWF_VERSION__ > 4
for (i=0; i<1000; i++) {
  x = chr (i);
  trace (i + " - "  + x + " - " + ord (x));
}
#endif

trace(ord("å"));
trace(ord("ä"));
trace(ord("ö"));
trace(ord("Å"));
trace(ord("Ä"));
trace(ord("Ö"));

loadMovie("FSCommand:quit", "");

