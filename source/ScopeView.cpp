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

#include "ScopeView.h"

void DrawEffect( BView * fBgView );

ScopeView::ScopeView( BRect rect, uint32 followFlags )
:	BView( rect, "scope", followFlags, B_WILL_DRAW ),
	fBgBitmap( 0 ),
	fBgView( 0 ),
	fMessageRunner( 0 ),
	fEffectNr( 0 )
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

void ScopeView::Draw( BRect updateRect ) {

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

void ScopeView::MessageReceived( BMessage *message ) {
	switch (message->what) {
		case 'Draw': UpdateView(); break;
		default:
			BView::MessageReceived( message );
	}
}

void ScopeView::MouseDown(BPoint where) {

	fEffectNr++;
	if (fEffectNr==NUM_EFFECTS) fEffectNr = 0;

	BView::MouseDown( where );
}

ScopeView::~ScopeView() {

	delete fMessageRunner;
	delete fBgBitmap;

}

void ScopeView::UpdateView() {

	// Initialize
	int32	size = Bounds().Height() - 1; // muss immer quadratisch sein!

	float		value;
	float		pos;
	int32		half_size = size >> 1;
	
	static int16	pLeft[MAX_SAMPLES], pRight[MAX_SAMPLES];
	be_synth->GetAudio(pLeft, pRight, MAX_SAMPLES );

	// Color
	static int			farb_nr;
	static rgb_color	color = { 0, 0, 0, 64 };
	
	farb_nr		= (farb_nr + 1 ) % 128;
	static const float	faktor = M_PI * 2 / 128; // um einen Bereich von 0..2pi zu bekommen

	color.red	= (int)(sin(farb_nr			* faktor) * 75 + 180);
	color.green	= (int)(sin((farb_nr + 85)	* faktor) * 75 + 180);
	color.blue	= (int)(sin((farb_nr + 170)	* faktor) * 75 + 180);

	// Draw
	fBgBitmap->Lock();

	fBgView->SetHighColor( 0, 0, 0, 10 );
	fBgView->FillRect( fBgView->Bounds() );

	// Paint effect
	switch (fEffectNr) {

		case 0: {
//			static BPoint	*ptArray[ MAX_SAMPLES ];
			for (int32 i = 0; i<MAX_SAMPLES; ++i) {
		
				pos = ((float)i / MAX_SAMPLES * 4 * M_PI);
				
				fBgView->SetHighColor( color );
				value = ((float)(((int32)pLeft[i] + pRight[i]) / 2) / 65536 ) * size * 1.5;
				fBgView->StrokeLine( BPoint( half_size + sin( pos ) * value, half_size + cos( pos ) * value ) );
		
			}
		} break;

		case 1: {
			fBgView->BeginLineArray( MAX_SAMPLES );
			for (int32 i = 0; i<MAX_SAMPLES; ++i) {
		
				pos = (int)((float)i / MAX_SAMPLES * size);
				value = half_size + ((float)(((int32)pLeft[i] + pRight[i]) / 2) / 65536 ) * size;
		 		fBgView->AddLine( BPoint( pos, half_size ), BPoint( pos, value ), color);
		
			}
			fBgView->EndLineArray();
		} break;
	}

	fBgView->Sync();
	fBgBitmap->Unlock();

	DrawBitmap( fBgBitmap, BPoint( 1.0, 1.0 ) );

}

