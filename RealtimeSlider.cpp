#include <Window.h>

#include <iostream>

#include "RealtimeSlider.h"

RealtimeSlider::RealtimeSlider(BRect frame, const char *name,
			const char *label, BMessage *message, int32 minValue,
			int32 maxValue, thumb_style thumbType,
			uint32 resizingMode, uint32 flags)

:	BSlider( frame, name, label, message, minValue, maxValue, thumbType,
		resizingMode, flags ),
	fMouseDown( false ),
	fDefaultValue( minValue )
{
}		

void RealtimeSlider::MouseMoved(BPoint pt, uint32 c, const BMessage *m) {
	BSlider::MouseMoved( pt, c, m );
	if (fMouseDown) Invoke();
}

void RealtimeSlider::MouseDown(BPoint pt) {
	BMessage	*message = Window()->CurrentMessage();
	int32		buttons = 0;
	message->FindInt32("buttons", &buttons);
	if (buttons & B_SECONDARY_MOUSE_BUTTON) {
		SetValue( fDefaultValue );
		Invoke();
	}
	else {
		fMouseDown = true;
		BSlider::MouseDown( pt );
	}
}

void RealtimeSlider::MouseUp(BPoint pt) {
	fMouseDown = false;
	BSlider::MouseUp( pt );
}
