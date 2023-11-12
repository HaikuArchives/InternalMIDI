/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "SettingsWindow.h"

#include <Catalog.h>
#include <Control.h>
#include <Deskbar.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Messenger.h>
#include <MidiSynth.h>
#include <Screen.h>
#include <Synth.h>
#include <View.h>

#include <iostream>

#include "App.h"
#include "DeskbarView.h"
#include "InternalMidi.h"
#include "Prefs.h"
#include "ScopeView.h"
#include "SignalView.h"
#include "SettingsMainView.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "SettingsWindow"

const float HEIGHT = 410;
const float WIDTH = 248;

SettingsWindow::SettingsWindow()
:	BWindow( BRect( 0, 0, WIDTH, HEIGHT ), B_TRANSLATE("InternalMIDI Settings"), B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS|B_WILL_ACCEPT_FIRST_CLICK|B_NOT_RESIZABLE|B_NOT_ZOOMABLE )
{
	BDeskbar	deskbar;
	BScreen		screen;
	BRect		screenFrame( screen.Frame() );
	
	switch (deskbar.Location() ) {
		case B_DESKBAR_TOP:
			MoveTo(screenFrame.right - Bounds().Width() - 20, 60 );
			break;
		case B_DESKBAR_BOTTOM:
			MoveTo(screenFrame.right - Bounds().Width() - 20, screenFrame.bottom - Bounds().Height() - 40 );
			break;
		case B_DESKBAR_LEFT_BOTTOM:
			MoveTo(130, screenFrame.bottom - Bounds().Height() - 20 );
			break;
		case B_DESKBAR_RIGHT_BOTTOM:
			MoveTo(screenFrame.right - Bounds().Width() - 130, screenFrame.bottom - Bounds().Height() - 20 );
			break;
		case B_DESKBAR_LEFT_TOP:
			MoveTo(130, 30 );
			break;
		case B_DESKBAR_RIGHT_TOP:
			MoveTo(screenFrame.right - Bounds().Width() - 130, 30 );
			break;
	}

	// Menu
	BMenuBar	*menuBar;
	BMenu		*menu;
	BMenuItem	*menuItem;
	
	menuBar	= new BMenuBar( BRect(0, 0, 10000, 10), "menubar" );

	// --- File ---
	menu	= new BMenu( B_TRANSLATE("File") );

	menuItem = new BMenuItem( B_TRANSLATE("Close window"), new BMessage( B_QUIT_REQUESTED ), 'W' );
	menu->AddItem( menuItem );

	menu->AddSeparatorItem();

	menuItem = new BMenuItem( B_TRANSLATE("Quit"), new BMessage( 'Quit' ), 'Q' );
	menu->AddItem( menuItem );
	
	menuBar->AddItem( menu );

	// --- Help ---
	menu	= new BMenu( B_TRANSLATE("Help") );
	menuItem = new BMenuItem( B_TRANSLATE("About" B_UTF8_ELLIPSIS), new BMessage( B_ABOUT_REQUESTED ), '?' );
	menu->AddItem( menuItem );
	
	menuBar->AddItem( menu );

	AddChild( menuBar );
	
	// Font sensivity...	
	BFont	font(be_plain_font);
	float min_width = font.StringWidth("2 point interpolationSynthesizer patches") + 60;

	ResizeBy( min_width - Bounds().Width(), menuBar->Bounds().Height() );

	// mainview
	SettingsMainView	*mainview = new SettingsMainView( Bounds().InsetByCopy( -1.0, -1.0 ).OffsetByCopy( 0, menuBar->Bounds().Height() ) );
	AddChild( mainview );

	Show();
}

void SettingsWindow::MessageReceived(BMessage *message) {

	switch (message->what) {

		case B_ABOUT_REQUESTED:
		case 'Paus':
		case 'Patc':
		case 'Rate':
		case 'Inte':
		case 'Gain':
		case 'Reve':
		case 'Tran':
		case 'Repl': {
			be_app->PostMessage( message );
		} break;

		case 'Quit': {
			be_app->PostMessage( new BMessage( B_QUIT_REQUESTED ) );
 		} break;
		
		default:
			BWindow::MessageReceived( message );
	}
	
}

bool SettingsWindow::QuitRequested() {

	Hide();

	// MessageRunner of Signal & Scope View remove
	SignalView *signalview = (SignalView *)FindView("signalview");
	if (signalview->fMessageRunner) {
		delete signalview->fMessageRunner;
		signalview->fMessageRunner = 0;
	}
	ScopeView *scope = (ScopeView *)FindView("scope");
	if (scope->fMessageRunner) {
		delete scope->fMessageRunner;
		scope->fMessageRunner = 0;
	}

	return false;
}

void SettingsWindow::Show() {

	// Adjust window position
	BScreen screen;
	
	float new_left = Frame().left, new_top = Frame().top;
	
	if (Frame().left < 0 ) new_left = 10;
	else if (Frame().right > screen.Frame().right ) new_left = screen.Frame().right - Bounds().Width() - 10;
	
	if (Frame().top < 0 ) new_top = 30;
	else if (Frame().bottom > screen.Frame().bottom ) new_top = screen.Frame().bottom - Bounds().Height() - 20;
	
	if (Frame().LeftTop() != BPoint( new_left, new_top ) ) MoveTo( new_left, new_top );
	
	// Replicant check
	BDeskbar deskbar;
	bool has = deskbar.HasItem(VIEW_NAME);
	if (has != prefs.fShowReplicant ) {
		if (prefs.fShowReplicant) DeskbarView::AddToDeskbar();
		else DeskbarView::RemoveFromDeskbar();
	}

	// Signal scope and need to view Message Runner
	SignalView *signalview = (SignalView *)FindView("signalview");
	if (!signalview->fMessageRunner)
		signalview->fMessageRunner = new BMessageRunner( signalview, new BMessage('Draw'), 50000 );
	ScopeView *scope = (ScopeView *)FindView("scope");
	if (!scope->fMessageRunner)
		scope->fMessageRunner = new BMessageRunner( scope, new BMessage('Draw'), 50000 );

	// Show window
	BWindow::Show();
}
