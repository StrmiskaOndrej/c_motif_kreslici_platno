/*
 * draw.c - GUX - 1. projekt - Ond¯ej Strmiska
 */

/*
 * Standard XToolkit and OSF/Motif include files.
 */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h> 

/*
 * Public include files for widgets used in this soubor.
 */
#include <Xm/MainW.h> 
#include <Xm/Form.h> 
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/MenuShell.h>
#include <Xm/CascadeB.h>
#include <Xm/Protocols.h>
#include <Xm/MessageB.h>
#include <Xm/ToggleB.h>
#include <Xm/Label.h>
#include <Xm/ComboBox.h>
#include <Xm/Scale.h>

#include <X11/keysym.h>
#include <Xm/Xm.h>


/*
 * Common C library include files
 */
#include <stdio.h>
#include <stdlib.h>

/*
 * Definice druh˘ barev
 */
 #define BARVA_POPREDI 0
#define BARVA_POZADI 1
#define BARVA_VYPLNE 2


 /* Definice jednotliv˝ch barev */
#define POCET_BAREV 11
#define BLACK 0
#define WHITE 1
#define RED 2
#define BLUE 3
#define GREEN 4
#define ORANGE 5
#define YELLOW 6
#define PURPLE 7
#define GRAY 8
#define AZURE 9
#define BROWN 10

 /* Definice druh˘ barev pro comboBox */
#define VYCHOZI_BARVA_POPREDI 0
#define VYCHOZI_BARVA_POZADI 1
#define VYCHOZI_BARVA_VYPLNE 0


 /* Definice tvar˘ */
#define VYCHOZI_TVAR 1
#define POINT 0
#define LINE 1
#define RECTANGLE 2
#define ELLIPSE 3
#define FILLED_RECTANGLE 4
#define FILLED_ELLIPSE 5

 /* Definice styl˘ */
#define VYCHOZI_STYL 0
#define PLNA 0
#define CARKOVANA 1

 /* Definice struktury pro kreslenÌ */
typedef struct drawing {
	Pixel barvaCaryPopredi;
	Pixel barvaCaryPozadi;
	Pixel barvaVyplne;

	int x1, y1;
	int x2, y2;
	int shape;
	int width;
	int line;
} Drawing;
Drawing *drawings = NULL;
Drawing inputDrawing;

int isButtonPressed; /* PromÏnn· pro zjiötÏnÌ, zda je stisknuto tlaËÌtko myöi pro kreslenÌ */
int width;  /* PromÏnn· pro nastavenÌ tlouöùky */
char *colorNames[POCET_BAREV] = { "Black", "White", "Red", "Blue", "Green", "Orange", "Yellow", "Purple", "Gray", "Azure", "Brown" };  /* NastavenÌ n·zv˘ barev */
Pixel colors[POCET_BAREV];
Colormap colorMap;
Pixel barvaCaryPopredi;
Pixel barvaCaryPozadi;
Pixel barvaVyplne;


size_t drawingsAllocated = 0;
size_t drawingsCount = 0;
int shape = VYCHOZI_TVAR;
int line = VYCHOZI_STYL;

void drawCallback(Widget, XtPointer, XtPointer);
void nastavBarvu(Widget, XtPointer, XtPointer);


 /* NastavenÌ textu */
 char *fbr[] = {
        "*menuBar.soubor.labelString: Soubor",
        "*menuBar.soubor.mnemonic: S",
        "*soubor_menu.vymazat.labelString: Vymazat",
        "*soubor_menu.vymazat.mnemonic: V",
        "*soubor_menu.vymazat.acceleratorText: Ctrl-C",
        "*soubor_menu.vymazat.accelerator: Ctrl<Key>C",
        "*soubor_menu.ukoncit.labelString: Ukoncit",
        "*soubor_menu.ukoncit.mnemonic: U",
        "*soubor_menu.ukoncit.acceleratorText: Ctrl-Q",
        "*soubor_menu.ukoncit.accelerator: Ctrl<Key>Q",

		"*quitDialog.dialogTitle: Ukonceni",
		"*quitDialog.messageString: Opravdu si prejete ukoncit aplikaci?",
		"*quitDialog.okLabelString: ANO",
		"*quitDialog.cancelLabelString: NE",
		"*quitDialog.messageAlignment: XmALIGNMENT_CENTER",

		"*nastroje.nastrojNazev.labelString: Nastroj:",
		"*nastrojeButtony.bod.labelString: Bod",
		"*nastrojeButtony.usecka.labelString: Usecka",
		"*nastrojeButtony.obdelnik.labelString: Obdelnik",
		"*nastrojeButtony.elipsa.labelString: Elipsa",
		"*nastrojeButtony.obdelnikVyplneny.labelString: Vyplneny obdelnik",
		"*nastrojeButtony.elipsaVyplnena.labelString: Vyplnena elipsa",
	
		"*nastroje.tloustkaCaryNazev.labelString: Tloustka cary:",

		"*nastroje.stylNazev.labelString: Styl cary:",
		"*stylButtony.plnaCara.labelString: Plna cara",
		"*stylButtony.carkovanaCara.labelString: Carkovana cara",

		"*.barvaCaryPoprediLabel.labelString: Barva popredi:",
		"*.barvaCaryPozadiLabel.labelString: Barva pozadi:",
		"*.barvaVyplneLabel.labelString: Barva vyplne:",

        NULL,
};

	void quitCallback(Widget, XtPointer, XtPointer);
	void quitDialogCallback(Widget, XtPointer, XtPointer);
	Widget quitDialog;

    XtAppContext app_context;

	 /* Deklarace jednotliv˝ch widget˘ */
    Widget topLevel, mainWin, menuBar, rowcol, frameLeva, framePrava, drawArea, nastroje;
	Widget soubor_shell, soubor_menu;
	Widget b_vymazat, b_ukoncit;
	Widget soubor_button;
	Widget nastrojeButtony, nastrojNazev, bod, usecka, obdelnik, elipsa, obdelnikVyplneny, elipsaVyplnena;
	Widget tloustkaCaryNazev, tloustkaCaryScale;
	Widget stylButtony, stylNazev, plnaCara, carkovanaCara;
	Widget barvaCaryPoprediLabel; Widget barvaCaryPoprediCombo; Widget barvaCaryPozadiLabel; Widget barvaCaryPozadiCombo; Widget barvaVyplneLabel; Widget barvaVyplneCombo; Widget fillBGColorLabel;


#define LINES_ALLOC_STEP	10	/* memory allocation stepping */
XSegment *lines = NULL;		/* array of line descriptors */

int x1, y1, x2, y2;		/* input points */ 

 /* Funkce pro nastavenÌ tlouöùky, funkce zavolan· ze scale widgetu */
void
   nastavTloustku(scale_w, client_data, call_data)
   Widget scale_w;
   XtPointer client_data;
   XtPointer call_data;
   {
       XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
	   width = cbs->value;

   }

 /* Funkce pro nastavenÌ tvaru objektu, funkce zavolan· aktivacÌ radio button widgetu */
void nastavTvar(Widget widget, XtPointer clientData, XtPointer callData)
{
	XmToggleButtonCallbackStruct *data = (XmToggleButtonCallbackStruct *) callData;

	if (data->set)
		shape = (intptr_t) clientData;
}

/* Funkce pro nastavenÌ stylu Ë·ry, funkce zavolan· aktivacÌ radio button widgetu */
void nastavStyl(Widget widget, XtPointer client_data, XtPointer call_data){
	XmToggleButtonCallbackStruct *data = (XmToggleButtonCallbackStruct *) call_data;

	if (data->set)
	{
		if ((intptr_t) client_data == CARKOVANA)
			line = LineDoubleDash;
		else
			line = LineSolid;
	}
}

/* Funkce pro nastavenÌ barvy jednotliv˝ch Ë·stÌ objektu, funkce zavol·na z comboBoxu */
void nastavBarvu(Widget widget, XtPointer clientData, XtPointer callData)
{
	XmComboBoxCallbackStruct *data = (XmComboBoxCallbackStruct *) callData;
	intptr_t type = (intptr_t) clientData;
	int selected = data->item_position;

	switch (type)
	{
		case BARVA_POPREDI:
			barvaCaryPopredi = colors[selected];
			break;
		case BARVA_POZADI:
			barvaCaryPozadi = colors[selected];
			break;
		case BARVA_VYPLNE:
			barvaVyplne = colors[selected];
			break;
		default:
			break;
	}
}

/* Funkce pro nastavenÌ stylu Ë·ry, funkce zavolan· aktivacÌ radio button widgetu */
void draw(Widget widget, GC gc, Drawing drawing)
{
	XGCValues values;
	int width = drawing.width;
	int x1 = drawing.x1;
	int y1 = drawing.y1;
	int x2 = drawing.x2;
	int y2 = drawing.y2;
	values.foreground = drawing.barvaCaryPopredi;
	values.line_width = drawing.width;
	values.line_style = drawing.line;

	XChangeGC(XtDisplay(widget), gc, GCLineStyle | GCLineWidth | GCForeground | GCBackground, &values);

	switch (drawing.shape)
	{
		case POINT:
			if (width > 0)
				XFillArc(XtDisplay(widget), XtWindow(widget), gc, (int) (x2 - width / 2.0), (int) (y2 - width / 2.0), width, width, 0, 360 * 64);
			else
				XDrawPoint(XtDisplay(widget), XtWindow(widget), gc, x2, y2);
			break;
		case LINE:
			XDrawLine(XtDisplay(widget), XtWindow(widget), gc, x1, y1, x2, y2);
			break;
		case FILLED_RECTANGLE:
			XSetForeground(XtDisplay(widget), gc, drawing.barvaVyplne);
			XFillRectangle(XtDisplay(widget), XtWindow(widget), gc, x1 < x2 ? x1 : x2, y1 < y2 ? y1 : y2, abs(x2 - x1), abs(y2 - y1));
			XSetForeground(XtDisplay(widget), gc, drawing.barvaCaryPopredi);
			XDrawRectangle(XtDisplay(widget), XtWindow(widget), gc, x1 < x2 ? x1 : x2, y1 < y2 ? y1 : y2, abs(x2 - x1), abs(y2 - y1));
			break;
		case RECTANGLE:
			XDrawRectangle(XtDisplay(widget), XtWindow(widget), gc, x1 < x2 ? x1 : x2, y1 < y2 ? y1 : y2, abs(x2 - x1), abs(y2 - y1));
			break;
		case FILLED_ELLIPSE:
			XSetForeground(XtDisplay(widget), gc, drawing.barvaVyplne);
			XFillArc(XtDisplay(widget), XtWindow(widget), gc, x1 - abs(x2 - x1), y1 - abs(y2 - y1), abs(x2 - x1) * 2, abs(y2 - y1) * 2, 0, 360 * 64);
			XSetForeground(XtDisplay(widget), gc, drawing.barvaCaryPopredi);
			XDrawArc(XtDisplay(widget), XtWindow(widget), gc, x1 - abs(x2 - x1), y1 - abs(y2 - y1), abs(x2 - x1) * 2, abs(y2 - y1) * 2, 0, 360 * 64);
			break;
		case ELLIPSE:
			XDrawArc(XtDisplay(widget), XtWindow(widget), gc, x1 - abs(x2 - x1), y1 - abs(y2 - y1), abs(x2 - x1) * 2, abs(y2 - y1) * 2, 0, 360 * 64);
			break;
		default:
			break;
	}
}

void motionHandler(Widget widget, XtPointer clientData, XEvent *event, Boolean *cont)
{
	static GC gc = 0;
	Pixel fg, bg;

	if (isButtonPressed)
	{
		if (!gc)
		{
			gc = XCreateGC(XtDisplay(widget), XtWindow(widget), 0, NULL);

			XtVaGetValues(widget, XmNforeground, &fg, XmNbackground, &bg, NULL);
			XSetForeground(XtDisplay(widget), gc, fg ^ bg);
			XSetFunction(XtDisplay(widget), gc, GXxor);
			XSetPlaneMask(XtDisplay(widget), gc, ~0);
		}

		if (isButtonPressed > 1)
			draw(widget, gc, inputDrawing);
		else
			isButtonPressed = 2;

		inputDrawing.x2 = event->xmotion.x;
		inputDrawing.y2 = event->xmotion.y;

		draw(widget, gc, inputDrawing);
	}
}

void exposeCallback(Widget widget, XtPointer clientData, XtPointer callData)
{
	static GC gc = 0;
	int i;

	if (drawingsCount > 0)
	{
		if (!gc)
			gc = XCreateGC(XtDisplay(widget), XtWindow(widget), 0, NULL);

		for (i = 0; i < drawingsCount; i++)
			draw(widget, gc, drawings[i]);
	}
}

void drawCallback(Widget widget, XtPointer clientData, XtPointer callData)
{
	XmDrawingAreaCallbackStruct *data = (XmDrawingAreaCallbackStruct *) callData;

	switch (data->event->type)
	{
		case ButtonPress:
			if (data->event->xbutton.button == Button1)
			{
				inputDrawing.barvaCaryPopredi = barvaCaryPopredi ^ colors[VYCHOZI_BARVA_POZADI];
				inputDrawing.barvaCaryPozadi = barvaCaryPozadi ^ colors[VYCHOZI_BARVA_POZADI];
				inputDrawing.barvaVyplne = barvaVyplne ^ colors[VYCHOZI_BARVA_VYPLNE];
				inputDrawing.x1 = data->event->xbutton.x;
				inputDrawing.y1 = data->event->xbutton.y;
				inputDrawing.shape = shape;
				inputDrawing.width = width;
				inputDrawing.line = line;
				isButtonPressed = 1;
			}
			break;
		case ButtonRelease:
			if (data->event->xbutton.button == Button1)
			{
				if (drawingsCount >= drawingsAllocated)
				{
					drawingsAllocated = drawingsAllocated ? drawingsAllocated << 1 : 1;
					drawings = (Drawing *) realloc(drawings, drawingsAllocated * sizeof(Drawing));
				}

				drawings[drawingsCount].barvaCaryPopredi = inputDrawing.barvaCaryPopredi ^ colors[VYCHOZI_BARVA_POZADI];
				drawings[drawingsCount].barvaCaryPozadi = inputDrawing.barvaCaryPozadi ^ colors[VYCHOZI_BARVA_POZADI];
				drawings[drawingsCount].barvaVyplne = inputDrawing.barvaVyplne ^ colors[VYCHOZI_BARVA_VYPLNE];
				drawings[drawingsCount].x2 = data->event->xbutton.x;
				drawings[drawingsCount].y2 = data->event->xbutton.y;
				drawings[drawingsCount].shape = inputDrawing.shape;
				drawings[drawingsCount].width = inputDrawing.width;
				drawings[drawingsCount].line = inputDrawing.line;
				drawings[drawingsCount].x1 = inputDrawing.x1;
				drawings[drawingsCount].y1 = inputDrawing.y1;
				isButtonPressed = 0;
				drawingsCount++;

				XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
			}
			break;
		default:
			break;
	}
}

/* Funkce pro vyËiötÏnÌ plochy, zavol·no z tlaËÌtka v menu */
void vymaz(Widget w, XtPointer client_data, XtPointer call_data)
{ 
    if (NULL != drawings)
		{
			free(drawings);

			drawingsAllocated = 0;
			drawingsCount = 0;
			drawings = NULL;
		}

		XClearWindow(XtDisplay(drawArea), XtWindow(drawArea));
}

/* Funkce pro UkonËenÌ aplikace, zavol·no z tlaËÌtka v menu */
void ukonci(Widget w, XtPointer client_data, XtPointer call_data)
{ 

    XtManageChild(quitDialog); 
}

/* Funkce pro spuötÏnÌ ukonËovacÌho dialogu */
void quitDialogCallback(Widget widget, XtPointer clientData, XtPointer callData)
{
	if ((intptr_t) clientData == 1)
	{
	
		exit(0);
	}
}

/* SpouötÏcÌ funkce */
int main(int argc, char **argv)
{
    XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);

	/* Widget aplikace */
    topLevel = XtVaAppInitialize(
      &app_context,		 	/* Application context */
      "Draw",				/* Application class */
      NULL, 0,				/* command line option list */
      &argc, argv,			/* command line args */
      fbr,				/* for missing app-defaults file */
	  XmNdeleteResponse, XmDO_NOTHING,
      NULL);				/* terminate varargs list */

	/* NastavenÌ barvy */
	  XColor xcolor, spare;
	char *colorName;
	int i;
	colorMap = DefaultColormapOfScreen(XtScreen(topLevel));

	for (i = 0; i < POCET_BAREV; i++)
	{
		colorName = colorNames[i];
		if (XAllocNamedColor(XtDisplay(topLevel), colorMap, colorName, &xcolor, &spare) == 0)
		{
			fprintf(stderr, "Error: Cannot allocate colormap entry for \"%s\"\n", colorName);
			exit(1);
		}

		colors[i] = xcolor.pixel;
	}
	barvaCaryPopredi = colors[VYCHOZI_BARVA_POPREDI];
	barvaCaryPozadi = colors[VYCHOZI_BARVA_POZADI];
	barvaVyplne = colors[VYCHOZI_BARVA_VYPLNE];

	/* HlavnÌ okno aplikace */
    mainWin = XtVaCreateManagedWidget(
      "mainWin",			/* widget name */
      xmMainWindowWidgetClass,		/* widget class */
      topLevel,				/* parent widget*/
      XmNcommandWindowLocation, XmCOMMAND_BELOW_WORKSPACE,
	  XmNwidth, 1000,			/* set startup width */
      XmNheight, 700,			/* set startup height */
      NULL);				/* terminate varargs list */

	  /* Menu aplikace */
	 menuBar = XtVaCreateManagedWidget(
	 "menuBar", 
     xmRowColumnWidgetClass,  
	 mainWin,  
     XmNrowColumnType,  XmMENU_BAR,  
     NULL);  
	
	/* V˝suvnÈ menu */
        soubor_shell = XtVaCreatePopupShell("soubor_shell",
			xmMenuShellWidgetClass, menuBar,
			XmNwidth, 1, 
			XmNheight, 1,
			XmNallowShellResize, True,
			XmNoverrideRedirect, True,
			NULL);
	soubor_menu = XtVaCreateWidget("soubor_menu",
			xmRowColumnWidgetClass, soubor_shell,
			XmNrowColumnType, XmMENU_PULLDOWN,
			NULL);

	/* tlaËÌtko pro vysunutÌ menu */
	soubor_button = XtVaCreateManagedWidget("soubor",
			xmCascadeButtonWidgetClass, menuBar,
			XmNsubMenuId, soubor_menu,
			NULL);

	/* tlaËÌtka ve v˝suvnÈm menu */
	b_vymazat = XtVaCreateManagedWidget("vymazat",
			xmPushButtonWidgetClass,
			soubor_menu,
			NULL);
	XtAddCallback(b_vymazat, XmNactivateCallback, vymaz, drawArea);
	b_ukoncit = XtVaCreateManagedWidget("ukoncit",
			xmPushButtonWidgetClass,
			soubor_menu,
			NULL);
	XtAddCallback(b_ukoncit, XmNactivateCallback, ukonci, 0);

	//RozloûenÌ hlavnÌho okna
	rowcol = XtVaCreateManagedWidget(
      "rowcol",				/* widget name */
      xmRowColumnWidgetClass,		/* widget class */
      mainWin,				/* parent widget */
	  XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
      XmNorientation, XmHORIZONTAL,	/* orientation */
      XmNpacking, XmPACK_TIGHT,	/* packing mode */
      NULL);

	/* Lev· Ë·st hlavnÌho okna */
    frameLeva = XtVaCreateManagedWidget(
      "frameLeva",				/* widget name */
      xmFrameWidgetClass,		/* widget class */
      rowcol,				/* parent widget */
	  XmNwidth,        200,
      XmNheight,       695,
      NULL);				/* terminate varargs list */

	/* Prav· Ë·st hlavnÌho okna */
	framePrava = XtVaCreateManagedWidget(
    "framePrava",				/* widget name */
    xmFrameWidgetClass,		/* widget class */
    rowcol,				/* parent widget */

	XmNwidth,        780,
      XmNheight,       695,
	  XmNresizePolicy, XmNONE,

    NULL);			
	/*KreslÌcÌ plocha*/
    drawArea = XtVaCreateManagedWidget(
      "drawingArea",			/* widget name */
      xmDrawingAreaWidgetClass,		/* widget class */
      framePrava,				/* parent widget*/
	 XmNbackground, colors[WHITE],
      NULL);				/* terminate varargs list */

	XtAddEventHandler(drawArea, ButtonMotionMask, False, motionHandler, NULL);
	XtAddCallback(drawArea, XmNexposeCallback, exposeCallback, drawArea);
	XtAddCallback(drawArea, XmNinputCallback, drawCallback, drawArea);
     
	 /*Widget pro n·stroje */  
    nastroje = XtVaCreateManagedWidget(
      "nastroje",			/* widget name */
      xmRowColumnWidgetClass,		/* widget class */
      frameLeva,				/* parent widget */
      XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
      XmNorientation, XmVERTICAL,	/* orientation */
      XmNpacking, XmPACK_TIGHT,	/* packing mode */
      NULL);				/* terminate varargs list */
	 
	 /*Widget pro nastavenÌ tvar˘*/
	 nastrojNazev = XtVaCreateManagedWidget("nastrojNazev", xmLabelWidgetClass, nastroje, NULL);
	  nastrojeButtony = XtVaCreateManagedWidget(
      "nastrojeButtony",			/* widget name */
      xmRowColumnWidgetClass,		/* widget class */
      nastroje,				/* parent widget */
      XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
      XmNorientation, XmVERTICAL,	/* orientation */
      XmNpacking, XmPACK_TIGHT,	/* packing mode */
	  XmNradioBehavior, True,
	  XmNisHomogeneous, True,
	  XmNentryClass, xmToggleButtonWidgetClass,
      NULL);
    
	/* NÌûe jsou uvedenÈ widgety, pro jednotlivÈ radio buttony tvar˘ */
	bod = XtVaCreateManagedWidget("bod",
	xmToggleButtonWidgetClass, nastrojeButtony,
	NULL);
	XtAddCallback(bod, XmNvalueChangedCallback, nastavTvar, (XtPointer)0);

	usecka = XtVaCreateManagedWidget("usecka",
	xmToggleButtonWidgetClass, nastrojeButtony,
	NULL);
	XtAddCallback(usecka, XmNvalueChangedCallback, nastavTvar, (XtPointer)1);

	obdelnik = XtVaCreateManagedWidget("obdelnik",
	xmToggleButtonWidgetClass, nastrojeButtony,
	NULL);
	XtAddCallback(obdelnik, XmNvalueChangedCallback, nastavTvar, (XtPointer)2);
	
	elipsa = XtVaCreateManagedWidget("elipsa",
	xmToggleButtonWidgetClass, nastrojeButtony,
	NULL);
	XtAddCallback(elipsa, XmNvalueChangedCallback, nastavTvar, (XtPointer)3);
	
	obdelnikVyplneny = XtVaCreateManagedWidget("obdelnikVyplneny",
	xmToggleButtonWidgetClass, nastrojeButtony,
	NULL);
	XtAddCallback(obdelnikVyplneny, XmNvalueChangedCallback, nastavTvar, (XtPointer)4);
	
	elipsaVyplnena = XtVaCreateManagedWidget("elipsaVyplnena",
	xmToggleButtonWidgetClass, nastrojeButtony,
	NULL);
	XtAddCallback(elipsaVyplnena, XmNvalueChangedCallback, nastavTvar, (XtPointer)5);

	/*NastavenÌ tlouöùky Ëar 0-8, pomocÌ widgetu scale*/
	tloustkaCaryNazev = XtVaCreateManagedWidget("tloustkaCaryNazev", xmLabelWidgetClass, nastroje, NULL);
	tloustkaCaryScale = XtVaCreateManagedWidget ("tloustkaCaryScale",
           xmScaleWidgetClass, nastroje,
           XmNmaximum,   8,
           XmNminimum,   0,
           XmNvalue,     1,
           XmNshowValue, True,
		   XmNorientation, XmHORIZONTAL,
           NULL);
	XtAddCallback (tloustkaCaryScale, XmNvalueChangedCallback, nastavTloustku, NULL);


	/*Widget pro volbu stylu Ë·ry*/
	stylNazev = XtVaCreateManagedWidget("stylNazev", xmLabelWidgetClass, nastroje, NULL);
	  stylButtony = XtVaCreateManagedWidget(
      "stylButtony",			/* widget name */
      xmRowColumnWidgetClass,		/* widget class */
      nastroje,				/* parent widget */
      XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
      XmNorientation, XmVERTICAL,	/* orientation */
      XmNpacking, XmPACK_TIGHT,	/* packing mode */
	  XmNradioBehavior, True,
	  XmNisHomogeneous, True,
	  XmNentryClass, xmToggleButtonWidgetClass,
      NULL);

    /* widget radioButtonu pro nastavenÌ plnÈ Ë·ry */
	  plnaCara = XtVaCreateManagedWidget("plnaCara",
	xmToggleButtonWidgetClass, stylButtony,
	NULL);
	XtAddCallback(plnaCara, XmNvalueChangedCallback, nastavStyl, (XtPointer)0);

	 /* widget radioButtonu pro nastavenÌ Ë·rkovanÈ Ë·ry */
	carkovanaCara = XtVaCreateManagedWidget("carkovanaCara",
	xmToggleButtonWidgetClass, stylButtony,
	NULL);
	XtAddCallback(carkovanaCara, XmNvalueChangedCallback, nastavStyl, (XtPointer)1);

	// BARVY
	XmString black = XmStringCreateSimple("Cerna");
	XmString white = XmStringCreateSimple("Bila");
	XmString red   = XmStringCreateSimple("Cervena");
	XmString blue  = XmStringCreateSimple("Modra");
	XmString green  = XmStringCreateSimple("Zelena");
	XmString orange = XmStringCreateSimple("Oranzova");
	XmString yellow = XmStringCreateSimple("Zluta");
	XmString purple  = XmStringCreateSimple("Fialova");
	XmString gray  = XmStringCreateSimple("Seda");
	XmString azure  = XmStringCreateSimple("Azurova");
	XmString brown  = XmStringCreateSimple("Hneda");
	XmString colorStrings[] = { black, white, red, blue, green, orange, yellow, purple, gray, azure, brown };

	// BARVA POPÿEDÕ
	barvaCaryPoprediLabel = XtVaCreateManagedWidget("barvaCaryPoprediLabel", xmLabelWidgetClass, nastroje, NULL);
	barvaCaryPoprediCombo = XtVaCreateManagedWidget(
		"barvaCaryPoprediCombo",
		xmComboBoxWidgetClass,
		nastroje,
		XmNcomboBoxType, XmDROP_DOWN_LIST,
		XmNitemCount, POCET_BAREV,
		XmNitems, colorStrings,
		XmNselectedPosition, VYCHOZI_BARVA_POPREDI,
		NULL);

	// BARVA POZADÕ
	barvaCaryPozadiLabel = XtVaCreateManagedWidget("barvaCaryPozadiLabel", xmLabelWidgetClass, nastroje, NULL);
	barvaCaryPozadiCombo = XtVaCreateManagedWidget(
		"barvaCaryPozadiCombo",
		xmComboBoxWidgetClass,
		nastroje,
		XmNcomboBoxType, XmDROP_DOWN_LIST,
		XmNitemCount, POCET_BAREV,
		XmNitems, colorStrings,
		XmNselectedPosition, VYCHOZI_BARVA_POZADI,
		NULL);

	// VYPLNENÕ POPÿEDÕ
	barvaVyplneLabel = XtVaCreateManagedWidget("barvaVyplneLabel", xmLabelWidgetClass, nastroje, NULL);
	barvaVyplneCombo = XtVaCreateManagedWidget(
		"barvaVyplneCombo",
		xmComboBoxWidgetClass,
		nastroje,
		XmNcomboBoxType, XmDROP_DOWN_LIST,
		XmNitemCount, POCET_BAREV,
		XmNitems, colorStrings,
		XmNselectedPosition, VYCHOZI_BARVA_VYPLNE,
		NULL);


	XtAddCallback(barvaCaryPoprediCombo, XmNselectionCallback, nastavBarvu, (XtPointer) BARVA_POPREDI);
	XtAddCallback(barvaCaryPozadiCombo, XmNselectionCallback, nastavBarvu, (XtPointer) BARVA_POZADI);
	XtAddCallback(barvaVyplneCombo, XmNselectionCallback, nastavBarvu, (XtPointer) BARVA_VYPLNE);


	XmStringFree(black);
	XmStringFree(white);
	XmStringFree(red);
	XmStringFree(blue);

	 /* UkonËujÌcÌ dialog */
	quitDialog = XmCreateQuestionDialog(topLevel, "quitDialog", NULL, 0);
	XtVaSetValues(quitDialog, XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL, NULL);
	XtUnmanageChild(XmMessageBoxGetChild(quitDialog, XmDIALOG_HELP_BUTTON));
	XtAddCallback(quitDialog, XmNcancelCallback, quitDialogCallback, (XtPointer) 0);
	XtAddCallback(quitDialog, XmNokCallback, quitDialogCallback, (XtPointer) 1);

	 /* Zavol·nÌ funkcÌ pro vykreslenÌ */
	XtAddEventHandler(drawArea, ButtonMotionMask, False, motionHandler, NULL);
	XtAddCallback(drawArea, XmNexposeCallback, exposeCallback, drawArea);
	XtAddCallback(drawArea, XmNinputCallback, drawCallback, drawArea);


	XtManageChild(mainWin);
	
	XtManageChild(menuBar);
	XtManageChild(nastrojeButtony);
	XtManageChild(tloustkaCaryScale);
	XtManageChild(stylButtony);
	XtManageChild(drawArea);
	XtManageChild (rowcol);
	
	 /* zavol·nÌ ukonËovacÌho dialogu p¯i zavÌr·nÌ aplikace */
	Atom wmDelete = XInternAtom(XtDisplay(topLevel), "WM_DELETE_WINDOW", False);
	XmAddWMProtocolCallback(topLevel, wmDelete, ukonci, NULL);

    XtRealizeWidget(topLevel);

    XtAppMainLoop(app_context);
	
	if (drawings != NULL)
		free(drawings);

    return 0;
}

