// makeswf -v 7 -s 200x150 -r 1 -o relational.swf relational.as

trace ("check relational operators Less, StringLess, Less2 and Greater");

#include "values.as"

for (i = 0; i < values.length; i++) {
  for (j = 0; j < values.length; j++) {
    trace ("---");
    x = values[i];
    y = values[j];
    asm {
      push "z", "x"
      getvariable
      push "y"
      getvariable
      oldlessthan
      setvariable
    };
    trace (names[i] + " lt " + names[j] + " = " + z);
    asm {
      push "z", "x"
      getvariable
      push "y"
      getvariable
      stringlessthan
      setvariable
    };
    trace (names[i] + " strlt " + names[j] + " = " + z);
    trace (names[i] + " < " + names[j] + " = " + (values[i] < values[j]));
#if __SWF_VERSION__ > 5
    trace (names[i] + " > " + names[j] + " = " + (values[i] > values[j]));
#endif
  };
};

loadMovie ("FSCommand:quit", "");
