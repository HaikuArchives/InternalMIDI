/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <Alert.h>
#include <Catalog.h>
#include <Control.h>
#include <Deskbar.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Messenger.h>
#include <Path.h>
#include <Window.h>

#include <private/interface/AboutWindow.h>

#include <iostream>

#include "App.h"
#include "DeskbarView.h"
#include "Prefs.h"
#include "InternalMidi.h"
#include "SettingsWindow.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "App"

App::App()
:	BApplication( APP_SIGNATURE ),
	fSettingsWindow( 0 )
{
	Run();
}

void App::ReadyToRun() {

	// MidiSynth
	fInternalMidi = new InternalMidi();

	// Deskbar replicant
	BDeskbar deskbar;
	
	if (prefs.fShowReplicant) {
		DeskbarView::AddToDeskbar();
	}
	else {
		DeskbarView::RemoveFromDeskbar();
	}

}


bool App::QuitRequested() {
	return true;
}

void App::MessageReceived(BMessage *message) {

	switch( message->what ) {
	
		// --- Common -----------------------------------------------

		case B_SILENT_RELAUNCH:
		case 'Sett': {
			if (fSettingsWindow) {
				fSettingsWindow->Lock();
				fSettingsWindow->SetWorkspaces( B_CURRENT_WORKSPACE );
				if (fSettingsWindow->IsHidden()) fSettingsWindow->Show();
				fSettingsWindow->Activate();
				fSettingsWindow->Unlock();
			}
			else {
				fSettingsWindow = new SettingsWindow();
			}
		} break;
		
		// --- Settings ------------------------------------------

		case 'Paus': {
			int32	value = 0;
			message->FindInt32( "be:value", &value );
			if (value) be_synth->Pause();
			else be_synth->Resume();
			prefs.fPause = value;
		} break;

		case 'Patc': {
			entry_ref	ref;
			message->FindRef( "patches", &ref );
			prefs.fSynthFile = ref.name;
			fInternalMidi->LoadFromPrefs();
		} break;
		
		case 'Rate': {
			int32	value = 0;
			message->FindInt32( "rate", &value );
			be_synth->SetSamplingRate( value );
			prefs.fSamplingRate = value;
		} break;
		
		case 'Inte': {
			int32	value = 0;
			message->FindInt32( "interpolation", &value );
			be_synth->SetInterpolation( (interpolation_mode)value );
			prefs.fInterpolation = (interpolation_mode)value;
		} break;

		case 'Gain': {
			int32	value = 0;
			message->FindInt32( "be:value", &value );
			fInternalMidi->fMidiSynth.SetVolume( (double)value / 100 );
			prefs.fGain = value;
		} break;

		case 'Reve': {
			int32	value = 0;
			message->FindInt32( "be:value", &value );
			if (value != prefs.fReverbMode ) {
				be_synth->SetReverb( (reverb_mode)value );
				prefs.fReverbMode = (reverb_mode)value;
			}
		} break;

		case 'Tran': {
			int32	value = 0;
			message->FindInt32( "be:value", &value );
			fInternalMidi->fMidiSynth.SetTransposition( value );
			prefs.fTransposition = value;
		} break;

		case 'Repl': {

			int32	value = 0;
			message->FindInt32( "be:value", &value );
			if (value) DeskbarView::AddToDeskbar();
			else DeskbarView::RemoveFromDeskbar();

			prefs.fShowReplicant = value;
			if (fSettingsWindow) {
				fSettingsWindow->Lock();
				BControl	*checkbox = (BControl *)fSettingsWindow->FindView( "showreplicant" );
				if (checkbox) checkbox->SetValue( prefs.fShowReplicant );
				fSettingsWindow->Unlock();
			}

		} break;
		
		// --- Rest -----------------------------------------------

		default:
			BApplication::MessageReceived( message );
	}
}

void App::AboutRequested() {
	BAboutWindow* about = new BAboutWindow("InternalMIDI", APP_SIGNATURE);
	about->AddCopyright(2023,"Werner Freytag");
	about->AddDescription(B_TRANSLATE("InternalMIDI creates a MIDI node for the internal Haiku General MIDI synthesizer.\n"
	"Use a utility like PatchBay to connect it to your MIDI applications!\n\n"
	"InternalMIDI is released under MIT license.\n\n"
	"Project Homepage:\nhttp://dev.osdrawer.net/projects/internalmidi\n"));
	about->Show();
}

App::~App() {

	if (fSettingsWindow) {
		fSettingsWindow->Lock();
		fSettingsWindow->Quit();
	}

	if (fInternalMidi->fIsLoaded) fInternalMidi->Release();

	DeskbarView::RemoveFromDeskbar();

}

int main() {

	delete new App;
	return 0;

}
