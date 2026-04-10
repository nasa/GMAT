/**
* jQuery Loader Plugin.
*
* @author Rémi Goyard (https://www.mimiz.fr/)
* @version 2.1.1
* @license MIT 
* @license GPLv2 
* @date 2013-07-21
**/
var jQueryLoaderOptions=null;!function(a){a.loader=function(b){switch(b){case"close":return jQueryLoaderOptions&&a("#"+jQueryLoaderOptions.id)&&a("#"+jQueryLoaderOptions.id+", #"+jQueryLoaderOptions.background.id).remove(),void 0;case"setContent":return jQueryLoaderOptions&&a("#"+jQueryLoaderOptions.id)&&(2==arguments.length?a("#"+jQueryLoaderOptions.id).html(arguments[1]):console?console.error("setContent method must have 2 arguments $.loader('setContent', 'new content');"):alert("setContent method must have 2 arguments $.loader('setContent', 'new content');")),void 0;default:var c=a.extend({content:"Loading ...",className:"loader",id:"jquery-loader",height:60,width:200,zIndex:3e4,background:{opacity:.4,id:"jquery-loader-background"}},b)}jQueryLoaderOptions=c;var d=a(document).height(),e=a(window).width(),f=a('<div id="'+c.background.id+'"/>');f.css({zIndex:c.zIndex,position:"absolute",top:"0px",left:"0px",width:e,height:d,opacity:c.background.opacity}),f.appendTo("body"),jQuery.bgiframe&&f.bgiframe();var g=a('<div id="'+c.id+'" class="'+c.className+'"></div>');g.css({zIndex:c.zIndex+1,width:c.width,height:c.height}),g.appendTo("body"),g.center(),g.html(c.content)},a.fn.center=function(){return this.css("position","absolute"),this.css("top",(a(window).height()-this.outerHeight())/2+a(window).scrollTop()+"px"),this.css("left",(a(window).width()-this.outerWidth())/2+a(window).scrollLeft()+"px"),this}}(jQuery);
