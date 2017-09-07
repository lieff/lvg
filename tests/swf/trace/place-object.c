/* gcc `pkg-config --libs --cflags libming glib-2.0` place-object.c -o place-object && ./place-object
 */

#include <ming.h>
#include <glib.h>

/* This is a rather complex test that outputs heaps of simple files.
 * What it does is create this movie:
 * Frame 1: PlaceObject
 * (Frame 2: RemoveObject)
 * Frame 2: PlaceObject
 * Frame 3: quit
 * The enum below changes properties of the second PlaceObject call.
 *
 * Note running this test requires a patched version of libmong 0.4, that adds
 * SWFDisplayItem_setMove() and allows calling SWFMovie_add() with a NULL 
 * character.
 */
enum {
  /* mark the PlaceObject as a move */
  MODIFY_MOVE =			(1 << 0),
  /* The next two control what character is set in the second PlaceObject:
   * HAS_CHAR & DIFFERENT_CHAR: a Button
   * DIFFERENT_CHAR: another MovieClip
   * HAS_CHAR: the same MovieClip as in the first PlaceObject
   * none: no character
   */
  MODIFY_HAS_CHAR =		(1 << 1),
  MODIFY_DIFFERENT_CHAR =	(1 << 2),
  /* the transform is different */
  MODIFY_TRANSFORM =	  	(1 << 3),
  /* the color transform is different */
  MODIFY_CTRANS =		(1 << 4),
  /* the ratio has changed */
  MODIFY_RATIO =		(1 << 5),
  /* the second PlaceObject has a name */
  MODIFY_NAME =			(1 << 6),
  /* the clip depth has changed */
  MODIFY_CLIP_DEPTH =		(1 << 7),
  /* a Remove tag is added directly before the second PlaceObject */
  MODIFY_REMOVE =		(1 << 8),
  /* add here */
  MODIFY_MAX =			(1 << 9)
};

static void
modify_item (SWFDisplayItem item, GString *name, guint mod)
{
  SWFDisplayItem_setDepth (item, 1);

  if (mod & MODIFY_MOVE) {
    if (name)
      g_string_append (name, "-move");
    else
      SWFDisplayItem_setMove (item);
  }
  if (mod & MODIFY_TRANSFORM) {
    if (name)
      g_string_append (name, "-transform");
    else
      SWFDisplayItem_moveTo (item, 10, 0);
  }
  if (mod & MODIFY_CTRANS) {
    if (name)
      g_string_append (name, "-ctrans");
    else
      SWFDisplayItem_setColorAdd (item, 10, 10, 10, 10);
  }
  if (mod & MODIFY_RATIO) {
    if (name)
      g_string_append (name, "-ratio");
    else
      SWFDisplayItem_setRatio (item, 0.5);
  }
  if (mod & MODIFY_NAME) {
    if (name)
      g_string_append (name, "-name");
    else
      SWFDisplayItem_setName (item, "instanceX");
  }
  if (mod & MODIFY_CLIP_DEPTH) {
    if (name)
      g_string_append (name, "-clip");
    else
      SWFDisplayItem_setMaskLevel (item, 5);
  }
  SWFDisplayItem_addAction (item, newSWFAction ("trace (_root._currentframe + \": init \" + this);"), SWFACTION_INIT);
  SWFDisplayItem_addAction (item, newSWFAction ("trace (_root._currentframe + \": load \" + this);"), SWFACTION_ONLOAD);
  SWFDisplayItem_addAction (item, newSWFAction ("trace (_root._currentframe + \": unload \" + this);"), SWFACTION_UNLOAD);
}

static char *
modify_placement (SWFMovie movie, guint mod)
{
  GString *name;
  SWFDisplayItem item;
  SWFBlock clip1, clip2;

  name = g_string_new ("place-object");
  if (mod & MODIFY_REMOVE)
    g_string_append (name, "-remove");
  clip1 = (SWFBlock) newSWFMovieClip ();
  if (mod & MODIFY_DIFFERENT_CHAR) {
    if (mod & MODIFY_HAS_CHAR) {
      clip2 = (SWFBlock) newSWFButton ();
      g_string_append (name, "-button");
    } else {
    clip2 = (SWFBlock) newSWFMovieClip ();
    g_string_append (name, "-movie");
    }
  } else if (mod & MODIFY_HAS_CHAR) {
    clip2 = NULL;
    g_string_append (name, "-char");
  } else {
    clip2 = (SWFBlock) clip1;
  }
  
  /* FIXME: I want the option to set a replace flag here */
  item = SWFMovie_add (movie, clip1);
  modify_item (item, name, mod);
  SWFMovie_nextFrame (movie);

  if (mod & MODIFY_REMOVE) {
    SWFDisplayItem_remove (item);
  }
  item = SWFMovie_add (movie, clip2);
  modify_item (item, NULL, mod);
  return g_string_free (name, FALSE);
}

static void
do_movie (int version)
{
  SWFMovie movie;
  char *name, *real_name;
  guint i;

  for (i = 0; i < MODIFY_MAX; i++) {
    movie = newSWFMovieWithVersion (version);
    movie = newSWFMovie();
    SWFMovie_setRate (movie, 256);
    SWFMovie_setDimension (movie, 200, 150);

    name = modify_placement (movie, i);
    SWFMovie_nextFrame (movie);
    
    SWFMovie_add (movie, (SWFBlock) newSWFAction (""
	  "loadMovie (\"FSCommand:quit\", \"\");"
	  ));
    SWFMovie_nextFrame (movie);

    real_name = g_strdup_printf ("%s-%d.swf", name, version);
    g_free (name);
    SWFMovie_save (movie, real_name);
    g_free (real_name);
  }
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
