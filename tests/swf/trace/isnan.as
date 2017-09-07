// makeswf -v 7 -s 200x150 -r 1 -o isnan.swf isnan.as

trace ("Check isNaN and isFinite");
numbers = [ Infinity, -Infinity, NaN, 0, this, true ];
for (i = 0; i < numbers.length; i++) {
  trace (numbers[i] + (isFinite (numbers[i]) ? ": finite" : ": inifinite") + (isNaN (numbers[i]) ? ", NaN" : ""));
};

loadMovie ("FSCommand:quit", "");
