/**
*
*	MessageAnimator
*
*		.Appear
*					.Right (FancyMessage fmMessage, Number nRepititions)
*					.Left (FancyMessage fmMessage, Number nRepititions)
*		
*		.Bounce
*					.RightToLeft (FancyMessage fmMessage, Number nRepititions)
*					.RightToCenter (FancyMessage fmMessage, Number nRepititions)
*					.CenterToRight (FancyMessage fmMessage, Number nRepititions)
*					.LeftToRight (FancyMessage fmMessage, Number nRepititions)
*					.LeftToCenter (FancyMessage fmMessage, Number nRepititions)
*					.CenterToLeft (FancyMessage fmMessage, Number nRepititions)
*
*		.Disappear
*					.Right (FancyMessage fmMessage, Number nRepititions)
*					.Left (FancyMessage fmMessage, Number nRepititions)
*		
*		.Flash
*					.Right (FancyMessage fmMessage, Number nRepititions)
*					.Center (FancyMessage fmMessage, Number nRepititions)
*					.Left (FancyMessage fmMessage, Number nRepititions)
*			
*		.Scroll
*					.RightToLeft (FancyMessage fmMessage, Number nRepititions)
*					.RightToCenter (FancyMessage fmMessage, Number nRepititions)
*					.CenterToRight (FancyMessage fmMessage, Number nRepititions)
*					.LeftToRight (FancyMessage fmMessage, Number nRepititions)
*					.LeftToCenter (FancyMessage fmMessage, Number nRepititions)
*					.CenterToLeft (FancyMessage fmMessage, Number nRepititions)
*
*		.Wrap
*					.RightToLeft (FancyMessage fmMessage, Number nRepititions)
*					.LeftToRight (FancyMessage fmMessage, Number nRepititions)
*
*/

// MessageAnimator object has inner objects and methods for animating a FancyMessage() object that you can create
var MessageAnimator = new function()
{
	// Validate arguments for all MessageAnimator method calls
	this.validFancyMessage = 
			function(fmMessage)
			{
				if((fmMessage == undefined) || (fmMessage.constructor.name != 'FancyMessage'))
				{
					print('MessageAnimator.'+ arguments.callee.caller.name.replace("_", ".", "g") +'() Error: fmMessage must be a FancyMessage Object');
					return false;
				}
				
				return true;
			};
			
			
	// Appear/Fade-in Text Effects
	this.Appear = new function()
	{
	
			this.Right = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var a = 0; a < nRepetitions; a++)
						{
							var sOutput = '';
							
							for(var x = 0; x <= fmMessage.sMessage.length; x++)
							{
								sOutput = fmMessage.sPrefix;
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nTotalLength - x);
								sOutput += fmMessage.sMessage.substr(0, x);
								
								say(sOutput);
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
			
			
			this.Left = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var a = 0; a < nRepetitions; a++)
						{
							var sOutput = '';
							
							for(var x = 0; x <= fmMessage.sMessage.length; x++)
							{
								sOutput = fmMessage.sPrefix;
								sOutput += (x == 0) ? '' : fmMessage.sMessage.substr(-x);
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nTotalLength - x);
								
								say(sOutput);
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
					
	};
	
	
	
	// Bounce-type Text Effects
	this.Bounce = new function()
	{
	
			this.RightToLeft = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var x = 0; x < nRepetitions; x++)
						{
							MessageAnimator.Scroll.RightToLeft(fmMessage, 1);
							MessageAnimator.Scroll.LeftToRight(fmMessage, 1);
						}
						
						return true;
					};
			
			
			this.RightToCenter = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var x = 0; x < nRepetitions; x++)
						{
							MessageAnimator.Scroll.RightToCenter(fmMessage, 1);
							MessageAnimator.Scroll.CenterToRight(fmMessage, 1);
						}
						
						return true;
					};
			
			
			this.CenterToRight = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var x = 0; x < nRepetitions; x++)
						{
							MessageAnimator.Scroll.CenterToRight(fmMessage, 1);
							MessageAnimator.Scroll.RightToCenter(fmMessage, 1);
						}
						
						return true;
					};
			
			
			this.LeftToRight = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var x = 0; x < nRepetitions; x++)
						{
							MessageAnimator.Scroll.LeftToRight(fmMessage, 1);
							MessageAnimator.Scroll.RightToLeft(fmMessage, 1);
						}
						
						return true;
					};
					
					
			this.LeftToCenter = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var x = 0; x < nRepetitions; x++)
						{
							MessageAnimator.Scroll.LeftToCenter(fmMessage, 1);
							MessageAnimator.Scroll.CenterToLeft(fmMessage, 1);
						}
						
						return true;
					};
			
			
			this.CenterToLeft = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var x = 0; x < nRepetitions; x++)
						{
							MessageAnimator.Scroll.CenterToLeft(fmMessage, 1);
							MessageAnimator.Scroll.LeftToCenter(fmMessage, 1);
						}
						
						return true;
					};
	
	};
	
	
	
	// Disappear/Fade-out Text Effects
	this.Disappear = new function()
	{
	
			this.Right = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var a = 0; a < nRepetitions; a++)
						{
							var sOutput = '';
							
							for(var x = 0; x <= fmMessage.sMessage.length; x++)
							{
								sOutput = fmMessage.sPrefix;
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nTotalLength - fmMessage.sMessage.length + x);
								sOutput += fmMessage.sMessage.substr(0, fmMessage.sMessage.length - x);
								
								say(sOutput);
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
			
			
			this.Left = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var a = 0; a < nRepetitions; a++)
						{
							var sOutput = '';
							
							for(var x = 0; x <= fmMessage.sMessage.length; x++)
							{
								sOutput = fmMessage.sPrefix;
								sOutput += fmMessage.sMessage.substr(x, fmMessage.sMessage.length);
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nTotalLength - fmMessage.sMessage.length + x);
								
								say(sOutput);
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
					
	};
	
	
	
	// Flash Text Effects
	this.Flash = new function()
	{
	
			this.Right = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						var sOutput = fmMessage.sPrefix + fmMessage.sSpacer.repeat(fmMessage.nTotalLength - fmMessage.sMessage.length) + fmMessage.sMessage;
						var sSpacerOutput = fmMessage.sPrefix + fmMessage.sSpacer.repeat(fmMessage.nTotalLength);
						
						for(var x = 0; x < nRepetitions; x++)
						{
							say(sOutput);
							delay(fmMessage.nDelay);
							say(sSpacerOutput);
							delay(fmMessage.nDelay);
						}
						
						return true;
					};
			
			
			this.Center = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						var nLeftspaces = fmMessage.sSpacer.repeat(fmMessage.nCenterIndex);
						
						var sOutput = fmMessage.sPrefix;
							sOutput += nLeftspaces;
							sOutput += fmMessage.sMessage;
							sOutput += fmMessage.sSpacer.repeat(fmMessage.nTotalLength - nLeftspaces.length - fmMessage.sMessage.length);
							
						var sSpacerOutput = fmMessage.sPrefix + fmMessage.sSpacer.repeat(fmMessage.nTotalLength);
						
						for(var x = 0; x < nRepetitions; x++)
						{
							say(sOutput);
							delay(fmMessage.nDelay);
							say(sSpacerOutput);
							delay(fmMessage.nDelay);
						}
						
						return true;
					};
			
			
			this.Left = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						var sOutput = fmMessage.sPrefix;
							sOutput += fmMessage.sMessage;
							sOutput += fmMessage.sSpacer.repeat(fmMessage.nTotalLength - fmMessage.sMessage.length);
							
						var sSpacerOutput = fmMessage.sPrefix + fmMessage.sSpacer.repeat(fmMessage.nTotalLength);
						
						for(var x = 0; x < nRepetitions; x++)
						{
							say(sOutput);
							delay(fmMessage.nDelay);
							say(sSpacerOutput);
							delay(fmMessage.nDelay);
						}
						
						return true;
					};
					
	};
	
	
	
	// Scroll-type Text Effects
	this.Scroll = new function()
	{
	
			this.RightToLeft = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var x = 0; x < nRepetitions; x++)
						{	
							for(var nLeft = (fmMessage.nTotalLength - fmMessage.sMessage.length), nRight = 0; nLeft >= 0; nLeft--, nRight++)
							{
								say(fmMessage.sPrefix + (fmMessage.sSpacer.repeat(nLeft)) + fmMessage.sMessage + (fmMessage.sSpacer.repeat(nRight)));
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
			
			
			this.RightToCenter = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var a = 0; a < nRepetitions; a++)
						{	
							var sOutput = '';
							var nLen = fmMessage.nRightIndex - fmMessage.nCenterIndex + 2;

							for(var x = 0; x < nLen; x++)
							{
								sOutput = fmMessage.sPrefix;
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nRightIndex - x + 1);
								sOutput += fmMessage.sMessage;
								sOutput += fmMessage.sSpacer.repeat(x);
								
								say(sOutput);
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
			
			
			this.CenterToRight = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var a = 0; a < nRepetitions; a++)
						{	
							var sOutput = '';
							var nLen = fmMessage.nRightIndex - fmMessage.nCenterIndex + 2;

							for(var x = 0; x < nLen; x++)
							{
								sOutput = fmMessage.sPrefix;
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nCenterIndex + x);
								sOutput += fmMessage.sMessage;
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nCenterIndex - x);
								
								say(sOutput);
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
			
			
			this.LeftToRight = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var x = 0; x < nRepetitions; x++)
						{	
							for(var nLeft = 0, nRight = (fmMessage.nTotalLength - fmMessage.sMessage.length); nRight >= 0; nLeft++, nRight--)
							{
								say((fmMessage.sSpacer.repeat(nLeft)) + fmMessage.sMessage + (fmMessage.sSpacer.repeat(nRight)));
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
			
			
			this.LeftToCenter = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var a = 0; a < nRepetitions; a++)
						{	
							var sOutput = '';
							var nLen = fmMessage.nCenterIndex - fmMessage.nLeftIndex;

							for(var x = 0; x <= nLen; x++)
							{
								sOutput = fmMessage.sPrefix;
								sOutput += fmMessage.sSpacer.repeat(x);
								sOutput += fmMessage.sMessage;
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nTotalLength - fmMessage.sMessage.length - x);
								
								say(sOutput);
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
					
					
			this.CenterToLeft = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var a = 0; a < nRepetitions; a++)
						{	
							var sOutput = '';
							var nLen = fmMessage.nCenterIndex - fmMessage.nLeftIndex;

							for(var x = 0; x <= nLen; x++)
							{
								sOutput = fmMessage.sPrefix;
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nCenterIndex - x)
								sOutput += fmMessage.sMessage;
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nCenterIndex + x);
								
								say(sOutput);
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
					
	};
	
	
	
	// Wrap-type Text Effects
	this.Wrap = new function()
	{
	
			this.RightToLeft = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var a = 0; a < nRepetitions; a++)
						{
							var sOutput = '';
							
							for(var x = 0; x <= fmMessage.sMessage.length; x++)
							{
								sOutput = fmMessage.sPrefix;
								sOutput += (x == 0) ? '' : fmMessage.sMessage.substr(-x);
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nTotalLength - fmMessage.sMessage.length);
								sOutput += fmMessage.sMessage.substr(0, (fmMessage.sMessage.length - x));
								
								say(sOutput);
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
			
			
			this.LeftToRight = 
					function(fmMessage, nRepetitions)
					{
						if(!MessageAnimator.validFancyMessage.apply(this, arguments)) { return false; };
						
						for(var a = 0; a < nRepetitions; a++)
						{
							var sOutput = '';
							
							for(var x = 0; x <= fmMessage.sMessage.length; x++)
							{
								sOutput = fmMessage.sPrefix;
								sOutput += fmMessage.sMessage.substr(x);
								sOutput += fmMessage.sSpacer.repeat(fmMessage.nTotalLength - fmMessage.sMessage.length);
								sOutput += fmMessage.sMessage.substr(0, x);
								
								say(sOutput);
								delay(fmMessage.nDelay);
							}
						}
						
						return true;
					};
					
	};
	
};
