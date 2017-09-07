/* gcc `pkg-config --libs --cflags libming` attachmovie-object.c -o attachmovie-object && ./attachmovie-object
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
  SWFMovie_addExport (movie, (SWFBlock) sprite, "export");
  SWFMovie_writeExports (movie);

  SWFMovie_add (movie, (SWFBlock) newSWFAction (""
	"var o = {};"
	"o[0] = 0;"
	"for (var i = 1; i <= 12; i++) {"
	"  o[i] = Math.pow (2, i);"
	"  ASSetPropFlags (o, i, Math.pow (2, i));"
	"}"
	"attachMovie (\"export\", \"foo\", 0, o);"
	"for (var i = 1; i <= 12; i++) {"
	"  trace (i + \" = \" + foo[i]);"
	"}"
	"ASSetPropFlags (o, null, 0, 8191);"
	"for (var i = 1; i <= 12; i++) {"
	"  trace (i + \" = \" + foo[i]);"
	"}"
	"loadMovie (\"FSCommand:quit\", \"\");"
	""));
  SWFMovie_nextFrame (movie);


  sprintf (name, "attachmovie-object-%d.swf", version);
  SWFMovie_save (movie, name);
}

int
main (int argc, char **argv)
{
  int i;

  if (Ming_init ())
    return 1;

  for (i = 5; i <= 7; i++)
    do_movie (i);

  return 0;
}
