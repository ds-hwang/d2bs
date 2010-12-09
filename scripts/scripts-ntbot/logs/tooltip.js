/**
 * @author agor
 * @projectDescription Itemstats Tooltip Display
 */
tt = null;
 
document.onmousemove = updateTT;
 
function updateTT(e) {
  if (tt != null) {
    x = (document.all) ? window.event.x + tt.offsetParent.scrollLeft : e.pageX;
    y = (document.all) ? window.event.y + tt.offsetParent.scrollTop  : e.pageY;
    tt.style.left = (x + 1) + "px";
    tt.style.top   = (y + 1) + "px";
  }
}
 
function showTT(id) {
  tt = document.getElementById(id);
  tt.style.display = "block"
}
 
function hideTT() {
  tt.style.display = "none";
}