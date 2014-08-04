var t_id = location.href.split("/")[3];
var timer;
var timeout = 1;
function timeout_listener(div,maker)
{
    div.remove()
    maker.mouseenter(function(e){
	var r_id = $(this).attr("href").split("_")[1];
	if (maker.hasClass("raed")) {
	    new_raed_popup($(this),r_id,e);
	} else {
	    new_ra_popup($(this),r_id,e);
	}
    });
}
function new_popup(maker,e,markup)
{
    var div = $("<div>");
    div.attr('id',"tmp_popup");
    div.html(markup);
    posX = e.pageX;
    posY = e.pageY-30;
    if ($('#popup2 div').length == 0) {
	div.appendTo($("#popup2"));
    } else {
	var list = $('#popup2 div');
	var prev = $(list[list.length-1]);
	div.appendTo(prev);
	posX = e.offsetX;
	posY = e.offsetY-30;
    }
    div.css({
	"top":posY+"px",
	"left":posX+"px",
	"position":"absolute","background":"#eee","padding":"0px 10px","border": "1px #999 solid"
    });
    div.mouseenter(function(){
	clearTimeout(timer);
    });
    div.mouseleave(function(){
	timer = setTimeout(function(){timeout_listener(div,maker)},timeout);
    });
    div.find("a.ra").mouseenter(function(e){
	var r_id = $(this).attr("href").split("_")[1];
	new_ra_popup($(this),r_id,e);
    });
    div.find("a.raed").mouseenter(function(e){
	var r_id = $(this).attr("href").split("_")[1];
	new_raed_popup($(this),r_id,e);
    });
    return div;
}
function rebind_mouseevents(target,div_target)
{
    target.unbind("mouseleave");
    target.unbind("mouseenter");
    target.mouseenter(function(e){
	clearTimeout(timer);
    });
    target.mouseleave(function(){
	timer = setTimeout(function(){timeout_listener(div_target,target)},timeout);
    });
}
function new_ra_popup(maker,r_id,e)
{
    $.get("api/getres", { t: t_id, r: r_id },
	  function(data){
	      var new_pup = new_popup(maker,e,data)
	      rebind_mouseevents(maker,new_pup);
	  });
}
function new_raed_popup(maker,r_id,e)
{
    var new_pup = new_popup(maker,e,$("div#a_"+r_id).html());
    rebind_mouseevents(maker,new_pup);
}
$("a.raed").mouseenter(function(e){
    var r_id = $(this).attr("href").split("_")[1];
    new_raed_popup($(this),r_id,e);
});
$("a.ra").unbind("mouseover").unbind("mousemove").unbind("mouveout");
$("a.ra").mouseenter(function(e){
    var r_id = $(this).attr("href").split("_")[1];
    new_ra_popup($(this),r_id,e);
});

