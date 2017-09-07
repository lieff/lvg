/* gcc `pkg-config --libs --cflags libming glib-2.0` initaction-queue.c -o initaction-queue && ./initaction-queue
 */

#include <ming.h>
#include <glib.h>

static void
do_movie (int version)
{
  SWFMovie movie;
  SWFMovieClip clip;
  SWFDisplayItem item;
  char *real_name;

  movie = newSWFMovieWithVersion (version);
  movie = newSWFMovie();
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);

  clip = newSWFMovieClip ();
  item = SWFMovie_add (movie, (SWFBlock) clip);
  SWFMovieClip_addInitAction (clip, newSWFAction ("_root.x = 42;"));
  SWFDisplayItem_addAction (item, newSWFAction ("trace (_root.x);"), SWFACTION_INIT);
  SWFDisplayItem_addAction (item, newSWFAction ("trace (_root.x);"), SWFACTION_CONSTRUCT);
  SWFDisplayItem_flush (item);
  SWFMovie_nextFrame (movie);
  
  SWFMovie_add (movie, (SWFBlock) newSWFAction (""
	"loadMovie (\"fscommand:quit\", \"\");"
	));
  SWFMovie_nextFrame (movie);

  real_name = g_strdup_printf ("initaction-queue-%d.swf", version);
  /* remove this after having understood that the generated code was modified to have
   * the DoInitAction tag placed _after_ the PlaceObject tag */
  g_assert (FALSE); 
  SWFMovie_save (movie, real_name);
  g_free (real_name);
}

int
main (int argc, char **argv)
{
  int i;

  if (Ming_init ())
    return 1;

  for (i = 8; i > 4; i--)
    do_movie (i);

  return 0;
}
