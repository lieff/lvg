/* gcc -Wall `pkg-config --libs --cflags libming glib-2.0` crash-0.5.4-goto-in-constructor.c -o crash-0.5.4-goto-in-constructor && ./crash-0.5.4-goto-in-constructor
 */

#include <ming.h>
#include <glib.h>
#include <stdio.h>

static SWFMovieClip
create_clip (void)
{
  SWFMovieClip clip, child;
  SWFDisplayItem item;

  child = newSWFMovieClip ();
  SWFMovieClip_nextFrame (child);

  clip = newSWFMovieClip ();
  item = SWFMovieClip_add (clip, (SWFBlock) child);
  SWFDisplayItem_setName (item, "one");
  SWFMovieClip_add (clip, (SWFBlock) newSWFAction (
      "trace (\"frame one:\");"
      "trace (one);"
      ));
  SWFMovieClip_nextFrame (clip);

  SWFMovieClip_remove (clip, item);
  SWFMovieClip_nextFrame (clip);

  item = SWFMovieClip_add (clip, (SWFBlock) child);
  SWFDisplayItem_setName (item, "two");
  SWFMovieClip_add (clip, (SWFBlock) newSWFAction (
      "trace (\"frame two:\");"
      "trace (two);"
      ));
  SWFMovieClip_nextFrame (clip);

  return clip;
}

static void
do_movie (int version)
{
  SWFMovieClip clip;
  SWFMovie movie;
  char *s;

  movie = newSWFMovieWithVersion (version);
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);

  clip = create_clip ();
  SWFMovie_addExport (movie, (SWFBlock) clip, "export");
  SWFMovie_writeExports (movie);
  SWFMovie_add (movie, (SWFBlock) newSWFAction (
      "function Test () {"
      "  trace (\"runnng constructor...\");"
      "	 trace (this.one);"
      "	 trace (this.two);"
      "  trace (\"...doing goto...\");"
      "  this.gotoAndPlay (2);"
      "	 trace (this.one);"
      "	 trace (this.two);"
      "	 trace (\"...done\");"
      "};"
      "asm {"
      "  push \"Test\""
      "  getvariable"
      "  push \"MovieClip\""
      "  getvariable"
      "  extends"
      "};"
      "Object.registerClass (\"export\", Test);"
      "attachMovie (\"export\", \"foo\", 0);"
      "trace (\"after attachMovie:\");"
      "trace (foo);"
      "trace (foo.one);"
      "trace (foo.two);"
      "loadMovie (\"fscommand:quit\", \"\");"
      ));
  SWFMovie_nextFrame (movie);

  s = g_strdup_printf ("crash-0.5.4-goto-in-constructor-%d.swf", version);
  SWFMovie_save (movie, s);
  g_free (s);
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
