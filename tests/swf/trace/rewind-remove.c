/* gcc `pkg-config --libs --cflags libming` rewind-remove.c -o rewind-remove && ./rewind-remove
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

  SWFMovie_add (movie, (SWFBlock) newSWFAction (""
	"if (was_here == undefined) {"
	"  was_here = true;"
	"  trace (\"Make sure only the right movies get deleted\");"
	"  createEmptyMovieClip (\"foo0\", 1000000);"
	"  createEmptyMovieClip (\"foo1\", 0);"
	"  createEmptyMovieClip (\"foo2\", -1);"
	"  createEmptyMovieClip (\"foo3\", -16384);"
	"  createEmptyMovieClip (\"foo4\", -16385);"
	"  createEmptyMovieClip (\"foo5\", -1000000);"
	"} else {"
	"  trace (foo0);"
	"  trace (foo1);"
	"  trace (foo2);"
	"  trace (foo3);"
	"  trace (foo4);"
	"  trace (foo5);"
	"  loadMovie (\"FSCommand:quit\", \"\");"
	"}"
	""));
  SWFMovie_nextFrame (movie);
  SWFMovie_nextFrame (movie);

  sprintf (name, "rewind-remove-%d.swf", version);
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
