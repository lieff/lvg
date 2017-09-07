// makeswf -v 7 -s 200x150 -r 1 -o stage-align.swf stage-align.as

trace ("Check Stage.align");

values = [
  "L",
  "R",
  "LR",
  "RL",
  "LRL",
  "RLR",
  "T",
  "B",
  "BT",
  "TB",
  "BTB",
  "TBT",
  "TRLB",
  "tlrb",
  "RmoviesdsdgaRsdgagaergBafsgafgaS"
];

trace (Stage.align);
for (i = 0; i < values.length; i++) {
  Stage.align = values[i];
  trace (values[i] + " ==> " + Stage.align);
};

loadMovie ("FSCommand:quit", "");

