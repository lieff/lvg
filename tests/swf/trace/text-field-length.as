// makeswf -v 7 -r 1 -o text-field-values-7.swf text-field-values.as

#include "values.as"

this.createTextField ("field", 1, 0, 0, 50, 50);

trace ("Testing length (default: " + field.length + ")");

values.push ("\r\n");
names.push ("(" + (values.length - 1) + ") \\r\\n (string)");

for (var j = 0; j < values.length; j++) {
  field.text = values[j];
  trace (names[j] + " = " + field.length);
}

loadMovie ("FSCommand:quit", "");
