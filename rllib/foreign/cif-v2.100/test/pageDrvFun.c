////////////////////////////////////////////////////////////////////
//  pageDrvFun.c
/////////////////////////////////////////////////////////////////////

#include <gtk/gtk.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../inc/cif_types.h"    
#include "../usr-inc/cif_user.h"   /* Include file for device driver API */
#include "../usr-inc/rcs_user.h"   /* Include file for RCS definition    */
#include "../usr-inc/asc_user.h"   /* Include file for ASCII protocols   */
#include "../usr-inc/nvr_user.h"   /* Include file for 3964R protocol    */
#include "../usr-inc/defines.h"    /* Include file for 3964R protocol    */

extern unsigned short DEV_INFO_FLAGS;

extern char *GetFilename(char *);
extern BOOL StringToHex ( char *, unsigned short, unsigned char *, unsigned short *, GtkStatusbar *, gint);
extern void HexToTextBox( GtkText *, gshort, unsigned char *);

/*{{ DrvFunPage */
gint          TimerWdog;
BOOL          fTimerWdog = FALSE;
unsigned char WATCHDOG_TRIGGER = WATCHDOG_STOP;

void DlgResetDev (char *szMessage, void (*YesFunc)(), void (*NoFunc)());
GtkWidget *CreateMenuCombobox ();
GtkWidget *CrMenuComboGTSKS ();
GtkWidget *CrMenuComboGTSKP ();
GtkWidget *CrMenuComboPTSKP ();
GtkWidget *CrMenuComboSHS ();
GtkWidget *DrvFunPage = NULL;
GtkWidget *DevMBXData, *HOSTMBXData;
    gint  context_id_DrvFun;
GtkWidget *status_bar_DrvFun;
GtkWidget *MBXStateDEV, *MBXStateHOST, *CtrlState, *WdogVal, *WdogInterv, *StartWdog;
GtkWidget *ReadSndData, *ReadSndOff, *ReadSndLen, *RWRawOff, *RWRawLen, *RWRawWData, *RWRawRData;
GtkWidget *ResetTimeout, *TASKStateSize, *valTaskState, *TaskParamSize, *valTaskParam, *TaskParamSizePut, *valTaskParamPut;
gshort    usResetFun    = COLDSTART;  
gshort    usHostState   = HOST_READY; 
gshort    usRWRfun = PARAMETER_READ;   // PARAMETER_WRITE
gshort    usSpCtrlState = SPECIAL_CONTROL_SET;// SPECIAL_CONTROL_CLEAR
unsigned short usTASKStateArea =  1, usTaskParamArea =  1;
unsigned short usTASKStateSize = 64, usTASKParamSize = 64, usTASKParamSizePut = 64;
long      lTimeout = 10000;      // Reset timeout
/*}} DrvFunPage */

void CloseDialog_ (GtkWidget *widget, gpointer data)
{
  gtk_widget_destroy (GTK_WIDGET (data));
}

void CloseTriggerWatchdogDialog_ (GtkWidget *widget, gpointer data)
{
  if ( fTimerWdog) {
    gtk_timeout_remove( TimerWdog);
    fTimerWdog = FALSE;
    WATCHDOG_TRIGGER = WATCHDOG_STOP;
  }
  gtk_widget_destroy (GTK_WIDGET (data));
}

void ClosingDialog (GtkWidget *widget, gpointer data)
{
  if ( fTimerWdog) {
    gtk_timeout_remove( TimerWdog);
    fTimerWdog = FALSE;
  }
  gtk_grab_remove (GTK_WIDGET (widget));
}

void DlgQuit (GtkWidget *widget, gpointer data)
{
  if ( fTimerWdog) {
    gtk_timeout_remove( TimerWdog);
    fTimerWdog = FALSE;
  }
  gtk_widget_destroy (GTK_WIDGET (data));
}

void Popup (char *szMessage)
{
    static GtkWidget *label;
    GtkWidget *button;
    GtkWidget *dialog_window;

    dialog_window = gtk_dialog_new ();

    gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
	                GTK_SIGNAL_FUNC (ClosingDialog),
	                &dialog_window);
    gtk_window_set_title (GTK_WINDOW (dialog_window), "Popup");
    gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);

    label = gtk_label_new (szMessage);
    gtk_misc_set_padding (GTK_MISC (label), 10, 10);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->vbox), 
                        label, TRUE, TRUE, 0);
    gtk_widget_show (label);
    button = gtk_button_new_with_label ("Ok");
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
	                GTK_SIGNAL_FUNC (CloseDialog_),
	                dialog_window);
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
			  button, TRUE, TRUE, 0);
    gtk_widget_grab_default (button);
    gtk_widget_show (button);
    gtk_widget_show (dialog_window);

    gtk_grab_add (dialog_window);
}

void combo_ResetFunSel (GtkWidget *widget, gpointer *data)
{
   usResetFun = (gint)data;
}

void combo_HStateSelect (GtkWidget *widget, gpointer *data)
{
   usHostState = (gint)data;
}

void combo_TStateSelect (GtkWidget *widget, gpointer *data)
{
   usTASKStateArea = (gint)data;
}

void combo_TParamSelect (GtkWidget *widget, gpointer *data)
{
   usTaskParamArea = (gint)data;
}

GtkWidget *make_menu_item( gchar         *name,
                           GtkSignalFunc  callback,
			   gpointer       data )
{
  GtkWidget *item;
  
  item = gtk_menu_item_new_with_label (name);
  gtk_signal_connect (GTK_OBJECT (item), "activate",
		      callback, data);
  gtk_widget_show (item);
  
  return(item);
}

GtkWidget *CreateMenuCombobox ()
{
  //#define NUM_COMBO_ITEMS 3   
  //    char *state_list[NUM_COMBO_ITEMS] = { "COLDSTART",
  //                                          "WARMSTART",
  //                                          "BOOTSTART" };
  GtkWidget *omenu;
  GtkWidget *menu;
  GtkWidget *menuitem;
  
  omenu = gtk_option_menu_new();
  menu = gtk_menu_new();

  usResetFun = COLDSTART;  

  menuitem = make_menu_item ("COLDSTART", GTK_SIGNAL_FUNC (combo_ResetFunSel), 
			     GINT_TO_POINTER(COLDSTART));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  menuitem = make_menu_item ("WARMSTART", GTK_SIGNAL_FUNC (combo_ResetFunSel),
			     GINT_TO_POINTER(WARMSTART));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  menuitem = make_menu_item ("BOOTSTART", GTK_SIGNAL_FUNC (combo_ResetFunSel),
			     GINT_TO_POINTER(BOOTSTART));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
  gtk_widget_show (omenu);

  return (omenu);
}

GtkWidget *CrMenuComboSHS ()
{
  GtkWidget *omenu;
  GtkWidget *menu;
  GtkWidget *menuitem;
  
  omenu = gtk_option_menu_new();
  menu = gtk_menu_new();

  usHostState   = HOST_READY; 
  
  menuitem = make_menu_item ("HOST_READY", GTK_SIGNAL_FUNC (combo_HStateSelect), 
			     GINT_TO_POINTER(HOST_READY));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  menuitem = make_menu_item ("HOST_NOT_READY", GTK_SIGNAL_FUNC (combo_HStateSelect),
			     GINT_TO_POINTER(HOST_NOT_READY));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
  gtk_widget_show (omenu);
  
  return (omenu);
}

GtkWidget *CrMenuComboPTSKP ()
{
  GtkWidget *omenu;
  GtkWidget *menu;
  GtkWidget *menuitem;
  
  omenu = gtk_option_menu_new();
  menu = gtk_menu_new();
  
  usTaskParamArea = 1;

  menuitem = make_menu_item ("TASK1", GTK_SIGNAL_FUNC (combo_TParamSelect), 
			     GINT_TO_POINTER( 1));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  menuitem = make_menu_item ("TASK2", GTK_SIGNAL_FUNC (combo_TParamSelect),
			     GINT_TO_POINTER( 2));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("TASK3", GTK_SIGNAL_FUNC (combo_TParamSelect),
			     GINT_TO_POINTER( 3));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("TASK4", GTK_SIGNAL_FUNC (combo_TParamSelect),
			     GINT_TO_POINTER( 4));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("TASK5", GTK_SIGNAL_FUNC (combo_TParamSelect),
			     GINT_TO_POINTER( 5));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("TASK6", GTK_SIGNAL_FUNC (combo_TParamSelect),
			       GINT_TO_POINTER( 6));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("TASK7", GTK_SIGNAL_FUNC (combo_TParamSelect),
			     GINT_TO_POINTER( 7));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
  gtk_widget_show (omenu);

  return (omenu);
}

GtkWidget *CrMenuComboGTSKP ()
{
  GtkWidget *omenu;
  GtkWidget *menu;
  GtkWidget *menuitem;
  
  usTaskParamArea = 1;

  omenu = gtk_option_menu_new();
  menu = gtk_menu_new();
  
  menuitem = make_menu_item ("TASK1", GTK_SIGNAL_FUNC (combo_TParamSelect), 
			     GINT_TO_POINTER( 1));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  menuitem = make_menu_item ("TASK2", GTK_SIGNAL_FUNC (combo_TParamSelect),
			     GINT_TO_POINTER( 2));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
  gtk_widget_show (omenu);
  
  return (omenu);
}

GtkWidget *CrMenuComboGTSKS ()
{
  GtkWidget *omenu;
  GtkWidget *menu;
  GtkWidget *menuitem;
  
  omenu = gtk_option_menu_new();
  menu = gtk_menu_new();

  usTASKStateArea = 1;
  
  menuitem = make_menu_item ("TASK1", GTK_SIGNAL_FUNC (combo_TStateSelect), 
			     GINT_TO_POINTER( 1));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  menuitem = make_menu_item ("TASK2", GTK_SIGNAL_FUNC (combo_TStateSelect),
			     GINT_TO_POINTER( 2));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
  gtk_widget_show (omenu);
  
  return (omenu);
}

void SetHOSTS (GtkWidget *widget, gpointer data)
{
  unsigned short  usBoard;
  short           sRet;          
  char            buff[96];
  gchar          *str;
  
       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    str = gtk_entry_get_text (GTK_ENTRY (ResetTimeout));
    sscanf(str, "%ld", &lTimeout);
    if ( (sRet = DevSetHostState( usBoard, usHostState, lTimeout)) != DRV_NO_ERROR) {// Show error
      g_snprintf(buff, 72, "Error by driver function DevSetHostState(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun,  buff);
    } else {// Reset OK
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun, "  DevSetHostState() OK !");
    }
  }
 leave: ;
}

void DevReset_ (GtkWidget *widget, gpointer data)
{
  unsigned short usBoard;
  short          sRet;        
  char           buff[96];
  gchar          *str;

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    str = gtk_entry_get_text (GTK_ENTRY (ResetTimeout));
    sscanf(str, "%ld", &lTimeout);
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun,  "                 ");
    if ( (sRet = DevReset( usBoard, usResetFun, lTimeout)) != DRV_NO_ERROR) {// Show error
      g_snprintf(buff, 72, "Error by driver function DevReset(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun,  buff);
    } else {// Reset OK
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun,  "  DevReset() OK !");
    }
  }
 leave: ;
}

void DlgResetDev (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget *label;
  GtkWidget *box, *button, *hbox;
  GtkWidget *dialog_window;

  dialog_window = gtk_dialog_new ();

  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "Reset Device");

  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);

  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Function: ");
  gtk_widget_set_usize( label, 80, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
  box = CreateMenuCombobox ();
  gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 10);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Timeout: ");
  gtk_widget_set_usize( label, 80, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
  ResetTimeout = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( ResetTimeout), "10000");
  gtk_entry_set_editable(GTK_ENTRY( ResetTimeout), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), ResetTimeout, FALSE, FALSE, 4);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  button = gtk_button_new_with_label ("Reset");  
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (YesFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_show (button);
  button = gtk_button_new_with_label ("Close");
  
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);

  gtk_widget_show (button);
  gtk_widget_show (dialog_window);

  gtk_grab_add (dialog_window);
}

void ResetDev (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " invoking DevReset() function ...");
  DlgResetDev ("", DevReset_, DlgQuit);
}

void DlgSetHostState (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget *label;
  GtkWidget *box, *button, *hbox;
  GtkWidget *dialog_window;

  dialog_window = gtk_dialog_new ();
  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "Set HOST state");
  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);
  
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("State: ");
  gtk_widget_set_usize( label, 80, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
  box = CrMenuComboSHS ();
  gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 10);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Timeout: ");
  gtk_widget_set_usize( label, 80, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
  ResetTimeout = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( ResetTimeout), "10000");
  gtk_entry_set_editable(GTK_ENTRY( ResetTimeout), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), ResetTimeout, FALSE, FALSE, 4);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  
  button = gtk_button_new_with_label ("set");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (YesFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_show (button);
  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (dialog_window);
  gtk_grab_add (dialog_window);
}

void SetHostState (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " invoking SetHostState() function ...");
  DlgSetHostState ("", SetHOSTS, DlgQuit);
}

void DlgGetTaskState (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget *label;
  GtkWidget *box, *button, *hbox;
  GtkWidget *dialog_window;
  //  GdkFont   *fixed_font;
  //  GtkStyle  *defaultstyle, style;
  
  dialog_window = gtk_dialog_new ();
  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "Get Task State");
  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);
  
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("State area: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  box = CrMenuComboGTSKS ();
  gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("size: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  TASKStateSize = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( TASKStateSize), "64");
  gtk_entry_set_editable(GTK_ENTRY( TASKStateSize), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), TASKStateSize, TRUE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("value: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  valTaskState = gtk_text_new (NULL, NULL);
  gtk_text_set_editable (GTK_TEXT ( valTaskState), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), valTaskState, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);

  //fixed_font = gdk_font_load ("-misc-fixed-medium-r-*-*-*-100-*-*-*-*-*-*");
  //defaultstyle = gtk_widget_get_style( (GtkWidget *)valTaskState);
  //style = *defaultstyle;
  //style.font = fixed_font;
  //gtk_widget_set_style( (GtkWidget *)valTaskState, (GtkStyle *)&style);
  //gtk_widget_set_default_style( defaultstyle);
  //gtk_box_pack_start (GTK_BOX (hbox), valTaskState, TRUE, TRUE, 0);
  //gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  //gtk_widget_show_all ( hbox);
  
  button = gtk_button_new_with_label ("Get");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (YesFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_show (button);
  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (dialog_window);
  gtk_grab_add (dialog_window);
}

void DoGetTaskState (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  unsigned char   abTaskState[64];    
  short           sRet;               
  char            buff[96];
       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    if ( (sRet = DevGetTaskState(usBoard,usTASKStateArea,usTASKStateSize,&abTaskState[0])) != DRV_NO_ERROR) {
	g_snprintf(buff, 72, "Error by driver function DevGetTaskState(). (Error: %d)", sRet);
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
	goto leave;
    } else {
      HexToTextBox( (GtkText *) valTaskState,
	                        usTASKStateSize, 
		                &abTaskState[0]);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " Done");
    }
  }
 leave: ;
}

void GetTaskState (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop (GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun );
  gtk_statusbar_push(GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun," invoking DevGetTaskState() function ...");
  DlgGetTaskState ("", DoGetTaskState, DlgQuit);
}

void WatchdogStart ( gpointer *data)
{
  short sRet;
  char  buff[96];
  unsigned short  usDevWatchDog = 0;    // Watchdog value
  unsigned short  usBoard = (gint)data;

  if ( (sRet = DevTriggerWatchDog( usBoard,
				   WATCHDOG_START,
				   &usDevWatchDog)) != DRV_NO_ERROR) {
    g_snprintf(buff, 72, "Error by driver function DevTriggerWatchdog(). (Error: %d)", sRet);
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun,  buff);
  } else {
    g_snprintf(buff, 72, "%d", usDevWatchDog);
    gtk_entry_set_text( GTK_ENTRY(WdogVal), buff);
  }
}

void DoTriggerWDog (GtkWidget *widget, gpointer data)
{
  unsigned short  usBoard;
  unsigned short  usDevWatchDog;    // Watchdog value
  long            lTime;           // Intervall time
  short           sRet;           
  char            buff[96];
  gchar           *str;

  str = gtk_entry_get_text (GTK_ENTRY (WdogInterv));
  sscanf( str, "%ld", &lTime);
  if (lTime <= 20) {// TIMEOUT NOT ALLOWED
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, 
			"Intervall less or equal 20 ms not allowed!");
    goto leave;
  }
       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    if ( WATCHDOG_TRIGGER == WATCHDOG_START) {
      if ( fTimerWdog) {
	gtk_timeout_remove( TimerWdog);
	fTimerWdog = FALSE;
      }
      gtk_widget_set_name( StartWdog,  "&Start watchdog");
      if ( (sRet = DevTriggerWatchDog( usBoard,
				       WATCHDOG_STOP,
				       &usDevWatchDog)) != DRV_NO_ERROR) {
	g_snprintf(buff, 72, "Error by driver function DevTriggerWatchdog(). (Error: %d)", sRet);
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun,  buff);
      } else {
	g_snprintf(buff, 72, "%d", usDevWatchDog);
	gtk_entry_set_text( GTK_ENTRY(WdogVal), buff);
      }
      WATCHDOG_TRIGGER = WATCHDOG_STOP;
    }
    else {
      WATCHDOG_TRIGGER = WATCHDOG_START;
      fTimerWdog = TRUE;
      gtk_widget_set_name((GtkWidget *)StartWdog,  "&Stop watchdog");
      TimerWdog  = gtk_timeout_add( lTime, (GtkFunction)WatchdogStart, GINT_TO_POINTER( (gint)usBoard));
    }
  }
 leave: ;
}

void DlgTriggerWDog (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget *label;
  GtkWidget *button, *hbox;
  GtkWidget *dialog_window;

  WATCHDOG_TRIGGER = WATCHDOG_STOP;

  dialog_window = gtk_dialog_new ();
  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "DevTriggerWatchdog()");
  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);

  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Intervall: ");
  gtk_widget_set_usize( label, 72, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
  WdogInterv = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( WdogInterv), "500");
  gtk_entry_set_editable(GTK_ENTRY( WdogInterv), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), WdogInterv, FALSE, FALSE, 4);
  label = gtk_label_new ("ms");
  gtk_widget_set_usize( label, 15, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 2);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Value: ");
  gtk_widget_set_usize( label, 72, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
  WdogVal = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( WdogVal), "0");
  gtk_entry_set_editable(GTK_ENTRY( WdogVal), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), WdogVal, FALSE, FALSE, 4);
  label = gtk_label_new ("  ");
  gtk_widget_set_usize( label, 15, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 2);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  
  StartWdog = gtk_toggle_button_new_with_label("Start/Stop watchdog");
  gtk_signal_connect (GTK_OBJECT (StartWdog),  "toggled", 
		      GTK_SIGNAL_FUNC ( YesFunc),
		      dialog_window);

  GTK_WIDGET_SET_FLAGS ( StartWdog, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      StartWdog, TRUE, TRUE, 0);
  gtk_widget_show ( StartWdog);
  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (dialog_window);
  gtk_grab_add (dialog_window);
}

void DevTriggerWatchdog (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop (GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun );
  gtk_statusbar_push(GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun, " invoking DevTriggerWatchdog() function ..");
  DlgTriggerWDog ("", DoTriggerWDog, DlgQuit);
}

void DlgPutTaskParam (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget *label;
  GtkWidget *box, *button, *hbox;
  GtkWidget *dialog_window;

  dialog_window = gtk_dialog_new ();
  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "Put Task Parameter");
  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);
  
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Parameter area: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  box = CrMenuComboPTSKP ();
  gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("size: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  TaskParamSizePut = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( TaskParamSizePut), "64");
  gtk_entry_set_editable(GTK_ENTRY( TaskParamSizePut), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), TaskParamSizePut, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Parameter data: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  valTaskParamPut = gtk_text_new (NULL, NULL);
  gtk_text_set_editable (GTK_TEXT ( valTaskParamPut), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), valTaskParamPut, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  
  button = gtk_button_new_with_label ("Put");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (YesFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_show (button);
  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (dialog_window);
  gtk_grab_add (dialog_window);
}

void DoPutTaskParam (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  unsigned char   abTaskParam[64];  
  short           sRet;            
  char            *chPutParam, buff[96];

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    chPutParam = gtk_editable_get_chars( GTK_EDITABLE( valTaskParamPut), (gint) 0, (gint) -1);
    if ( StringToHex( chPutParam,
		      sizeof(abTaskParam),
		      &abTaskParam[0],
		      &usTASKParamSizePut,
		      (GtkStatusbar *)status_bar_DrvFun,
		      context_id_DrvFun) ) {
      if ((sRet=DevPutTaskParameter(usBoard, usTaskParamArea, usTASKParamSizePut, &abTaskParam[0]))!=DRV_NO_ERROR) {
	g_snprintf(buff, 72, "Error by driver function DevPutTaskParam(). (Error: %d)", sRet);
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
	goto leave;
      } else {
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " Done");
      }
    }
  }
 leave: ;
}

void PutTaskParam (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop (GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun );
  gtk_statusbar_push(GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun,
		     " Function 'PutTaskParam()' not implemented yet!");
  DlgPutTaskParam ("", DoPutTaskParam, DlgQuit);
}

void DlgGetTaskParam (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget *label;
  GtkWidget *box, *button, *hbox;
  GtkWidget *dialog_window;

  dialog_window = gtk_dialog_new ();
  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "Get Task Parameter");
  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);
  
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Parameter area: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  box = CrMenuComboGTSKP ();
  gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("size: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  TaskParamSize = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( TaskParamSize), "64");
  gtk_entry_set_editable(GTK_ENTRY( TaskParamSize), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), TaskParamSize, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Parameter data: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  valTaskParam = gtk_text_new (NULL, NULL);
  gtk_text_set_editable (GTK_TEXT ( valTaskParam), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), valTaskParam, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);

  button = gtk_button_new_with_label ("Get");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (YesFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_show (button);
  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (dialog_window);
  gtk_grab_add (dialog_window);
}

void DoGetTaskParam (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  unsigned char   abTaskParam[64];  
  short           sRet;            
  char            buff[96];

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    if ((sRet=DevGetTaskParameter(usBoard,usTaskParamArea,usTASKParamSize,&abTaskParam[0]))!=DRV_NO_ERROR) {
	g_snprintf(buff, 72, "Error by driver function DevGetTaskParam(). (Error: %d)", sRet);
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
	goto leave;
    } else {
      HexToTextBox( (GtkText *) valTaskParam,
                                usTASKParamSize, 
                                &abTaskParam[0]);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " Done");
    }
  }
 leave: ;
}

void GetTaskParam (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop (GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun );
  gtk_statusbar_push(GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun, " invoking DevGetTaskParam() function ...");
  DlgGetTaskParam ("", DoGetTaskParam, DlgQuit);
}

void Download (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  short           sRet;            
  char            buff[128];
  unsigned long   dwNumOfBytes;
  char            *fileName;

  gtk_statusbar_pop (GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun );
  gtk_statusbar_push(GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun, " invoking DevDownload() function ...");

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    fileName = GetFilename (" get file to download (*.h*, *.dbm)");
    if (fileName) {
      if( strstr( fileName, ".DBM") || strstr( fileName, ".dbm") ) {   
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "Downloading Configuration ..."); 
	if ( (sRet = DevDownload (  usBoard,                           // Board number
				    CONFIGURATION_DOWNLOAD,            // Mode
				    (unsigned char*)&fileName[0],      // File name
				    &dwNumOfBytes)) != DRV_NO_ERROR) { // Number of bytes transfered
	  g_snprintf(buff, 128, "DevDownload() error: %d, \nconfiguration file: %s", sRet, fileName);
	  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
	} else {
	  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "Configuration downloaded!");
	}
      } else if ( strstr( fileName, ".H") || strstr( fileName, ".h") ) {
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "Downloading Firmware ..."); 
	if ( (sRet = DevDownload (  usBoard,                          
				    FIRMWARE_DOWNLOAD,                
				    (unsigned char*)&fileName[0],     
				    &dwNumOfBytes)) != DRV_NO_ERROR) { // Number of bytes transfered
	  g_snprintf(buff, 128, "DevDownload() error: %d, \nfirmware file: %s", sRet, fileName);
	  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
	} else {
	  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "Firmware downloaded!");
	}
      }
      else {
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "uNKNoWN file extention!");
      }
    }
  }
 leave: ;
}

void GetMBXSTATE (GtkWidget *widget, gpointer data)
{
  unsigned short  usBoard;
  unsigned short  usHOSTState;
  unsigned short  usDEVICEState;
  short           sRet;    
  char            buff[96];

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    if ( (sRet = DevGetMBXState( usBoard, &usDEVICEState, &usHOSTState)) != DRV_NO_ERROR) {
      g_snprintf(buff, 72, "Error by driver function DevGetMBXState(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun,  buff);
    } else {
      if ( usHOSTState == HOST_MBX_EMPTY) gtk_entry_set_text((GtkEntry *)MBXStateHOST, "Message NOT available");
      else gtk_entry_set_text((GtkEntry *)MBXStateHOST, "Message available");

      if ( usDEVICEState == DEVICE_MBX_EMPTY) gtk_entry_set_text((GtkEntry *)MBXStateDEV, "Device mailbox EMPTY");
      else gtk_entry_set_text((GtkEntry *)MBXStateDEV, "Device mailbox FULL");

      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "  DevGetMBXState() OK !");
    }
  }
 leave: ;
}

void DlgGetMBXState (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget *label;
  GtkWidget *button, *hbox;
  GtkWidget *dialog_window;

  dialog_window = gtk_dialog_new ();

  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "DevGetMBXState()");
  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);

  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Mailbox State ");
  gtk_widget_set_usize( label, 80, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 4);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("DEVICE: ");
  gtk_widget_set_usize( label, 72, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
  MBXStateDEV = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( MBXStateDEV), "UNKNOWN");
  gtk_entry_set_editable(GTK_ENTRY( MBXStateDEV), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), MBXStateDEV, FALSE, FALSE, 4);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("HOST: ");
  gtk_widget_set_usize( label, 72, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
  MBXStateHOST = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( MBXStateHOST), "UNKNOWN");
  gtk_entry_set_editable(GTK_ENTRY( MBXStateHOST), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), MBXStateHOST, FALSE, FALSE, 4);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);

  button = gtk_button_new_with_label ("get");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (YesFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_show (button);
  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (dialog_window);
  gtk_grab_add (dialog_window);
}

void GetMBXState (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " invoking GetMBXState() function ...");
  DlgGetMBXState ("", GetMBXSTATE, DlgQuit);
}

void GetMBXDATA (GtkWidget *widget, gpointer data)
{
  unsigned short  usBoard;
  MSG_STRUC       tDevBuffer;
  MSG_STRUC       tHostBuffer;
  short           sRet;          
  char            buff[96];

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    if ( (sRet = DevGetMBXData( usBoard,
				sizeof( tHostBuffer),
				(void *)&tHostBuffer,
				sizeof( tDevBuffer),
				(void *)&tDevBuffer)) != DRV_NO_ERROR) {
      g_snprintf(buff, 72, "Error by driver function DevGetMBXData(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun,  buff);
    } else {
      HexToTextBox( (GtkText *) DevMBXData,
		    sizeof( tDevBuffer), 
		    (char *)&tDevBuffer);
      HexToTextBox( (GtkText *) HOSTMBXData,
		    sizeof( tHostBuffer), 
		    (char *)&tHostBuffer);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "  DevGetMBXData() OK !");
    }
  }
 leave: ;
}

void DlgGetMBXData (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget  *label;
  GtkWidget  *table, *tableR;
  GtkWidget  *vscrollbar;
  GtkWidget  *button, *hbox, *vbox;
  GtkWidget  *dialog_window;

  dialog_window = gtk_dialog_new ();

  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "DevGetMBXData()");
  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);
  
  vbox = gtk_vbox_new (FALSE, 0);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Device MBX: ");
  gtk_widget_set_usize( label, 72, 10);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 4);

  table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacing (GTK_TABLE (table), 0, 2);
  gtk_table_set_col_spacing (GTK_TABLE (table), 0, 2);
  gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0);
  gtk_widget_show (table);

  DevMBXData = gtk_text_new (NULL, NULL);
  gtk_text_set_editable(GTK_TEXT( DevMBXData), FALSE);
  gtk_table_attach (GTK_TABLE (table), DevMBXData, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show ( DevMBXData);

  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (DevMBXData)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (vscrollbar);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  vbox = gtk_vbox_new (FALSE, 0);
  label = gtk_label_new ("HOST MBX: ");
  gtk_widget_set_usize( label, 72, 10);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 4);

  tableR = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacing (GTK_TABLE (tableR), 0, 2);
  gtk_table_set_col_spacing (GTK_TABLE (tableR), 0, 2);
  gtk_box_pack_start (GTK_BOX (vbox), tableR, TRUE, TRUE, 0);
  gtk_widget_show (tableR);

  HOSTMBXData = gtk_text_new (NULL, NULL);
  gtk_text_set_editable(GTK_TEXT( HOSTMBXData), FALSE);
  gtk_table_attach (GTK_TABLE (tableR), HOSTMBXData, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show ( HOSTMBXData);

  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (HOSTMBXData)->vadj);
  gtk_table_attach (GTK_TABLE (tableR), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (vscrollbar);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
  gtk_widget_show_all ( hbox);
  button = gtk_button_new_with_label ("get");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (YesFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_show (button);
  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (dialog_window);
  gtk_grab_add (dialog_window);
}

void GetMBXData (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " invoking GetMBXState() function ...");
  DlgGetMBXData ("", GetMBXDATA, DlgQuit);
}


void ReadSendDATA (GtkWidget *widget, gpointer data)
{
  unsigned short usBoard;
  unsigned char  abDataBuffer[512];  // Data buffer
  long           lOffset;            // Data offset
  long           lSize;              // Data size
  short          sRet;          
  gchar          *str;
  char           buff[96];

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    str = gtk_entry_get_text (GTK_ENTRY (ReadSndOff));
    sscanf(str, "%ld", &lOffset);
    str = gtk_entry_get_text (GTK_ENTRY (ReadSndLen));
    sscanf(str, "%ld", &lSize);
    if( lSize > sizeof( abDataBuffer) || lSize == 0) { 
      gtk_text_backward_delete ( (GtkText *) ReadSndData, gtk_text_get_length( (GtkText *) ReadSndData));
      if( lSize == 0) {
	gtk_text_insert ( (GtkText *) ReadSndData, NULL, NULL, NULL,
			  "\n ERROR:  size is zero!", -1);
	g_snprintf(buff, 72, " DevReadSendData(): size is zero!");
      }
      else {
	gtk_text_insert ( (GtkText *) ReadSndData, NULL, NULL, NULL,
			  "\n ERROR:  size too long!", -1);
	g_snprintf(buff, 72, " DevReadSendData(): size too long!");
      }
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun,  buff);
    } else {
      if ( (sRet = DevReadSendData( usBoard,
				    (unsigned short)lOffset,
				    (unsigned short)lSize,
				    &abDataBuffer[0])) != DRV_NO_ERROR) {
	g_snprintf(buff, 72, "Error by driver function DevReadSendData(). (Error: %d)", sRet);
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun,  buff);
      } else {
	HexToTextBox( (GtkText *) ReadSndData,
		      lSize,///sizeof( abDataBuffer), 
		      (char *)abDataBuffer);
	
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "  DevReadSendData() OK !");
      }
    }
  }
 leave: ;
}

void DlgReadSendData (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget  *label;
  GtkWidget  *tableR;
  GtkWidget  *vscrollbar;
  GtkWidget  *button, *hbox, *vbox;
  GtkWidget  *dialog_window;

  dialog_window = gtk_dialog_new ();
  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "DevReadSendData()");
  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);
  
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Offset: ");
  gtk_widget_set_usize( label, 64, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  ReadSndOff = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( ReadSndOff), "0");
  gtk_entry_set_editable(GTK_ENTRY( ReadSndOff), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), ReadSndOff, FALSE, FALSE, 2);
  label = gtk_label_new ("Size: ");
  gtk_widget_set_usize( label, 64, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  ReadSndLen = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( ReadSndLen), "0");
  gtk_entry_set_editable(GTK_ENTRY( ReadSndLen), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), ReadSndLen, FALSE, FALSE, 2);

  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);

  vbox = gtk_vbox_new (FALSE, 0);
  label = gtk_label_new ("Send data: ");
  gtk_widget_set_usize( label, 72, 10);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 4);

  tableR = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacing (GTK_TABLE (tableR), 0, 2);
  gtk_table_set_col_spacing (GTK_TABLE (tableR), 0, 2);
  gtk_box_pack_start (GTK_BOX (vbox), tableR, TRUE, TRUE, 0);
  gtk_widget_show (tableR);

  ReadSndData = gtk_text_new (NULL, NULL);
  gtk_text_set_editable(GTK_TEXT( ReadSndData), FALSE);
  gtk_table_attach (GTK_TABLE (tableR), ReadSndData, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show ( ReadSndData);

  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (ReadSndData)->vadj);
  gtk_table_attach (GTK_TABLE (tableR), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (vscrollbar);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), vbox);
  gtk_widget_show_all ( vbox);

  button = gtk_button_new_with_label ("get");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (YesFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_show (button);
  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (dialog_window);
  gtk_grab_add (dialog_window);
}

void ReadSendData (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " invoking DevReadSendData() function ...");
  DlgReadSendData ("", ReadSendDATA, DlgQuit);
}

void combo_SpecialCtrl (GtkWidget *widget, gpointer *data)
{
  usSpCtrlState = (gint)data;
}

GtkWidget *CrMenuComboSPC ()
{
  GtkWidget *omenu;
  GtkWidget *menu;
  GtkWidget *menuitem;
  
  omenu = gtk_option_menu_new();
  menu = gtk_menu_new();

  usSpCtrlState = SPECIAL_CONTROL_SET;// SPECIAL_CONTROL_CLEAR

  menuitem = make_menu_item ("SPECIAL_CONTROL_SET", GTK_SIGNAL_FUNC (combo_SpecialCtrl), 
			     GINT_TO_POINTER( 1));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  menuitem = make_menu_item ("SPECIAL_CONTROL_CLEAR", GTK_SIGNAL_FUNC (combo_SpecialCtrl),
			     GINT_TO_POINTER( 2));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
  gtk_widget_show (omenu);
  
  return (omenu);
}

void DlgSpecialCtrl (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget *label;
  GtkWidget *box, *button, *hbox;
  GtkWidget *dialog_window;
  
  dialog_window = gtk_dialog_new ();
  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "SpecialControl()");
  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);

  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("State: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  box = CrMenuComboSPC ();
  gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Control State: ");
  gtk_widget_set_usize( label, 100, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  CtrlState = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( CtrlState), "UNKNOWN");
  gtk_entry_set_editable(GTK_ENTRY( CtrlState), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), CtrlState, TRUE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
  gtk_widget_show_all ( hbox);
  
  button = gtk_button_new_with_label ("Set state");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (YesFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_show (button);
  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (dialog_window);
  gtk_grab_add (dialog_window);
}

void DoSpecialCtrl (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  unsigned short  usDeviceState = 1;    // Device State
  char            buff[96];
  short           sRet;               

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    if(usSpCtrlState == 1) usSpCtrlState = SPECIAL_CONTROL_SET;
    else                   usSpCtrlState = SPECIAL_CONTROL_CLEAR;
    if ( (sRet = DevSpecialControl( usBoard, usSpCtrlState, &usDeviceState)) != DRV_NO_ERROR) {
      g_snprintf(buff, 72, "Error by driver function DevSpecialControl(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
    } else {
      if     ( usDeviceState == SPECIAL_CONTROL_SET)   gtk_entry_set_text  ( GTK_ENTRY(CtrlState), "SPECIAL_CONTROL_SET");
      else if( usDeviceState == SPECIAL_CONTROL_CLEAR) gtk_entry_set_text( GTK_ENTRY(CtrlState), "SPECIAL_CONTROL_CLEAR"); 
      else                                             gtk_entry_set_text( GTK_ENTRY(CtrlState), "UNKNOWN");
      
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " Done");
    }
  }
 leave: ;
}

void SetSpecialControl (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop (GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun );
  gtk_statusbar_push(GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun," invoking SetSpecialCtrl() function ...");
  DlgSpecialCtrl ("", DoSpecialCtrl, DlgQuit);
}

void combo_RWR (GtkWidget *widget, gpointer *data)
{
  usRWRfun = (gint)data;
}

GtkWidget *CrMenuComboRWR ()
{
  GtkWidget *omenu;
  GtkWidget *menu;
  GtkWidget *menuitem;
  
  omenu = gtk_option_menu_new();
  menu  = gtk_menu_new();

  usRWRfun = PARAMETER_READ;   // PARAMETER_WRITE
  
  menuitem = make_menu_item ("READ", GTK_SIGNAL_FUNC (combo_RWR), 
			     GINT_TO_POINTER( 1));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  menuitem = make_menu_item ("WRITE", GTK_SIGNAL_FUNC (combo_RWR),
			     GINT_TO_POINTER( 2));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
  gtk_widget_show (omenu);

  return (omenu);
}

void DlgRWRaw (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
  GtkWidget *label;
  GtkWidget *box, *button, *hbox, *vbox, *vbox1, *vbox2;
  GtkWidget *dialog_window;
  GtkWidget *tableW, *tableR;
  GtkWidget *vscrollbarW, *vscrollbarR;
  
  dialog_window = gtk_dialog_new ();
  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC (ClosingDialog),
		      &dialog_window);
  gtk_window_set_title (GTK_WINDOW (dialog_window), "ReadWriteRaw()");
  gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);
  
  vbox = gtk_vbox_new (FALSE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("Function: ");
  gtk_widget_set_usize( label, 80, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  box = CrMenuComboRWR ();
  gtk_box_pack_start (GTK_BOX (hbox), box, FALSE, FALSE, 0);
  label = gtk_label_new ("DPM offset: ");
  gtk_widget_set_usize( label, 80, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  RWRawOff = gtk_entry_new ();
  gtk_widget_set_usize( RWRawOff, 80, 20);
  gtk_entry_set_text (GTK_ENTRY ( RWRawOff), "0");
  gtk_entry_set_editable(GTK_ENTRY( RWRawOff), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), RWRawOff, FALSE, FALSE, 0);
  label = gtk_label_new ("Length: ");
  gtk_widget_set_usize( label, 80, 10);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  RWRawLen = gtk_entry_new ();
  gtk_widget_set_usize( RWRawLen, 80, 20);
  gtk_entry_set_text (GTK_ENTRY ( RWRawLen), "0");
  gtk_entry_set_editable(GTK_ENTRY( RWRawLen), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), RWRawLen, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_widget_show_all ( hbox);
  
  vbox1 = gtk_vbox_new (FALSE, 2);
  vbox2 = gtk_vbox_new (FALSE, 2);
  hbox = gtk_hbox_new (FALSE, 0);
  
  tableW = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacing (GTK_TABLE (tableW), 0, 2);
  gtk_table_set_col_spacing (GTK_TABLE (tableW), 0, 2);
  gtk_box_pack_start (GTK_BOX (vbox1), tableW, TRUE, TRUE, 0);
  gtk_widget_show (tableW);

  RWRawWData = gtk_text_new (NULL, NULL);
  gtk_text_set_editable (GTK_TEXT ( RWRawWData), TRUE);
  gtk_table_attach (GTK_TABLE (tableW), RWRawWData, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show ( RWRawWData);

  vscrollbarW = gtk_vscrollbar_new (GTK_TEXT (RWRawWData)->vadj);
  gtk_table_attach (GTK_TABLE (tableW), vscrollbarW, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (vscrollbarW);

  label = gtk_label_new ("Write Data");
  gtk_widget_set_usize( label, 80, 15);
  gtk_box_pack_start (GTK_BOX (vbox1), label, FALSE, FALSE, 0);
  
  tableR = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacing (GTK_TABLE (tableR), 0, 2);
  gtk_table_set_col_spacing (GTK_TABLE (tableR), 0, 2);
  gtk_box_pack_start (GTK_BOX (vbox2), tableR, TRUE, TRUE, 0);
  gtk_widget_show (tableR);

  RWRawRData = gtk_text_new (NULL, NULL);
  gtk_text_set_editable (GTK_TEXT ( RWRawRData), FALSE);
  gtk_table_attach (GTK_TABLE (tableR), RWRawRData, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show ( RWRawRData);
  vscrollbarR = gtk_vscrollbar_new (GTK_TEXT (RWRawRData)->vadj);
  gtk_table_attach (GTK_TABLE (tableR), vscrollbarR, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (vscrollbarR);

  label = gtk_label_new ("Read Data");
  gtk_widget_set_usize( label, 80, 15);
  gtk_box_pack_start (GTK_BOX (vbox2), label, FALSE, FALSE, 0);
  
  gtk_container_add (GTK_CONTAINER (hbox), vbox1);
  gtk_container_add (GTK_CONTAINER (hbox), vbox2);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_widget_show_all ( hbox);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), vbox);
  gtk_widget_show_all ( vbox);

  button = gtk_button_new_with_label ("Execute");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (YesFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (NoFunc),
		      dialog_window);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  gtk_widget_show (dialog_window);
  gtk_grab_add (dialog_window);
}

void DoRWRaw (GtkWidget *widget, gpointer *data)
{
  unsigned short usBoard;
  RAWDATA        tDataBuffer;
  long           lOffset;          // DPM offset
  long           lSize;            // Data size
  short          sRet;            
  gchar          *str, *chWriteRaw;
  char           buff[96];

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
     gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
      goto leave;
  }
  else {
    str = gtk_entry_get_text (GTK_ENTRY ( RWRawOff));
    sscanf(str, "%ld", &lOffset);
    str = gtk_entry_get_text (GTK_ENTRY ( RWRawLen));
    sscanf(str, "%ld", &lSize);
    if ( (lSize + lOffset) > sizeof(RAWDATA) ) {
      if( usRWRfun == PARAMETER_READ) {
	gtk_text_backward_delete ( (GtkText *) RWRawRData, gtk_text_get_length( (GtkText *) RWRawRData));
	gtk_text_insert ( (GtkText *) RWRawRData, NULL, NULL, NULL,
			  "\n  Error: size+offset beyond the range!", -1);
      }
      else if( usRWRfun == PARAMETER_WRITE) {
	gtk_text_backward_delete ( (GtkText *) RWRawWData, gtk_text_get_length( (GtkText *) RWRawWData));
	gtk_text_insert ( (GtkText *) RWRawWData, NULL, NULL, NULL,
			  "\n  Error: size+offset beyond the range!", -1);
      }
      g_snprintf(buff, 92, "DevReadWriteRaw(). Error: size+offset beyond the range ->%d", sizeof(RAWDATA));
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
    }else {
      switch ( usRWRfun) {      
      case PARAMETER_READ:  // Read data   
	memset( &tDataBuffer, 0, sizeof(tDataBuffer));
	if ( (sRet = DevReadWriteDPMRaw( usBoard,
					 PARAMETER_READ,
					 (unsigned short)lOffset,
					 (unsigned short)lSize,
					 &tDataBuffer.abRawData[0])) != DRV_NO_ERROR) {
	  g_snprintf(buff, 92, "Error by driver function DevReadWriteRaw(). (Error: %d)", sRet);
	  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
	  goto leave;
	} else {
	  HexToTextBox( (GtkText *) RWRawRData,
			lSize, 
			&tDataBuffer.abRawData[0]);
	  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	  gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " Done");
	}
	break;
      case PARAMETER_WRITE: // Write data
	// Convert data to buffer, Read edit box
	chWriteRaw = gtk_editable_get_chars( GTK_EDITABLE( RWRawWData), (gint) 0, (gint) -1);
	if ( StringToHex( chWriteRaw,
			  sizeof(tDataBuffer),       
			  &tDataBuffer.abRawData[0], 
			  (unsigned short*)&lSize,   
			  (GtkStatusbar *)status_bar_DrvFun,
			  context_id_DrvFun) ) {
	  if ( (sRet = DevReadWriteDPMRaw( usBoard,
					   PARAMETER_WRITE,
					   (unsigned short)lOffset,
					   (unsigned short)lSize,
					   &tDataBuffer.abRawData[0])) != DRV_NO_ERROR) {
	    g_snprintf(buff, 72, "Error by driver function DevGetTaskParam(). (Error: %d)", sRet);
	    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	    gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, buff);
	    goto leave;
	  }
	  else {
	    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun );
	    gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvFun), context_id_DrvFun, " Done");
	  }
	}
	break;
      default:
	break;
      } /* end switch */
    }
  }
 leave: ;
}

void RWRaw (GtkWidget *widget, gpointer *data)
{
  gtk_statusbar_pop (GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun );
  gtk_statusbar_push(GTK_STATUSBAR(status_bar_DrvFun),context_id_DrvFun," invoking DevGetTaskParam() function ...");
  DlgRWRaw ("", DoRWRaw, DlgQuit);
}

GtkWidget *create_bbox1( gint  horizontal,
			 char *title,
			 gint  spacing,
			 gint  child_w,
			 gint  child_h,
			 gint  layout,
			 gint  group )
{
  GtkWidget *bbox, *button;

  if (horizontal)
    bbox = gtk_hbutton_box_new ();
  else
    bbox = gtk_vbutton_box_new ();

  gtk_container_set_border_width (GTK_CONTAINER (bbox), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), layout);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (bbox), spacing);
  gtk_button_box_set_child_size (GTK_BUTTON_BOX (bbox), child_w, child_h);
  if( group == 1) {
    button = gtk_button_new_with_label ("DevReset()");
    gtk_widget_set_usize( GTK_WIDGET( button), 140,12); // all other buttons in the box will be adjusted
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(ResetDev),NULL);
    button = gtk_button_new_with_label ("SetHostState()");
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(SetHostState),NULL);
    button = gtk_button_new_with_label ("GetTaskState()");
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(GetTaskState),NULL);
    button = gtk_button_new_with_label ("GetMBXState()");
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(GetMBXState),NULL);
  }
  else if( group == 2) {
    button = gtk_button_new_with_label ("GetMBXData()");
    gtk_widget_set_usize( GTK_WIDGET( button), 140,12);
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(GetMBXData),NULL);
    button = gtk_button_new_with_label ("ReadSendData()");
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(ReadSendData),NULL);
    button = gtk_button_new_with_label ("ReadWriteRaw()");
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(RWRaw),NULL);
    button = gtk_button_new_with_label ("DevTriggerWatchdog()");
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(DevTriggerWatchdog),NULL);
  }
  else if( group == 3) {
    button = gtk_button_new_with_label ("PutTaskParam()");
    gtk_widget_set_usize( GTK_WIDGET( button), 140,12);
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(PutTaskParam),NULL);
    button = gtk_button_new_with_label ("GetTaskParam()");
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(GetTaskParam),NULL);
    button = gtk_button_new_with_label ("SpecialControl()");
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(SetSpecialControl),NULL);
    button = gtk_button_new_with_label ("Download()");
    gtk_container_add (GTK_CONTAINER (bbox), button);
    gtk_signal_connect (GTK_OBJECT (button), "released", 
			GTK_SIGNAL_FUNC(Download),NULL);
  }
  return(bbox);
}

void PopulateDrvFun ()
{
  GtkWidget *hbox, *main_vbox, *separator;

  main_vbox = gtk_vbox_new (FALSE, 20);
  gtk_container_add (GTK_CONTAINER ( DrvFunPage), main_vbox);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (main_vbox), hbox, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), 
		      create_bbox1 (FALSE, "Select Function", 20, 150, 20, GTK_BUTTONBOX_START, 1),
		      FALSE, FALSE, 4);
  gtk_box_pack_start (GTK_BOX (hbox), 
		      create_bbox1 (FALSE, "Select Function", 20, 150, 20, GTK_BUTTONBOX_START, 2),
		      FALSE, FALSE, 4);
  gtk_box_pack_start (GTK_BOX (hbox), 
		      create_bbox1 (FALSE, "Select Function", 20, 150, 20, GTK_BUTTONBOX_START, 3),
		      FALSE, FALSE, 4);
  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (main_vbox), separator, FALSE, FALSE, 0);

  status_bar_DrvFun = gtk_statusbar_new ();      
  gtk_box_pack_start (GTK_BOX (main_vbox), status_bar_DrvFun, TRUE, TRUE, 0);
  context_id_DrvFun = gtk_statusbar_get_context_id( GTK_STATUSBAR(status_bar_DrvFun), "DrvFun");
  gtk_widget_show_all (DrvFunPage);
}
