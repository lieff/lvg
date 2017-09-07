/* gcc `pkg-config --libs --cflags libming` instance-name-loaded.c -o instance-name-loaded && ./instance-name-loaded
 */

#include <ming.h>

static void
do_movie (const char *name, int version)
{
  char s[1000];
  SWFMovie movie;
  SWFShape shape;
  SWFMovieClip clip;
  SWFDisplayItem item;

  movie = newSWFMovieWithVersion (version);
  movie = newSWFMovie();
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);

  sprintf (s, 
      "if (!_level1) {"
      "	 trace (\"Check how instances are auto-named when loaded from external files.\");"
      "  loadMovie (\"%s\", \"_level1\");"
      "} else {"
      "  loadMovie (\"FSCommand:quit\", \"\");"
      "};"
      "", name);
  SWFMovie_add (movie, (SWFBlock) newSWFAction (s));
  clip = newSWFMovieClip ();
  item = SWFMovie_add (movie, (SWFBlock) clip);
  SWFDisplayItem_setDepth (item, 1);
  SWFDisplayItem_addAction (item, newSWFAction ("trace (this + \".load\");"), SWFACTION_ONLOAD);
  SWFDisplayItem_addAction (item, newSWFAction ("trace (this + \".unload\");"), SWFACTION_UNLOAD);
  SWFMovie_nextFrame (movie);

  SWFMovie_save (movie, name);
}

int
main (int argc, char **argv)
{
  char name[100];
  int i;

  if (Ming_init ())
    return 1;

  for (i = 5; i < 8; i++) {
    sprintf (name, "instance-name-loaded-%d.swf", i);
    do_movie (name, i);
  }

  return 0;
}
