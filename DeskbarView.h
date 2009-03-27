/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _DESKBAR_VIEW_H
#define _DESKBAR_VIEW_H

#include <View.h>

class BBitmap;

#define VIEW_NAME	"InternalMIDI DeskbarView"
#define VIEW_RECT	BRect(0, 0, 15, 15)

class BBitmap;
class InternalMidi;
class BPopUpMenu;
class SettingsWindow;
class MessageRunner;
class Messenger;

class _EXPORT DeskbarView : public BView {

public:
					DeskbarView();
					DeskbarView(BMessage *archive);
					~DeskbarView();
				
void				Draw(BRect updateRect);
void				MessageReceived(BMessage *message);

static BArchivable	*Instantiate(BMessage *archive);
status_t			Archive(BMessage *archive, bool deep) const;
void				MouseDown(BPoint where);
void				MouseMoved(BPoint pt, uint32 c, const BMessage *m);
void				AttachedToWindow();
static void			AddToDeskbar();
static void			RemoveFromDeskbar();

private:
void				Init();
void				PopUpMenu();	

bool				fIsReplicant;

BBitmap				*fIcon;
BPopUpMenu			*fPopUpMenu;
BMessageRunner		*fMessageRunner;
BMessenger			*fMyAppMessenger;
};

#endif
