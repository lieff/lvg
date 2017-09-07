// makeswf -v 7 -s 200x150 -r 1 -o clonesprite-depths.swf clonesprite-depths.as

trace ("Check depths of movies created via duplicateMovieClip and CloneSprite");

createEmptyMovieClip ("foo", 10);
trace(foo.getDepth());
duplicateMovieClip ("foo", "foo2", 40);
trace(foo2.getDepth());
foo.duplicateMovieClip ("foo3", 60);
trace(foo3.getDepth());

loadMovie ("FSCommand:quit", "");
