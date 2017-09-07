/* gcc `pkg-config --libs --cflags libming` duplicateMovieClip-events.c -o duplicateMovieClip-events && ./duplicateMovieClip-events
 */

#include <ming.h>

static void
do_movie (int version)
{
  SWFMovie movie;
  SWFMovieClip clip;
  SWFDisplayItem item;
  char name[100];

  movie = newSWFMovieWithVersion (version);
  movie = newSWFMovie();
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);

  clip = newSWFMovieClip ();
  item = SWFMovie_add (movie, (SWFBlock) clip);
  SWFDisplayItem_setName (item, "movie");
  SWFDisplayItem_addAction (item, newSWFAction ("trace (this + \": init\");"), SWFACTION_INIT);
  SWFDisplayItem_addAction (item, newSWFAction ("trace (this + \": construct\");"), SWFACTION_CONSTRUCT);
  SWFDisplayItem_addAction (item, newSWFAction ("trace (this + \": load\");"), SWFACTION_ONLOAD);
  SWFDisplayItem_addAction (item, newSWFAction ("trace (this + \": unload\");"), SWFACTION_UNLOAD);
  SWFMovie_add (movie, (SWFBlock) newSWFAction (""
	"trace (\"duplicating...\");"
	"movie.duplicateMovieClip (\"foo\", 0);"
	"trace (\"...done\");"
	""));
  SWFMovie_nextFrame (movie);
  SWFMovie_add (movie, (SWFBlock) newSWFAction (""
	"loadMovie (\"FSCommand:quit\", \"\");"
	""));
  SWFMovie_nextFrame (movie);

  sprintf (name, "duplicateMovieClip-events-%d.swf", version);
  SWFMovie_save (movie, name);
}

int
main (int argc, char **argv)
{
  int i;

  if (Ming_init ())
    return 1;

  for (i = 5; i < 9; i++)
    do_movie (i);

  return 0;
}
