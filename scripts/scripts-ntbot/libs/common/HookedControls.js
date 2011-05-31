
/// <reference path="/../../d2bsAPI.js" />
addEventListener("keyup", myKeyHandler)
addEventListener("mouseclick", GlobalCheckBoxClick);
var GlobalCheckBoxArray = new Array;
var GlobalTextBoxArray = new Array;
var GlobalVScrollArray = new Array;
var GlobalDropDownArray = new Array;
//var test = new TextBox(100, 100, 300, 20, "", 3, 0)
	//test.editable=true
var focusedControl = false;
var clickCount = 2;
function myKeyHandler(key) {
	if (!focusedControl) return true;
	if (focusedControl.setKeyVal) {
		focusedControl.text = key+" "
		if (typeof(focusedControl.valueChanged)=='function')
			  focusedControl.valueChanged(key)
		focusedControl = false
		return true
	}
	var abc = "abcdefghijklmnopqrstuvwxyz"
	var ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	var one = "0123456789"
	var ONE = ")!@#$%^&*("
	var code =""
	if (keystate(16)) {	 //0=48
		code = ABC.substring(key - 65, key - 64)
		code=code+ONE.substring(key-48,key-47)
	} else {
		code = abc.substring(key - 65, key - 64)
		code = code + one.substring(key - 48, key - 47)
	}
	if (key == 32) code = " "
	   focusedControl.text=focusedControl.text + code
	if (key==8 )
		focusedControl.text = focusedControl.text.substring(0, focusedControl.text.length - 1)
	if (typeof (focusedControl.valueChanged) == 'function')
		focusedControl.valueChanged(focusedControl.text)

	if (key == 28) focusedControl = false
	return true;
}
function myGetTextSize (txt,font){
if (typeof(getTextWidthHeight) == "function")
	return getTextWidthHeight(txt,font);
else
    return getTextSize(txt, font)    
    
}
	 
function CheckBox(x, y,Txt,Checked,boxOnLeft) {
 	//text "text",x,y,color,font,alingment
 this.x = x; 
 this.y = y; 
 this.boxSize = 10;
 this.textColor =2;
 this.font = 6;
 this.boxColor=8
 this.boxOnRight = true;
 if (boxOnLeft) this.boxOnRight =false
 this.text = Txt;
 this.checked = Checked;
 this.checkedColor = 8;
 this.visible = true;
 this.checkedChanged =false //function to set 
 //this.width = width; 
 //this.height = height; 
// this.hooks = new Array;
 
 
  this.textHook = new Text(this.text,this.x ,this.y+this.boxSize,this.textColor,this.font,1)
  this.boxLine1 = new Line(this.x,this.y,this.x+this.boxSize,this.y,this.boxColor)
  this.boxLine2 = new Line(this.x,this.y,this.x,this.y+this.boxSize,this.boxColor)
  this.boxLine3 = new Line(this.x,this.y+this.boxSize,this.x+this.boxSize,this.y + this.boxSize,this.boxColor)
  this.boxLine4 = new Line(this.x+this.boxSize,this.y,this.x+this.boxSize,this.y +this.boxSize,this.boxColor)
  if (this.checked)
	this.box = new Box(this.x,this.y,this.boxSize,this.boxSize,this.checkedColor,7)
  else 
	this.box = new Box(this.x,this.y,this.boxSize,this.boxSize,0,7)
if (!this.boxOnRight) this.update()
	this.watch("text", function(id,oldval,newval) {
		this.text = newval
		this.update();
		return newval;	
	});
	this.watch("x", function(id,oldval,newval) {
		this.x = newval
		this.update();
		return newval;	
	});
	this.watch("y", function(id,oldval,newval) {
		this.y = newval
		this.update();
		return newval;	
	});
	this.watch("checked", function(id,oldval,newval) {
		this.checked = newval;
		Checked=newval;
		this.update();
		if (typeof (this.checkedChanged) == "function")
            this.checkedChanged(newval)
		return newval;	
	});
	this.watch("visible", function(id,oldval,newval) {
		this.visible = newval
		this.update();
		return newval;	
	});
	this.watch("boxOnRight", function(id,oldval,newval) {
		this.boxOnRight = newval
		this.update();
		return newval;	
	});
	this.watch("textColor", function(id,oldval,newval) {
		this.textColor = newval
		this.update();
		return newval;	
	});
	this.watch("boxColor", function(id,oldval,newval) {
		this.boxColor = newval
		this.update();
		return newval;	
	});
//print(this.box.toSource());
//this.box.parent=this;


GlobalCheckBoxArray.push (this);



this.box.click = function (b, x, y) { 
// empty function that just blocks the click
	}


}

CheckBox.prototype.update = function () {
		this.textHook.text=this.text;
		this.textHook.x= (this.boxOnRight ? this.x -5 : this.x + this.boxSize +5);
		this.textHook.y = this.y+ this.boxSize;
		this.textHook.color = this.textColor;
		this.textHook.font = this.font;
		this.textHook.align=(this.boxOnRight ? 1 : 3);
		this.textHook.visible=this.visible
		
		this.boxLine1.x=this.x;
		this.boxLine1.y = this.y;
		this.boxLine1.x1 = this.x+this.boxSize;
		this.boxLine1.y1 = this.y;
		this.boxLine1.color= this.boxColor;
		this.boxLine1.visible=this.visible
		
		this.boxLine2.x=this.x;
		this.boxLine2.y = this.y;
		this.boxLine2.x1 = this.x;
		this.boxLine2.y1 = this.y+this.boxSize;
		this.boxLine2.color= this.boxColor;
		this.boxLine2.visible=this.visible
		
		this.boxLine3.x=this.x;
		this.boxLine3.y = this.y+this.boxSize;
		this.boxLine3.x1 = this.x+this.boxSize;
		this.boxLine3.y1 = this.y+this.boxSize;
		this.boxLine3.color= this.boxColor;
		this.boxLine3.visible=this.visible
		
		this.boxLine4.x=this.x+this.boxSize;
		this.boxLine4.y = this.y;
		this.boxLine4.x1 = this.x+this.boxSize;
		this.boxLine4.y1 = this.y+this.boxSize;
		this.boxLine4.color= this.boxColor;
		this.boxLine4.visible=this.visible
		
		this.box.x= this.x+1;
		this.box.y= this.y+1;
		this.box.xsize=this.boxSize-1;
		this.box.ysize=this.boxSize-1;
		this.box.color=(this.checked ? this.boxColor : 0);
		if (this.visible){
			this.box.color=(this.checked ? this.checkedColor : 0);
		}else{
			this.box.color= 0;
		}
		this.box.opacity=(this.visible ? 7 : 0)
		this.box.visible=this.visible;
}

function updateControls(){
	for (var j = 0 ; j < GlobalCheckBoxArray.length; j++){					
		GlobalCheckBoxArray[j].update();
	}
}
function GlobalCheckBoxClick(b,x,y,up){

clickCount = clickCount +1;
 //click event dosent tell up or down so every other event is good enough for me
//print(" b "+b+" x" + x+" y" +y)
var j = 0
 if (b == 0 && clickCount % 2 == 1){	
	for ( j = 0 ; j < GlobalCheckBoxArray.length; j++){	
		if (GlobalCheckBoxArray[j].x < x && GlobalCheckBoxArray[j].y < y && GlobalCheckBoxArray[j].x + GlobalCheckBoxArray[j].boxSize > x && GlobalCheckBoxArray[j].y + GlobalCheckBoxArray[j].boxSize > y && GlobalCheckBoxArray[j].visible){			
			GlobalCheckBoxArray[j].checked = (!GlobalCheckBoxArray[j].checked);
			GlobalCheckBoxArray[j].update(); //this isnt working			
		}
	}
	for ( j = 0; j < GlobalVScrollArray.length; j++){
		if(GlobalVScrollArray[j].x <x && GlobalVScrollArray[j].y < y && GlobalVScrollArray[j].x +GlobalVScrollArray[j].xSize > x && GlobalVScrollArray[j].y+GlobalVScrollArray[j].ySize >y && GlobalVScrollArray[j].visible){
		
			if (GlobalVScrollArray[j].verticle && y > GlobalVScrollArray[j].slider.y - 10 && y < GlobalVScrollArray[j].slider.y ){
				GlobalVScrollArray[j].dragMode = true;
				break;
			}
			
			if (!GlobalVScrollArray[j].verticle && x > GlobalVScrollArray[j].slider.x - 5 && x < GlobalVScrollArray[j].slider.x+5 ){			
				GlobalVScrollArray[j].dragMode = true;
				break;
			}
			if (y < GlobalVScrollArray[j].y+GlobalVScrollArray[j].weight && x < GlobalVScrollArray[j].x +GlobalVScrollArray[j].weight){
				GlobalVScrollArray[j].value -= 1;
				break;
			}
			if ((y < GlobalVScrollArray[j].slider.y && GlobalVScrollArray[j].verticle)||(!GlobalVScrollArray[j].verticle && x < GlobalVScrollArray[j].slider.x)){
				GlobalVScrollArray[j].value -= GlobalVScrollArray[j].bigChange;
				break;
			}			
			if ((GlobalVScrollArray[j].verticle && y < GlobalVScrollArray[j].y+GlobalVScrollArray[j].ySize-GlobalVScrollArray[j].weight)||(!GlobalVScrollArray[j].verticle && x < GlobalVScrollArray[j].x+GlobalVScrollArray[j].xSize-GlobalVScrollArray[j].weight)){
			GlobalVScrollArray[j].value += GlobalVScrollArray[j].bigChange;
				break;
			}
			GlobalVScrollArray[j].value +=1;			
		}
	}
	focusedControl=false
	for ( j = 0; j < GlobalTextBoxArray.length; j++){
		if (GlobalTextBoxArray[j].enableRowSelection && GlobalTextBoxArray[j].x < x && GlobalTextBoxArray[j].y < y && GlobalTextBoxArray[j].x + GlobalTextBoxArray[j].xSize > x && GlobalTextBoxArray[j].y + GlobalTextBoxArray[j].ySize > y && GlobalTextBoxArray[j].visible) {
			if (GlobalTextBoxArray[j].editable) focusedControl = GlobalTextBoxArray[j]
			if(typeof(GlobalTextBoxArray[j].clickFunction) == 'function')
			    GlobalTextBoxArray[j].clickFunction(GlobalTextBoxArray[j]);
			for (var l = 0; l < GlobalTextBoxArray[j].lines.length; l++){	
				//print(GlobalTextBoxArray[j].lines[l].y +" > "+ y)
				if(GlobalTextBoxArray[j].lines[l].y  > y ){				
					GlobalTextBoxArray[j].selectedRow=l;
					GlobalTextBoxArray[j].selectedText=GlobalTextBoxArray[j].lines[l].text;						
					break;				
				}				
			}				
		}
	}
	
	
 }
else {
	for ( j = 0; j < GlobalVScrollArray.length; j++){
		if (GlobalVScrollArray[j].dragMode){
			if (GlobalVScrollArray[j].verticle)
				GlobalVScrollArray[j].value =parseInt(GlobalVScrollArray[j].max * (y - GlobalVScrollArray[j].scrollBox.y  )/GlobalVScrollArray[j].scrollBox.ysize);			
			else
				GlobalVScrollArray[j].value =parseInt(GlobalVScrollArray[j].max * (x - GlobalVScrollArray[j].scrollBox.x  )/GlobalVScrollArray[j].scrollBox.xsize);	
			
		}
		GlobalVScrollArray[j].dragMode= false;	
	}
 
 }	
  for (  j = 0; j < GlobalDropDownArray.length; j++){
	if (GlobalDropDownArray[j].droped)
		if (x < GlobalDropDownArray[j].x || x > GlobalDropDownArray[j].x +GlobalDropDownArray[j].autoWide+20 || y < GlobalDropDownArray[j].y || y> GlobalDropDownArray[j].y+120){
			GlobalDropDownArray[j].droped=false;
			GlobalDropDownArray[j].update();
		}
  }
}
function TextBox(x, y,xSize,ySize,Txt,txtColor,backColor) {
 	//text "text",x,y,color,font,alingment
	this.editable = false  //can type into text boxes
	this.setKeyVal = false;	// sets text = keypress
 this.x = x; 
 this.y = y; 
 this.xSize = xSize;
 this.ySize = ySize;
 this.scrollIndex = 0;
 this.boxSize = 10;
 this.textColor =0;
 this.font = 6;
 this.boxColor=0;
 this.boxOnRight =true
 this.ShowOutline = true;
 this.text = Txt;
 this.backColor = (backColor) ? backColor : 18;  //2 = gray
 this.textColor = (txtColor) ? txtColor :13;
 this.visible = true;
 this.fillFromTop = true;
 this.parent = false;  //place holder for linked controls
 this.child = false;	//place holder for linked controls
  this.enableRowSelection =true;
 this.selectedRow = 0;
 this.selectedText = "";
 this.selectedColor = 9;
 this.oldScrollVal=0; // hack
 this.listboxMode = false;
 this.clickFunction = false;
 // this.textHook = new Text(this.text1,this.x ,this.y+this.ySize,this.textColor,this.font,1)
  this.boxLine1 = new Line(this.x,this.y,this.x+this.xSize,this.y,this.boxColor)
  this.boxLine2 = new Line(this.x,this.y,this.x,this.y+this.ySize,this.boxColor)
  this.boxLine3 = new Line(this.x,this.y+this.ySize,this.x+this.xSize,this.y + this.ySize,this.boxColor)
  this.boxLine4 = new Line(this.x+this.xSize,this.y,this.x+this.xSize,this.y +this.ySize,this.boxColor)
 
  this.background = new Box(this.x+1,this.y+1,this.xSize-1,this.ySize-1,this.backColor,1)
  this.background.click = function (b, x, y) { 
// empty function that just blocks the click
	}
  this.lines = new Array;
  this.textLines = new Array;
  for (var l = this.y; l < this.y +this.ySize; l += 10){
	var line = new Text("",this.x+2,l+10,this.textColor, this.font,3)
	this.lines.push (line)  
  }

 //this.redrawHooks()
//if  (this.textLines.length > this.lines.length){
	this.scrollbar = new ScrollBar(this.x+this.xSize,this.y,this.ySize,this.backColor,this.textColor)
	this.scrollbar.max =Math.abs(this.textLines.length - this.lines.length);
	this.scrollbar.parent = this;
	if (!this.fillFromTop)
		this.scrollbar.value=this.scrollbar.max;

 this.updateText();
//print (this.background.toSource())
 //this.scrollbar.visible = (this.textLines.length > this.lines.length);
 //this.scrollbar.update();
 this.watch("scrollIndex", function(id,oldval,newval) {	 
	this.selectedRow=this.selectedRow +(this.oldScrollVal - this.scrollbar.value)
	this.oldScrollVal=this.scrollbar.value  //hack oldval and new dont seem to work nfc	
	this.lineArrayToHooks();	
	return newval;
	});	
 this.watch("x", function(id,oldval,newval) {
	this.x = newval
	this.redrawHooks()
	return newval;
	});
 this.watch("y", function(id,oldval,newval) {
	this.y = newval
	this.redrawHooks()
	return newval;
	});
this.watch("xSize", function(id,oldval,newval) {
	this.xSize = newval
	this.redrawHooks();
	this.updateText();
	return newval;
	});
this.watch("ySize", function(id,oldval,newval) {
	this.ySize = newval
	this.redrawHooks();
	this.updateText();
	return newval;
	});
this.watch("textColor", function(id,oldval,newval) {
	this.textColor = newval
	this.redrawHooks();	
	return newval;
	});
this.watch("backColor", function(id,oldval,newval) {
    this.backColor = newval
	this.redrawHooks();	
	return newval;
	});
this.watch("text", function(id,oldval,newval) {
	this.text = newval
	this.redrawHooks();	
	this.updateText();
	return newval;
	
	});
this.watch("selectedRow", function(id,oldval,newval) {
	this.selectedRow = newval
	this.redrawHooks();	
	this.updateText();
	if (this.parent)
		if (typeof(this.parent.childUpate) == 'function'){
			this.parent.childUpate();
		}
	return newval;	
	});
this.watch("textLines", function(id,oldval,newval) {
	this.textLines = newval
	this.lineArrayToHooks()	
	return newval;	
	});
this.watch("font", function(id,oldval,newval) {
	this.font = newval
	this.redrawHooks()	
	return newval;	
	});
this.watch("selectedColor", function(id,oldval,newval) {
	this.selectedColor = newval
	this.redrawHooks()	
	return newval;	
	});
this.watch("visible", function(id,oldval,newval) {
	this.visible = newval
	this.redrawHooks()	
	return newval;	
	});
 GlobalTextBoxArray.push (this);
 }

 TextBox.prototype.redrawHooks = function () {	
		this.boxLine1.x= this.x;
		this.boxLine1.y = this.y;
		this.boxLine1.xsize = this.x+this.xSize;
		this.boxLine1.ysize = this.y;
		this.boxLine1.color = this.backColor;
		this.boxLine1.visible = (this.visible && this.ShowOutline);
		
		this.boxLine2.x=this.x;
		this.boxLine2.y=this.y;
		this.boxLine2.xSize=this.x;
		this.boxLine2.ysize= this.y +this.ySize;
		this.boxLine2.color = this.backColor;
		this.boxLine2.visible = (this.visible && this.ShowOutline);
		
		this.boxLine3.x= this.x;
		this.boxLine3.y=this.y+this.ySize;
		this.boxLine3.xsize=this.x+this.xSize;
		this.boxLine3.ysize=this.y+this.ySize;
		this.boxLine3.color = this.backColor;
		this.boxLine3.visible = (this.visible && this.ShowOutline);
		
		this.boxLine4.x= this.x+this.xSize;
		this.boxLine4.y=this.y;
		this.boxLine4.xsize=this.x+this.xSize;
		this.boxLine4.ysize=this.y+this.ySize;
		this.boxLine4.color = this.backColor;
		this.boxLine4.visible = (this.visible && this.ShowOutline);
		
		//this.background = new Box(this.x+1,this.y+1,this.xSize-1,this.ySize-1,this.backColor,1)
		this.background.x=this.x+1;
		this.background.y = this.y+1;
		this.background.ysize=this.ySize-1
		this.background.xsize = this.xSize - 1
        this.background.color=this.backColor
		this.background.visible=(this.visible && this.ShowOutline);
		if (this.visible)
			this.scrollbar.visible = (this.textLines.length > this.lines.length);
		else 
			this.scrollbar.visible=false;
		
		var fontSize= 10;
		if (this.font == 5)
			fontSize =18;
		if (this.font == 3)
			fontSize =40;
	for (var j = 0; j < this.lines.length; j ++){
		//var line = new Text("",this.x+2,l+10,this.textColor, this.font,3)
			this.lines[j].x=this.x+2;
			this.lines[j].y= this.y+fontSize+(j*fontSize);
			this.lines[j].color=(j== this.selectedRow)? this.selectedColor : this.textColor;
			this.lines[j].font=this.font;
			this.lines[j].visible= this.visible;
	}		
 }
 TextBox.prototype.updateText = function () {	//rebuild fron .text	
	if (this.listboxMode)
		return true;
		//print("rebuilding text")
		this.textLines.remove(0,this.textLines.length-1)
		var templine ="";
		var linenum = (this.fillFromTop) ? 0 : this.lines.length -1;
		var spaceAt = 0;
  if (this.text){
		for (var t = 0; t<this.text.length ; t++){
			templine=templine+ this.text[t];  
			if (myGetTextSize(templine,this.font)[0] >this.xSize -4){ // 8 controls how close to get to the edge
				spaceAt =templine.lastIndexOf(" "); //wrap at space
				if (spaceAt){
					t=t-(templine.length-spaceAt)+1;
					templine = templine.substring(0,spaceAt) ;		
				}		
				this.textLines.push(templine)
				templine = "";		
			}	
		}		
	if (templine.length >0)
		this.textLines.push(templine)	
	}
	this.lineArrayToHooks();
return true;
 }

TextBox.prototype.lineArrayToHooks = function () {

this.scrollbar.max =Math.abs(this.textLines.length - this.lines.length);
	if (this.visible)
			this.scrollbar.visible = (this.textLines.length > this.lines.length);
		else 
			this.scrollbar.visible=false;
  	var sboffset = (this.scrollbar) ? this.scrollbar.value : 0;
	
	if (this.fillFromTop == true){	
		for(var f = 0 ; f < this.lines.length ; f++){	
			if (this.textLines[f+sboffset])
				this.lines[f].text = this.textLines[f+sboffset];
			else	
				this.lines[f].text =""
		}
	}
	else{
		var index = this.textLines.length-1;
		for(var ff = this.lines.length -1; ff>-1 ; ff--){			
		this.lines[ff].text = this.textLines[index];
			index --;
			if (index == -1)
				break;		
		}
	}
}
TextBox.prototype.deleteSelectedRow = function () {
	this.textLines.remove(this.selectedRow)
	
	if (this.textLines[this.selectedRow])
		this.selectedText=this.textLines[this.selectedRow]
	else
		this.selectedText ="";
	this.lineArrayToHooks();
	this.redrawHooks();

}
TextBox.prototype.moveSelectedRow = function (dir) {//1=down;	
	var temp = this.textLines[this.selectedRow];	
	if (dir == 1){ //1=down;	
		this.textLines.remove(this.selectedRow)
		this.textLines.splice(this.selectedRow+1,0,temp)
		this.selectedRow ++;
		this.selectedText=this.textLines[this.selectedRow];
	}
	if (dir == -1 && this.selectedRow >0){
		this.textLines.remove(this.selectedRow)
		this.textLines.splice(this.selectedRow-1,0,temp)
		this.selectedRow =this.selectedRow -1
		this.selectedText=this.textLines[this.selectedRow];	
	}
	print (this.textLines.toSource());
	this.lineArrayToHooks();
	this.redrawHooks();

}

function DP(text){
sendCopyData(null, "OOG", 0,text+" ")
}
TextBox.prototype.addLine = function (text) {
this.textLines.push(text)
//print(this.textLines.toSource())
this.scrollbar.max =this.textLines.length - this.lines.length;
this.lineArrayToHooks();

}
function ScrollBar(x, y,length,backColor,forColor,initalValue) {
 	//text "text",x,y,color,font,alingment
 this.x = x; 
 this.y = y; 
 this.verticle = true;
 this.length = length;
 this.xSize = (this.verticle) ?  15 :length  ;
 this.ySize = (this.verticle) ?  length :15  ;
 
 this.boxSize = 10;
 this.backColor = backColor
 this.forColor = forColor
 this.weight = 15 //thickness
 this.dragMode=false;
 this.value =0;
 this.max = 100;
 this.bigChange = 5;
 this.visible = true;
 this.parent =false;

 // this.textHook = new Text(this.text1,this.x ,this.y+this.ySize,this.textColor,this.font,1)
  this.topBox = new Box(this.x,this.y,this.weight,this.weight,this.backColor,2)
  this.botBox = new Box(this.x,this.y+this.ySize-this.weight,this.weight,this.weight,this.backColor,2)
  this.scrollBox = new Box(this.x,this.y+this.weight+1,this.weight,this.ySize -(this.weight *2),this.backColor,1)
  this.uptext=new Text(String.fromCharCode(47,92),this.x+3,this.y+15,this.forColor,5)
  this.downtext = new Text(String.fromCharCode(92,47),this.x+3,this.y+this.ySize,this.forColor,5)
  this.slider = new Text("=",this.x+1, this.y+(this.ySize/2),this.forColor,5)
	this.watch("visible", function(id,oldval,newval) {
			this.visible = newval;
	this.update();	
	return newval;
	});
	this.watch("value", function(id,oldval,newval) {
		if (newval <0)
			newval=0;
		if (newval > this.max)
			newval = this.max;
			
		this.value = newval;
		this.update();	
		if (this.parent){			
			this.parent.scrollIndex=this.index;
		}
	return newval;
	});
	this.watch("dragMode", function(id,oldval,newval) {					
		this.dragMode = newval;		
	return newval;
	});
	this.watch("x", function(id,oldval,newval) {					
		this.x = newval;	
		this.update();			
	return newval;
	});
	this.watch("y", function(id,oldval,newval) {					
		this.y = newval;	
		this.update();			
	return newval;
	});
	this.watch("backColor", function(id,oldval,newval) {					
		this.backColor = newval;	
		this.update();			
	return newval;
	});
	this.watch("forColor", function(id,oldval,newval) {					
		this.forColor = newval;	
		this.update();			
	return newval;
	});
	this.watch("bigChange", function(id,oldval,newval) {					
		this.bigChange = newval;	
		this.update();			
	return newval;
	});
	this.watch("max", function(id,oldval,newval) {					
		this.max = newval;	
		this.update();			
	return newval;
	});
	this.update = function(){
		this.xSize = (this.verticle) ? 15: this.length;
		this.ySize = (this.verticle) ?  this.length :15 ;
 
	//this.topBox = new Box(this.x,this.y,this.weight,this.weight,this.backColor,2)
	this.topBox.visible=this.visible;
	this.topBox.x=this.x;
	this.topBox.y=this.y;
	this.topBox.ysize =this.weight;
	this.topBox.xsize=this.weight;
	this.topBox.color=this.backColor;
	this.topBox.click = function (b, x, y) { 
// empty function that just blocks the click
	}
	//this.botBox = new Box(this.x,this.y+this.ySize-this.weight,this.weight,this.weight,this.backColor,2)	
	this.botBox.visible=this.visible;	
	this.botBox.x= (this.verticle) ? this.x : this.xSize +this.x -this.weight
	this.botBox.y= (this.verticle) ? this.y+this.ySize-this.weight : this.y ;
	this.botBox.ysize =this.weight;
	this.botBox.xsize=this.weight;
	this.botBox.color=this.backColor;
	this.botBox.click = function (b, x, y) { 
// empty function that just blocks the click
	}	
	//this.scrollBox = new Box(this.x,this.y+this.weight+1,this.weight,this.ySize -(this.weight *2),this.backColor,1)
	this.scrollBox.visible=this.visible;
	this.scrollBox.x= (this.verticle) ? this.x : this.x +this.weight
	this.scrollBox.y=(this.verticle) ? this.y+this.weight : this.y;
	this.scrollBox.ysize=(this.verticle) ? this.ySize-(this.weight *2):this.weight;
	this.scrollBox.xsize=(this.verticle) ? this.weight: this.xSize-(this.weight *2)
	this.scrollBox.color = this.backColor;
	this.scrollBox.click = function (b, x, y) { 
// empty function that just blocks the click
	}
	//this.uptext=new Text(String.fromCharCode(47,92),this.x+3,this.y+15,this.forColor,5)
	this.uptext.visible=this.visible;
	this.uptext.x=this.x+3;
	this.uptext.y=(this.verticle) ? this.y+15 : this.y+17;
	this.uptext.color =this.forColor;
	this.uptext.text = (this.verticle) ? String.fromCharCode(47,92) : "<";	
	 //this.downtext = new Text(String.fromCharCode(92,47),this.x+3,this.y+this.ySize,this.forColor,5)	
	this.downtext.visible=this.visible;
	this.downtext.x= (this.verticle) ? this.x+3: this.x+this.xSize- this.weight;
	this.downtext.y=(this.verticle) ? this.y+this.ySize: this.y+this.weight+2;
	this.color=this.forColor;
	this.downtext.text = (this.verticle) ? String.fromCharCode(92,47) : ">";
	//this.slider = new Text("=",this.x+1, this.y+(this.ySize/2),this.forColor,5)
	if (this.value <0)
		this.value=0;
	if (this.value > this.max)
		this.value = this.max;
	this.slider.visible=this.visible;
	this.slider.x= (this.verticle) ? this.x : this.x+this.weight+parseInt(this.value*this.scrollBox.xsize/this.max) -4 
	this.slider.y=(this.verticle) ? this.y+this.weight+parseInt(this.value*this.scrollBox.ysize/this.max) +8 : this.y+this.weight
	this.slider.text = (this.verticle) ? "=" : "|"
	this.color=this.forColor;
	
	}
		
	GlobalVScrollArray.push (this);
	
	
}
function SliderWText(x, y,text,maxVal,initVal,txtColor,backColor) {
	this.x=x;
	this.y=y;
	this.backColor = (backColor) ? backColor : 18;  //2 = gray
	this.textColor = (txtColor) ? txtColor :13;
	this.font=6;
	this.value =initVal;
	this.text = text;
	this.valueChanged = false;
	this.maxVal = maxVal;
	this.visible = true;
	this.infoText = new Text(this.text,this.x,this.y,this.textColor,this.font,3)
//function ScrollBar(x, y,length,backColor,forColor,initalValue) {
	this.slider= new ScrollBar( this.x,this.y,115,this.backColor,this.textColor,this.value)
	this.slider.max =this.maxVal;
	this.slider.verticle=false; this.slider.value =initVal;
	//this.valueText = new Text("boobs",this.x+120,this.y,this.textColor,this.font,1)
this.scrollIndex =this.value;
this.slider.parent = this;
this.slider.update();
this.infoText.text =this.text +": "+this.slider.value ;
this.watch("scrollIndex", function(id,oldval,newval) {		
		this.scrollIndex = this.slider.value;	
		this.value = this.slider.value;
		this.infoText.text = this.text + ": " + this.slider.value;
		  if(typeof(this.valueChanged == 'function'))
		  	this.valueChanged(this.value)
		return newval;
		});	
this.watch("visible", function(id,oldval,newval) {		
		this.visible = newval;	
		this.slider.visible=this.visible;
		this.infoText.visible=this.visible;		
		return newval;
		});
}
function TabControl(x, y,xSize,ySize,txtColor,backColor,tabArray) {
this.x=x;
this.y=y;
this.x2=xSize -x ;
this.y2=ySize-y;
this.xSize=xSize
this.ySize=ySize
this.txtColor=txtColor;
this.backColor = backColor;
this.tabChanged = false;
this.selectedText=""
this.tabArray = [];
this.topButtonArray = [];
this.visible = true;
this.tabs =[];
for (var t = 0; t < tabArray.length; t++) {
    var tt = new tabPage()
    tt.name = tabArray[t]
    tt.controls = new Array()
    this.tabs.push(tt)
}
    

this.font =5;
this.selectedTab=0

this.visible = true; 
this.box = new Box(this.x,this.y,this.x2,this.y2,this.backColor)
//this.sepLine = new Line(this.x,this.y+15,this.x2 +this.x,this.y+15,this.txtColor,1)

var sp =0;
    for (var j = 0; j < this.tabs.length; j++) {       
        var a = new TextBox(this.x + sp, this.y, myGetTextSize(this.tabs[j].name, this.font)[0] + 10, myGetTextSize(this.tabs[j].name, this.font)[1], tabArray[j], this.txtColor, this.backColor)
        sp = sp  + myGetTextSize(this.tabs[j].name, this.font)[0]+10;
        a.selectedColor = this.txtColor;
        a.font=this.font
        a.parent = this
        a.clickFunction=this.tabClick
        this.topButtonArray.push(a)
        //this.tabArray.push(a)
      
    }
       this.selectedTab = 1
	this.selectedText=this.tabs[0].name
    this.topButtonArray[0].backColor = 5
    this.watch("visible", function (id, oldval, newval) {

	for (var k = 0; k < this.tabs[this.selectedTab-1].controls.length; k++)
		this.tabs[this.selectedTab-1].controls[k].visible = newval
    	this.box.visible = newval
		for (var j =0;j<this.topButtonArray.length; j++)
			this.topButtonArray[j].visible = newval
	
		return newval
    });
    this.watch("selectedTab", function (id, oldval, newval) {
        this.topButtonArray[oldval - 1].backColor = this.backColor
        for (var k = 0; k < this.tabs[oldval-1].controls.length; k++)
            this.tabs[oldval - 1].controls[k].visible = false
        
        this.topButtonArray[newval -1].backColor = 5
        for (var kk = 0; k < this.tabs[newval-1 ].controls.length; kk++)
            this.tabs[newval].controls[kk].visible = true

                    
        return newval;
    });
}
TabControl.prototype.tabClick = function (sender) {
    for (var j = 0; j < sender.parent.topButtonArray.length; j++) {
    	if (sender.text == sender.parent.topButtonArray[j].text) {
    		if (typeof (sender.parent.tabChanged) == 'function')
				// tabChanged(oldtext,newtext,oldtab#, newtab#)
    			sender.parent.tabChanged(sender.parent.tabs[sender.parent.selectedTab-1].name,sender.text,sender.parent.selectedTab,j)
    		sender.parent.selectedTab = j + 1;
			sender.parent.selectedText=sender.text
           
		}
	}
}

function tabPage (){}
tabPage.prototype.name = ""
tabPage.controls = new Array();

function DropDownBox(x, y, text, txtColor, backColor, textList, heading) {
	this.valueChanged = false;
this.x=x;
this.head=(heading)?heading :"testing ";
this.y=y;
this.text=text;
this.textColor=txtColor;
this.backColor=this.backColor;
this.font = 6;
this.visible = true;;
this.textList=(textList)? textList :[];
this.droped = false;
this.autoWide =myGetTextSize(text,this.font)[0]+5;

for (var j = 0; j < this.textList.length; j++){
	if (myGetTextSize(this.textList[j],this.font)[0]+5 > this.autoWide)
		this.autoWide = myGetTextSize(this.textList[j],this.font)[0]+5;
}
this.headText =new Text(this.head,this.x,this.y-5,this.textColor,this.font,3)
this.dropDown = new TextBox(this.x+this.autoWide,this.y,11,11,"V")

this.dropDown.parent = this;
	this.dropDown.clickFunction  = function(){
		this.parent.droped = !this.parent.droped
		this.parent.update();
	}

this.topText = new TextBox(this.x,this.y,this.autoWide,myGetTextSize(text,this.font)[1],this.text,this.textColor,this.backColor)
	
this.listbox = new TextBox(this.x,this.y+myGetTextSize(text,this.font)[1],this.autoWide,100," ");




this.listbox.visible=false;
this.listbox.listboxMode = true;
this.listbox.selectedRow = 0; this.listbox.selectedText=this.listbox.textLines[this.listbox.selectedRow];
this.listbox.textLines =this.textList;
this.listbox.parent = this;

this.watch("visible", function(id,oldval,newval) {		
		this.visible = newval;	
		this.update();
		return newval;
		});		


GlobalDropDownArray.push(this)
}
DropDownBox.prototype.childUpate = function () {	
		if (this.topText.text != this.listbox.textLines[this.listbox.selectedRow+this.listbox.scrollbar.value]){
			this.topText.text =this.listbox.textLines[this.listbox.selectedRow+this.listbox.scrollbar.value];
			this.listbox.visible = false;
			if (typeof(this.valueChanged) == 'function')
				this.valueChanged(this.topText.text)
		}
}
DropDownBox.prototype.update = function () {
	this.headText.visible= this.visible;
	this.topText.visible = this.visible;
	this.dropDown.visible = this.visible;
	this.listbox.visible = (this.visible && this.droped);

}
function LinkedListBoxes (x,y,xsize,ysize,fullList,partialList,mode){
this.x=x
this.y=y
this.xsize =xsize
this.ysize = ysize
this.startFullList = []
for (var a = 0; a < fullList.length; a++)
	this.startFullList.push(fullList[a])
this.fullList = fullList
this.partialList = partialList
this.visible = true;
this.valueChanged =false;
this.mode=mode // 2 for add sub 4 for add subb and sort.
	this.txtFull = new TextBox(this.x, this.y, this.xsize,this.ysize, " "); 
	this.txtValue = new TextBox(this.x+this.xsize+50, this.y, this.xsize, this.ysize, " "); 
	this.txtValue.parent=this;
	this.cmdAdd = new TextBox(this.x +this.xsize +20, this.y + 5, 20, 20, ">"); 
	this.cmdSub = new TextBox(this.x +this.xsize +20, this.y + 25, 20, 20, "<"); 
	if (this.mode ==4){
		this.cmdDown = new TextBox(this.x +this.xsize +20, this.y + 50, 20, 20, "V"); 	 
		this.cmdUp = new TextBox(this.x +this.xsize +20, this.y + 75, 20, 20, String.fromCharCode(47, 92)); 
	 }   
	this.txtFull.listboxMode = true;
	this.txtFull.textLines = this.fullList; this.txtFull.selectedRow = 0; this.txtFull.selectedText = this.txtFull.textLines[this.txtFull.selectedRow];
	this.removeDuplicats()
	this.txtValue.listboxMode = true;
	this.txtValue.textLines = this.partialList; this.txtValue.selectedRow = 0; this.txtValue.selectedText = this.txtValue.textLines[this.txtValue.selectedRow];

	this.cmdAdd.selectedColor = this.cmdAdd.textColor; this.cmdAdd.font = 5; this.cmdAdd.selectedRow = this.cmdAdd.textLines[0];
	this.cmdAdd.parent = this.txtValue; this.cmdAdd.child = this.txtFull;
	this.cmdAdd.clickFunction = function () {
		if (this.child.selectedText) {
			this.parent.parent.partialList.push(this.child.selectedText)
			this.parent.textLines = this.parent.parent.partialList;
			this.child.deleteSelectedRow()
			if (typeof(this.parent.parent.valueChanged) == 'function')
				this.parent.parent.valueChanged(this.parent.parent.partialList)

		}
	};
	this.cmdSub.selectedColor = this.cmdSub.textColor; this.cmdSub.font = 5; this.cmdSub.selectedRow = this.cmdSub.textLines[0];
	this.cmdSub.parent = this.txtValue; this.cmdSub.child = this.txtFull;
	this.cmdSub.clickFunction = function () {
		if (this.parent.selectedText) {
			this.parent.deleteSelectedRow()
		}	
		for (var j = 0; j < this.parent.parent.partialList.length; j++) {
			for (var k = 0; k < this.parent.parent.fullList.length; k++) {
				if (this.parent.parent.partialList[j] == this.parent.parent.fullList[k])
					this.parent.parent.fullList.remove(k)
					
			}
		}

		this.child.textLines = this.parent.parent.fullList;
		this.parent.parent.removeDuplicats()
		if (typeof(this.parent.parent.valueChanged) == 'function')
				this.parent.parent.valueChanged(this.parent.parent.partialList)
	};
	if (this.mode ==4){
		this.cmdDown.selectedColor = this.cmdDown.textColor; this.cmdDown.font = 5;
		this.cmdDown.parent = this.txtValue;
		this.cmdDown.clickFunction = function () {	this.parent.moveSelectedRow(1);	};

		this.cmdUp.selectedColor = this.cmdUp.textColor; this.cmdUp.font = 5;
		this.cmdUp.parent = this.txtValue;
		this.cmdUp.clickFunction = function () { this.parent.moveSelectedRow(-1);};
	}
	
	this.watch("visible", function(id,oldval,newval) {		
		this.visible = newval;	
		this.txtFull.visible = newval;
		this.txtValue.visible = newval;
		this.cmdAdd.visible = newval;
		this.cmdSub.visible = newval;
		if (this.mode ==4){
			this.cmdDown.visible = newval;
			this.cmdUp.visible = newval;
		}   
		return newval;
	});
	
}
LinkedListBoxes.prototype.removeDuplicats = function () {
	var tempList = []
	var found = false
	for (var a = 0; a < this.startFullList.length; a++) {
	found = false;
		for (var j = 0; j < this.partialList.length; j++)
			if (this.startFullList[a] == this.partialList[j])
				found = true
		if (!found)
		tempList.push(this.startFullList[a])
	}
	
	this.txtFull.textLines = tempList;
	this.txtFull.lineArrayToHooks()
	this.txtFull.redrawHooks();
	
}