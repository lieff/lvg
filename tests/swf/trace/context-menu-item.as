// makeswf -v 7 -r 1 -o context-menu-item-7.swf context-menu-item.as

function trace_cmi (cmi) {
  trace ("caption: " + cmi.caption);
  trace ("onSelect: " + cmi.onSelect);
  trace ("separatorBefore: " + cmi.separatorBefore);
  trace ("enabled: " + cmi.enabled);
  trace ("visible: " + cmi.visible);
}

trace ("Normal");
var cmi = new ContextMenuItem ("test", Math.pow, true, false, false);
trace_cmi (cmi);

trace ("No parameters");
cmi = new ContextMenuItem ();
trace_cmi (cmi);

trace ("Weird parameter");
cmi = new ContextMenuItem (123, 123, 123, 123, 123);
trace_cmi (cmi);

trace ("Copying");
cmi = new ContextMenuItem (123, 123, 123, 123, 123);
var cmi_copy = cmi.copy ();
trace ("Original:");
trace_cmi (cmi);
trace ("Copy:");
trace_cmi (cmi_copy);

loadMovie ("FSCommand:quit", "");
