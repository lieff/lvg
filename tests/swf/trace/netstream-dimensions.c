/* gcc `pkg-config --libs --cflags libming` netstream-dimensions.c -o netstream-dimensions && ./netstream-dimensions
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
      "trace (\"Test Video object's width and height properties\");"
      "nc = new NetConnection ();"
      "nc.connect (null);"
      "ns = new NetStream (nc);"
      "ns.onStatus = function (info)"
      "{"
      "  trace ('onStatus ' + info.code);"
      "  trace ('width = ' + video.width);"
      "  trace ('height = ' + video.height);"
      "  video.width++;"
      "  video.height++;"
      "  if (info.code == 'NetStream.Play.Stop')"
      "    loadMovie ('FSCommand:quit', '');"
      "};"
      "video.attachVideo (ns);"
      "ns.setBufferTime (5);"
      "trace (\"Calling play\");"
      "ns.play (\"video.flv\");"
      "trace (\"done calling play\");"
      "trace ('width = ' + video.width);"
      "trace ('height = ' + video.height);"
      "");
  SWFMovie_add (movie, (SWFBlock) action);
  SWFMovie_save (movie, "netstream-dimensions.swf");
  return 0;
}
