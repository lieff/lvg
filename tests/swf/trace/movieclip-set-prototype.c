/* gcc `pkg-config --libs --cflags libming glib-2.0` movieclip-set-prototype.c -o movieclip-set-prototype && ./movieclip-set-prototype
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
  SWFMovie_addExport (movie, (SWFBlock) clip, "export");
  SWFMovie_writeExports (movie);
  SWFMovieClip_addInitAction (clip, newSWFAction (
	"_root.fun = function () { trace (\"hi\"); };"
	"Object.registerClass (\"export\", fun);"
	));
  item = SWFMovie_add (movie, (SWFBlock) clip);
  SWFDisplayItem_addAction (item, newSWFAction (
	/* work around a bug in Flash and another one in Ming */
	"asm { push \"__proto__\" getvariable pop };"
	"trace (\"running init\");"
	"trace (this.__proto__ == MovieClip.prototype);"
	), SWFACTION_INIT);
  SWFDisplayItem_addAction (item, newSWFAction ("trace (this.__proto__ == _root.fun.prototype);"), SWFACTION_CONSTRUCT);
  SWFMovie_nextFrame (movie);
  
  SWFMovie_add (movie, (SWFBlock) newSWFAction (""
	"loadMovie (\"fscommand:quit\", \"\");"
	));
  SWFMovie_nextFrame (movie);

  real_name = g_strdup_printf ("movieclip-set-prototype-%d.swf", version);
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
