/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _SCOPE_VIEW
#define _SCOPE_VIEW

#include <View.h>

class BBitmap;
class BMessageRunner;

class ScopeView : public BView {

friend class SettingsWindow;

public:
			ScopeView( BRect bounds, uint32 followFlags = B_FOLLOW_NONE );
			~ScopeView();
			
void		Draw( BRect bounds );

void		MessageReceived(BMessage *message);
void		MouseDown(BPoint where);

void		UpdateView();

private:

BBitmap			*fBgBitmap;
BView			*fBgView;
BMessageRunner	*fMessageRunner;
int32			fEffectNr;

};

#endif

