// makeswf -v 7 -s 200x150 -r 1 -o scalemode.swf scalemode.as

trace ("test Stage.scaleMode");

trace (Stage.scaleMode);
modes = [
  "noborder", "noBorder", "NOBORDER",
  "exactfit", "exactFit", "EXACTFIT",
  "showall", "showAll", "SHOWALL",
  "noscale", "noScale", "NOSCALE",
  "foo", "bar", null, undefined, "",
  0, 1, 2, 3, 4,
  "thishasnoborder"
];

for (i = 0; i < modes.length; i++) {
  Stage.scaleMode = modes[i];
  trace (modes[i] + " ==> " + Stage.scaleMode);
  Stage.scaleMode = "showAll";
};

Stage.scaleMode = "noBorder";
foo = ASnative (666, 2);
foo ();
trace (Stage.scaleMode);
foo ("exactfit");
trace (Stage.scaleMode);

loadMovie ("FSCommand:quit", "");
