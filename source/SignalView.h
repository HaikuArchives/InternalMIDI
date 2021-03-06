/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _SIGNAL_VIEW
#define _SIGNAL_VIEW

#include <View.h>

class BBitmap;
class BMessageRunner;

class SignalView : public BView {

friend class SettingsWindow;

public:
			SignalView( BRect bounds, uint32 followFlags = B_FOLLOW_NONE );
			~SignalView();
			
void		UpdateView();
void		Draw( BRect bounds );
void		DrawPing(int8 velocity);

void		MessageReceived(BMessage *message);

private:

BBitmap			*fBgBitmap;
BView			*fBgView;
BMessageRunner	*fMessageRunner;
int32			fEffectNr;

};

#endif
