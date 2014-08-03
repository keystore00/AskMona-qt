var cnt = 0;
var t_id = location.href.split("/")[3];
function new_popup(e,markup)
{
    var div = $("<div>");
    var id = "#tmp_popup_"+cnt++;
    div.attr('id',id);
    div.attr('z-index',cnt);
    div.addClass("tmp_"+cnt);
    div.html(markup);
    posX = e.pageX-4;
    posY = e.pageY-10;
    if (cnt==1) {
	div.appendTo($("#popup2"));
    } else {
	var prev = $("#popup2").find(".tmp_"+(cnt-1));
	div.appendTo(prev);
	posX = e.offsetX;
	posY = e.offsetY;
    }
    div.fadeIn("fast");
    var leftpx = Math.min(e.pageX,$("body").width()*0.2);
    div.css({
	"top":posY+"px",
	"left":posX+"px",
	"position":"absolute",
	"background":"#eee",
	"padding":"0px 10px",
	"border": "1px #999 solid"
    });
    div.fadeOut(1000, function(){$(this).remove();--cnt;});
    div.mouseenter(function(){
	div.stop();
	div.fadeTo(10,1.0);
    });
    div.find("a.ra").mouseover(function(e){
	var r_id = $(this).attr("href").split("_")[1];
	new_ra_popup(r_id,e);
    });
    div.find("a.raed").mouseover(function(e){
	var r_id = $(this).attr("href").split("_")[1];
	new_raed_popup(r_id,e);
    });
    div.mouseleave(function(){
	div.remove();
	--cnt;
    });
}
function new_ra_popup(r_id,e)
{
    $.get("api/getres", { t: t_id, r: r_id },
	  function(data){
	      new_popup(e,data)
	  });
}
function new_raed_popup(r_id,e)
{
    new_popup(e,$("div#a_"+r_id).html());
}
$("a.raed").mouseover(function(e){
    var r_id = $(this).attr("href").split("_")[1];
    new_pup = new_raed_popup(r_id,e);
});
$("a.ra").unbind("mouseover").unbind("mousemove").unbind("mouveout");
$("a.ra").mouseover(function(e){
    var r_id = $(this).attr("href").split("_")[1];
    new_pup = new_ra_popup(r_id,e);
});
