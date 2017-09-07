/* gcc `pkg-config --libs --cflags libming` system-onstatus-netstream.c -o system-onstatus-netstream && ./system-onstatus-netstream
 */

#include <ming.h>

int
main (int argc, char **argv)
{
  SWFMovie movie;
  SWFVideoStream video;
  SWFDisplayItem item;
  SWFAction action;

  if (Ming_init ())
    return 1;
  Ming_useSWFVersion (7);

  movie = newSWFMovie();
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);
  video = newSWFVideoStream ();
  SWFVideoStream_setDimension (video, 200, 150);
  item = SWFMovie_add (movie, (SWFBlock) video);
  SWFDisplayItem_setName (item, "video");
  action = compileSWFActionCode (""
      "trace (\"Test System.onStatus\");"
      ""
      "System.onStatus = function (info)"
      "{"
      "  trace ('System.onStatus ' + info.code);"
      "  trace (arguments);"
      "  trace (arguments.caller);"
      "  trace (arguments.callee);"
      "  delete (ns.onStatus);"
      ""
      "  ns.onStatus = function () {"
      "    trace ('ns.onStatus ' + info.code);"
      "    getURL ('fscommand:quit', '');"
      "  };"
      ""
      "  trace (\"Calling play\");"
      "  ns.play (\"404.flv\");"
      "};"
      ""
      "var nc = new NetConnection ();"
      "nc.connect (null);"
      "var ns = new NetStream (nc);"
      "video.attachVideo (ns);"
      "ns.setBufferTime (5);"
      ""
      "function get () { trace ('get'); return null; };"
      "function set () { trace ('set: ' + arguments); };"
      "ns.addProperty ('onStatus', get, set);"
      ""
      "trace (\"Calling play\");"
      "ns.play (\"404.flv\");"
      "");
  SWFMovie_add (movie, (SWFBlock) action);
  SWFMovie_save (movie, "system-onstatus-netstream.swf");
  return 0;
}
