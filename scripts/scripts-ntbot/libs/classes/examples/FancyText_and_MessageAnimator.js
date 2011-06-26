// Could use to make a custom animation and play it
include('objects/MessageAnimator.js');
include('classes/FancyMessage.jsclass');

function playAnimation()
{
	var helloMessage = new FancyMessage('!', 'Hi There!', 30, ' ', 150);
	var lolMessage = new FancyMessage('!', 'hehe', 30, ' ', 150);
	var buyMessage = new FancyMessage('!', 'Want to trade?', 30, ' ', 200);
	var goodbyeMessage = new FancyMessage('!', 'Goodbye!', 30, ' ', 150);
	
	MessageAnimator.Appear.Right(helloMessage, 1);
	MessageAnimator.Scroll.RightToLeft(helloMessage, 1);
	MessageAnimator.Flash.Center(buyMessage, 10);
	MessageAnimator.Bounce.CenterToLeft(lolMessage, 1);
	MessageAnimator.Scroll.CenterToRight(goodbyeMessage, 1);
	MessageAnimator.Wrap.RightToLeft(goodbyeMessage, 1);
	MessageAnimator.Disappear.Left(goodbyeMessage, 1);
}

