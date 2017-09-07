/* gcc `pkg-config --libs --cflags libming` export-case.c -o export-case && ./export-case
 */

#include <ming.h>

static void
do_movie (int version)
{
  char name[100];
  SWFMovie movie;
  SWFSprite sprite;

  movie = newSWFMovieWithVersion (version);
  movie = newSWFMovie();
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);

  sprite = newSWFSprite ();
  SWFSprite_addBlock (sprite, (SWFBlock) newSWFAction ("trace (\"got the export\");"));
  SWFMovie_addExport (movie, (SWFBlock) sprite, "eXportÜ");
  SWFMovie_writeExports (movie);

  SWFMovie_add (movie, (SWFBlock) newSWFAction (""
	"attachMovie (\"exportü\", \"foo\", 0);"
	"trace (foo);"
	"attachMovie (\"eXportü\", \"foo2\", 1);"
	"trace (foo2);"
	"attachMovie (\"exportÜ\", \"foo3\", 2);"
	"trace (foo3);"
	"attachMovie (\"eXportÜ\", \"foo4\", 3);"
	"trace (foo4);"
	"loadMovie (\"FSCommand:quit\", \"\");"
	""));
  SWFMovie_nextFrame (movie);


  sprintf (name, "export-case-%d.swf", version);
  SWFMovie_save (movie, name);
}

int
main (int argc, char **argv)
{
  int i;

  if (Ming_init ())
    return 1;

  for (i = 5; i < 8; i++)
    do_movie (i);

  return 0;
}
