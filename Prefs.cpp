/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <MidiDefs.h>
#include <Path.h>
#include <Synth.h>

#include <iostream>

#include "App.h"
#include "Prefs.h"

Prefs prefs;

Prefs::Prefs()
{

	// Pfade initialisieren
	find_directory(B_SYNTH_DIRECTORY, &fSynthDirectory );
	fSynthDirectory.SetTo( fSynthDirectory.Path(), "synth" );

	find_directory(B_USER_SETTINGS_DIRECTORY, &fSavePath);
	fSavePath.SetTo( fSavePath.Path(), PREFS_FILENAME );
	
	// --- Einstellungen vornehmen
	BMessage	message;
	BFile		file( fSavePath.Path(), B_READ_ONLY );
	
	message.Unflatten( &file );

	// --- Pause
	if (message.FindBool("pause", &fPause ) != B_OK )
		fPause = false;

	// --- SynthFile (default: erste gefundene )
	if (message.FindString("synthfile", &fSynthFile ) != B_OK ) {
		entry_ref	ref;

		BDirectory	directory( fSynthDirectory.Path() );
		directory.GetNextRef( &ref );
		fSynthFile = ref.name;
	}
	
	// -- Sampling Rate
	if ( message.FindInt32("samplingrate", &fSamplingRate ) != B_OK )
		fSamplingRate = 22050;
	
	// -- Interpolation
	if ( message.FindInt32("interpolation", (int32 *)&fInterpolation ) != B_OK )
		fInterpolation = B_LINEAR_INTERPOLATION;
	
	// -- Gain
	if ( message.FindInt32("gain", &fGain ) != B_OK )
		fGain = 100;
	
	// -- Reverb
	if ( message.FindInt32("reverbmode", (int32 *)&fReverbMode ) != B_OK )
		fReverbMode = B_REVERB_BALLROOM;

	// -- Transposition
	if ( message.FindInt32("transposition", &fTransposition ) != B_OK )
		fTransposition = 0;

	// -- ShowReplicant
	if ( message.FindBool("showreplicant", &fShowReplicant ) != B_OK )
		fShowReplicant = true;

}

Prefs::~Prefs() {

	BMessage	message;
	
	message.AddBool("pause", fPause );
	message.AddString("synthfile", fSynthFile );
	message.AddInt32("samplingrate", fSamplingRate );
	message.AddInt32("interpolation", (int32)fInterpolation );
	message.AddInt32("gain", fGain );
	message.AddInt32("reverbmode", (int32)fReverbMode );
	message.AddInt32("transposition", fTransposition );
	message.AddBool("showreplicant", fShowReplicant );

	BFile		file( fSavePath.Path(), B_WRITE_ONLY|B_CREATE_FILE );

	message.Flatten( &file );

}
