/*
 * First project for lecture GUX.
 * File: draw.c
 * Author: Vojtech Havlena, xhavle03@stud.fit.vutbr.cz
 * Date: 26.10.2016
 */

#include "draw.h"

/*
* "InputLine" event handler
*/
void InputLineEH(Widget w, XtPointer client_data, XEvent *event, Boolean *cont)
{
   if (button_pressed)
   {
      if (!inputGC)
      {
         inputGC = XCreateGC(XtDisplay(w), XtWindow(w), 0, NULL);
      }

      XSetFunction(XtDisplay(w), inputGC, GXxor);
      XSetLineAttributes(XtDisplay(w), inputGC,
      currentShape.lineWidth, currentShape.lineStyle, CapRound, JoinRound);

      if (button_pressed > 1)
      {
         /* erase previous position */
         DrawShape(w, inputGC, currentShape);
      }
      else
      {
         /* remember first MotionNotify */
         button_pressed = 2;
      }

      /* set current shape coord. */
      currentShape.coord.x2 = event->xmotion.x;
      currentShape.coord.y2 = event->xmotion.y;
      DrawShape(w, inputGC, currentShape);
   }
}

/*
 * Draw line callback function
 */
void DrawLineCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   int fore, back; /* background, foreground color */
   XmDrawingAreaCallbackStruct *d = (XmDrawingAreaCallbackStruct*) call_data;

   switch (d->event->type)
   {
      case ButtonPress:
         if (d->event->xbutton.button == Button1)
         {
            button_pressed = 1;
            currentShape.coord.x1 = d->event->xbutton.x;
            currentShape.coord.y1 = d->event->xbutton.y;
         }
         break;

      case ButtonRelease:
         if (d->event->xbutton.button == Button1)
         {
            /* realocation shape array */
            if (++nlines > maxlines)
            {
               maxlines += LINES_ALLOC_STEP;
               lines = (SHAPE*) XtRealloc((char*)lines,
               (Cardinal)(sizeof(SHAPE) * maxlines));
            }

            lines[nlines - 1] = currentShape;
            lines[nlines - 1].coord.x2 = d->event->xbutton.x;
            lines[nlines - 1].coord.y2 = d->event->xbutton.y;

            button_pressed = 0;

            if (!drawGC)
            {
               drawGC = XCreateGC(XtDisplay(w), XtWindow(w), 0 , NULL);
            }

            XtVaGetValues(w, XmNforeground, &fore, XmNbackground, &back, NULL);
            lines[nlines - 1].lineFG =  back ^ currentShape.lineFG;
            lines[nlines - 1].lineBG = back ^ currentShape.lineBG;
            lines[nlines - 1].fillFG =  back ^ currentShape.fillFG;
            lines[nlines - 1].fillBG = back ^ currentShape.fillBG;

            DrawShape(w, drawGC, lines[nlines - 1]);
         }
         break;
   }
}

/*
 * Expose callback function
 */
void ExposeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   int i;

   if (nlines <= 0)
      return;
   if (!drawGC)
   {
      drawGC = XCreateGC(XtDisplay(w), XtWindow(w), 0 , NULL);
   }

   /* draw stored shapes */
   for(i = 0; i < nlines; i++)
   {
      DrawShape(w, drawGC, lines[i]);
   }
}

/*
 * Clear button callback function.
 */
void ClearCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Widget wcd = (Widget) client_data;

   nlines = 0;
   XClearWindow(XtDisplay(wcd), XtWindow(wcd));
}

/*
 * Shape selection button callback function (shape depend on client_data).
 */
void ShapeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   int type = (uintptr_t)client_data;
   int i;

   /* set button shadow */
   for(i = 0; i < SHAPE_BUTTONS; i++)
   {
      XtVaSetValues (shapeButtons[i], XmNpushButtonEnabled, 1,
         XmNshadowType, XmSHADOW_ETCHED_OUT, NULL);
   }
   XtVaSetValues (w, XmNpushButtonEnabled, 0,
      XmNshadowType, XmSHADOW_ETCHED_IN, NULL);

   currentShape.type = type;
}

/*
 * Combobox selection line width callback.
 */
void SelectionLineWidth(Widget w, XtPointer client_data, XtPointer call_data)
{
   XmComboBoxCallbackStruct *cb = (XmComboBoxCallbackStruct *) call_data;
   char *choice = (char *) XmStringUnparse (cb->item_or_text, XmFONTLIST_DEFAULT_TAG,
      XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);

   if(cb->event != NULL)
   {
      currentShape.lineWidth = atoi(choice);
   }
   XtFree (choice);
}

/*
 * Combobox selection line style callback.
 */
void SelectionLineStyle(Widget w, XtPointer client_data, XtPointer call_data)
{
   XmComboBoxCallbackStruct *cb = (XmComboBoxCallbackStruct *) call_data;
   char *choice = (char *) XmStringUnparse (cb->item_or_text,
         XmFONTLIST_DEFAULT_TAG,XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);

   if(cb->event != NULL)
   {
      if(strcmp(choice, "solid") == 0)
      {
         currentShape.lineStyle = LineSolid;
      }
      else if(strcmp(choice, "dashed") == 0)
      {
         currentShape.lineStyle = LineDoubleDash;
      }
   }
   XtFree (choice);
}

/*
 * Combobox selection line foreground callback.
 */
void SelectionLineFg(Widget w, XtPointer client_data, XtPointer call_data)
{
   XmComboBoxCallbackStruct *cb = (XmComboBoxCallbackStruct *) call_data;
   char *choice = (char *) XmStringUnparse (cb->item_or_text,
      XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
   int index;

   if(cb->event != NULL)
   {
      index = GetIndex(colors, COLORS, choice);
      if(index != -1)
         currentShape.lineFG = colorPixel[index];
   }
   XtFree (choice);
}

/*
 * Combobox selection line background callback.
 */
void SelectionLineBg(Widget w, XtPointer client_data, XtPointer call_data)
{
   XmComboBoxCallbackStruct *cb = (XmComboBoxCallbackStruct *) call_data;
   char *choice = (char *) XmStringUnparse (cb->item_or_text, XmFONTLIST_DEFAULT_TAG,
      XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
   int index;

   if(cb->event != NULL)
   {
      index = GetIndex(colors, COLORS, choice);
      if(index != -1)
         currentShape.lineBG = colorPixel[index];
   }
   XtFree (choice);
}

/*
 * Combobox selection fill background callback.
 */
void SelectionFillBg(Widget w, XtPointer client_data, XtPointer call_data)
{
   XmComboBoxCallbackStruct *cb = (XmComboBoxCallbackStruct *) call_data;
   char *choice = (char *) XmStringUnparse (cb->item_or_text, XmFONTLIST_DEFAULT_TAG,
      XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
   int index;

   if(cb->event != NULL)
   {
      index = GetIndex(colors, COLORS, choice);
      if(index != -1)
         currentShape.fillBG = colorPixel[index];
   }
   XtFree (choice);
}

/*
 * Combobox selection fill foreground callback.
 */
void SelectionFillFg(Widget w, XtPointer client_data, XtPointer call_data)
{
   XmComboBoxCallbackStruct *cb = (XmComboBoxCallbackStruct *) call_data;
   char *choice = (char *) XmStringUnparse (cb->item_or_text, XmFONTLIST_DEFAULT_TAG,
      XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
   int index;

   if(cb->event != NULL)
   {
      index = GetIndex(colors, COLORS, choice);
      if(index != -1)
         currentShape.fillFG = colorPixel[index];
   }
   XtFree (choice);
}

/*
 * Combobox selection fill style callback.
 */
void SelectionFillStyle(Widget w, XtPointer client_data, XtPointer call_data)
{
   XmComboBoxCallbackStruct *cb = (XmComboBoxCallbackStruct *) call_data;
   char *choice = (char *) XmStringUnparse (cb->item_or_text, XmFONTLIST_DEFAULT_TAG,
      XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);

   if(cb->event != NULL)
   {
      if(strcmp(choice, "fill") == 0)
      {
         currentShape.fillStyle = FILL;
      }
      else
      {
         currentShape.fillStyle = NONE;
      }
   }
   XtFree (choice);
}

/*
 * Free shapes array.
 */
void FreeShapes()
{
   XtFree((char *)lines);
}

/*
 * Dialog question callback
 */
void QuestionCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   switch ((uintptr_t)client_data)
   {
      case 0: /* ok */
         FreeShapes();
         exit(0);
         break;
      case 1: /* cancel */
         break;
   }
}

/*
 * Quit button callback.
 */
void QuitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   XtManageChild(question);
}

/*
 * Draw given shape (contains info about color, shape type, line width ...).
 */
void DrawShape(Widget w, GC gc, SHAPE shape)
{
   int dx, dy, fg, bg;

   XtVaGetValues(w, XmNforeground, &fg, XmNbackground, &bg, NULL);
   XSetLineAttributes(XtDisplay(w), gc,
      shape.lineWidth, shape.lineStyle, CapRound, JoinRound);

   if(shape.fillStyle == FILL)
   {
      XSetForeground(XtDisplay(w), gc, bg ^ shape.fillFG);
      XSetBackground(XtDisplay(w), gc, bg ^ shape.fillBG);
   }

   switch(shape.type)
   {
      case LINE:
         XSetForeground(XtDisplay(w), gc, bg ^ shape.lineFG);
         XSetBackground(XtDisplay(w), gc, bg ^ shape.lineBG);
         XDrawLine(XtDisplay(w), XtWindow(w), gc, shape.coord.x1,
                shape.coord.y1, shape.coord.x2, shape.coord.y2);
         break;

      case RECT:
         if(shape.fillStyle == FILL)
         {
            XFillRectangle(XtDisplay(w), XtWindow(w), gc, min(shape.coord.x1,
                 shape.coord.x2), min(shape.coord.y1, shape.coord.y2),
                 abs(shape.coord.x1-shape.coord.x2), abs(shape.coord.y1-shape.coord.y2));
         }

         XSetForeground(XtDisplay(w), gc, bg ^ shape.lineFG);
         XSetBackground(XtDisplay(w), gc, bg ^ shape.lineBG);
         XDrawRectangle(XtDisplay(w), XtWindow(w), gc, min(shape.coord.x1,
            shape.coord.x2), min(shape.coord.y1, shape.coord.y2),
            abs(shape.coord.x1-shape.coord.x2), abs(shape.coord.y1-shape.coord.y2));
         break;

      case ELLIPSE:
         dx = abs(shape.coord.x1- shape.coord.x2);
         dy = abs(shape.coord.y1- shape.coord.y2);
         if(shape.fillStyle == FILL)
         {
            XFillArc(XtDisplay(w), XtWindow(w), gc,
               shape.coord.x1-dx, shape.coord.y1-dy, 2*dx, 2*dy, 0, 360*64);
         }
         XSetForeground(XtDisplay(w), gc, bg ^ shape.lineFG);
         XSetBackground(XtDisplay(w), gc, bg ^ shape.lineBG);
         XDrawArc(XtDisplay(w), XtWindow(w), gc,
            shape.coord.x1-dx, shape.coord.y1-dy, 2*dx, 2*dy, 0, 360*64);
         break;

      case PT:
         XSetForeground(XtDisplay(w), gc, bg ^ shape.lineFG);
         XSetBackground(XtDisplay(w), gc, bg ^ shape.lineBG);
         if(shape.lineWidth == 0)
         {
            XDrawPoint(XtDisplay(w), XtWindow(w), gc, shape.coord.x1,
               shape.coord.y1);
         }
         else
         {
            dx = abs(shape.coord.x1- shape.lineWidth);
            dy = abs(shape.coord.y1- shape.lineWidth);
            XFillArc(XtDisplay(w), XtWindow(w), gc, shape.coord.x1 - (shape.lineWidth / 2),
               shape.coord.y1 - (shape.lineWidth / 2), shape.lineWidth, shape.lineWidth, 0, 360*64);
         }
         break;
    }
}

/*
 * Create widget combobox with given items.
 */
void CreateComboBox(Widget *widget, Widget parent, char *items[], int itemsCount)
{
   int i;
   XmStringTable strList = (XmStringTable) XtMalloc (itemsCount * sizeof (XmString *));

   for (i = 0; i < itemsCount; i++)
   strList[i] = XmStringCreateLocalized (items[i]);

   *widget = XtVaCreateManagedWidget("combo", xmComboBoxWidgetClass, parent, XmNcomboBoxType,
      XmDROP_DOWN_LIST, XmNitems, strList, XmNitemCount, itemsCount, NULL);

   for (i = 0; i < itemsCount; i++)
      XmStringFree (strList[i]);
   XtFree ((XtPointer) strList);
}

/*
 * Get array item index.
 */
int GetIndex(char *array[], int size, char * str)
{
   int i;
   for(i = 0; i < size; i++)
   {
      if(strcmp(array[i], str) == 0)
         return i;
   }
   return -1;
}

/*
 * Select shape at startup.
 */
void SelectDefaultShape()
{
   currentShape.type = LINE;
   currentShape.fillStyle = NONE;
   currentShape.lineWidth = 0;
   currentShape.lineStyle = LineSolid;

   currentShape.lineBG = colorPixel[0];
   currentShape.lineFG = colorPixel[0];
   currentShape.fillBG = colorPixel[0];
   currentShape.fillFG = colorPixel[0];
}

/*
 * Allocate colors.
 */
int AllocateColors(Widget w)
{
   XColor xcolour, spare;	/* xlib color struct */
   int i;
   Colormap cmap;

   cmap = DefaultColormapOfScreen(XtScreen(w));
   for(i  = 0; i < COLORS; i++)
   {
      if (XAllocNamedColor(XtDisplay(w), cmap, colors[i], &xcolour, &spare) == 0)
      {
         return -1;
      }
      colorPixel[i] = xcolour.pixel;
   }
   return 0;
}

/*
 * main.
 */
int main(int argc, char **argv)
{
   int i;
   XmString btnText;
   XtAppContext app_context;


   /*
   * Register the default language procedure
   */
   XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);

   topLevel = XtVaAppInitialize(
      &app_context,		 	/* Application context */
      "draw",				/* Application class */
      NULL, 0,				/* command line option list */
      &argc, argv,			/* command line args */
      fall,				/* for missing app-defaults file */
      XmNheight, (int)600,
      XmNwidth, (int)450,
      XmNminWidth, 350,
      XmNminHeight, 400,
      XmNdeleteResponse, XmDO_NOTHING,
      NULL);

      /*ac = 0;
    	XtSetArg(al[ac], XmNscrollBarDisplayPolicy, XmAS_NEEDED); ac++;
    	XtSetArg(al[ac], XmNscrollingPolicy, XmAUTOMATIC); ac++;*/
   mainWin = XtVaCreateManagedWidget(
      "mainWin",			/* widget name */
      xmMainWindowWidgetClass,		/* widget class */
      topLevel,				/* parent widget*/
      XmNcommandWindowLocation, XmCOMMAND_BELOW_WORKSPACE,
      NULL);

   frame = XtVaCreateManagedWidget(
      "frame",				/* widget name */
      xmFrameWidgetClass,		/* widget class */
      mainWin,				/* parent widget */
      NULL);

   drawArea = XtVaCreateManagedWidget(
      "drawingArea",			/* widget name */
      xmDrawingAreaWidgetClass,		/* widget class */
      frame,				/* parent widget*/
      XmNwidth, 200,			/* set startup width */
      XmNheight, 100,			/* set startup height */
      XmNresizePolicy, XmRESIZE_ANY,
      NULL);

/*
    XSelectInput(XtDisplay(drawArea), XtWindow(drawArea),
      KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask
      | Button1MotionMask );
*/

   rowColumn = XtVaCreateManagedWidget(
      "rowColumn",			/* widget name */
      xmRowColumnWidgetClass,		/* widget class */
      mainWin,				/* parent widget */
      XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
      XmNorientation, XmHORIZONTAL,	/* orientation */
      XmNpacking, XmPACK_TIGHT,	/* packing mode */
      NULL);


    /* RowColumn panels for pairs label -- combobox and buttons */
    for(i = 0; i < PAIR_PANELS; i++)
    {
        pairPanels[i] = XtVaCreateManagedWidget(panelsName[i], xmRowColumnWidgetClass,
          rowColumn, XmNpacking, XmPACK_TIGHT, XmNnumColumns, 1, XmNorientation,
          XmVERTICAL,  NULL);
    }

    clearBtn = XtVaCreateManagedWidget(
      "Clear",				/* widget name */
      xmPushButtonWidgetClass,		/* widget class */
      pairPanels[0],			/* parent widget*/
      XmNshadowType, XmSHADOW_ETCHED_OUT, NULL);

    quitBtn = XtVaCreateManagedWidget(
      "Quit",				/* widget name */
      xmPushButtonWidgetClass,		/* widget class */
      pairPanels[0],			/* parent widget*/
      NULL);


   question = XmCreateQuestionDialog(topLevel, "question", NULL, 0);
   XtVaSetValues(question,
      XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL, NULL);
   XtUnmanageChild(XmMessageBoxGetChild(question, XmDIALOG_HELP_BUTTON));
   XtAddCallback(question, XmNokCallback, QuestionCB, (XtPointer)0);
   XtAddCallback(question, XmNcancelCallback, QuestionCB, (XtPointer)1);

   wm_delete = XInternAtom(XtDisplay(topLevel), "WM_DELETE_WINDOW", False);
   XmAddWMProtocolCallback(topLevel, wm_delete, QuitCB, NULL);
   XmActivateWMProtocol(topLevel, wm_delete);


    /* Shape buttons */
   for(i = 0; i < SHAPE_BUTTONS; i++)
   {
      btnText = XmStringCreateLocalized (buttonLabels[i]);
      shapeButtons[i] = XtVaCreateManagedWidget (buttonsName[i], xmDrawnButtonWidgetClass,
            pairPanels[i/2 + 1], XmNlabelString, btnText, XmNpushButtonEnabled, 1,
            XmNshadowType, XmSHADOW_ETCHED_OUT, NULL);
      XmStringFree (btnText);
   }
   XtVaSetValues (shapeButtons[1], XmNpushButtonEnabled, 0,
        XmNshadowType, XmSHADOW_ETCHED_IN, NULL);

   if(AllocateColors(drawArea) == -1)
   {
      printf("Cannot load colors.\n");
      return -1;
   }
   SelectDefaultShape();

   /* Colors choices */
   XtVaCreateManagedWidget("label1", xmLabelWidgetClass, pairPanels[3],
      XmNlabelString, XmStringCreateLocalized("Line background:"), NULL);
   CreateComboBox(&comboLineBackground, pairPanels[3], colors, XtNumber(colors));
   XtVaCreateManagedWidget("label2", xmLabelWidgetClass, pairPanels[4],
      XmNlabelString, XmStringCreateLocalized("Line foreground:"), NULL);
   CreateComboBox(&comboLineForeground, pairPanels[4], colors, XtNumber(colors));
   XtVaCreateManagedWidget("label3", xmLabelWidgetClass, pairPanels[5],
      XmNlabelString, XmStringCreateLocalized("Fill background:"), NULL);
   CreateComboBox(&comboFillBackground, pairPanels[5], colors, XtNumber(colors));
   XtVaCreateManagedWidget("label4", xmLabelWidgetClass, pairPanels[6],
      XmNlabelString, XmStringCreateLocalized("Fill foreground:"), NULL);
   CreateComboBox(&comboFillForeground, pairPanels[6], colors, XtNumber(colors));

   /* Style choices */
   XtVaCreateManagedWidget("label5", xmLabelWidgetClass, pairPanels[7],
      XmNlabelString, XmStringCreateLocalized("Line width:"), NULL);
   CreateComboBox(&comboLineWidth, pairPanels[7], lineWidth, XtNumber(lineWidth));
   XtVaCreateManagedWidget("label6", xmLabelWidgetClass, pairPanels[8],
      XmNlabelString, XmStringCreateLocalized("Line style:"), NULL);
   CreateComboBox(&comboLineStyle, pairPanels[8], lineStyle, XtNumber(lineStyle));
   XtVaCreateManagedWidget("label7", xmLabelWidgetClass, pairPanels[9],
      XmNlabelString, XmStringCreateLocalized("Fill style:"), NULL);
   CreateComboBox(&comboFillStyle, pairPanels[9], fillStyle, XtNumber(fillStyle));


   XmMainWindowSetAreas(mainWin, NULL, rowColumn, NULL, NULL, frame);
   XtAddCallback(drawArea, XmNinputCallback, DrawLineCB, drawArea);
   XtAddEventHandler(drawArea, ButtonMotionMask, False, InputLineEH, NULL);
   XtAddCallback(drawArea, XmNexposeCallback, ExposeCB, drawArea);

   XtAddCallback(clearBtn, XmNactivateCallback, ClearCB, drawArea);
   XtAddCallback(quitBtn, XmNactivateCallback, QuitCB, 0);

   XtAddCallback(shapeButtons[0], XmNactivateCallback, ShapeCB, (XtPointer)PT);
   XtAddCallback(shapeButtons[1], XmNactivateCallback, ShapeCB, (XtPointer)LINE);
   XtAddCallback(shapeButtons[2], XmNactivateCallback, ShapeCB, (XtPointer)RECT);
   XtAddCallback(shapeButtons[3], XmNactivateCallback, ShapeCB, (XtPointer)ELLIPSE);

   XtAddCallback (comboLineWidth, XmNselectionCallback, SelectionLineWidth, NULL);
   XtAddCallback (comboLineStyle, XmNselectionCallback, SelectionLineStyle, NULL);
   XtAddCallback (comboLineForeground, XmNselectionCallback, SelectionLineFg, NULL);
   XtAddCallback (comboLineBackground, XmNselectionCallback, SelectionLineBg, NULL);
   XtAddCallback (comboFillForeground, XmNselectionCallback, SelectionFillFg, NULL);
   XtAddCallback (comboFillBackground, XmNselectionCallback, SelectionFillBg, NULL);
   XtAddCallback (comboFillStyle, XmNselectionCallback, SelectionFillStyle, NULL);

   XtRealizeWidget(topLevel);

   /*XSetWindowAttributes attrs;
   attrs.bit_gravity = ForgetGravity;
   XChangeWindowAttributes ( XtDisplay (drawArea),
   XtWindow (drawArea), CWBitGravity, &attrs);*/

   XtAppMainLoop(app_context);

   return 0;
}
