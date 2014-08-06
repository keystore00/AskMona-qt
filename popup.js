var t_id = location.href.split("/")[3];
function new_popup(e,markup,parent)
{
    var div = $("<div>");
    div.html(markup);
    div.find("p").removeAttr("id");
    div.appendTo(parent);
    if (parent.attr("id")=="origin") {
	posX = e.pageX;
	posY = e.pageY-20;
    } else {
	posX = e.offsetX;
	posY = e.offsetY-20;
    }
    div.css({
	"top":posY+"px",
	"left":posX+"px",
	"position":"absolute",
	"background":"#eee",
	"padding":"0px 10px",
	"border": "1px #999 solid"
    });
    div.find("a.ra").mouseenter(function(e){
	mouseenter_listener($(this),e,"popup");
    });
    div.find("a.raed").mouseenter(function(e){
	mouseenter_listener($(this),e,"popup");
    });
    parent.mouseleave(function(){
	div.remove();
	parent.find("a").unwrap();
    });
}
function new_ra_popup(r_id,e,parent)
{
    $.get("api/getres", { t: t_id, r: r_id },
	  function(data){
	      new_popup(e,data,parent)
	  });
}
function new_raed_popup(r_id,e,parent)
{
    new_popup(e,$("div#a_"+r_id).html(),parent);
}

function mouseenter_listener(caller,e,id)
{
    var r_id = caller.attr("href").split("_")[1];
    caller.wrap("<div class=\"popup\" id=\""+id+"\" style=\"display:inline-block;\"></div>");
    var new_div = caller.parent().closest('div');
    if (caller.hasClass("ra")) {
	new_ra_popup(r_id,e,new_div);
    } else {
	new_raed_popup(r_id,e,new_div);
    }
    caller.unbind("mouseenter");
    new_div.mouseleave(function(){
	caller.mouseenter(function(e){
	    mouseenter_listener($(this),e,id);
	});
    });
}
$("a.raed").mouseenter(function(e){
    mouseenter_listener($(this),e,"origin");
});
$("a.ra").unbind();
$("a.ra").mouseenter(function(e){
    mouseenter_listener($(this),e,"origin");
});
