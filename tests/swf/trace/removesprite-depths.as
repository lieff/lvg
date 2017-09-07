// makeswf -v 7 -s 200x150 -r 1 -o removesprite-depths.swf removesprite-depths.as

trace ("Check RemoveSprite action with various depths");

depths = [-2000000, -20000, -10000, -1, 0, 1000000, 2000000 ];
for (i = 0; i < depths.length; i++) {
  x = createEmptyMovieClip ("movie" + depths[i], depths[i]);
  trace (this["movie" + depths[i]] + " @ " + x.getDepth ());
  asm {
    push "x"
    getvariable
    removemovieclip
  };
  trace (this["movie" + depths[i]] + " @ " + x.getDepth ());
}

loadMovie ("FSCommand:quit", "");
