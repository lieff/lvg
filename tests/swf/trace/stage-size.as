// makeswf -v 7 -s 200x150 -r 1 -o stage-size.swf stage-size.as

trace ("Simple test for Stage size");

trace (Stage.width);
Stage.width = Stage.width / 2;
trace (Stage.width);

trace (Stage.height);
Stage.height = Stage.height / 2;
trace (Stage.height);

loadMovie ("FSCommand:quit", "");
