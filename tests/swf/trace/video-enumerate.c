/* gcc `pkg-config --libs --cflags libming` video-enumerate.c -o video-enumerate && ./video-enumerate
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
  Ming_useSWFVersion (8);

  movie = newSWFMovie();
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);
  video = newSWFVideoStream ();
  SWFVideoStream_setDimension (video, 200, 150);
  item = SWFMovie_add (movie, (SWFBlock) video);
  SWFDisplayItem_setName (item, "video");
  action = newSWFAction (""
      "trace (\"Test if the special properties of Video object show up when enumerating\");"
      "video.x = 'hello';"
      "props = [];"
      "for (var prop in video) {"
      "  props.push(prop);"
      "}"
      "props.sort ();"
      "trace (props);"
      "getURL ('fscommand:quit', '');"
      "");
  SWFMovie_add (movie, (SWFBlock) action);
  SWFMovie_save (movie, "video-enumerate.swf");
  return 0;
}
