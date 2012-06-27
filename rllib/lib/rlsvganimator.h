/***************************************************************************
                      rlsvganimator.h  -  description
                             -------------------
    begin                : Tue Apr 10 2006
    copyright            : (C) 2006 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_SVG_ANIMATOR_
#define _RL_SVG_ANIMATOR_
#include "rldefine.h"
#include "rlinifile.h"
#include "rlspreadsheet.h"
#include "rlstring.h"

/*! <pre>
This class holds the position of an SVG object.
Use it together with rlSvgAnimator::setMatrix()
</pre> */
class rlSvgPosition
{
  public:
    rlSvgPosition();
    rlSvgPosition(float sx_init, float a_init, float x0_init, float y0_init, float cx_init, float cy_init);
    virtual ~rlSvgPosition();
    float sx, alpha, x0, y0, cx, cy;
    struct rlPositionInit {float sx, alpha, x0, y0, w, h;} init;
    void setInit(float x0_init, float y0_init, float w_init, float h_init);
    void move(float x, float y);
    void moveRelative(float dx, float dy);
    void scale(float s);
    void scaleRelative(float ds);
    void rotate(float alpha, float cx, float cy);
};

/*! <pre>
This class allows you to animate SVG graphics within pvbrowser.
First the SVG is send to pvbrowser.
Then you may modify the SVG within the pvbrowser client.
</pre> */
class rlSvgAnimator
{
  public:
    rlSvgAnimator();
    virtual ~rlSvgAnimator();

    /*! initialize the socket with pvbrowser p->s */
    int setSocket(int *socket);
    /*! initialize the id with the pvbrowser object id */
    int setId(int Id);
    /*! read SVG file infile and load it into the pvbrowser client.
        if inifile is given inifile will be set with properties within the SVG XML file.
        The id's of the SVG objects will result in section names of the inifile. */
    int read(const char *infile, rlIniFile *inifile=NULL);
    /*! update the SVG graphic with:
        gBeginDraw(p,id); d->svgAnimator.writeSocket(); gEndDraw(p); */
    //! The following methods are for modifying a object within a SVG graphic identified by objectname
    int writeSocket();
    /*! change a property of tag = "name=" */

    int svgPrintf(const char *objectname, const char *tag, const char *format, ...);
    /*! recursively change a property of tag = "name=" */
    int svgRecursivePrintf(const char *objectname, const char *tag, const char *format, ...);
    /*! search for "before" within "tag=" property and replace it with "after". You may use wildcards whin "before" */
    int svgSearchAndReplace(const char *objectname, const char *tag, const char *before, const char *after);
    /*! recursively search for "before" within "tag=" property and replace it with "after". You may use wildcards within "before" */
    int svgRecursiveSearchAndReplace(const char *objectname, const char *tag, const char *before, const char *after);
    /*! change the text of "objectname" */
    int svgTextPrintf(const char *objectname, const char *format, ...);
    /*! remove a style option of "objectname". option must end with ':'. Example: option="stroke:" */
    int svgRemoveStyleOption(const char *objectname, const char *option);
    /*! recursively remove a style option of "objectname". option must end with ':'. Example: option="stroke:"  */
    int svgRecursiveRemoveStyleOption(const char *objectname, const char *option);
    /*! change a style option of "objectname". option must end with ':'. Example: option="stroke:" value="#000000" */
    int svgChangeStyleOption(const char *objectname, const char *option, const char *value);
    /*! recursively change a style option of "objectname". option must end with ':'. Example: option="stroke:" value="#000000" */
    int svgRecursiveChangeStyleOption(const char *objectname, const char *option, const char *value);
    /*! set a style option of "objectname". option must end with ':'. Example: value="fill:#9d9d9d;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:3.5;stroke-linecap:butt;stroke-linejoin:miter;stroke-dasharray:none;stroke-opacity:1"  */
    int svgSetStyleOption(const char *objectname, const char *value);
    /*! recursively set a style option of "objectname". option must end with ':'. Example: value="fill:#9d9d9d;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:3.5;stroke-linecap:butt;stroke-linejoin:miter;stroke-dasharray:none;stroke-opacity:1"  */
    int svgRecursiveSetStyleOption(const char *objectname, const char *value);
    /*! hide/show object state := 0=hide 1=show */
    int show(const char *objectname, int state); // state := 0|1
    /*! set transformation matrix of object */
    int setMatrix(const char *objectname, float sx, float alpha, float x0, float y0, float cx, float cy);
    /*! set transformation matrix of object */
    //! The following methods are for moveing and zooming the whole SVG identified by mainObject. default: main
    int setMatrix(const char *objectname, rlSvgPosition &pos);
    /*! set/get the name of the MainObject . The object name holding the whole SVG graphic. default: main */

    int setMainObject(const char *main_object);
    const char *mainObject();
    /*! set/get x0,y0 coordinates for the MainObject */
    int setXY0(float x0, float y0);
    float x0();
    float y0();
    /*! set/get mouse position 0 for the MainObject */
    int setMouseXY0(float x0, float y0);
    float mouseX0();
    float mouseY0();
    /*! set/get mouse position 1 for the MainObject */
    int setMouseXY1(float x1, float y1);
    float mouseX1();
    float mouseY1();
    /*! set/get the scaling factor for the MainObject */
    int setScale(float scale);
    float scale();
    /*! zooms the whole SVG graphic keeping it centered to the viewport */
    int zoomCenter(float newScale);
    /*! zooms the whole SVG graphic so that the visible section is from x0,x0 to x1,y1 */
    int zoomRect();
    /*! sets the MainObject matrix according to scale,x0,y0 */
    int setMainObjectMatrix();
    /*! call this method when the widget is resized */
    int setWindowSize(int width, int height);
    float windowWidth();
    float windowHeight();
    /*! move MainObject to position */
    int moveMainObject(float x, float y);

    int isModified;

  private:
    int tcpsend(const char *buf, int len);
    int fillIniFile(rlIniFile *inifile, const char *line);
    int fileFillIniFile(const char *infile, rlIniFile *inifile);
    int inifileState, inifileCount;
    rlSpreadsheetCell  inifileID;
    rlSpreadsheetTable inifileTable;
    int  *s;
    int  id;
    // zoomer variables follow
    float svgX0, svgY0, svgWindowWidth, svgWindowHeight, svgScale, svgMouseX0, svgMouseY0, svgMouseX1, svgMouseY1;
    rlString main_object_name;
};
#endif
