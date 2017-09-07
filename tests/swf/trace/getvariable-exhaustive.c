/* gcc -Wall `pkg-config --libs --cflags libming glib-2.0` getvariable-exhaustive.c -o getvariable-exhaustive && ./getvariable-exhaustive
 */

#include <glib.h>
#include <ming.h>
#include <string.h>

static void
apply_string (GHashTable *table, const char *start, guint left)
{
  const char *names[] = { "", "..", "a" };
  const char *delims[] = { "", "/", ".", ":", "//", "/.", "/:", "./", "..", ".:", ":/", ":.", "::" };
  guint i, j;

  if (left == 0)
    return;

  left--;
  for (i = 0; i < G_N_ELEMENTS(names); i++) {
    for (j = 0; j < G_N_ELEMENTS(delims); j++) {
      char *tmp = g_strconcat (start, names[i], delims[j], NULL);
      apply_string (table, tmp, left);
      g_hash_table_insert (table, tmp, NULL);
    }
  }
}

static void
collect_strings (gpointer key, gpointer value, gpointer listp)
{
  GList **list = listp;

  *list = g_list_prepend (*list, key);
}

static char *
create_script (void)
{
  GHashTable *table = g_hash_table_new (g_str_hash, g_str_equal);
  GList *walk, *list = NULL;
  GString *str;

  g_hash_table_insert (table, g_strdup (""), NULL);
  apply_string (table, "", 3);
  g_hash_table_foreach (table, collect_strings, &list);
  list = g_list_sort (list, (GCompareFunc) strcmp);
  g_hash_table_destroy (table);

  str = g_string_new ("trace (\"Check all GetVariable strings\");");
  for (walk = list; walk; walk = walk->next) {
    char *s = walk->data;
    g_string_append_printf (str,
	"asm {"
	"  push \"%s => \", \"%s\""
	"  getvariable"
	"  concat"
	"  trace"
	"};", s, s);
  }
  g_string_append (str, "loadMovie (\"FSCommand:quit\", \"\");");
  g_list_foreach (list, (GFunc) g_free, NULL);
  g_list_free (list);

  return g_string_free (str, FALSE);
}

static SWFBlock
add_clip (guint depth, const char *script)
{
  SWFMovieClip clip;
  
  clip = newSWFMovieClip ();
  if (depth > 0) {
    SWFBlock block = add_clip (depth - 1, script);
    SWFDisplayItem item = SWFMovieClip_add (clip, block);
    SWFDisplayItem_setName (item, "a");
  }
  if (depth == 3) {
    SWFMovieClip_add (clip, (SWFBlock) newSWFAction (script));
  }
  SWFMovieClip_nextFrame(clip);
  return (SWFBlock) clip;
}

static void
do_movie (int version, const char *script)
{
  SWFMovie movie;
  SWFBlock block;
  char *s;

  movie = newSWFMovieWithVersion (version);
  SWFMovie_setRate (movie, 1);
  SWFMovie_setDimension (movie, 200, 150);

  block = add_clip (5, script);
  SWFDisplayItem item = SWFMovie_add (movie, block);
  SWFDisplayItem_setName (item, "a");
  SWFMovie_nextFrame (movie);

  s = g_strdup_printf ("getvariable-exhaustive-%d.swf", version);
  SWFMovie_save (movie, s);
  g_free (s);
}

int
main (int argc, char **argv)
{
  int i;
  char *script;

  if (Ming_init ())
    return 1;

  script = create_script ();
  for (i = 5; i < 9; i++)
    do_movie (i, script);
  g_free (script);

  return 0;
}
