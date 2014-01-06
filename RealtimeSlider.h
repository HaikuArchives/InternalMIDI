#ifndef _REALTIME_SLIDER_H
#define _REALTIME_SLIDER_H

#include <Slider.h>

class RealtimeSlider : public BSlider {

public:

		RealtimeSlider(BRect frame, const char *name, const char *label,
			BMessage *message, int32 minValue,
			int32 maxValue, thumb_style thumbType = B_BLOCK_THUMB,
			uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);
	
void	MouseMoved(BPoint pt, uint32 c, const BMessage *m);
void 	MouseDown(BPoint pt);
void 	MouseUp(BPoint pt);
void	SetDefaultValue( int32 value ) { fDefaultValue = value; }

private:

bool	fMouseDown;
int32	fDefaultValue;
};

#endif
