/****************************************************************************
**
** Copyright (C) 2000-2006 Lehrig Software Engineering.
**
** This file is part of the pvbrowser project.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
/*
    S7_200 update        : Wed Mar 21 2007
    copyright            : (C) 2007 by Aljosa Merljak
    Email                : aljosa.merljak@datapan.si
*/

#include "../pvbrowser/pvdefine.h"
#include "dlginsertfunction.h"
#include "ui_dlginsertfunction.h"
#include <QPushButton>
#include <QTreeWidget>
#include "opt.h"

extern OPT_DEVELOP opt;

dlgInsertFunction::dlgInsertFunction()
{
  QTreeWidgetItem *item,*subitem,*subsubitem;

  form = new Ui_Dialog;
  form->setupUi(this);
  QObject::connect(form->pushButtonCancel,SIGNAL(clicked()),this,SLOT(reject()));
  QObject::connect(form->pushButtonInsert,SIGNAL(clicked()),this,SLOT(accept()));

  form->treeWidget->setColumnCount(1);
  form->treeWidget->clear();
  form->treeWidget->sortItems(0,Qt::AscendingOrder);

  item = new QTreeWidgetItem();
  item->setText(0,"Function Types");
  form->treeWidget->setHeaderItem(item);

  item = top("rlPPIClient");
  subitem = sub(item,"ppi.write(slave,rlPPIClient::daveDB,dbnum,start,len,data[]);");
  subitem = sub(item,"ppi.writeFloat(slave,rlPPIClient::daveDB,dbnum,start,len,val[]);");
  subitem = sub(item,"ppi.writeDword(slave,rlPPIClient::daveDB,dbnum,start,len,val[]);");
  subitem = sub(item,"ppi.writeShort(slave,rlPPIClient::daveDB,dbnum,start,len,val[]);");
  subitem = sub(item,"ppi.writeUDword(slave,rlPPIClient::daveDB,dbnum,start,len,val[]);");
  subitem = sub(item,"ppi.writeUShort(slave,rlPPIClient::daveDB,dbnum,start,len,val[]);");
  subitem = sub(item,"ppi.read(offset,len);");
  subitem = sub(item,"ppi.buf[0]&BIT0");
  subitem = sub(item,"val = ppi.Float(index);");
  subitem = sub(item,"val = ppi.Dword(index);");
  subitem = sub(item,"val = ppi.Short(index);");
  subitem = sub(item,"val = ppi.UDword(index);");
  subitem = sub(item,"val = ppi.UShort(index);");  

  item = top("rlSiemensTCPClient");
  subitem = sub(item,"siemensTCP.writeBit(slave,rlSiemensTCPClient::ORG_DB,dbnum,start,offset,len,cbuf[]);");
  subitem = sub(item,"siemensTCP.writeByte(slave,rlSiemensTCPClient::ORG_DB,dbnum,start,len,cbuf[]);");
  subitem = sub(item,"siemensTCP.writeFloat(slave,rlSiemensTCPClient::ORG_DB,dbnum,start,len,fval[]);");
  subitem = sub(item,"siemensTCP.writeDword(slave,rlSiemensTCPClient::ORG_DB,dbnum,start,len,ival[]);");
  subitem = sub(item,"siemensTCP.writeShort(slave,rlSiemensTCPClient::ORG_DB,dbnum,start,len,sval[]);");
  subitem = sub(item,"siemensTCP.writeUDword(slave,rlSiemensTCPClient::ORG_DB,dbnum,start,len,ival[]);");
  subitem = sub(item,"siemensTCP.writeUShort(slave,rlSiemensTCPClient::ORG_DB,dbnum,start,len,sval[]);");
  subitem = sub(item,"siemensTCP.read(offset,len);");
  subitem = sub(item,"siemensTCP.buf[0]&BIT0");
  subitem = sub(item,"val = siemensTCP.Float(index);");
  subitem = sub(item,"val = siemensTCP.Dword(index);");
  subitem = sub(item,"val = siemensTCP.Short(index);");
  subitem = sub(item,"val = siemensTCP.UDword(index);");
  subitem = sub(item,"val = siemensTCP.UShort(index);");

  item = top("rlModbusClient");
  subitem = sub(item,"modbus.write(slave,function,data,len);");
  subitem = sub(item,"modbus.writeSingleCoil(slave,adr,value);");
  subitem = sub(item,"modbus.writeMultipleCoils(slave,adr,values[],num_coils);");
  subitem = sub(item,"modbus.writePresetSingleRegister(slave,adr,value);");
  subitem = sub(item,"modbus.writePresetMultipleRegisters(slave,adr,values[],num_values);");
  subitem = sub(item,"val = modbus.readBit(offset,number);");
  subitem = sub(item,"val = modbus.readByte(offset,number);");  
  subitem = sub(item,"val = modbus.readShort(offset,number);");  

  item = top("pvFunctions");
  subitem = sub(item,"pvQWidget(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");

  subitem = sub(item,"pvQLabel(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,i// needed for libnodaved,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetAlignment(p,id,alignment);");
  subsubitem = sub(subitem,"pvSetText(p,id,\"text\");");
  subsubitem = sub(subitem,"pvPrintf(p,id,\"format\");");
  subsubitem = sub(subitem,"pvSetBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvText(p,id);");
  subsubitem = sub(subitem,"pvSetStyle(p,id,shape,shadow,line_width,margin);");
  subsubitem = sub(subitem,"pvSetMovie(p,id,background,\"filename\");");
  subsubitem = sub(subitem,"pvMovieControl(p,id,step);");
  subsubitem = sub(subitem,"pvMovieSpeed(p,id,speed);");

  subitem = sub(item,"pvQComboBox(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvPrintf(p,id,\"format\");");
  subsubitem = sub(subitem,"pvClear(p,id);");
  subsubitem = sub(subitem,"pvInsertItem(p,id,\"file.bmp\",\"text\");");
  subsubitem = sub(subitem,"pvRemoveItem(p,id,index);");
  subsubitem = sub(subitem,"pvRemoveItemByName(p,id,\"name\");");
  subsubitem = sub(subitem,"pvSetCurrentItem(p,id,index);");

  subitem = sub(item,"pvQLineEdit(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetAlignment(p,id);");
  subsubitem = sub(subitem,"pvPrintf(p,id,\"format\");");
  subsubitem = sub(subitem,"pvText(p,id);");
  subsubitem = sub(subitem,"pvSetEditable(p,id,editable);");
  subsubitem = sub(subitem,"pvSetStyle(p,id,shape,shadow,line_width,margin);");

  subitem = sub(item,"pvQPushButton(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvPrintf(p,id,\"format\");");
  subsubitem = sub(subitem,"pvSetPixmap(p,id,\"file.bmp\");");
  subsubitem = sub(subitem,"pvText(p,id);");

  subitem = sub(item,"pvQLCDNumber(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvDisplayNum(p,id,num);");
  subsubitem = sub(subitem,"pvDisplayFloat(p,id,fval);");
  subsubitem = sub(subitem,"pvDisplayStr(p,id,\"string\");");
  subsubitem = sub(subitem,"pvSetStyle(p,id,shape,shadow,line_width,margin);");

  subitem = sub(item,"pvQSlider(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetValue(p,id,value);");

  subitem = sub(item,"pvQButtonGroup(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");

  subitem = sub(item,"pvQRadioButton(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvPrintf(p,id,\"format\");");
  subsubitem = sub(subitem,"pvSetChecked(p,id,state);");

  subitem = sub(item,"pvQCheckBox(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvPrintf(p,id,\"format\");");
  subsubitem = sub(subitem,"pvSetChecked(p,id,state);");

  subitem = sub(item,"pvQFrame(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetStyle(p,id,shape,shadow,line_width,margin);");

  subitem = sub(item,"pvQDraw(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetZoomX(p,id,fzoom);");
  subsubitem = sub(subitem,"pvSetZoomY(p,id,fzoom);");
  subsubitem = sub(subitem,"gWriteFile(\"file.pvm\");");
  subsubitem = sub(subitem,"gCloseFile();");
  subsubitem = sub(subitem,"gBeginDraw(p,id);");
  subsubitem = sub(subitem,"gBox(p,x,y,w,h);");
  subsubitem = sub(subitem,"gRect(p,x,y,w,h);");
  subsubitem = sub(subitem,"gEndDraw(p);");
  subsubitem = sub(subitem,"gLineTo(p,x,y);");
  subsubitem = sub(subitem,"gBufferedLine(p);");
  subsubitem = sub(subitem,"gLine(p,fx[],fy[],n);");
  subsubitem = sub(subitem,"gMoveTo(p,x,y);");
  subsubitem = sub(subitem,"gRightYAxis(p,fstart,fdelta,fend,draw);");
  subsubitem = sub(subitem,"gSetColor(p,r,g,b);");
  subsubitem = sub(subitem,"gSetWidth(p,w);");
  subsubitem = sub(subitem,"gSetStyle(p,style);");
  subsubitem = sub(subitem,"gDrawArc(p,x,y,w,h,start_angle,angle_length);");
  subsubitem = sub(subitem,"gDrawPie(p,x,y,w,h,start_angle,angle_length);");
  subsubitem = sub(subitem,"gDrawPolygon(p,*x,*y,n);");
  subsubitem = sub(subitem,"gSetFont(p,\"family\",size,weight,italic);");
  subsubitem = sub(subitem,"gSetLinestyle(p,style);");
  subsubitem = sub(subitem,"gText(p,x,y,\"text\",alignment);");
  subsubitem = sub(subitem,"gTextInAxis(p,fx,fy,\"text\",alignment);");
  subsubitem = sub(subitem,"gSetFloatFormat(p,\"text\");");
  subsubitem = sub(subitem,"gXAxis(p,fstart,fdelta,fend,draw);");
  subsubitem = sub(subitem,"gYAxis(p,fstart,fdelta,fend,draw);");
  subsubitem = sub(subitem,"gXGrid(p);");
  subsubitem = sub(subitem,"gYGrid(p);");
  subsubitem = sub(subitem,"gBoxWithText(p,x,y,w,h,fontsize,\"xlabel\",\"ylabel\",\"rylabel\");");
  subsubitem = sub(subitem,"gComment(p,\"comment\");");
  subsubitem = sub(subitem,"gPlaySVG(p,\"file.svg\");");
  subsubitem = sub(subitem,"gSocketPlaySVG(p,\"svgstring\");");
  subsubitem = sub(subitem,"gTranslate(p,fx,fy);");
  subsubitem = sub(subitem,"gRotate(p,fangle);");
  subsubitem = sub(subitem,"gScale(p,fsx,fsy);");

  subitem = sub(item,"pvQImage(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetImage(p,id,\"imagename\");");

  subitem = sub(item,"pvQGL(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");

  subitem = sub(item,"pvQTabWidget(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetValue(p,id,value);");
  subsubitem = sub(subitem,"pvSetTabPosition(p,id,pos);");

  subitem = sub(item,"pvQToolBox(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetValue(p,id,value);");
  subsubitem = sub(subitem,"pvSetStyle(p,id,shape,shadow,line_width,margin);");

  subitem = sub(item,"pvQGroupBox(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetStyle(p,id,shape,shadow,line_width,margin);");

  subitem = sub(item,"pvQListBox(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvClear(p,id);");
  subsubitem = sub(subitem,"pvChangeItem(p,id,index,\"file.bmp\",\"text\");");
  subsubitem = sub(subitem,"pvInsertItem(p,id,index,\"file.bmp\",\"text\");");
  subsubitem = sub(subitem,"pvRemoveItem(p,id,index);");
  subsubitem = sub(subitem,"pvRemoveItemByName(p,id,\"name\");");
  subsubitem = sub(subitem,"pvSelection(p,id);");

  subitem = sub(item,"pvQTable(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetTableText(p,id,x,y,\"text\");");
  subsubitem = sub(subitem,"pvSetTableButton(p,id,x,y,\"text\");");
  subsubitem = sub(subitem,"pvSetTableCheckBox(p,id,x,y,state,\"text\");");
  subsubitem = sub(subitem,"pvSetTableComboBox(p,id,x,y,editable,\"choice1,choice2\");");
  subsubitem = sub(subitem,"pvSetTableLabel(p,id,x,y,\"text\");");
  subsubitem = sub(subitem,"pvTablePrintf(p,id,x,y,\"format\");");
  subsubitem = sub(subitem,"pvSetColumnWidth(p,id,column,width);");
  subsubitem = sub(subitem,"pvSetWordWrap(p,id,wrap);");
  subsubitem = sub(subitem,"pvSetTablePixmap(p,id,x,y,\"file.bmp\");");
  subsubitem = sub(subitem,"pvEnsureCellVisible(p,id,row,col);");
  subsubitem = sub(subitem,"pvSetEditable(p,id,editable);");
  subsubitem = sub(subitem,"pvTableSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvTableSetHeaderResizeEnabled(p,id,horizontal,enabled,section);");
  subsubitem = sub(subitem,"pvSetNumRows(p,id,num);");
  subsubitem = sub(subitem,"pvSetNumCols(p,id,num);");
  subsubitem = sub(subitem,"pvInsertRows(p,id,row,count);");
  subsubitem = sub(subitem,"pvInsertColumns(p,id,col,count);");
  subsubitem = sub(subitem,"pvRemoveRow(p,id,row);");
  subsubitem = sub(subitem,"pvRemoveColumn(p,id,col);");
  subsubitem = sub(subitem,"pvSetTableTextAlignment(p,id,x,y,alignment);");
  subsubitem = sub(subitem,"pvSave(p,id);");
  subsubitem = sub(subitem,"pvMysqldump(p,id,command);");
  subsubitem = sub(subitem,"pvCSVdump(p,id,filename);");
  subsubitem = sub(subitem,"pvCSV(p,id,command);");

  subitem = sub(item,"pvQSpinBox(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetValue(p,id,value);");

  subitem = sub(item,"pvQDial(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetValue(p,id,value);");

  subitem = sub(item,"pvQProgressBar(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetValue(p,id,value);");
  subsubitem = sub(subitem,"pvSetStyle(p,id,shape,shadow,line_width,margin);");

  subitem = sub(item,"pvQMultiLineEdit(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvPrintf(p,id,\"format\");");
  subsubitem = sub(subitem,"pvClear(p,id);");
  subsubitem = sub(subitem,"pvText(p,id);");
  subsubitem = sub(subitem,"pvSetEditable(p,id,editable);");
  subsubitem = sub(subitem,"pvMoveCursor(p,id,cursor);");

  subitem = sub(item,"pvQTextBrowser(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvPrintf(p,id,\"format\");");
  subsubitem = sub(subitem,"pvSetSource(p,id,\"file.html\");");
  subsubitem = sub(subitem,"pvMoveContent(p,id,pos);");
  subsubitem = sub(subitem,"pvMoveCursor(p,id,cursor);");

  subitem = sub(item,"pvQListView(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvClear(p,id);");
  subsubitem = sub(subitem,"pvAddColumn(p,id,\"text\",size);");
  subsubitem = sub(subitem,"pvSetListViewText(p,id,\"/path/to\",column,\"text\");");
  subsubitem = sub(subitem,"pvListViewPrintf(p,id,\"/path/to\",column,\"format\");");
  subsubitem = sub(subitem,"pvSetListViewPixmap(p,id,\"/path/to\",\"file.bmp\",column);");
  subsubitem = sub(subitem,"pvRemoveListViewItem(p,id,\"/path/to\");");
  subsubitem = sub(subitem,"pvSelection(p,id);");
  subsubitem = sub(subitem,"pvSetSorting(p,id,column,mode);");
  subsubitem = sub(subitem,"pvListViewEnsureVisible(p,id,\"/path/to\");");

  subitem = sub(item,"pvQIconView(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvClear(p,id);");
  subsubitem = sub(subitem,"pvRemoveIconViewItem(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetIconViewItem(p,id,\"file.bmp\",\"text\");");

  subitem = sub(item,"pvQVtkTclWidget(...);");
  subsubitem = sub(subitem,"pvVtkTcl(p,id,\"tclcommand\");");
  subsubitem = sub(subitem,"pvVtkTclPrintf(p,id,\"format\");");
  subsubitem = sub(subitem,"pvVtkTclScript(p,id,\"file.tcl\");");

  subitem = sub(item,"pvQwtPlotWidget(...);");
  subsubitem = sub(subitem,"qpwSetCurveData(p,id,curve,count,x[],y[]);");
  subsubitem = sub(subitem,"qpwSetBufferedCurveData(p,id,curve);");
  subsubitem = sub(subitem,"qpwReplot(p,id);");
  subsubitem = sub(subitem,"qpwSetTitle(p,id,\"text\");");
  subsubitem = sub(subitem,"qpwSetCanvasBackground(p,id,r,g,b);");
  subsubitem = sub(subitem,"qpwEnableOutline(p,id,ival);");
  subsubitem = sub(subitem,"qpwSetOutlinePen(p,id,r,g,b);");
  subsubitem = sub(subitem,"qpwSetAutoLegend(p,id,ival);");
  subsubitem = sub(subitem,"qpwEnableLegend(p,id,ival);");
  subsubitem = sub(subitem,"qpwSetLegendPos(p,id,pos);");
  subsubitem = sub(subitem,"qpwSetLegendFrameStyle(p,id,style);");
  subsubitem = sub(subitem,"qpwEnableGridXMin(p,id);");
  subsubitem = sub(subitem,"qpwSetGridMajPen(p,id,r,g,b,style);");
  subsubitem = sub(subitem,"qpwSetGridMinPen(p,id,r,g,b,style);");
  subsubitem = sub(subitem,"qpwEnableAxis(p,id,pos);");
  subsubitem = sub(subitem,"qpwSetAxisTitle(p,id,pos,\"text\");");
  subsubitem = sub(subitem,"qpwSetAxisOptions(p,id,pos,ival);");
  subsubitem = sub(subitem,"qpwSetAxisMaxMajor(p,id,pos,ival);");
  subsubitem = sub(subitem,"qpwSetAxisMaxMinor(p,id,pos,ival);");
  subsubitem = sub(subitem,"qpwInsertCurve(p,id,index,\"text\");");
  subsubitem = sub(subitem,"qpwRemoveCurve(p,id,index);");
  subsubitem = sub(subitem,"qpwSetCurvePen(p,id,index,r,g,b,width,style);");
  subsubitem = sub(subitem,"qpwSetCurveSymbol(p,id,index,symbol,r1,g1,b1,r2,g2,b2,w,h);");
  subsubitem = sub(subitem,"qpwSetCurveYAxis(p,id,index,pos);");
  subsubitem = sub(subitem,"qpwInsertMarker(p,id,index);");
  subsubitem = sub(subitem,"qpwSetMarkerLineStyle(p,id,index,style);");
  subsubitem = sub(subitem,"qpwSetMarkerPos(p,id,index,x,y);");
  subsubitem = sub(subitem,"qpwSetMarkerLabelAlign(p,id,index,align);");
  subsubitem = sub(subitem,"qpwSetMarkerPen(p,id,index,r,g,b,style);");
  subsubitem = sub(subitem,"qpwSetMarkerLabel(p,id,number,\"text\");");
  subsubitem = sub(subitem,"qpwSetMarkerFont(p,id,index,\"family\",size,style);");
  subsubitem = sub(subitem,"qpwSetMarkerSymbol(p,id,index,symbol,r1,g1,b1,r2,g2,b2,w,h);");
  subsubitem = sub(subitem,"qpwInsertLineMarker(p,id,index,\"text\",pos);");
  subsubitem = sub(subitem,"qpwSetAxisScaleDraw(p,id,pos,\"text\");");
  subsubitem = sub(subitem,"qpwSetAxisScale(p,id,pos,min,max,step);");

  subitem = sub(item,"pvQwtScale(...);");
  subsubitem = sub(subitem,"qwtScaleSetTitle(p,id,\"text\");");
  subsubitem = sub(subitem,"qwtScaleSetTitleColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"qwtScaleSetTitleFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"qwtScaleSetTitleAlignment(p,id,flags);");
  subsubitem = sub(subitem,"qwtScaleSetBorderDist(p,id,start,end);");
  subsubitem = sub(subitem,"qwtScaleSetBaselineDist(p,id,bd);");
  subsubitem = sub(subitem,"qwtScaleSetScaleDiv(p,id,lBound,hBound,maxMaj,maxMin,log,step,ascend);");
  subsubitem = sub(subitem,"qwtScaleSetLabelFormat(p,id,f,prec,fieldWidth);");
  subsubitem = sub(subitem,"qwtScaleSetLabelAlignment(p,id,align);");
  subsubitem = sub(subitem,"qwtScaleSetLabelRotation(p,id,rotation);");
  subsubitem = sub(subitem,"qwtScaleSetPosition(p,id,position);");

  subitem = sub(item,"pvQwtThermo(...);");
  subsubitem = sub(subitem,"qwtThermoSetScale(p,id,min,max,step,logarithmic);");
  subsubitem = sub(subitem,"qwtThermoSetOrientation(p,id,orientation,position);");
  subsubitem = sub(subitem,"qwtThermoSetBorderWidth(p,id,width);");
  subsubitem = sub(subitem,"qwtThermoSetFillColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"qwtThermoSetAlarmColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"qwtThermoSetAlarmLevel(p,id,level);");
  subsubitem = sub(subitem,"qwtThermoSetAlarmEnabled(p,id,tf);");
  subsubitem = sub(subitem,"qwtThermoSetPipeWidth(p,id,width);");
  subsubitem = sub(subitem,"qwtThermoSetRange(p,id,vmin,vmax);");
  subsubitem = sub(subitem,"qwtThermoSetMargin(p,id,margin);");
  subsubitem = sub(subitem,"qwtThermoSetValue(p,id,value);");

  subitem = sub(item,"pvQwtKnob(...);");
  subsubitem = sub(subitem,"qwtKnobSetScale(p,id,min,max,step,logarithmic);");
  subsubitem = sub(subitem,"qwtKnobSetMass(p,id,mass);");
  subsubitem = sub(subitem,"qwtKnobSetOrientation(p,id,orientation);");
  subsubitem = sub(subitem,"qwtKnobSetReadOnly(p,id,rdonly);");
  subsubitem = sub(subitem,"qwtKnobSetKnobWidth(p,id,width);");
  subsubitem = sub(subitem,"qwtKnobSetTotalAngle(p,id,angle);");
  subsubitem = sub(subitem,"qwtKnobSetBorderWidth(p,id,width);");
  subsubitem = sub(subitem,"qwtKnobSetSymbol(p,id,symbol);");
  subsubitem = sub(subitem,"qwtKnobSetValue(p,id,value);");

  subitem = sub(item,"pvQwtCounter(...);");
  subsubitem = sub(subitem,"qwtCounterSetStep(p,id,step);");
  subsubitem = sub(subitem,"qwtCounterSetMinValue(p,id,value);");
  subsubitem = sub(subitem,"qwtCounterSetMaxValue(p,id,value);");
  subsubitem = sub(subitem,"qwtCounterSetStepButton1(p,id,n);");
  subsubitem = sub(subitem,"qwtCounterSetStepButton2(p,id,n);");
  subsubitem = sub(subitem,"qwtCounterSetStepButton3(p,id,n);");
  subsubitem = sub(subitem,"qwtCounterSetNumButtons(p,id,n);");
  subsubitem = sub(subitem,"qwtCounterSetIncSteps(p,id,button,n);");
  subsubitem = sub(subitem,"qwtCounterSetValue(p,id,value);");

  subitem = sub(item,"pvQwtWheel(...);");
  subsubitem = sub(subitem,"qwtWheelSetMass(p,id,mass);");
  subsubitem = sub(subitem,"qwtWheelSetOrientation(p,id,orientation);");
  subsubitem = sub(subitem,"qwtWheelSetReadOnly(p,id,rdonly);");
  subsubitem = sub(subitem,"qwtWheelSetTotalAngle(p,id,angle);");
  subsubitem = sub(subitem,"qwtWheelSetTickCnt(p,id,cnt);");
  subsubitem = sub(subitem,"qwtWheelSetViewAngle(p,id,angle);");
  subsubitem = sub(subitem,"qwtWheelSetInternalBorder(p,id,width);");
  subsubitem = sub(subitem,"qwtWheelSetWheelWidth(p,id,width);");
  subsubitem = sub(subitem,"qwtWheelSetValue(p,id,value);");

  subitem = sub(item,"pvQwtSlider(...);");
  subsubitem = sub(subitem,"qwtSliderSetScale(p,id,min,max,step,logarithmic);");
  subsubitem = sub(subitem,"qwtSliderSetMass(p, id, mass);");
  subsubitem = sub(subitem,"qwtSliderSetOrientation(p,id,orientation);");
  subsubitem = sub(subitem,"qwtSliderSetReadOnly(p,id,rdonly);");
  subsubitem = sub(subitem,"qwtSliderSetBgStyle(p,id,style);");
  subsubitem = sub(subitem,"qwtSliderSetScalePos(p,id,pos);");
  subsubitem = sub(subitem,"qwtSliderSetThumbLength(p,id,length);");
  subsubitem = sub(subitem,"qwtSliderSetThumbWidth(p,id,width);");
  subsubitem = sub(subitem,"qwtSliderSetBorderWidth(p,id,width);");
  subsubitem = sub(subitem,"qwtSliderSetMargins(p,id,x,y);");
  subsubitem = sub(subitem,"qwtSliderSetValue(p,id,value);");

  subitem = sub(item,"pvQwtCompass(...);");
  subsubitem = sub(subitem,"qwtCompassSetSimpleCompassRose(p,id,numThorns,numThornLevels,width);");
  subsubitem = sub(subitem,"qwtCompassSetMass(p,id,mass);");
  subsubitem = sub(subitem,"qwtCompassSetReadOnly(p,id,rdonly);");
  subsubitem = sub(subitem,"qwtCompassSetFrameShadow(p,id,shadow);");
  subsubitem = sub(subitem,"qwtCompassShowBackground(p,id,show);");
  subsubitem = sub(subitem,"qwtCompassSetLineWidth(p,id,width);");
  subsubitem = sub(subitem,"qwtCompassSetMode(p,id,mode);");
  subsubitem = sub(subitem,"qwtCompassSetWrapping(p,id,wrap);");
  subsubitem = sub(subitem,"qwtCompassSetScale(p,id,maxMajIntv,maxMinIntv,step);");
  subsubitem = sub(subitem,"qwtCompassSetScaleArc(p,id,min,max);");
  subsubitem = sub(subitem,"qwtCompassSetOrigin(p,id,orig);");
  subsubitem = sub(subitem,"qwtCompassSetNeedle(p,id,which,r1,g1,b1,r2,g2,b2,r3,g3,b3);");
  subsubitem = sub(subitem,"qwtCompassSetValue(p,id,value);");

  subitem = sub(item,"pvQwtDial(...);");
  subsubitem = sub(subitem,"qwtDialSetRange(p,id,vmin,vmax,step);");
  subsubitem = sub(subitem,"qwtDialSetMass(p,id,mass);");
  subsubitem = sub(subitem,"qwtDialSetReadOnly(p,id,rdonly);");
  subsubitem = sub(subitem,"qwtDialSetFrameShadow(p,id,shadow);");
  subsubitem = sub(subitem,"qwtDialShowBackground(p,id,show);");
  subsubitem = sub(subitem,"qwtDialSetLineWidth(p,id,width);");
  subsubitem = sub(subitem,"qwtDialSetMode(p,id,mode);");
  subsubitem = sub(subitem,"qwtDialSetWrapping(p,id,wrap);");
  subsubitem = sub(subitem,"qwtDialSetScale(p,id,maxMajIntv,maxMinIntv,step);");
  subsubitem = sub(subitem,"qwtDialSetScaleArc(p,id,min,max);");
  subsubitem = sub(subitem,"qwtDialSetOrigin(p,id,orig);");
  subsubitem = sub(subitem,"qwtDialSetNeedle(p,id,which,r1,g1,b1,r2,g2,b2,r3,g3,b3);");
  subsubitem = sub(subitem,"qwtDialSetValue(p,id,value);");

  subitem = sub(item,"pvQwtAnalogClock(...);");
  subsubitem = sub(subitem,"qwtAnalogClockSetTime(p,id,hour,minute,second);");
  subsubitem = sub(subitem,"qwtAnalogClockSetMass(p,id,mass);");
  subsubitem = sub(subitem,"qwtAnalogClockSetReadOnly(p,id,rdonly);");
  subsubitem = sub(subitem,"qwtAnalogClockSetFrameShadow(p,id,shadow);");
  subsubitem = sub(subitem,"qwtAnalogClockShowBackground(p,id,show);");
  subsubitem = sub(subitem,"qwtAnalogClockSetLineWidth(p,id,width);");
  subsubitem = sub(subitem,"qwtAnalogClockSetMode(p,id,mode);");
  subsubitem = sub(subitem,"qwtAnalogClockSetWrapping(p,id,wrap);");
  subsubitem = sub(subitem,"qwtAnalogClockSetScale(p,id,maxMajIntv,maxMinIntv,step);");
  subsubitem = sub(subitem,"qwtAnalogClockSetScaleArc(p,id,min,max);");
  subsubitem = sub(subitem,"qwtAnalogClockSetOrigin(p,id,orig);");
  subsubitem = sub(subitem,"qwtAnalogClockSetNeedle(p,id,which,r1,g1,b1,r2,g2,b2,r3,g3,b3);");
  subsubitem = sub(subitem,"qwtAnalogClockSetValue(p,id,value);");

  subitem = sub(item,"pvQDateEdit(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetDate(p,id,year,month,day);");
  subsubitem = sub(subitem,"pvSetMinDate(p,id,year,month,day);");
  subsubitem = sub(subitem,"pvSetMaxDate(p,id,year,month,day);");
  subsubitem = sub(subitem,"pvSetDateOrder(p,id,MDY);");

  subitem = sub(item,"pvQTimeEdit(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetTime(p,id,hour,minute,second,msec);");
  subsubitem = sub(subitem,"pvSetMinTime(p,id,hour,minute,second,msec);");
  subsubitem = sub(subitem,"pvSetMaxTime(p,id,hour,minute,second,msec);");
  subsubitem = sub(subitem,"pvSetTimeEditDisplay(p,id,hour,minute,second,ampm);");

  subitem = sub(item,"pvQDateTimeEdit(...);");
  subsubitem = sub(subitem,"pvToolTip(p,id,\"text\");");
  subsubitem = sub(subitem,"pvSetGeometry(p,id,x,y,w,h);");
  subsubitem = sub(subitem,"pvMove(p,id,x,y);");
  subsubitem = sub(subitem,"pvResize(p,id,w,h);");
  subsubitem = sub(subitem,"pvHide(p,id);");
  subsubitem = sub(subitem,"pvShow(p,id);");
  subsubitem = sub(subitem,"pvSetPaletteBackgroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetPaletteForegroundColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFontColor(p,id,r,g,b);");
  subsubitem = sub(subitem,"pvSetFont(p,id,\"family\",pointsize,bold,italic,underline,strikeout);");
  subsubitem = sub(subitem,"pvSetEnabled(p,id,enabled);");
  subsubitem = sub(subitem,"pvCopyToClipboard(p,id);");
  subsubitem = sub(subitem,"pvSaveAsBmp(p,id,\"filename\");");
  subsubitem = sub(subitem,"pvSetDate(p,id,year,month,day);");
  subsubitem = sub(subitem,"pvSetMinDate(p,id,year,month,day);");
  subsubitem = sub(subitem,"pvSetMaxDate(p,id,year,month,day);");
  subsubitem = sub(subitem,"pvSetTime(p,id,hour,minute,second,msec);");
  subsubitem = sub(subitem,"pvSetMinTime(p,id,hour,minute,second,msec);");
  subsubitem = sub(subitem,"pvSetMaxTime(p,id,hour,minute,second,msec);");
  subsubitem = sub(subitem,"pvSetDateOrder(p,id,MDY);");

  subitem = sub(item,"Dialogs(...);");
  subsubitem = sub(subitem,"pvFileDialog(p,id_return,type);");
  subsubitem = sub(subitem,"pvPopupMenu(p,id_return,\"menu1,menu2,,menu3\");");
  subsubitem = sub(subitem,"pvMessageBox(p,id_return,type,\"text\",button0,button1,button2);");
  subsubitem = sub(subitem,"pvInputDialog(p,id_return,\"text\",\"default_text\");");
  subsubitem = sub(subitem,"pvRunModalDialog(p,width,height,showMask(),userData,readData(),showData(),d);");
  subsubitem = sub(subitem,"pvRunModalDialogScript(p,width,height);");
  subsubitem = sub(subitem,"pvTerminateModalDialog(p);");
  subsubitem = sub(subitem,"pvUpdateBaseWindow(p);");
  subsubitem = sub(subitem,"pvUpdateBaseWindowOnOff(p,onoff);");  

  subitem = sub(item,"Global(...);");
  subsubitem = sub(subitem,"pvDownloadFile(p,\"filename\");");
  subsubitem = sub(subitem,"pvDownloadFileAs(p,\"filename\",\"newname\");");
  subsubitem = sub(subitem,"pvXYAllocate(p,n);");
  subsubitem = sub(subitem,"pvSetXY(p,i,fx,fy);");
  subsubitem = sub(subitem,"pvInitInternal(p);");
  subsubitem = sub(subitem,"pvInit(ac,av,p);");
  subsubitem = sub(subitem,"pvAccept(p);");
  subsubitem = sub(subitem,"pvCreateThread(p,s);");
  subsubitem = sub(subitem,"pvGetInitialMask(p);");
  subsubitem = sub(subitem,"pvMain(p);");
  subsubitem = sub(subitem,"pvSetCleanup(p,cleanup(),app_data);");
  subsubitem = sub(subitem,"pvGetEvent(p);");
  subsubitem = sub(subitem,"pvPollEvent(p,event);");
  subsubitem = sub(subitem,"pvWait(p,\"pattern\");");
  subsubitem = sub(subitem,"pvGlUpdate(p,id);");
  subsubitem = sub(subitem,"pvSleep(milliseconds);");
  subsubitem = sub(subitem,"pvWarning(p,\"text\");");
  subsubitem = sub(subitem,"pvMainFatal(p,\"text\");");
  subsubitem = sub(subitem,"pvThreadFatal(p,\"text\");");
  subsubitem = sub(subitem,"pvScreenHint(p,w,h);");  
  subsubitem = sub(subitem,"pvBeep(p);");
  subsubitem = sub(subitem,"pvPlaySound(p,filename);");
  subsubitem = sub(subitem,"pvClientCommand(p,command,filename);");
  subsubitem = sub(subitem,"pvCSVcreate(p,command,filename);");
  subsubitem = sub(subitem,"pvHyperlink(p,<+>link);");

  if(subsubitem == NULL) return; // suppress warning
}

dlgInsertFunction::~dlgInsertFunction()
{
  delete form;
}

QTreeWidgetItem *dlgInsertFunction::top(const char *text)
{
  QTreeWidgetItem *item;
  item = new QTreeWidgetItem();
  item->setText(0,text);
  form->treeWidget->addTopLevelItem(item);
  return item;
}

QTreeWidgetItem *dlgInsertFunction::sub(QTreeWidgetItem *root, const char *text)
{
  QTreeWidgetItem *item;
  item = new QTreeWidgetItem(root);
  item->setText(0,text);
  return item;
}

QString dlgInsertFunction::run()
{
  QTreeWidgetItem *item;
  int ret = exec();
  if(ret == QDialog::Accepted)
  {
    item = form->treeWidget->currentItem();
    if(item == NULL) return "";
    return item->text(0);
  }
  return "";
}

