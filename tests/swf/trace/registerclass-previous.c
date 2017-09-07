/* gcc `pkg-config --libs --cflags libming` registerclass-previous.c -o registerclass-previous && ./registerclass-previous
 */

#include <ming.h>

int
main (int argc, char **argv)
{
  SWFMovie movie;
  SWFMovieClip clip;
  SWFDisplayItem item;

  if (Ming_init ())
    return 1;
  Ming_useSWFVersion (7);

  movie = newSWFMovie();
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);

  clip = newSWFMovieClip ();
  SWFMovie_addExport (movie, (SWFBlock) clip, "export");
  item = SWFMovie_add (movie, (SWFBlock) clip);
  SWFDisplayItem_setName (item, "m");
  SWFMovie_add (movie, (SWFBlock) compileSWFActionCode (""
      "trace (\"Check that registerClass does not influence previously created instances\");"
      "fun = function () {"
      "  trace (this + \": Constructor\");"
      "};"
      "fun.prototype = new Object ();"
      "fun.prototype.foo = function () {"
      "  trace (\"foo called on \" + this);"
      "};"
      "Object.registerClass (\"export\", fun);"
      "m.foo ();"
      ""));
  SWFMovie_nextFrame (movie);

  item = SWFMovie_add (movie, (SWFBlock) clip);
  SWFDisplayItem_setName (item, "m2");
  SWFMovie_add (movie, (SWFBlock) compileSWFActionCode (""
      "m.foo ();"
      "m2.foo ();"
      "stop ();"
      ""));

  SWFMovie_save (movie, "registerclass-previous.swf");
  return 0;
}
