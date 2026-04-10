// JavaScript Document

function setupRequiredFieldsDisplay()
{
  for(var i in CKEDITOR.instances)
  {
    var divFieldName = "#" + CKEDITOR.instances[i].name + "_REQUIRED";
    var required = $(divFieldName).attr('required');
    if (required == 'required')
    {
      var editorName = CKEDITOR.instances[i].name; 
      $("#" + editorName + "_REQUIRED").removeClass('bordered'); 
      if(CKEDITOR.instances[i].getData().length < 7){ $("#" + editorName + "_REQUIRED").addClass('bordered'); }   
  
      CKEDITOR.instances[i].on('contentChanged', function(e) {
         var editorName = e.editor.name; 
         $("#" + editorName + "_REQUIRED").removeClass('bordered'); 
         if(e.editor.getData().length < 7){ $("#" + editorName + "_REQUIRED").addClass('bordered'); }   
      });
    }
  }
  
  $("div [RequiredTextField]").each(function (i, obj)
  {
    var id = obj.id;
    var controlid = id.substring(0, id.length - 9);
    
    $('#' + id).removeClass('control-group error');    
    if($('#' + controlid).val().length == 0){ $('#' + id).addClass('control-group error'); }     
            
    $('#' + controlid).keyup(function() { 
      $('#' + id).removeClass('control-group error');    
      if($('#' + controlid).val().length == 0){ $('#' + id).addClass('control-group error'); } 
    });
    
    $('#' + controlid).on('paste', function() { 
      $('#' + id).removeClass('control-group error');    
      if($('#' + controlid).val().length == 0){ $('#' + id).addClass('control-group error'); }         
    });
    
    $('#' + controlid).on('cut', function() { 
      $('#' + id).removeClass('control-group error');    
      if($('#' + controlid).val().length == 0){ $('#' + id).addClass('control-group error'); }         
    });
  });
}
