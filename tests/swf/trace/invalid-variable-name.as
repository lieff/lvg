// makeswf -v 7 -s 200x150 -r 1 -o invalid-variable-name.swf invalid-variable-name.as

trace ("Check for invalid variable names");

for (i = 0; i < 300; i++) {
  name = String.fromCharCode (i);
  trace (">>> " + name + " (" + i + ")");
  if (name != "i" && name != "I") {
    this[name] = 42;
    trace (this[name]);
    asm {
      push "name"
      getvariable
      getvariable
      trace
    };
    trace (this.hasOwnProperty (name));
    name = "a" + name;
    this[name] = 42;
    trace (this[name]);
    asm {
      push "name"
      getvariable
      getvariable
      trace
    };
    trace (this.hasOwnProperty (name));
    name = name + "b";
    this[name] = 42;
    trace (this[name]);
    asm {
      push "name"
      getvariable
      getvariable
      trace
    };
    trace (this.hasOwnProperty (name));
  }
}

loadMovie ("FSCommand:quit", "");
