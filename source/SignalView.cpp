/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <Bitmap.h>
#include <MessageRunner.h>
#include <Synth.h>

#include <String.h>

#include <iostream>
#include <math.h>

#define	MAX_SAMPLES	128
#define	NUM_EFFECTS 2

#include "SignalView.h"

void DrawEffect( BView * fBgView );

SignalView::SignalView( BRect rect, uint32 followFlags )
:	BView( rect, "signalview", followFlags, B_WILL_DRAW ),
	fBgBitmap( 0 ),
	fBgView( 0 ),
	fMessageRunner( 0 )
{
	rect.InsetBy(1.0, 1.0);
	rect.OffsetTo( 0.0, 0.0 );
	
	fBgBitmap	= new BBitmap( rect, B_RGBA32, true );
	fBgView		= new BView( rect, "bgview", B_FOLLOW_NONE, B_WILL_DRAW );

	fBgBitmap->Lock();
	fBgBitmap->AddChild( fBgView );
	fBgView->SetViewColor( 0, 0, 0 );
	fBgView->SetHighColor( 0, 0, 0 );
	fBgView->FillRect( fBgView->Bounds() );
	fBgView->SetDrawingMode( B_OP_ALPHA );
	fBgView->SetBlendingMode( B_CONSTANT_ALPHA, B_ALPHA_COMPOSITE );
	fBgBitmap->Unlock();

}

void SignalView::Draw( BRect updateRect ) {

	BRect	bounds( Bounds() );
	SetHighColor( 255, 255, 255 );
	StrokeLine( BPoint( bounds.right, bounds.top + 1 ), 
				BPoint( bounds.right, bounds.bottom ) );
	StrokeLine( BPoint( bounds.left + 1, bounds.bottom ) );

	SetHighColor( 120, 120, 120 );
	StrokeLine( BPoint( bounds.left, bounds.bottom - 1 ), 
				BPoint( bounds.left, bounds.top ) );
	StrokeLine( BPoint( bounds.right - 1, bounds.top ) );

	UpdateView();
}

void SignalView::DrawPing(int8 velocity) {

	fBgBitmap->Lock();
	fBgView->SetHighColor( 102, 152, 203 );
	BRect bounds(Bounds());
	bounds.top = bounds.bottom - bounds.Height() / 127 * velocity;
	fBgView->FillRect( bounds );
	fBgBitmap->Unlock();

}

void SignalView::MessageReceived( BMessage *message ) {
	switch (message->what) {
		case 'Draw': UpdateView(); break;
		case 'Ping': {
			int8 velocity;
			if (message->FindInt8("velocity", &velocity)!=B_OK) velocity = 127;
			DrawPing( velocity);
		} break;
		default:
			BView::MessageReceived( message );
	}
}

SignalView::~SignalView() {

	delete fMessageRunner;
	delete fBgBitmap;

}

void SignalView::UpdateView() {

	fBgBitmap->Lock();

	fBgView->SetHighColor( 0, 0, 0, 10 );
	fBgView->FillRect( fBgView->Bounds() );

	fBgView->Sync();
	fBgBitmap->Unlock();

	DrawBitmap( fBgBitmap, BPoint( 1.0, 1.0 ) );

}

