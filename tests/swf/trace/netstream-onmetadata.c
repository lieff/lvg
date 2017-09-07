/* gcc `pkg-config --libs --cflags libming` netstream-onmetadata.c -o netstream-onmetadata && ./netstream-onmetadata
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
      "trace (\"Test parsing of onMetaData\");"
      "nc = new NetConnection ();"
      "nc.connect (null);"
      "ns = new NetStream (nc);"
      "ns.onMetaData = function (info)"
      "{"
      "  trace ('onMetaData');"
      "  var props = [];"
      "  for (var prop in info) {"
      "    props.push (prop);"
      "  }"
      "  props.sort ();"
      "  for (var i = 0; i < props.length; i++) {"
      "    if (typeof (info[props[i]]) == 'object') {"
      "      var keys = [];"
      "      for (var key in info[props[i]]) {"
      "        keys.push (key);"
      "      }"
      "      keys.sort ();"
      "      trace (props[i] + ' = ');"
      "      for (var j = 0; j < keys.length; j++) {"
      "        trace ('  ' + keys[j] + ' = ' + typeof (info[props[i]][keys[j]]) + ': ' + info[props[i]][keys[j]]);"
      "      }"
      "    } else {"
      "      trace (props[i] + ' = ' + typeof (info[props[i]]) + ': ' + info[props[i]]);"
      "    }"
      "  }"
      "  loadMovie ('FSCommand:quit', '');"
      "};"
      "video.attachVideo (ns);"
      "ns.setBufferTime (5);"
      "trace (\"Calling play\");"
      "ns.play (\"video.flv\");"
      "trace (\"done calling play\");"
      "");
  SWFMovie_add (movie, (SWFBlock) action);
  SWFMovie_save (movie, "netstream-onmetadata.swf");
  return 0;
}
