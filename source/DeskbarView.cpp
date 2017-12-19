/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <AppFileInfo.h>
#include <Bitmap.h>
#include <Deskbar.h>
#include <Entry.h>
#include <Message.h>
#include <MessageRunner.h>
#include <MenuItem.h>
#include <MidiConsumer.h>
#include <MidiSynth.h>
#include <NodeInfo.h>
#include <PopUpMenu.h>
#include <Roster.h>
#include <Window.h>

#include <iostream>
#include <string.h>

#include "App.h"
#include "DeskbarView.h"
#include "InternalMidi.h"
#include "Prefs.h"

extern "C" _EXPORT BView *instantiate_deskbar_item();

BView *instantiate_deskbar_item()
{
	return new DeskbarView();
}

DeskbarView::DeskbarView()
:	BView(VIEW_RECT, VIEW_NAME, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW ),
	fIsReplicant( false )
{
	Init();
}

DeskbarView::DeskbarView(BMessage *archive)
:	BView( archive ),
	fIsReplicant( true )
{

	Init();
	
	// If the application is not running terminate the replicant
	// terminate itself
	if (!be_roster->IsRunning( APP_SIGNATURE ) ) {
		RemoveFromDeskbar();
	}

	// Icon
	entry_ref ref;
	be_roster->FindApp( APP_SIGNATURE, &ref);
	
	fIcon = new BBitmap(BRect(0,0,15,15), B_RGBA32 );
	BNodeInfo::GetTrackerIcon(&ref, fIcon, B_MINI_ICON);
	
}

void DeskbarView::Init() {
	fMyAppMessenger = 0;
	fIcon = 0;
	fPopUpMenu = 0;
	fMessageRunner = 0;
}

BArchivable *DeskbarView::Instantiate(BMessage *archive) 
{
	if ( !validate_instantiation(archive, "DeskbarView") )
		return NULL;
	return new DeskbarView(archive); 
}

void DeskbarView::Draw(BRect updateRect) {

	DrawBitmap( fIcon );
}

void DeskbarView::MouseDown( BPoint where ) {

	BMessage	*msg = Window()->CurrentMessage();
	
	int32	buttons = 0;
	msg->FindInt32("buttons", &buttons );

	int32	clicks = 0;
	msg->FindInt32("clicks", &clicks );

	// Message runner end
	delete fMessageRunner;
	fMessageRunner = 0;

	if ( ( (buttons & B_PRIMARY_MOUSE_BUTTON) && clicks>1) || ( buttons & B_TERTIARY_MOUSE_BUTTON ) ) {
		BMessenger(this).SendMessage( new BMessage('Sett') );
	}
	else if ( buttons & B_PRIMARY_MOUSE_BUTTON && !(modifiers() & B_OPTION_KEY ) ) {
		fMessageRunner = new BMessageRunner(BMessenger( this ), new BMessage('MsDn'), 500000, 1 );
	}
	else if ( (buttons & B_SECONDARY_MOUSE_BUTTON ) || (buttons & B_PRIMARY_MOUSE_BUTTON && modifiers() & B_OPTION_KEY ) ) {
		PopUpMenu();	
	}
}

void DeskbarView::MouseMoved(BPoint pt, uint32 c, const BMessage *m) {

	// Message runner end
	delete fMessageRunner;
	fMessageRunner = 0;
	
	// Main program still exist? If not quit.
	if (!be_roster->IsRunning( APP_SIGNATURE )) RemoveFromDeskbar();
	
	// Continue normal move
	BView::MouseMoved(pt, c, m);

}

void DeskbarView::PopUpMenu() {

	BPoint	where;
	uint32	buttons;
	GetMouse( &where, &buttons );
	
	ConvertToScreen(&where);
	BRect r(-5, -5, 5, 5);
	r = ConvertToScreen(r);
	r.OffsetTo(where);

	// Create Popupmenu
	if (!fPopUpMenu) {

		fPopUpMenu = new BPopUpMenu("fPopUpMenu", false, false);
		fPopUpMenu->SetFontSize( 10.0 );
		
		BMessenger messenger(this);
		
		BMenuItem	*item;
	
		item = new BMenuItem("Settings...", new BMessage('Sett'));
		item->SetTarget( messenger );
		fPopUpMenu->AddItem( item );

		item = new BMenuItem("Show this replicant", new BMessage('Repl'));
		item->SetMarked( true );
		item->SetTarget( messenger );
		fPopUpMenu->AddItem( item );

		item = new BMenuItem("About...", new BMessage( B_ABOUT_REQUESTED ));
		item->SetTarget( messenger );
		fPopUpMenu->AddItem( item );
	
		fPopUpMenu->AddSeparatorItem();
	
		item = new BMenuItem("Quit", new BMessage( B_QUIT_REQUESTED ));
		item->SetTarget( messenger );
		fPopUpMenu->AddItem( item );
	}
	fPopUpMenu->Go(where, true, true, r, true);

}

status_t DeskbarView::Archive(BMessage *archive, bool deep) const
{
	BView::Archive(archive, deep);
	archive->AddString("add_on", APP_SIGNATURE);
	archive->AddString("class", VIEW_NAME);

	return B_OK;
}

void DeskbarView::MessageReceived(BMessage *message) {

	// Messenger find
	delete fMyAppMessenger;
	fMyAppMessenger = new BMessenger( APP_SIGNATURE );

	switch( message->what ) {

		case 'MsDn': {
			BPoint	where;
			uint32	buttons;
			GetMouse( &where, &buttons );
			if ( buttons & B_PRIMARY_MOUSE_BUTTON ) PopUpMenu();
		} break;
		
		case 'Sett':
		case 'Repl':
		case B_ABOUT_REQUESTED: {

			fMyAppMessenger->SendMessage( message );
			
		} break;
		
		case B_QUIT_REQUESTED: {
			
			if (fMyAppMessenger->IsValid()) {
				fMyAppMessenger->SendMessage( message );
			}
			else {
				RemoveFromDeskbar();
			}
			
		} break;

		default:
			BView::MessageReceived( message );
	}
}

void DeskbarView::AttachedToWindow()
{
	SetViewColor(Parent()->ViewColor());
	SetDrawingMode(B_OP_OVER);
	
	BView::AttachedToWindow();

}

void DeskbarView::AddToDeskbar() {

	entry_ref ref;
	BDeskbar deskbar;

	be_roster->FindApp( APP_SIGNATURE, &ref);
	status_t err = deskbar.AddItem(&ref);
	if (err != B_OK)
		cerr << "Error while creating deskbar replicant: " << strerror(err) << endl;
}

void DeskbarView::RemoveFromDeskbar() {

	BDeskbar deskbar;
	deskbar.RemoveItem(VIEW_NAME);

}

DeskbarView::~DeskbarView() {

	delete fMyAppMessenger;
	delete fIcon;
	delete fPopUpMenu;
	delete fMessageRunner;

}
