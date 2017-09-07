/* gcc `pkg-config --libs --cflags libming` onload-childparent.c -o onload-childparent && ./onload-childparent
 */

#include <ming.h>

int
main (int argc, char **argv)
{
  SWFMovie movie;
  SWFMovieClip parent, child;
  SWFDisplayItem item;

  if (Ming_init ())
    return 1;
  Ming_useSWFVersion (7);

  movie = newSWFMovie();
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);
  SWFMovie_add (movie, (SWFBlock) compileSWFActionCode (""
      "  trace (\"Check that the parent onLoad event happens before the child's onLoad\");"
      ""));
  parent = newSWFMovieClip ();
  child = newSWFMovieClip ();
  item = SWFMovieClip_add (parent, (SWFBlock) child);
  SWFDisplayItem_addAction (item, compileSWFActionCode (""
      "  trace (\"child\");"
      ""), SWFACTION_ONLOAD);
  SWFMovieClip_nextFrame (parent);
  item = SWFMovie_add (movie, (SWFBlock) parent);
  SWFDisplayItem_addAction (item, compileSWFActionCode (""
      "  trace (\"parent\");"
      ""), SWFACTION_ONLOAD);
  SWFMovie_nextFrame (movie);

  SWFMovie_save (movie, "onload-childparent.swf");
  return 0;
}
