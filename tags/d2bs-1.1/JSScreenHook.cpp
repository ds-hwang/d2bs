#include "JSScreenHook.h"
#include "ScreenHook.h"
#include "Script.h"
#include "File.h"
#include "CDebug.h"

#include "debugnew/debug_new.h"

using namespace std;

void hook_finalize(JSContext *cx, JSObject *obj) {
	CDebug cDbg("hook finalize");
	Genhook* hook = (Genhook*)JS_GetPrivate(cx, obj);

	if(hook)
	{
		JS_SetPrivate(cx, obj, NULL);
		delete hook;
	}
}

JSAPI_FUNC(hook_remove) {
	CDebug cDbg("hook remove");

	Genhook* hook = (Genhook*)JS_GetPrivate(cx, obj);
	if(hook)
	{
		hook->SetIsVisible(false);
		delete hook;
	}

	JS_SetPrivate(cx, obj, NULL);
	JS_ClearScope(cx, obj);
	JS_ValueToObject(cx, JSVAL_VOID, &obj);

	return JS_TRUE;
}

// Function to create a frame which gets called on a "new Frame ()"
// Parameters: x, y, xsize, ysize, alignment, automap, onClick, onHover
JSAPI_FUNC(frame_ctor) {
	CDebug cDbg("frame_ctor");

	Script* script = (Script*)JS_GetContextPrivate(cx);

	uint x = 0, y = 0, x2 = 0, y2 = 0;
	Align align = Left;
	bool automap = false;
	jsval click = JSVAL_VOID, hover = JSVAL_VOID;

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		x = JSVAL_TO_INT(argv[0]);
	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		y = JSVAL_TO_INT(argv[1]);
	if(argc > 2 && JSVAL_IS_INT(argv[2]))
		x2 = JSVAL_TO_INT(argv[2]);
	if(argc > 3 && JSVAL_IS_INT(argv[3]))
		y2 = JSVAL_TO_INT(argv[3]);
	if(argc > 4 && JSVAL_IS_INT(argv[4]))
		align = (Align)JSVAL_TO_INT(argv[4]);
	if(argc > 5 && JSVAL_IS_BOOLEAN(argv[5]))
		automap = !!JSVAL_TO_BOOLEAN(argv[5]);
	if(argc > 6 && JSVAL_IS_FUNCTION(cx, argv[6]))
		click = argv[6];
	if(argc > 7 && JSVAL_IS_FUNCTION(cx, argv[7]))
		hover = argv[7];

	// framehooks don't work out of game -- they just crash
	FrameHook* pFramehook = new FrameHook(script, x, y, x2, y2, automap, align, IG);

	if (!pFramehook)
		THROW_ERROR(cx, obj, "Failed to create framehook");

	pFramehook->SetClickHandler(click);
	pFramehook->SetHoverHandler(hover);

	JSObject* hook = BuildObject(cx, &frame_class, frame_methods, frame_props, pFramehook);
	if(!hook)
		THROW_ERROR(cx, obj, "Failed to create frame object");

	*rval = OBJECT_TO_JSVAL(hook);

	return JS_TRUE;
}

JSAPI_PROP(frame_getProperty) {
	CDebug cDbg("frame_getProperty");

	FrameHook* pFramehook = (FrameHook*)JS_GetPrivate(cx, obj);
	if(!pFramehook)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id)) {
		case FRAME_X:
			*vp = INT_TO_JSVAL(pFramehook->GetX());
			break;
		case FRAME_Y:
			*vp = INT_TO_JSVAL(pFramehook->GetY());
			break;
		case FRAME_XSIZE:
			*vp = INT_TO_JSVAL(pFramehook->GetXSize());
			break;
		case FRAME_YSIZE:
			*vp = INT_TO_JSVAL(pFramehook->GetYSize());
			break;
		case FRAME_ALIGN:
			*vp = INT_TO_JSVAL(pFramehook->GetAlign());
			break;
		case FRAME_VISIBLE:
			*vp = BOOLEAN_TO_JSVAL(pFramehook->GetIsVisible());
			break;
		case FRAME_ZORDER:
			*vp = INT_TO_JSVAL(pFramehook->GetZOrder());
			break;
		case FRAME_ONCLICK:
			*vp = pFramehook->GetClickHandler();
			break;
		case FRAME_ONHOVER:
			*vp = pFramehook->GetHoverHandler();
			break;
	}
	return JS_TRUE;
}
JSAPI_PROP(frame_setProperty) {
	CDebug cDbg("frame_setProperty");

	FrameHook* pFramehook = (FrameHook*)JS_GetPrivate(cx, obj);
	if(!pFramehook)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id)) {
		case FRAME_X:
			if(JSVAL_IS_INT(*vp))
				pFramehook->SetX(JSVAL_TO_INT(*vp));
			break;
		case FRAME_Y:
			if(JSVAL_IS_INT(*vp))
				pFramehook->SetY(JSVAL_TO_INT(*vp));
			break;
		case FRAME_XSIZE:
			if(JSVAL_IS_INT(*vp))
				pFramehook->SetXSize(JSVAL_TO_INT(*vp));
			break;
		case FRAME_YSIZE:
			if(JSVAL_IS_INT(*vp))
				pFramehook->SetYSize(JSVAL_TO_INT(*vp));
			break;
		case FRAME_ALIGN:
			if(JSVAL_IS_INT(*vp))
				pFramehook->SetAlign((Align)JSVAL_TO_INT(*vp));
			break;
		case FRAME_VISIBLE:
			if(JSVAL_IS_BOOLEAN(*vp))
				pFramehook->SetIsVisible(!!JSVAL_TO_BOOLEAN(*vp));
			break;
		case FRAME_ZORDER:
			if(JSVAL_IS_INT(*vp))
				pFramehook->SetZOrder((ushort)JSVAL_TO_INT(*vp));
			break;
		case FRAME_ONCLICK:
			pFramehook->SetClickHandler(*vp);
			break;
		case FRAME_ONHOVER:
			pFramehook->SetHoverHandler(*vp);
			break;
	}
	return JS_TRUE;
}

//Box functions

//Parameters: x, y, xsize, ysize, color, opacity, alignment, automap, onClick, onHover
JSAPI_FUNC(box_ctor) {
	CDebug cDbg("box_ctor");

	Script* script = (Script*)JS_GetContextPrivate(cx);

	ScreenhookState state = (script->GetState () == OutOfGame) ? OOG : IG;
	uint x = 0, y = 0, x2 = 0, y2 = 0;
	ushort color = 0, opacity = 0;
	Align align = Left;
	bool automap = false;
	jsval click = JSVAL_VOID, hover = JSVAL_VOID;

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		x = JSVAL_TO_INT(argv[0]);
	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		y = JSVAL_TO_INT(argv[1]);
	if(argc > 2 && JSVAL_IS_INT(argv[2]))
		x2 = JSVAL_TO_INT(argv[2]);
	if(argc > 3 && JSVAL_IS_INT(argv[3]))
		y2 = JSVAL_TO_INT(argv[3]);
	if(argc > 4 && JSVAL_IS_INT(argv[4]))
		color = (ushort)JSVAL_TO_INT(argv[4]);
	if(argc > 5 && JSVAL_IS_INT(argv[5]))
		opacity = (ushort)JSVAL_TO_INT(argv[5]);
	if(argc > 6 && JSVAL_IS_INT(argv[6]))
		align = (Align)JSVAL_TO_INT(argv[6]);
	if(argc > 7 && JSVAL_IS_BOOLEAN(argv[7]))
		automap = !!JSVAL_TO_BOOLEAN(argv[7]);
	if(argc > 8 && JSVAL_IS_FUNCTION(cx, argv[8]))
		click = argv[8];
	if(argc > 9 && JSVAL_IS_FUNCTION(cx, argv[9]))
		hover = argv[9];

	BoxHook* pBoxHook = new BoxHook(script, x, y, x2, y2, color, opacity, automap, align, state);

	if (!pBoxHook)
		THROW_ERROR(cx, obj, "Unable to initalize a box class.");

	pBoxHook->SetClickHandler(click);
	pBoxHook->SetHoverHandler(hover);

	JSObject* hook = BuildObject(cx, &box_class, box_methods, box_props, pBoxHook);
	if(!hook)
		THROW_ERROR(cx, obj, "Failed to create box object");

	*rval = OBJECT_TO_JSVAL(hook);

	return JS_TRUE;
}
JSAPI_PROP(box_getProperty) {
	CDebug cDbg("box_getProperty");

	BoxHook* pBoxHook = (BoxHook*)JS_GetPrivate(cx, obj);
	if(!pBoxHook)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id)) {
		case BOX_X:
			*vp = INT_TO_JSVAL(pBoxHook->GetX());
			break;
		case BOX_Y:
			*vp = INT_TO_JSVAL(pBoxHook->GetY());
			break;
		case BOX_XSIZE:
			*vp = INT_TO_JSVAL(pBoxHook->GetXSize());
			break;
		case BOX_YSIZE:
			*vp = INT_TO_JSVAL(pBoxHook->GetYSize());
			break;
		case BOX_ALIGN:
			*vp = INT_TO_JSVAL(pBoxHook->GetAlign());
			break;
		case BOX_COLOR:
			*vp = INT_TO_JSVAL(pBoxHook->GetColor());
			break;
		case BOX_OPACITY:
			*vp = INT_TO_JSVAL(pBoxHook->GetOpacity());
			break;
		case BOX_VISIBLE:
			*vp = BOOLEAN_TO_JSVAL(pBoxHook->GetIsVisible());
			break;
		case BOX_ZORDER:
			*vp = INT_TO_JSVAL(pBoxHook->GetZOrder());
			break;
		case BOX_ONCLICK:
			*vp = pBoxHook->GetClickHandler();
			break;
		case BOX_ONHOVER:
			*vp = pBoxHook->GetHoverHandler();
			break;
	}
	return JS_TRUE;
}
JSAPI_PROP(box_setProperty) {
	CDebug cDbg("box_setProperty");

	BoxHook* pBoxHook = (BoxHook*)JS_GetPrivate(cx, obj);
	if(!pBoxHook)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id)) {
		case BOX_X:
			if(JSVAL_IS_INT(*vp))
				pBoxHook->SetX(JSVAL_TO_INT(*vp));
			break;
		case BOX_Y:
			if(JSVAL_IS_INT(*vp))
				pBoxHook->SetY(JSVAL_TO_INT(*vp));
			break;
		case BOX_XSIZE:
			if(JSVAL_IS_INT(*vp))
				pBoxHook->SetXSize(JSVAL_TO_INT(*vp));
			break;
		case BOX_YSIZE:
			if(JSVAL_IS_INT(*vp))
				pBoxHook->SetYSize(JSVAL_TO_INT(*vp));
			break;
		case BOX_OPACITY:
			if(JSVAL_IS_INT(*vp))
				pBoxHook->SetOpacity((ushort)JSVAL_TO_INT(*vp));
			break;
		case BOX_COLOR:
			if(JSVAL_IS_INT(*vp))
				pBoxHook->SetColor((ushort)JSVAL_TO_INT(*vp));
			break;
		case BOX_ALIGN:
			if(JSVAL_IS_INT(*vp))
				pBoxHook->SetAlign((Align)JSVAL_TO_INT(*vp));
			break;
		case BOX_VISIBLE:
			if(JSVAL_IS_BOOLEAN(*vp))
				pBoxHook->SetIsVisible(!!JSVAL_TO_BOOLEAN(*vp));
			break;
		case BOX_ZORDER:
			if(JSVAL_IS_INT(*vp))
				pBoxHook->SetZOrder((ushort)JSVAL_TO_INT(*vp));
			break;
		case BOX_ONCLICK:
			pBoxHook->SetClickHandler(*vp);
			break;
		case BOX_ONHOVER:
			pBoxHook->SetHoverHandler(*vp);
			break;
	}
	return JS_TRUE;
}


//Line functions

// Parameters: x, y, x2, y2, color, automap, click, hover
JSAPI_FUNC(line_ctor) {
	CDebug cDbg("line_ctor");

	Script* script = (Script*)JS_GetContextPrivate(cx);

	ScreenhookState state = (script->GetState () == OutOfGame) ? OOG : IG;
	int x = 0, y = 0, x2 = 0, y2 = 0;
	ushort color = 0;
	bool automap = false;
	jsval click = JSVAL_VOID, hover = JSVAL_VOID;

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		x = JSVAL_TO_INT(argv[0]);
	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		y = JSVAL_TO_INT(argv[1]);
	if(argc > 2 && JSVAL_IS_INT(argv[2]))
		x2 = JSVAL_TO_INT(argv[2]);
	if(argc > 3 && JSVAL_IS_INT(argv[3]))
		y2 = JSVAL_TO_INT(argv[3]);
	if(argc > 4 && JSVAL_IS_INT(argv[4]))
		color = (ushort)JSVAL_TO_INT(argv[4]);
	if(argc > 5 && JSVAL_IS_BOOLEAN(argv[5]))
		automap = !!JSVAL_TO_BOOLEAN(argv[5]);
	if(argc > 6 && JSVAL_IS_FUNCTION(cx, argv[6]))
		click = argv[6];
	if(argc > 7 && JSVAL_IS_FUNCTION(cx, argv[7]))
		hover = argv[7];

	LineHook* pLineHook = new LineHook(script, x, y, x2, y2, color, automap, Left, state);

	if (!pLineHook)
		THROW_ERROR(cx, obj, "Unable to initalize a line class.");

	pLineHook->SetClickHandler(click);
	pLineHook->SetHoverHandler(hover);

	JSObject* hook = BuildObject(cx, &line_class, line_methods, line_props, pLineHook);
	if(!hook)
		THROW_ERROR(cx, obj, "Failed to create line object");

	*rval = OBJECT_TO_JSVAL(hook);

	return JS_TRUE;
}


JSAPI_PROP(line_getProperty) {
	CDebug cDbg("line_getProperty");

	LineHook* pLineHook = (LineHook*)JS_GetPrivate(cx, obj);
	if(!pLineHook)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id)) {
		case LINE_X:
			*vp = INT_TO_JSVAL(pLineHook->GetX());
			break;
		case LINE_Y:
			*vp = INT_TO_JSVAL(pLineHook->GetY());
			break;
		case LINE_XSIZE:
			*vp = INT_TO_JSVAL(pLineHook->GetX2());
			break;
		case LINE_YSIZE:
			*vp = INT_TO_JSVAL(pLineHook->GetY2());
			break;
		case LINE_COLOR:
			*vp = INT_TO_JSVAL(pLineHook->GetColor());
			break;
		case LINE_VISIBLE:
			*vp = BOOLEAN_TO_JSVAL(pLineHook->GetIsVisible());
			break;
		case LINE_ZORDER:
			*vp = INT_TO_JSVAL(pLineHook->GetZOrder());
			break;
		case LINE_ONCLICK:
			*vp = pLineHook->GetClickHandler();
			break;
		case LINE_ONHOVER:
			*vp = pLineHook->GetHoverHandler();
			break;
	}
	return JS_TRUE;
}

JSAPI_PROP(line_setProperty) {
	CDebug cDbg("line_setProperty");

	LineHook* pLineHook = (LineHook*)JS_GetPrivate(cx, obj);
	if(!pLineHook)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id)) {
		case LINE_X:
			if (JSVAL_IS_INT(*vp))
				pLineHook->SetX(JSVAL_TO_INT(*vp));
			break;
		case LINE_Y:
			if (JSVAL_IS_INT(*vp))
				pLineHook->SetY(JSVAL_TO_INT(*vp));
			break;
		case LINE_XSIZE:
			if (JSVAL_IS_INT(*vp))
				pLineHook->SetX2(JSVAL_TO_INT(*vp));
			break;
		case LINE_YSIZE:
			if (JSVAL_IS_INT(*vp))
				pLineHook->SetY2(JSVAL_TO_INT(*vp));
			break;
		case LINE_COLOR:
			if (JSVAL_IS_INT(*vp))
				pLineHook->SetColor((ushort)JSVAL_TO_INT(*vp));
			break;
		case LINE_VISIBLE:
			if (JSVAL_IS_BOOLEAN(*vp))
				pLineHook->SetIsVisible(!!JSVAL_TO_BOOLEAN(*vp));
			break;
		case LINE_ZORDER:
			if(JSVAL_IS_INT(*vp))
				pLineHook->SetZOrder((ushort)JSVAL_TO_INT(*vp));
			break;
		case LINE_ONCLICK:
			pLineHook->SetClickHandler(*vp);
			break;
		case LINE_ONHOVER:
			pLineHook->SetHoverHandler(*vp);
			break;
	}
	return JS_TRUE;
}


// Function to create a text which gets called on a "new text ()"

// Parameters: text, x, y, color, font, align, automap, onHover, onText
JSAPI_FUNC(text_ctor) {
	CDebug cDbg("text_ctor");

	Script* script = (Script*)JS_GetContextPrivate(cx);

	ScreenhookState state = (script->GetState () == OutOfGame) ? OOG : IG;
	uint x = 0, y = 0;
	ushort color = 0, font = 0;
	Align align = Left;
	bool automap = false;
	jsval click = JSVAL_VOID, hover = JSVAL_VOID;
	char* szText = "";

	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
		szText = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		if(!szText)
			return JS_TRUE;
	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		x = JSVAL_TO_INT(argv[1]);
	if(argc > 2 && JSVAL_IS_INT(argv[2]))
		y = JSVAL_TO_INT(argv[2]);
	if(argc > 3 && JSVAL_IS_INT(argv[3]))
		color = (ushort)JSVAL_TO_INT(argv[3]);
	if(argc > 4 && JSVAL_IS_INT(argv[4]))
		font = (ushort)JSVAL_TO_INT(argv[4]);
	if(argc > 5 && JSVAL_IS_INT(argv[5]))
		align = (Align)JSVAL_TO_INT(argv[5]);
	if(argc > 6 && JSVAL_IS_BOOLEAN(argv[6]))
		automap = !!JSVAL_TO_BOOLEAN(argv[6]);
	if(argc > 7 && JSVAL_IS_FUNCTION(cx, argv[7]))
		click = argv[7];
	if(argc > 8 && JSVAL_IS_FUNCTION(cx, argv[8]))
		hover = argv[8];

	TextHook* pTextHook = new TextHook(script, szText, x, y, font, color, automap, align, state);

	if(!pTextHook)
		THROW_ERROR(cx, obj, "Failed to create texthook");

	pTextHook->SetClickHandler(click);
	pTextHook->SetHoverHandler(hover);

	JSObject* hook = BuildObject(cx, &text_class, text_methods, text_props, pTextHook);
	if(!hook)
		THROW_ERROR(cx, obj, "Failed to create text object");

	*rval = OBJECT_TO_JSVAL(hook);

	return JS_TRUE;
}

JSAPI_PROP(text_getProperty) {
	CDebug cDbg("text_getProperty");

	TextHook* pTextHook = (TextHook*)JS_GetPrivate(cx, obj);
	if(!pTextHook)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id)) {
		case TEXT_X:
			*vp = INT_TO_JSVAL(pTextHook->GetX());
			break;
		case TEXT_Y:
			*vp = INT_TO_JSVAL(pTextHook->GetY());
			break;
		case TEXT_COLOR:
			*vp = INT_TO_JSVAL(pTextHook->GetColor());
			break;
		case TEXT_FONT:
			*vp = INT_TO_JSVAL(pTextHook->GetFont());
			break;
		case TEXT_TEXT:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pTextHook->GetText()));
			break;
		case TEXT_ALIGN:
			*vp = INT_TO_JSVAL(pTextHook->GetAlign());
			break;
		case TEXT_VISIBLE:
			*vp = BOOLEAN_TO_JSVAL(pTextHook->GetIsVisible());
			break;
		case TEXT_ZORDER:
			*vp = INT_TO_JSVAL(pTextHook->GetZOrder());
			break;
		case TEXT_ONCLICK:
			*vp = pTextHook->GetClickHandler();
			break;
		case TEXT_ONHOVER:
			*vp = pTextHook->GetHoverHandler();
			break;
	}
	return JS_TRUE;
}

JSAPI_PROP(text_setProperty) {
	CDebug cDbg("text_setProperty");

	TextHook* pTextHook = (TextHook*)JS_GetPrivate(cx, obj);
	if(!pTextHook)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id)) {
		case TEXT_X:
			if(JSVAL_IS_INT(*vp))
				pTextHook->SetX(JSVAL_TO_INT(*vp));
			break;
		case TEXT_Y:
			if(JSVAL_IS_INT(*vp))
				pTextHook->SetY(JSVAL_TO_INT(*vp));
			break;
		case TEXT_COLOR:
			if(JSVAL_IS_INT(*vp))
				pTextHook->SetColor((ushort)JSVAL_TO_INT(*vp));
			break;
		case TEXT_FONT:
			if(JSVAL_IS_INT(*vp))
				pTextHook->SetFont((ushort)JSVAL_TO_INT(*vp));
			break;
		case TEXT_TEXT:
			if(JSVAL_IS_STRING(*vp))
			{
				CHAR* pText = JS_GetStringBytes(JS_ValueToString(cx, *vp));
				if(!pText)
					return JS_TRUE;
				pTextHook->SetText(pText);
			}
			break;
		case TEXT_ALIGN:
			if(JSVAL_IS_INT(*vp))
				pTextHook->SetAlign((Align)JSVAL_TO_INT(*vp));
			break;
		case TEXT_VISIBLE:
			if(JSVAL_IS_BOOLEAN(*vp))
				pTextHook->SetIsVisible(!!JSVAL_TO_BOOLEAN(*vp));
			break;
		case TEXT_ZORDER:
			if(JSVAL_IS_INT(*vp))
				pTextHook->SetZOrder((ushort)JSVAL_TO_INT(*vp));
			break;
		case TEXT_ONCLICK:
			pTextHook->SetClickHandler(*vp);
			break;
		case TEXT_ONHOVER:
			pTextHook->SetHoverHandler(*vp);
			break;
	}
	return JS_TRUE;
}


// Function to create a image which gets called on a "new Image ()"

// Parameters: image, x, y, color, align, automap, onHover, onimage
JSAPI_FUNC(image_ctor) {
	CDebug cDbg("image_ctor");

	Script* script = (Script*)JS_GetContextPrivate(cx);

	ScreenhookState state = (script->GetState () == OutOfGame) ? OOG : IG;
	uint x = 0, y = 0;
	ushort color = 0;
	Align align = Left;
	bool automap = false;
	jsval click = JSVAL_VOID, hover = JSVAL_VOID;
	char* szText = "";
	char path[_MAX_FNAME+_MAX_PATH];

	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
		szText	= JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		if(!szText)
			return JS_TRUE;
	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		x = JSVAL_TO_INT(argv[1]);
	if(argc > 2 && JSVAL_IS_INT(argv[2]))
		y = JSVAL_TO_INT(argv[2]);
	if(argc > 3 && JSVAL_IS_INT(argv[3]))
		color = (ushort)JSVAL_TO_INT(argv[3]);
	if(argc > 4 && JSVAL_IS_INT(argv[4]))
		align = (Align)JSVAL_TO_INT(argv[4]);
	if(argc > 5 && JSVAL_IS_BOOLEAN(argv[5]))
		automap = !!JSVAL_TO_BOOLEAN(argv[5]);
	if(argc > 6 && JSVAL_IS_FUNCTION(cx, argv[6]))
		click = argv[6];
	if(argc > 7 && JSVAL_IS_FUNCTION(cx, argv[7]))
		hover = argv[7];

	if(!isValidPath(path))
		THROW_ERROR(cx, obj, "Invalid image file path");

	sprintf_s(path, sizeof(path), "%s\\%s", Vars.szScriptPath, szText);
	ImageHook* pImageHook = new ImageHook(script, path, x, y, color, automap, align, state);

	if(!pImageHook)
		THROW_ERROR(cx, obj, "Failed to create ImageHook");

	pImageHook->SetClickHandler(click);
	pImageHook->SetHoverHandler(hover);

	JSObject* hook = BuildObject(cx, &image_class, image_methods, image_props, pImageHook);
	if(!hook)
		THROW_ERROR(cx, obj, "Failed to create image object");

	*rval = OBJECT_TO_JSVAL(hook);

	return JS_TRUE;
}

JSAPI_PROP(image_getProperty) {
	CDebug cDbg("image_getProperty");

	ImageHook* pImageHook = (ImageHook*)JS_GetPrivate(cx, obj);
	if(!pImageHook)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id)) {
		case IMAGE_X:
			*vp = INT_TO_JSVAL(pImageHook->GetX());
			break;
		case IMAGE_Y:
			*vp = INT_TO_JSVAL(pImageHook->GetY());
			break;
		case IMAGE_LOCATION:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pImageHook->GetImage()));
			break;
		case IMAGE_ALIGN:
			*vp = INT_TO_JSVAL(pImageHook->GetAlign());
			break;
		case IMAGE_VISIBLE:
			*vp = BOOLEAN_TO_JSVAL(pImageHook->GetIsVisible());
			break;
		case IMAGE_ZORDER:
			*vp = INT_TO_JSVAL(pImageHook->GetZOrder());
			break;
		case IMAGE_ONCLICK:
			*vp = pImageHook->GetClickHandler();
			break;
		case IMAGE_ONHOVER:
			*vp = pImageHook->GetHoverHandler();
			break;
	}
	return JS_TRUE;
}

JSAPI_PROP(image_setProperty) {
	CDebug cDbg("image_setProperty");

	ImageHook* pImageHook = (ImageHook*)JS_GetPrivate(cx, obj);
	if(!pImageHook)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id)) {
		case IMAGE_X:
			if(JSVAL_IS_INT(*vp))
				pImageHook->SetX(JSVAL_TO_INT(*vp));
			break;
		case IMAGE_Y:
			if(JSVAL_IS_INT(*vp))
				pImageHook->SetY(JSVAL_TO_INT(*vp));
			break;
		case IMAGE_LOCATION:
			if(JSVAL_IS_STRING(*vp))
			{
				char* pimage = JS_GetStringBytes(JS_ValueToString(cx, *vp));
				if(!pimage)
					return JS_TRUE;
				pImageHook->SetImage(pimage);
			}
			break;
		case IMAGE_ALIGN:
			if(JSVAL_IS_INT(*vp))
				pImageHook->SetAlign((Align)JSVAL_TO_INT(*vp));
			break;
		case IMAGE_VISIBLE:
			if(JSVAL_IS_BOOLEAN(*vp))
				pImageHook->SetIsVisible(!!JSVAL_TO_BOOLEAN(*vp));
			break;
		case IMAGE_ZORDER:
			if(JSVAL_IS_INT(*vp))
				pImageHook->SetZOrder((ushort)JSVAL_TO_INT(*vp));
			break;
		case IMAGE_ONCLICK:
			pImageHook->SetClickHandler(*vp);
			break;
		case IMAGE_ONHOVER:
			pImageHook->SetHoverHandler(*vp);
			break;
	}
	return JS_TRUE;
}

