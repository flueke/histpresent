function SetCSSValue(where,what,to){
   var r='cssRules';
   if(document.all) r='rules';
   var i;
   for(i=0;i<document.styleSheets.length;++i) {
      var cssrules=document.styleSheets[i][r]
      for(j=0;j<cssrules.length;++j)
         if(cssrules[j].selectorText.toUpperCase()==where.toUpperCase()) {
            cssrules[j].style[what]=to;
            return;
         }
   }
}
var elements=new Array('dispoptCBInh.checked','dispoptCBPub.checked');
function SetValuesFromCookie() {
   var i;
   var arrcookie=document.cookie.split(";");
   for(i=0; i<arrcookie.length; ++i) {
      while(arrcookie[i].charAt(0)==' ') 
         arrcookie[i]=arrcookie[i].substring(1,arrcookie[i].length);
      if (arrcookie[i].indexOf("ROOT")==0) {
         var arrval=arrcookie[i].substring(5).split(':');
         for (i=0; i<arrval.length; ++i) {
            var posdelim=elements[i].indexOf(".");
            var what=elements[i].substring(0,posdelim);
            var mem =elements[i].substring(posdelim+1);
            var val=arrval[i];
            if (val=='false') val=false;
            else if (val=='true') val=true;
            var el=document.getElementById(what);
            if (!el) return;
            el[mem]=val;
            CBChanged(el);
         }
         return;
      }
   }
}
function UpdateCookie() {
   var cookietxt="ROOT=";
   var i;
   for (i=0; i<elements.length; ++i) {
      var posdelim=elements[i].indexOf(".");
      var what=elements[i].substring(0,posdelim);
      var mem =elements[i].substring(posdelim+1);
      var val=document.getElementById(what)[mem];
      if (i>0) cookietxt+=':';
      cookietxt+=val;
   }
   var ayear=new Date();
   ayear.setTime(ayear.getTime()+31536000000);
   cookietxt+=";path=/;expires="+ayear.toUTCString();
   document.cookie=cookietxt;
}
function CBChanged(cb){
   if(cb.id=='dispoptCBInh') {
      SetCSSValue('tr.funcinh','display',cb.checked?'':'none');
      SetCSSValue('tr.datainh','display',cb.checked?'':'none');
   } else if(cb.id=='dispoptCBPub') {
      SetCSSValue('#funcprot','display',cb.checked?'':'none');
      SetCSSValue('#funcpriv','display',cb.checked?'':'none');
      SetCSSValue('#dataprot','display',cb.checked?'':'none');
      SetCSSValue('#datapriv','display',cb.checked?'':'none');
      SetCSSValue('#enumprot','display',cb.checked?'':'none');
      SetCSSValue('#enumpriv','display',cb.checked?'':'none');
   }
   UpdateCookie();
}

