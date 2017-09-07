// makeswf -v 7 -r 1 -o test-7.swf test.as

var x = new XML ("<a>  <![CDATA[bl</a>#¤!  a &lt;ah  ]]>  </a>");
trace (x);
trace (x.status);
x.ignoreWhite = true;

x.parseXML ("<a><![CDATA[bl</a>#¤!  a &lt;ah  ]]>  </a>");
trace (x);
trace (x.status);

x.parseXML ("<a><![CDATA[bl</a>#¤!  a &lt;ah]></a>");
trace (x);
trace (x.status);

x.parseXML ("<a><![CDATA[bl</a>#¤!  ]]]>a &lt;ah]]></a>");
trace (x);
trace (x.status);

x.parseXML ("<a><![CDATA[bl</a>#¤!  a &lt;ah] ]></a>");
trace (x);
trace (x.status);

x.parseXML ("<a><![CDATA[bl</a>#¤!  a &lt;ah]] ></a>");
trace (x);
trace (x.status);

loadMovie ("FSCommand:quit", "");
