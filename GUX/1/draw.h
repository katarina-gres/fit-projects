/*
 * First project for lecture GUX.
 * File: draw.h
 * Author: Vojtech Havlena, xhavle03@stud.fit.vutbr.cz
 * Date: 26.10.2016
 */

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

/*
 * Public include files for widgets used in this file.
 */
#include <Xm/MainW.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ComboBox.h>
#include <Xm/Label.h>
#include <Xm/DrawnB.h>
#include <Xm/Protocols.h>
#include <X11/Xmu/Editres.h>
#include <Xm/MessageB.h>

/*
 * Common C library include files
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


/*
 * Minimum macro for drawing shapes.
 */
#define min(a,b) (((a) < (b)) ? (a) : (b))

/*
 * Constants
 */
#define SHAPE_BUTTONS 4 /* number of shape buttons */
#define PAIR_PANELS 10 /* number of combobox -- label panels */
#define LINES_ALLOC_STEP	10	/* memory allocation stepping */
#define COLORS 9 /* number of colors */

/*
 * Structures and enums
 */

typedef enum
{
   FILL,
   NONE
} FILL_STYLE;

typedef enum
{
   PT,
   LINE,
   RECT,
   ELLIPSE
} SHAPE_TYPE;

typedef struct
{
   int x1;
   int x2;
   int y1;
   int y2;
} POINT;

typedef struct
{
   SHAPE_TYPE type;
   POINT coord;
   int lineWidth;
   int lineStyle;
   int lineFG;
   int lineBG;
   FILL_STYLE fillStyle;
   int fillFG;
   int fillBG;
} SHAPE;


/*
 * Shared variables
 */
SHAPE *lines = NULL;		/* array of line descriptors */
int maxlines = 0;		/* space allocated for max lines */
int nlines = 0;			/* current number of lines */

GC drawGC = 0;			/* GC used for final drawing */
GC inputGC = 0;			/* GC used for drawing current position */

int button_pressed = 0;		/* input state */
SHAPE currentShape; /* current shape */

/* Widgets */
Widget topLevel, mainWin, rowColumn, frame, drawArea, quitBtn, clearBtn,
shapeButtons[SHAPE_BUTTONS], pairPanels[PAIR_PANELS], comboLineForeground,
comboLineBackground, comboFillBackground, comboFillForeground, comboLineWidth, comboLineStyle,
comboFillStyle;

Widget question;
Atom wm_delete;

/* string constants */
char *buttonLabels[] = {"Point", "Line", "Rect", "Ellipse"};
char *colors[] = { "black", "red", "green", "blue", "orange", "purple",
   "pink", "white", "yellow" };
unsigned long colorPixel[COLORS];
char *lineWidth[] = {"0", "3", "8"};
char *lineStyle[] = {"solid", "dashed"};
char *fillStyle[] = {"none", "fill"};

char *panelsName[] = {"panel1", "panel2", "panel3", "panel4", 
   "panel5", "panel6", "panel7", "panel8", "panel9", "panel10"};
char *buttonsName[] = {"btn1", "btn2", "btn3", "btn4"};

char *fall[] = {
   "*question.dialogTitle: Final question",
   "*question.messageString: Are you sure want to quit?",
   "*question.okLabelString: Yes",
   "*question.cancelLabelString: No",
   "*question.messageAlignment: XmALIGNMENT_CENTER",
   NULL
};

/*
 * Function declaration
 */
void CreateComboBox(Widget *widget, Widget parent, char *items[], int itemsCount);
void DrawShape(Widget w, GC gc, SHAPE shape);
void InputLineEH(Widget w, XtPointer client_data, XEvent *event, Boolean *cont);
void DrawLineCB(Widget w, XtPointer client_data, XtPointer call_data);
void ExposeCB(Widget w, XtPointer client_data, XtPointer call_data);
void ClearCB(Widget w, XtPointer client_data, XtPointer call_data);
void ShapeCB(Widget w, XtPointer client_data, XtPointer call_data);
void ShapeCB(Widget w, XtPointer client_data, XtPointer call_data);

void SelectionLineWidth(Widget w, XtPointer client_data, XtPointer call_data);
void SelectionLineStyle(Widget w, XtPointer client_data, XtPointer call_data);
void SelectionLineFg(Widget w, XtPointer client_data, XtPointer call_data);
void SelectionLineBg(Widget w, XtPointer client_data, XtPointer call_data);
void SelectionFillBg(Widget w, XtPointer client_data, XtPointer call_data);
void SelectionFillFg(Widget w, XtPointer client_data, XtPointer call_data);
void SelectionFillStyle(Widget w, XtPointer client_data, XtPointer call_data);
void QuestionCB(Widget w, XtPointer client_data, XtPointer call_data);
void QuitCB(Widget w, XtPointer client_data, XtPointer call_data);

void DrawShape(Widget w, GC gc, SHAPE shape);
void CreateComboBox(Widget *widget, Widget parent, char *items[], int itemsCount);
int GetIndex(char *array[], int size, char * str);
int AllocateColors(Widget w);
void SelectDefaultShape();
