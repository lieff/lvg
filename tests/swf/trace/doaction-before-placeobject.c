/* gcc `pkg-config --libs --cflags libming` doaction-before-placeobject.c -o doaction-before-placeobject && ./doaction-before-placeobject
 */

#include <ming.h>

/* The file doaction-after-placeobject.swf was created from 
 * doaction-before-placeobject.swf by manually swapping the DoAction and
 * PlaceObject2 tags in the file with a hex editor
 */
int
main (int argc, char **argv)
{
  SWFMovie movie;
  SWFMovieClip clip;
  SWFDisplayItem item;

  if (Ming_init ())
    return 1;
  Ming_useSWFVersion (7);

  movie = newSWFMovie();
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);

  clip = newSWFMovieClip ();
  item = SWFMovie_add (movie, (SWFBlock) clip);
  SWFDisplayItem_setName (item, "m");
  SWFDisplayItem_addAction (item, compileSWFActionCode (""
      "  trace (\"clipEvent (load)\");"
      "  trace (this);"
      ""), SWFACTION_ONLOAD);
  SWFMovie_add (movie, (SWFBlock) compileSWFActionCode (""
      "trace (\"Frame 0\");"
      ""));

  SWFMovie_save (movie, "doaction-before-placeobject.swf");
  return 0;
}
