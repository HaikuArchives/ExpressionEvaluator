
#ifndef __EVALUATOR_WINDOW_H_INCLUDED
#define __EVALUATOR_WINDOW_H_INCLUDED

#include <AppKit.h>
#include <InterfaceKit.h>
#include "evaluator.h"

const float W_WIDTH			= 400.0f;
const float W_HEIGHT		= 250.0f;
const float W_MARGIN		= 11.0f;
const float BUTTON_WIDTH	= 70.0f;
const float BUTTON_HEIGHT	= 25.0f;
const float EDIT_HEIGHT		= 20.0f;
const float RBOX_WIDTH		= 70.0f;
const float BOX_SHIFT		= 8.0f;

class EvaluatorView: public BView {
public:
	EvaluatorView( BRect r );
	void AttachedToWindow();
	void MessageReceived( BMessage *msg );
private:
	void copy( const char* s );
	BBox* new_box( BView* parent, float x1, float y1, float x2, float y2, const char* label );
	BRadioButton* new_radio( BView* parent, float x1, float y1, float x2, float y2, const char* label );
	BTextControl* new_text( BView* parent, float x1, float y1, float x2, float y2, alignment align );
	BButton* new_button( BView* parent, float x, float y, const char* label, uint32 msg );

	BButton *b_eval, *b_copy, *b_close;
	BTextControl *e_input, *e_dec, *e_hex, *e_bin, *e_oct, *e_exp;
	BRadioButton *r_dec, *r_hex, *r_bin, *r_oct, *r_exp;
	EEvaluator evaluator;
};

class EvaluatorWindow: public BWindow {
public:
	EvaluatorWindow():
	BWindow( BRect( 0, 0, W_WIDTH, W_HEIGHT ), "Evaluator",
			B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_NOT_RESIZABLE ) {
		AddChild( new EvaluatorView( Bounds() ) );
		BScreen screen( this );
		uint32 scrw = (uint32)(screen.Frame().right + 1);
		uint32 scrh = (uint32)(screen.Frame().bottom + 1);
		uint32 wndw = (uint32)(Frame().right - Frame().left);
		uint32 wndh = (uint32)(Frame().bottom - Frame().top);
		MoveTo( BPoint( (scrw-wndw)>>1, (scrh-wndh)>>1 ) );
	}
	bool QuitRequested();
};

class EvaluatorApp: public BApplication {
public:
	EvaluatorApp(): BApplication( "application/x-vnd.Evaluator" ) {
		(new EvaluatorWindow())->Show();
	}
};

#endif // __EVALUATOR_WINDOW_H_INCLUDED
