fmt = new TextFormat ();
fmt.font = "_sans";
fmt.size = 12;

extent = fmt.getTextExtent ("");

trace ("Check if getTextExtent on an empty string returns 0 for width, height");
trace (extent.height);
trace (extent.width);

loadMovie ("FSCommand:quit", "");
