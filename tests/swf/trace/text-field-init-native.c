/* gcc `pkg-config --libs --cflags libming` text-field-init-native.c -o text-field-init-native && ./text-field-init-native
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
  display = SWFMovie_add (movie, (SWFBlock) text);

  SWFMovie_add (movie, (SWFBlock) newSWFAction (""
	"trace ('Test whether native properties of TextField are initialized if the movie contains an EditText tag');"
	"_global.TextField.prototype.hasOwnProperty = ASnative (101, 5);"
	"trace (_global.TextField.prototype.hasOwnProperty ('text'));"
	"_global.TextField.prototype = new Object ();"
	"loadMovie (\"FSCommand:quit\", \"\");"
	""));
  SWFMovie_nextFrame (movie);

  sprintf (name, "text-field-init-native-%d.swf", version);
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

