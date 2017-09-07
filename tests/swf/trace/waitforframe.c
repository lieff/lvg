/* gcc -Wall `pkg-config --libs --cflags libming glib-2.0` waitforframe.c -o waitforframe && ./waitforframe
 */

#include <ming.h>
#include <glib.h>
#include <stdio.h>

#define BYTES_PER_FRAME 10000
#define N_FRAMES 10

static void
add_n_bytes_frame (SWFMovie movie, guint n_bytes)
{
  char *action;
  
  g_return_if_fail (n_bytes >= 34);

  action = g_strdup_printf ("asm { push \"%*s\" pop };", n_bytes - 33, "x");
  SWFMovie_add (movie, (SWFBlock) newSWFAction (action));
  SWFMovie_nextFrame (movie);
  g_free (action);
}

static void
do_movie (int version)
{
  SWFMovie movie;
  guint i;
  char *s;
  GString *str;

  movie = newSWFMovieWithVersion (4);
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);

  str = g_string_new ("");
  for (i = 0; i < 10; i++) {
    if (version == 1) {
      g_string_append_printf (str,
	  "if (! frameLoaded (%u)) {"
	  "  trace (\"%u\");"
	  "}", i, i);
    } else {
      g_string_append_printf (str,
	  "if (! frameLoaded (%u * 1)) {"
	  "  trace (\"%u\");"
	  "}", i, i);
    }
  }
  g_string_append (str, "getURL (\"fscommand:quit\", \"\");");
  SWFMovie_add (movie, (SWFBlock) newSWFAction (str->str));
  g_string_free (str, TRUE);
  add_n_bytes_frame (movie, BYTES_PER_FRAME - 90);
  for (i = 1; i < N_FRAMES - 1; i++) {
    add_n_bytes_frame (movie, BYTES_PER_FRAME);
  }
  add_n_bytes_frame (movie, BYTES_PER_FRAME - 3);

  s = g_strdup_printf ("waitforframe%d-4.swf", version);
  SWFMovie_save (movie, s);
  g_free (s);
}

int
main (int argc, char **argv)
{
  if (Ming_init ())
    return 1;
  Ming_setSWFCompression (-1);

  do_movie (1);
  do_movie (2);

  return 0;
}
