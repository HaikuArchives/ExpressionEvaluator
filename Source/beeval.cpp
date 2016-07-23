
#include "beeval.h"

const uint32 MSG_EVAL		= 'eval';
const uint32 MSG_COPY		= 'copy';
const uint32 MSG_EDIT		= 'edit';

int main()
{
	EvaluatorApp app;
	app.Run();
}

EvaluatorView::EvaluatorView( BRect r ):
BView( r, NULL, B_FOLLOW_ALL_SIDES, B_NAVIGABLE | B_WILL_DRAW )
{
	// Retrieve dimensions
	r = Bounds();
	float width = r.right - r.left;
	float height = r.bottom - r.top;
	
	// Add group box
	float BOX_WIDTH = width - 2*W_MARGIN;
	float BOX_HEIGHT = height - 3*W_MARGIN - BUTTON_HEIGHT;
	BBox* box = new_box( this, W_MARGIN, W_MARGIN, W_MARGIN+BOX_WIDTH,
			W_MARGIN+BOX_HEIGHT, "" );

	// Add edit control
	e_input = new_text( box, W_MARGIN-3, W_MARGIN, BOX_WIDTH-W_MARGIN, W_MARGIN+EDIT_HEIGHT, B_ALIGN_LEFT );
	e_input->SetModificationMessage( new BMessage( MSG_EDIT ) );

	// Add radiobuttons
	float RBOX_HEIGHT = BOX_HEIGHT - 3*W_MARGIN - EDIT_HEIGHT;
	BBox* rbox = new_box( box, W_MARGIN, W_MARGIN*2+EDIT_HEIGHT,
		W_MARGIN+RBOX_WIDTH, BOX_HEIGHT-W_MARGIN, "Copy As" );
	float RADIO_WIDTH = RBOX_WIDTH - 2*W_MARGIN;
	float RADIO_DELTA = (RBOX_HEIGHT - 2*W_MARGIN - 5*EDIT_HEIGHT - BOX_SHIFT) /  4;
	float rx = W_MARGIN;
	float ry = W_MARGIN + BOX_SHIFT;
	r_dec = new_radio( rbox, rx, ry, rx+RADIO_WIDTH, ry+EDIT_HEIGHT, "Dec" );
	r_dec->SetValue( B_CONTROL_ON );
	ry += EDIT_HEIGHT + RADIO_DELTA;
	r_hex = new_radio( rbox, rx, ry, rx+RADIO_WIDTH, ry+EDIT_HEIGHT, "Hex" );
	ry += EDIT_HEIGHT + RADIO_DELTA;
	r_bin = new_radio( rbox, rx, ry, rx+RADIO_WIDTH, ry+EDIT_HEIGHT, "Bin" );
	ry += EDIT_HEIGHT + RADIO_DELTA;
	r_oct = new_radio( rbox, rx, ry, rx+RADIO_WIDTH, ry+EDIT_HEIGHT, "Oct" );
	ry += EDIT_HEIGHT + RADIO_DELTA;
	r_exp = new_radio( rbox, rx, ry, rx+RADIO_WIDTH, ry+EDIT_HEIGHT, "Exp" );

	// Add result texts
	rbox = new_box( box, 2*W_MARGIN+RBOX_WIDTH, 2*W_MARGIN+EDIT_HEIGHT,
		BOX_WIDTH-W_MARGIN, BOX_HEIGHT-W_MARGIN, "Results" );
	float ABOX_WIDTH = BOX_WIDTH - 3*W_MARGIN - RBOX_WIDTH;
	RADIO_WIDTH = ABOX_WIDTH - 2*W_MARGIN;
	ry = W_MARGIN + BOX_SHIFT;
	e_dec = new_text( rbox, rx, ry, rx+RADIO_WIDTH, ry+EDIT_HEIGHT, B_ALIGN_RIGHT );
	e_dec->SetEnabled( false );
	ry += EDIT_HEIGHT + RADIO_DELTA;
	e_hex = new_text( rbox, rx, ry, rx+RADIO_WIDTH, ry+EDIT_HEIGHT, B_ALIGN_RIGHT );
	e_hex->SetEnabled( false );
	ry += EDIT_HEIGHT + RADIO_DELTA;
	e_bin = new_text( rbox, rx, ry, rx+RADIO_WIDTH, ry+EDIT_HEIGHT, B_ALIGN_RIGHT );
	e_bin->SetEnabled( false );
	ry += EDIT_HEIGHT + RADIO_DELTA;
	e_oct = new_text( rbox, rx, ry, rx+RADIO_WIDTH, ry+EDIT_HEIGHT, B_ALIGN_RIGHT );
	e_oct->SetEnabled( false );
	ry += EDIT_HEIGHT + RADIO_DELTA;
	e_exp = new_text( rbox, rx, ry, rx+RADIO_WIDTH, ry+EDIT_HEIGHT, B_ALIGN_RIGHT );
	e_exp->SetEnabled( false );

	// Add buttons
	float bx = width - 3*W_MARGIN - 3*BUTTON_WIDTH;
	float by = height - W_MARGIN - BUTTON_HEIGHT;
	b_eval = new_button( this, bx, by, "Evaluate", MSG_EVAL );
	bx += W_MARGIN + BUTTON_WIDTH;
	b_copy = new_button( this, bx, by, "Copy", MSG_COPY );
	bx += W_MARGIN + BUTTON_WIDTH;
	b_close = new_button( this, bx, by, "Close", B_QUIT_REQUESTED );

	// Set background color
	rgb_color kGray = {219, 219, 219, 255};
	SetViewColor (kGray);
}

void EvaluatorView::AttachedToWindow()
{
	Window()->SetDefaultButton( b_eval );
	b_close->SetTarget( Window() );
	b_eval->SetTarget( this );
	b_copy->SetTarget( this );
	e_input->SetTarget( this );
	r_dec->SetTarget( this );
	r_hex->SetTarget( this );
	r_bin->SetTarget( this );
	r_oct->SetTarget( this );
	r_exp->SetTarget( this );
	e_input->MakeFocus();
}

void EvaluatorView::MessageReceived( BMessage *msg )
{
	char *dec, *hex, *bin, *oct, *exp;
	bool b_ok = false;
	if ( msg->what == MSG_EDIT || msg->what == MSG_EVAL || msg->what == MSG_COPY )
		b_ok = evaluator.evaluate( e_input->Text(), dec, hex, bin, oct, exp );
	switch ( msg->what ) {
	case MSG_EDIT:
		if ( b_ok ) {
			e_dec->SetText( dec );
			e_hex->SetText( hex );
			e_bin->SetText( bin );
			e_oct->SetText( oct );
			e_exp->SetText( exp );
		} else {
			e_dec->SetText( "Error" );
			e_hex->SetText( "Error" );
			e_bin->SetText( "Error" );
			e_oct->SetText( "Error" );
			e_exp->SetText( "Error" );
		}
		break;
	case MSG_EVAL:
		if ( b_ok ) e_input->SetText( dec );
		else e_input->SetText( "0" );
		break;
	case MSG_COPY:
		if ( r_dec->Value() == B_CONTROL_ON ) copy( dec );
		else if ( r_hex->Value() == B_CONTROL_ON ) copy( hex );
		else if ( r_bin->Value() == B_CONTROL_ON ) copy( bin );
		else if ( r_oct->Value() == B_CONTROL_ON ) copy( oct );
		else if ( r_exp->Value() == B_CONTROL_ON ) copy( exp );
		break;
	default:
		BView::MessageReceived( msg );
	}
	if ( b_ok ) {
		delete[] dec;
		delete[] hex;
		delete[] bin;
		delete[] oct;
		delete[] exp;
	}
}

void EvaluatorView::copy( const char* s )
{
	BMessage* clip = NULL;
	if ( be_clipboard->Lock() ) {
		be_clipboard->Clear();
		if ( (clip = be_clipboard->Data()) ) {
			clip->AddData( "text/plain", B_MIME_TYPE, s, strlen( s ) );
			be_clipboard->Commit();
		}
		be_clipboard->Unlock();
	}
}

BBox* EvaluatorView::new_box( BView* parent, float x1, float y1, float x2, float y2, const char* label )
{
	BBox* box = new BBox( BRect( x1, y1, x2, y2 ) );
	box->SetLabel( label );
	parent->AddChild( box );
	return box;
}

BRadioButton* EvaluatorView::new_radio( BView* parent, float x1, float y1, float x2, float y2, const char* label )
{
	BRadioButton* radio = new BRadioButton( BRect( x1, y1, x2, y2 ),
		NULL, label, NULL );
	parent->AddChild( radio );
	return radio;
}

BTextControl* EvaluatorView::new_text( BView* parent, float x1, float y1, float x2, float y2, alignment align )
{
	BTextControl* edit = new BTextControl( BRect( x1-3, y1, x2, y2 ),
		NULL, NULL, NULL, NULL );
	edit->SetAlignment( B_ALIGN_LEFT, align );
	parent->AddChild( edit );
	return edit;
}

BButton* EvaluatorView::new_button( BView* parent, float x, float y, const char* label, uint32 msg )
{
	BButton* button = new BButton(
		BRect( x, y, x+BUTTON_WIDTH, y+BUTTON_HEIGHT ),
		NULL, label, new BMessage( msg ) );
	parent->AddChild( button );
	return button;
}

bool EvaluatorWindow::QuitRequested()
{
	be_app->PostMessage( B_QUIT_REQUESTED );
	return true;
}
