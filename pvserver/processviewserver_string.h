// You may use this header if you use C++ string's
// avoid casts with .c_str()
// Warning: This file is NOT maintained on a regular basis, so some functions may be missing
//          If you observe this please contact: R. Lehrig lehrig@t-online.de
#ifndef _PROCESS_VIEW_SERVER_STRING_H_
#define _PROCESS_VIEW_SERVER_STRING_H_
#include "processviewserver.h"
#include <string>
using namespace std;

inline int pvWait(PARAM *p, string pattern) { return pvWait(p, pattern.c_str()); }
inline int pvWarning(PARAM *p, string text) { return pvWarning(p, text.c_str()); }
inline int pvMainFatal(PARAM *p, string text) { return pvMainFatal(p, text.c_str()); }
inline int pvThreadFatal(PARAM *p, string text) { return pvThreadFatal(p, text.c_str()); }
inline int pvQButtonGroup(PARAM *p, int id, int parent, int columns, int orientation, string title) { return pvQButtonGroup(p, id, parent, columns, orientation, title.c_str()); }
inline int pvQImage(PARAM *p, int id, int parent, string imagename, int *w, int *h, int *depth) { return pvQImage(p, id, parent, imagename.c_str(), w, h, depth); }
inline int pvQGroupBox(PARAM *p, int id, int parent, int columns, int orientation, string title) { return pvQGroupBox(p, id, parent, columns, orientation, title.c_str()); }
inline int pvSetCaption(PARAM *p, string text) { return pvSetCaption(p, text.c_str()); }
inline int pvToolTip(PARAM *p, int id, string text) { return pvToolTip(p, id, text.c_str()); }
inline int pvSetText(PARAM *p, int id, string text) { return pvSetText(p, id, text.c_str()); }
inline int pvChangeItem(PARAM *p, int id, int index, string bmp_file, string text) { return pvChangeItem(p, id, index, bmp_file.c_str(), text.c_str()); }
inline int pvInsertItem(PARAM *p, int id, int index, string bmp_file, string text) { return pvInsertItem(p, id, index, bmp_file.c_str(), text.c_str()); }
inline int pvRemoveItemByName(PARAM *p, int id, string name) { return pvRemoveItemByName(p, id, name.c_str()); }
inline int pvAddColumn(PARAM *p, int id, string text, int size) { return pvAddColumn(p, id, text.c_str(), size); }
inline int pvSetTableText(PARAM *p, int id, int x, int y, string text) { return pvSetTableText(p, id, x, y, text.c_str()); }
inline int pvSetTableCheckBox(PARAM *p, int id, int x, int y, int state, string text) { return pvSetTableCheckBox(p, id, x, y, state, text.c_str()); }
inline int pvSetTableComboBox(PARAM *p, int id, int x, int y, int editable, string textlist) { return pvSetTableComboBox(p, id, x, y, editable, textlist.c_str()); }
inline int pvSetListViewText(PARAM *p, int id, string path, int column, string text) { return pvSetListViewText(p, id, path.c_str(), column, text.c_str()); }
inline int pvSetPixmap(PARAM *p, int id, string bmp_file) { return pvSetPixmap(p, id, bmp_file.c_str()); }
inline int pvSetTablePixmap(PARAM *p, int id, int x, int y, string bmp_file) { return pvSetTablePixmap(p, id, x, y, bmp_file.c_str()); }
inline int pvSetSource(PARAM *p, int id, string html_file) { return pvSetSource(p, id, html_file.c_str()); }
inline int pvSetImage(PARAM *p, int id, string filename) { return pvSetImage(p, id, filename.c_str()); }
inline int pvSetFont(PARAM *p, int id, string family, int pointsize, int bold, int italic , int underline, int strikeout) { return pvSetFont(p, id, family.c_str(), pointsize, bold, italic , underline, strikeout); }
inline int pvDisplayStr(PARAM *p, int id, string str) { return pvDisplayStr(p, id, str.c_str()); }
inline int pvAddTab(PARAM *p, int id, int id_child, string str) { return pvAddTab(p, id, id_child, str.c_str()); }
inline int pvSetListViewPixmap(PARAM *p, int id, string path, string bmp_file, int column) { return pvSetListViewPixmap(p, id, path.c_str(), bmp_file.c_str(), column); }
inline int pvRemoveListViewItem(PARAM *p, int id, string path) { return pvRemoveListViewItem(p, id, path.c_str()); }
inline int pvRemoveIconViewItem(PARAM *p, int id, string text) { return pvRemoveIconViewItem(p, id, text.c_str()); }
inline int pvSetIconViewItem(PARAM *p, int id, string bmp_file, string text) { return pvSetIconViewItem(p, id, bmp_file.c_str(), text.c_str()); }
inline int pvListViewEnsureVisible(PARAM *p, int id, string path) { return pvListViewEnsureVisible(p, id, path.c_str()); }
inline int pvListViewSetOpen(PARAM *p, int id, string path, int open) { return pvListViewSetOpen(p, id, path.c_str(), open); }
inline int pvVtkTcl(PARAM *p, int id, string tcl_command) { return pvVtkTcl(p, id, tcl_command.c_str()); }
inline int pvVtkTclScript(PARAM *p, int id, string filename) { return pvVtkTclScript(p, id, filename.c_str()); }
inline int pvHyperlink(PARAM *p, string link) { return pvHyperlink(p, link.c_str()); }
inline int pvWriteFile(PARAM *p, string filename, int width, int height) { return pvWriteFile(p, filename.c_str(), width, height); }
inline int pvSave(PARAM *p, int id, string filename) { return pvSave(p, id, filename.c_str()); }
inline int pvSaveAsBmp(PARAM *p, int id, string filename) { return pvSaveAsBmp(p, id, filename.c_str()); }
inline int pvSendFile(PARAM *p, string filename) { return pvSendFile(p, filename.c_str()); }
inline int pvDownloadFileAs(PARAM *p, string filename, string newname) { return pvDownloadFileAs(p, filename.c_str(), newname.c_str()); }
inline int pvDownloadFile(PARAM *p, string filename) { return pvDownloadFile(p, filename.c_str()); }
inline int pvPopupMenu(PARAM *p, int id_return, string text) { return pvPopupMenu(p, id_return, text.c_str()); }
inline int pvMessageBox(PARAM *p, int id_return, int type, string text, int button0, int button1, int button2) { return pvMessageBox(p, id_return, type, text.c_str(), button0, button1, button2); }
inline int pvInputDialog(PARAM *p, int id_return, string text, string default_text) { return pvInputDialog(p, id_return, text.c_str(), default_text.c_str()); }
inline int qpwSetTitle(PARAM *p, int id, string text) { return qpwSetTitle(p, id, text.c_str()); }
inline int qpwSetAxisTitle(PARAM *p, int id, int pos, string text) { return qpwSetAxisTitle(p, id, pos, text.c_str()); }
inline int qpwInsertCurve(PARAM *p, int id, int index, string text) { return qpwInsertCurve(p, id, index, text.c_str()); }
inline int qpwSetMarkerLabel(PARAM *p, int id, int number, string  text) { return qpwSetMarkerLabel(p, id, number, text.c_str()); }
inline int qpwSetMarkerFont(PARAM *p, int id, int index, string family, int size, int style) { return qpwSetMarkerFont(p, id, index, family.c_str(), size, style); }
inline int qpwInsertLineMarker(PARAM *p, int id, int index, string text, int pos) { return qpwInsertLineMarker(p, id, index, text.c_str(), pos); }
inline int qpwSetAxisScaleDraw( PARAM *p, int id, int pos, string  text ) { return qpwSetAxisScaleDraw( p, id, pos, text.c_str() ); }
inline int gWriteFile(string file) { return gWriteFile(file.c_str()); }
inline int gSetFont(PARAM *p, string family, int size, int weight, int italic) { return gSetFont(p, family.c_str(), size, weight, italic); }
inline int gText(PARAM *p, int x, int y, string text, int alignment) { return gText(p, x, y, text.c_str(), alignment); }
inline int gTextInAxis(PARAM *p, float x, float y, string text, int alignment) { return gTextInAxis(p, x, y, text.c_str(), alignment); }
inline int gSetFloatFormat(PARAM *p, string text) { return gSetFloatFormat(p, text.c_str()); }
inline int gBoxWithText(PARAM *p, int x, int y, int w, int h, int fontsize, string xlabel, string ylabel, string rylabel) { return gBoxWithText(p, x, y, w, h, fontsize, xlabel.c_str(), ylabel.c_str(), rylabel.c_str()); }
inline int gComment(PARAM *p, string comment) { return gComment(p, comment.c_str()); }
inline int gPlaySVG(PARAM *p, string filename) { return gPlaySVG(p, filename.c_str()); }
inline int qwtScaleSetTitle(PARAM *p, int id, string text) { return qwtScaleSetTitle(p, id, text.c_str()); }
inline int qwtScaleSetTitleFont(PARAM *p, int id, string family, int pointsize, int bold, int italic, int underline, int strikeout) { return qwtScaleSetTitleFont(p, id, family.c_str(), pointsize, bold, italic, underline, strikeout); }

#endif
