// makeswf -v 7 -s 200x150 -r 1 -o loadvars-decode.swf loadvars-decode.as

test = function (encoded) {
  // print something about the test we're about to run
  trace (">>> " + encoded);

  // create a new Object.
  o = {};

  // get a special function that we know is used to decode urlencoded data.
  // This is actually the function used as LoadVars.prototype.decode, but it
  // works on any type of object.
  o.decode = ASnative (301, 3);

  // decode the string we got
  o.decode (encoded);

  // now print the properties that got set. Note that we can't just use a for-in
  // loop, because they have an undefined order. Instead we collect all 
  // variables that were set into an array, sort that array, and then print the
  // data sorted
  var array = [];
  for (var i in o) {
    array.push (i);
  };
  array.sort ();
  for (var i = 0; i < array.length; i++) {
    trace (array[i] + " = " + o[array[i]]);
  };
};

// Let's create an array of all the strings we want to test
tests = [
  "a=b&c=d",
  "%26=%3d&%3D=%26",
  "",
  "???=???",
  "a=",
  "=b",
  "a=&=b&c&d=e",
  // www.kanal5.se
  "id=mplayer&javascriptid=mplayer&frontcolor=0xFFFFFF&backcolor=0x000000&enablejs=true&largecontrols=false&file=http://kanal5.se/c/ext/k5video/playlist?selectType=playlist-name%26selectWord=Mazda%26selectType2=playlist-name%26selectWord2=Fraga Olle 1107&bufferlength=6&displayheight=168&height=168&width=225&autostart=false&repeat=list&shuffle=false&overstretch=fit&volume=80&rotatetime=5&showicons=true&bwfile=/html/themes/kanal5/images/100k.jpg&bwstreams=400_400,640_700&useaudio=false&usecaptions=false&usekeys=false&callback=http://kanal5.se/c/ext/k5video/counter&showvolume=true&usefullscreen=true&allowscriptaccess=always",
  "6049",
  "img_id=181477",
  // www.svenskfast.se
  "xmlUrl=http://www.svenskfast.se/Pages/ObjectPicturesXML.aspx?ObjectID=3ELJF2R975UU6KB8",
  // youtube
  "v=1",
  "hl=en&video_id=Ra9tHf5oTGE&l=174&t=OEgsToPDskIiI57FokAGWDU_cVWkk0YQ&sk=jx4HbVzSeNwxQnLPSJ-AeQC&plid=AAQ-wZXLj0LXb-xH&playnext=0",
  "ns=yt&plid=AAQ-wZXLj0LXb-xH&vid=jx4HbVzSeNwxQnLPSJ-AeQC&docid=Ra9tHf5oTGE&el=detailpage&et=0.04&fv=WIN%209%2C0%2C100%2C0&nbe=0&st=0.04&rt=3.7&len=174",
  // www.bredband.com
  "f=/wps/wcm/resources/file/eb2d284fab2722c/popcorn.swf&clickTag=javascript:try{Instadia_sendEvent('011kr', 'BANNER # B2_PRIVAT_START BANNER BREDBAND', '');}catch(e){;}finally{location.href='/bredband';}",
  "f=/portal/wcmobject/document/swf/fsecuretavling_puff.swf&clickTag=javascript:try{Instadia_sendEvent('011kr', 'BANNER # B2_PRIVAT_KAMPANJ FSECURE TAVLING', '');}catch(e){;}finally{location.href='/fsecuretavling';}",
  "f=/wps/wcm/resources/file/ebc66e4b245ddf2/telefoni-puff_new.swf&clickTag=javascript:try{Instadia_sendEvent('011kr', 'BANNER # B2_PRIVAT_START PUFF TELEFONI', '');}catch(e){;}finally{location.href='/telefoni';}",
  "f=/wps/wcm/resources/file/ebc66c4b2452908/film.swf&clickTag=javascript:try{Instadia_sendEvent('011kr', 'BANNER # B2_PRIVAT_START PUFF VOD', '');}catch(e){;}finally{location.href='/portal/redirect/privat/digitaltv?page=new%26WCM_GLOBAL_CONTEXT=/wps/wcm/connect/b2/privat/digitaltv/filmnarduvill';}&rootdir=",
  "f=/wps/wcm/resources/file/ebc66d4b24596b5/sakerhet_puff.swf&clickTag=javascript:try{Instadia_sendEvent('011kr', 'BANNER # B2_PRIVAT_START PUFF SAKERHET', '');}catch(e){;}finally{location.href='/sakerhet';}",
  // www.2advanced.com 
  "lang=en&enforce=0",
  // www.smhi.se
  "id=22&initView=EU,SE&lang=SV&home=EU,SE&skin=Default",
  "linkUrl=http://www.smhi.se/cmp/jsp/polopoly.jsp?d=5236&l=sv&linkTarget=_top",
  // www.flash.com
  "txt=<a href=%22asfunction:_root.launchURL,0%22>ADOBE FLASH CS3 PROFESSIONAL</a>&textcolor=#ffffff&hovercolor=#c00c00&linkcolor=#ffffff&w=640&h=28&sifr_url_0=/products/flash/",
  // www.sf.se
  "simpleDrapeSwf=http://www.sf.se/sf/component/startplayer/drapeSimple.swf&animDrapeSwf=http://www.sf.se/sf/component/startplayer/drapeAnimation.swf&curl=cycube://internal/document/106364&id=101546&xmlPath=http://www.sf.se/DataProvider",
  "txt=ÂKOMMANDEÂ FILMER&textcolor=%23ffffff&emcolor=%23fed63b&w=265&h=19",
  "txt=ÂKONCEPTÂBIO&textcolor=%23ffffff&emcolor=%23fed63b&w=265&h=19",
  "txt=ÂNYÂHETER&textcolor=%23ffffff&emcolor=%23fed63b&w=265&h=19",
  "txt=ÂTOPPÂLISTAN&textcolor=%23ffffff&emcolor=%23fed63b&w=265&h=19",
  "txt=ÂFRÅGA ÂRONNY&textcolor=%23ffffff&emcolor=%23fed63b&w=265&h=19",
  "txt=ÂMOBILÂGODIS&textcolor=%23ffffff&emcolor=%23fed63b&w=265&h=19",
  "txt=ÂPRESENT-Â OCH RABATTKORT&textcolor=%23ffffff&emcolor=%23fed63b&w=265&h=19",
  "pHost=www.sf.se&pContextPath=/bio&pServer=www.sf.se&pPort=4001&pNumSeats=2&pSeats=&pSplitSeats=false&pAuditorium=2008_53&pVip=false&pListen=2008%2353%23200711131830&pNextUrl=/bio/Booking?cmd=showPaymentTypePage&pAbortUrl=/bio/Booking?cmd=listFilms",
  // www.aftonbladet.se
  "&dynimg=http://img.aftonbladet.se/special/webbtv/bilder2/Noje/0711/p1101779.jpg&dynurl=http://www.aftonbladet.se/atv2/init.html?id=categories/Noje/0711/8220&category=noje&commercial=yes",

  // add more here.


  // done.
  "foo=bar"

];

for (i = 0; i < tests.length; i++) {
  test (tests[i]);
};


loadMovie ("fscommand:quit", "");
