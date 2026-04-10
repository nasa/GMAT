var currentArea;
var currentAreaType;
var currentTarget=1;
var submitted=false;
var requestMade=0;
var msqOnRequestMade=true;
var gCookieName;
var gCookieValueRet;

gCookieName='COOKIE_TEST';
gCookieValue='TESTING_COOKIE';
gCookieValueRet='test'

setCookie(gCookieName,gCookieValue,1);
gCookieValueRet=getCookie(gCookieName);

if (gCookieValueRet == null)
  alert('Your browser must accept cookies for this application to perform correctly. Please change your browser settting.');

function getCookie(name){
  var pos
  var token = name + '=';
  var tnlen = token.length;
  var cklen = document.cookie.length;
  var i = 0;
  var j;
  while (i < cklen)
  {
    j = i + tnlen;
    if (document.cookie.substring(i, j) == token)
    {
      pos = document.cookie.indexOf (';', j);
      if (pos == -1)
       pos = document.cookie.length;
       
      return unescape(document.cookie.substring(j, pos));
   }
   i = document.cookie.indexOf(' ', i) + 1;
   if (i == 0) break;
  } 
  return null;
}

function setCookie(name, value){
  document.cookie = name + '=' + escape(value)
}

function setNextPage(nextPage){
  document.forms[0].target = '' ;
  document.forms[0].NEXT_PAGE.value=nextPage;
  if (requestMade > 1) 
  { 
     if(msqOnRequestMade){alert("The form has been submitted, please wait until it has completed before pressing any more buttons.");}
     return false;
  }
  else 
  { 
     document.forms[0].submit();
  }
}

function setNextPageWithPaging(nextPage, currentPage){
  document.forms[0].target = '' ;
  document.forms[0].CURRENT_PAGE_NO.value = currentPage;
  setNextPage(nextPage);
}

function setData(fieldname, data){
  document.forms[0].target = '' ;
  document.forms[0][fieldname].value=data;
  document.forms[0].submit();
}

function setDataNoPost(fieldname, data){
  document.forms[0][fieldname].value=data;
}

function setDataAndNextPage(fieldname, data, nextPage){
  document.forms[0].target = '' ;
  document.forms[0][fieldname].value=data;
  setNextPage(nextPage);
}

function set2DataAndNextPage(fieldname1, data1, fieldname2, data2, nextPage){
  document.forms[0].target = '' ;
  document.forms[0][fieldname1].value=data1;
  document.forms[0][fieldname2].value=data2;
  setNextPage(nextPage);
}

function setFormAction(newActionName){
  document.forms[0].action=newActionName;
}

function setField(fieldname, data){
  document.forms[0].target = '' ;
  document.forms[0][fieldname].value=data;
}

function setDataAndNextPageWithPaging(fieldname, data, nextPage, currentPage){
  document.forms[0].target = '' ;
  document.forms[0][fieldname].value=data;
  document.forms[0].CURRENT_PAGE_NO.value = currentPage;
  setNextPage(nextPage);
}

function setDataAndNextPageNewWorkflow(fieldname, data, nextPage){
  if (fieldname == 'WORKFLOW_ID' && data == '1') document.forms[0].action = '/login' ;
  if (fieldname == 'WORKFLOW_ID' && data == '2') document.forms[0].action = '/abstract' ;
  if (fieldname == 'WORKFLOW_ID' && data == '3') document.forms[0].action = '/abstract' ;
  if (fieldname == 'WORKFLOW_ID' && data == '4') document.forms[0].action = '/abstract' ;
  if (fieldname == 'WORKFLOW_ID' && data == '5') document.forms[0].action = '/abstract' ;
  if (fieldname == 'WORKFLOW_ID' && data == '6') document.forms[0].action = '/session' ;
  if (fieldname == 'WORKFLOW_ID' && data == '7') document.forms[0].action = '/admin' ;
  if (fieldname == 'WORKFLOW_ID' && data == '8') document.forms[0].action = '/config' ;
  if (fieldname == 'WORKFLOW_ID' && data == '10') document.forms[0].action = '/login' ;
  if (fieldname == 'WORKFLOW_ID' && data == '11') document.forms[0].action = '/viewer' ;
  if (fieldname == 'WORKFLOW_ID' && data == '12') document.forms[0].action = '/planner' ;
  if (fieldname == 'WORKFLOW_ID' && data == '19') document.forms[0].action = '/submission' ;
  if (fieldname == 'WORKFLOW_ID' && data == '20') document.forms[0].action = '/sessionProposal' ;
  document.forms[0].target = '' ;
  document.forms[0][fieldname].value=data;
  document.forms[0]['NEW_WORKFLOW_TAB_FL'].value='Y';
  setNextPage(nextPage);
}

function setDataAndNextPageConfirmUnnasign(fieldname, data, nextPage){
  var yn = confirm('This will delete any Scores,Recommendations, etc already done by this person');
  if(yn)
  {
    document.forms[0].target = '' ;
    document.forms[0][fieldname].value=data;
    setNextPage(nextPage);
  }
}

function setDataAndNextPageTarget(fieldname, data, nextPage){
  document.forms[0][fieldname].value=data;
  var newTarget = '_' + currentTarget++ ;
  document.forms[0].target = newTarget;
  document.forms[0].NEXT_PAGE.value=nextPage;
  document.forms[0].submit();
}

function setNextPageTarget(nextPage){
  var newTarget = '_' + currentTarget++ ;
  document.forms[0].target = newTarget;
  document.forms[0].NEXT_PAGE.value=nextPage;
  document.forms[0].submit();
}

function setDataAndNextPageNoTarget(fieldname, data, nextPage){
  document.forms[0][fieldname].value=data;
  var newTarget = '_' + currentTarget++ ;
  document.forms[0].NEXT_PAGE.value=nextPage;
  document.forms[0].submit();
}

function setDataAndNextPageSpecifyTarget(fieldname, data, nextPage, newTarget){
  document.forms[0][fieldname].value=data;
  document.forms[0].target = newTarget ;
  setNextPage(nextPage);
}

function setActionData(action, fieldname, data){
  document.forms[0].target = '' ;
  document.forms[0].TAG_ACTION.value=action;
  document.forms[0][fieldname].value=data;
  document.forms[0].submit();
}

function setActionPageData(action, page, fieldname, data){
  document.forms[0].target = '' ;
  document.forms[0].TAG_ACTION.value=action;
  document.forms[0].NEXT_PAGE.value=page;
  document.forms[0][fieldname].value=data;
  document.forms[0].submit();
}

function setPostActionPageData(postAction, page, fieldname, data){
  document.forms[0].target = '' ;
  document.forms[0].POST_ACTION.value=postAction;
  document.forms[0].NEXT_PAGE.value=page;
  document.forms[0][fieldname].value=data;
  document.forms[0].submit();
}

function postPage(){
  document.forms[0].target = '' ;
  document.forms[0].submit();
}

function selectCheckBoxes(fieldName){
  var field = fieldName;for (var i = 0; i<document.forms[0].elements.length; i++) {
    if ((document.forms[0].elements[i].name.indexOf(field) > -1)) {
      document.forms[0].elements[i].checked = true;
    }
  }
}
 
function unselectCheckBoxes(fieldName){
  var field = fieldName;for (var i = 0; i<document.forms[0].elements.length; i++) {
    if ((document.forms[0].elements[i].name.indexOf(field) > -1)) {
      document.forms[0].elements[i].checked = false;
    }
  }
}
 
function selectCheckBoxesForRange(fieldName){
  var field = fieldName;var start = document.forms[0].CHECK_START.value;
  var end = document.forms[0].CHECK_STOP.value;
  var counter = 0;
  for (var i = 0; i<document.forms[0].elements.length; i++) {
    if ((document.forms[0].elements[i].name.indexOf(field) > -1)) {
    counter++;
    if(counter >= start && counter <= end){
      document.forms[0].elements[i].checked = true;
      }
    }
  }
}
 
function selectCheckBoxesForRange(fieldName,offset){
  var field = fieldName;var start = document.forms[0].CHECK_START.value;
  var end = document.forms[0].CHECK_STOP.value;
  var counter = offset;
  for (var i = 0; i<document.forms[0].elements.length; i++) {
    if ((document.forms[0].elements[i].name.indexOf(field) > -1)) {
    counter++;
    if(counter >= start && counter <= end){
      document.forms[0].elements[i].checked = true;
      }
    }
  }
}
 
function popWindow(url){
  winStats='toolbar=no,location=no,directories=no,menubar=no,resizable=yes,'
  winStats+='scrollbars=yes,width=400,height=800'
  window.open(url,'',winStats)     
}

function popUpWindow(url){
  winStats='resizable=yes,width=700,height=600,return=true,scrollbars=1'
  window.open(url,'_proof',winStats)     
}

function popUpWindowHref(url,type,winStats){
  window.open(url,type,winStats)     
}

function popWindowAb(url){
  winStats='toolbar=no,location=no,directories=no,menubar=no,resizable=yes,'
  winStats+='scrollbars=yes,width=750,height=550'
  if (navigator.appName.indexOf('Microsoft')>=0) {
   winStats+=',left=10,top=25'
   }else{
     winStats+=',screenX=10,screenY=25'
   }
  window.open(url,'',winStats)     
}

function setDecisionStatus(list)
{
  var index;
  index = list.selectedIndex;
  for (var i = 0; i<document.forms[0].elements.length; i++)
  {
    if ((document.forms[0].elements[i].name.indexOf('DECISION') > -1)&&(document.forms[0].elements[i] != list))
    {
      document.forms[0].elements[i].selectedIndex = index;
    }
  }
}

function deselectCheckBoxes(fieldName){
  for (var i = 0; i<document.forms[0].elements.length; i++) {
  if ((document.forms[0].elements[i].name.indexOf(fieldName) > -1)) {
    document.forms[0].elements[i].checked = false;
    }
  }
}
 
function enableControls(fieldName, enable){
  for (var i = 0; i<document.forms[0].elements.length; i++) {
  if ((document.forms[0].elements[i].name.indexOf(fieldName) > -1)) {
    if(enable == "true")document.forms[0].elements[i].disabled = false;
      else document.forms[0].elements[i].disabled = true;
    }
  }
}
 
function popList(lstParent, lstChild, holder, displayNum, selectedChildId){
  clearList(lstChild);
  var k = 0;
  lstChild.options[k] = new Option('Select', '');
  var n = lstParent.options[lstParent.selectedIndex].value;
  if ( n > 0 ){
    var num = 1;
    for(i=0;i< holder.length;i++){
      var array = holder[i];
      if(array['parent']==n){
        if(displayNum == 'N'){
          txtItem = array['name'];
        } else {
          txtItem = num + '. ' + array['name'];
        } 
        num = num + 1;
        optValue = array['id'];
        lstChild.options[++k] = new Option(txtItem, optValue);
        if(optValue == selectedChildId){
          lstChild.selectedIndex=i;
        } else {
          lstChild.selectedIndex=0;        }
      }
    }
  }
}

function clearList(lstChild)
{
  var longi = lstChild.length;
  for ( var i=longi-1; i >= 0; i-- )
  {
    lstChild.remove(i);
  }
}

var locked = false;

function lock_buttons() {
  if (locked) {
    return false;
  }
  locked = true;
  return true;
}

function isSelected(fieldName){
  var field = fieldName;
  for (var i = 0; i<document.forms[0].elements.length; i++) {
    if ((document.forms[0].elements[i].name.indexOf(field) > -1) && (document.forms[0].elements[i].checked == true))
    return true;
  }
  
  alert('You must first make a selection.'); 
  return false; 
}
 
function getRadioGroupValue(fieldName)  
{  
  var radioGroup = document.forms[0][fieldName];  
  if (radioGroup != undefined)  
  {  
    if (radioGroup.length == undefined)  
    {  
      if (radioGroup.checked)  
      {  
        return radioGroup.value;  
      }  
    }  
    else  
    {  
      for (var i = 0; i < radioGroup.length; i++)  
      {  
        if (radioGroup[i].checked)  
        {  
          return radioGroup[i].value;  
        }  
      }  
    }  
  }  
  return '';  
}  

function limitFieldValue(field, limit)  
{  
  if (field.value.length > limit)  
  {  
    field.value = field.value.substring(0, limit);  
  }  
}  

function addEventHandler(obj, eventName, handler) {  
  if (obj.addEventListener) {  
    obj.addEventListener(eventName, handler, false);  
  }  
  else if (obj.attachEvent) {  
    obj.attachEvent('on' + eventName, handler);  
  }  
  else {  
    obj['on' + eventName] = handler;  
  }  
}  

function showProgress(uniqueID) {  
  document.getElementById('upload'+uniqueID).style.display = 'none';  
  document.getElementById('progress'+uniqueID).style.display = 'block';  
  document.getElementById('progressGif'+uniqueID).style.display = 'block';  
  document.getElementById('progress'+uniqueID).style.marginTop = -15+'px';  
  setTimeout('document.images["progressGif"].src = "/images/progress.gif"', 200);  
  window.scrollTo(0,0);  
  return true;  
}

function isMaxLength(objectIn)
{
  var maxLength = objectIn.getAttribute ? parseInt(objectIn.getAttribute("textlength")) : "";
  if (objectIn.getAttribute && objectIn.value.length > maxLength)
  {
    objectIn.value = objectIn.value.substring(0, maxLength);
    return false;
  }
  return true;
}

function isMaxLines(objectIn)
{
  var maxLines = objectIn.getAttribute ? parseInt(objectIn.getAttribute("maxlines")) : "";
  var count = objectIn.value.split(/\r|\r\n|\n/).length;
  if (objectIn.getAttribute && count > maxLines)
  {
    var value = '';
    var lines = objectIn.value.split(/\r|\r\n|\n/);
    for (var i = 0; i < maxLines; i++) {
      var line = lines[i];
      value += line;
      if (i != maxLines - 1)
      {
        value += '\n';
      }
    }

    objectIn.value = value;
    return false;
  }
  return true;
}

function setSelectedIndex(s, v) {
    for ( var i = 0; i < s.options.length; i++ ) {
        if ( s.options[i].value == v ) {
            s.options[i].selected = true;
            return;
        }
    }
}

// Support IE8 trim()
String.prototype.trim = function() {
    return this.replace(/^\s+|\s+$/g,"");
}
String.prototype.ltrim = function() {
    return this.replace(/^\s+/,"");
}
String.prototype.rtrim = function() {
    return this.replace(/\s+$/,"");
}

/**
 * Disable hidden input by certain name and value.
 * US-000965
 *
 * @param name
 * @param value
 */
function disableHiddenInput(name, value) {
  $('input[name=' + name + ']:hidden').filter('[value=' + value + ']').prop('disabled', true);
}

/**
 * Dynamically add new hidden inputs by selected options in multiple select control.
 * US-000965
 *
 * @param controlName
 * @param hiddenName
 */
function addSelectedMultipleSelectedOptionsAsHiddenInputs(controlName, hiddenName) {
  var $control = $('[name=' + controlName + ']');
  var $selectedOptions = $control.find('option:selected');
  var selectedOptionValues = $.map($selectedOptions, function(elem, key) {
    return $(elem).val();
  });

  var $existingOptions = $('[name=' + hiddenName + ']');
  var $form = $('form').eq(0);
  for (var i = 0; i < selectedOptionValues.length; i++) {
    var value = selectedOptionValues[i];
    if ($existingOptions.filter('[value=' + value + ']').length == 0) {
      $form.append('<input type="hidden" name="' + hiddenName + '" value="' + value + '">');
    }
  }
}

function enableDisableRadio(chkele){
	if(chkele.checked){
		document.getElementById('rad_'+chkele.id).checked=true;
		document.getElementById('rad_'+chkele.id).disabled=false;
	} else {
		document.getElementById('rad_'+chkele.id).checked=false;
		document.getElementById('rad_'+chkele.id).disabled=true;
	}
}

function validateFileNameExtension(fileName, allowedExtensions)
{
    if (fileName)
    {
        var startExtensionIndex = fileName.lastIndexOf('.');
        if (startExtensionIndex != -1)
        {
            var extension = fileName.substring(startExtensionIndex + 1).toLowerCase();
            if (allowedExtensions.toLowerCase().indexOf(extension) == -1)
            {
                return false;
            }
        }
    }
    
    return true;
}
