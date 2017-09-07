/* gcc `pkg-config --libs --cflags libming` crash-0.5.3-text-field-root-variable.c -o crash-0.5.3-text-field-root-variable && ./crash-0.5.3-text-field-root-variable
 */

#include <ming.h>

static void
do_movie (int version)
{
  char name[100];
  SWFMovie movie;
  SWFTextField text;
  SWFDisplayItem display;

  movie = newSWFMovieWithVersion (version);
  movie = newSWFMovie();
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);

  text = newSWFTextField ();
  SWFTextField_setVariableName (text, "/:foo");
  display = SWFMovie_add (movie, (SWFBlock) text);
  SWFMovie_nextFrame (movie);

  SWFMovie_add (movie, (SWFBlock) newSWFAction (
	"loadMovie (\"FSCommand:quit\", \"\");"
	));
  SWFMovie_nextFrame (movie);

  sprintf (name, "crash-0.5.3-text-field-root-variable-%d.swf", version);
  SWFMovie_save (movie, name);
}

int
main (int argc, char **argv)
{
  int i;

  if (Ming_init ())
    return 1;

  for (i = 8; i >= 5; i--) {
    do_movie (i);
  }

  return 0;
}

