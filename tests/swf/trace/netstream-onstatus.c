/* gcc `pkg-config --libs --cflags libming` netstream-onstatus.c -o netstream-onstatus && ./netstream-onstatus
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
      "trace (\"Test what onStatus messages exist in an average movie.\");"
      "nc = new NetConnection ();"
      "nc.connect (null);"
      "ns = new NetStream (nc);"
      "ns.onStatus = function (info) {"
      "    trace (\"NetStream onStatus called\");"
      "	   trace (info.code);"
      "	   trace (info.level);"
      "};"
      "video.attachVideo (ns);"
      "ns.setBufferTime (5);"
      "trace (\"Calling play\");"
      "ns.play (\"video.flv\");"
      "trace (\"done calling play\");"
      "");
  SWFMovie_add (movie, (SWFBlock) action);
  SWFMovie_save (movie, "netstream-onstatus.swf");
  return 0;
}
