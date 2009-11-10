#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/cif_types.h"      
#include "../usr-inc/cif_user.h"    /* Include file for device driver API */

BOOL fQuitting = FALSE;      // needed in PageSwitch()

char *GetFilename            ( char *sTitle);
void SelectMenu              ( GtkWidget *widget, gpointer data);
void DeSelectMenu            ( GtkWidget *widget, gpointer data);
void CreateToolbar           ( GtkWidget *vbox_main);
void SetMenuButton           ( char *szButton, int nState) ;
void CreateNotebook          ( GtkWidget *window);
void SetToolbarButton        ( char *szButton, int nState);

static void CreateMainWindow ( void);
GtkWidget  *CreateCombobox   ( void);
GtkWidget  *CreateMenuItem   ( GtkWidget *menu, 
			       char *szName, 
			       char *szAccel,
			       char *szTip, 
			       GtkSignalFunc func,
			       gpointer data);
GtkWidget *CreateSubMenu     ( GtkWidget *menubar, char *szName);
GtkWidget *CreateBarSubMenu  ( GtkWidget *menu, char *szName);

GtkWidget           *menuRed, *menuBlue, *menuGreen, *menuColor, *menuBold, *menuItalics, *menuUnderline;
GtkWidget           *toolbar, *tool_bold, *tool_italics, *tool_underline, *toolbar;
GtkAccelGroup       *accelerator_table;
GtkWidget           *win_main;
GtkTooltips         *tooltips;

extern void Popup         (char *szMessage);
extern void DlgQuit       (GtkWidget *, gpointer);
extern void ClosingDialog (GtkWidget *, gpointer);

extern char    pabDrvVersion[17];
extern gint    pExIORcvTimer, pExIOSndTimer;
extern BOOL    fExIORcvTimer, fExIOSndTimer;
extern gint    pMsgTimer;
extern BOOL    fMsgTimer;
extern unsigned short  usBoardAct;

gint EndProgram ()
{
  short  sRet;    
  char   buff[96];

  if( usBoardAct != 0xFFFF) {
    if ( (sRet = DevExitBoard(usBoardAct)) != 0) {//DRV_NO_ERROR) {
      g_snprintf(buff, 56, " Error in DevExitBoard(). (Error: %d)", sRet);
      Popup( buff);
    }
    if ( (sRet = DevCloseDriver()) != 0) {//DRV_NO_ERROR) {
      g_snprintf(buff, 60, " Error in DevCloseDriver(). (Error: %d)", sRet);
      Popup( buff);
    }
  }
  if ( fExIORcvTimer) {
    gtk_timeout_remove ( pExIORcvTimer);
    fExIORcvTimer = FALSE;
  }
  if ( fExIOSndTimer) {
    gtk_timeout_remove ( pExIOSndTimer);
    fExIORcvTimer = FALSE;
  }
  if ( fMsgTimer) {
    gtk_timeout_remove ( pMsgTimer);
    fMsgTimer = FALSE;
  }
  fQuitting = TRUE; // because of next line, prevent PageSwitch do nonsense
  gtk_main_quit (); // implies recursive PageSwitch()'es
  
  return (FALSE);
}

int main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);

    tooltips = gtk_tooltips_new();

    CreateMainWindow ();

    gtk_main();

    return 0;
}

void QuitFunc (GtkWidget *widget, gpointer data)
{
  EndProgram();
}

void DlgAbout (char *szMessage)//, void (*YesFunc)(), void (*NoFunc)())
{
    GtkWidget *label;
    GtkWidget *button, *hbox;
    GtkWidget *dialog_window;

    dialog_window = gtk_dialog_new ();

    gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
	                GTK_SIGNAL_FUNC (ClosingDialog),
	                &dialog_window);
    gtk_window_set_title (GTK_WINDOW (dialog_window), "About");

    gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);

    hbox = gtk_hbox_new (FALSE, 0);
    label = gtk_label_new ("CIF Device Driver Test Program ");

    gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
    gtk_widget_set_usize( label, 180, 18);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
    gtk_widget_show_all ( hbox);
    hbox = gtk_hbox_new (FALSE, 0);
    label = gtk_label_new ( "");
    //g_print( pabDrvVersion);
    gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
    gtk_widget_set_usize( label, 200, 18);
    gtk_label_set_text( (GtkLabel *)label, pabDrvVersion);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
    gtk_widget_show_all ( hbox);

    hbox = gtk_hbox_new (FALSE, 0);
    label = gtk_label_new ("Hilscher GmbH, Copyright 2000");

    gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
    gtk_widget_set_usize( label, 180, 18);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 4);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog_window)->vbox), hbox);
    gtk_widget_show_all ( hbox);

    button = gtk_button_new_with_label ("Close");
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
	                GTK_SIGNAL_FUNC (DlgQuit),
	                dialog_window);
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);

    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
			  button, TRUE, TRUE, 0);
    gtk_widget_grab_default (button);
    gtk_widget_show (button);
    gtk_widget_show (dialog_window);
}

static void CreateMainWindow ()
{
    GtkWidget *vbox_main;
    GtkWidget *menubar;
    GtkWidget *menu;
    GtkWidget *menuitem;

    win_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(win_main, 600, 340);
    gtk_window_set_title (GTK_WINDOW (win_main), "CIF device Driver Setup and Test Program");
    gtk_container_border_width (GTK_CONTAINER (win_main), 0);

    accelerator_table = gtk_accel_group_new ();

    gtk_signal_connect (GTK_OBJECT (win_main), "delete_event",
              GTK_SIGNAL_FUNC (EndProgram), NULL);

    vbox_main = gtk_vbox_new (FALSE, 0);

    gtk_container_add (GTK_CONTAINER (win_main), vbox_main);
    gtk_widget_show (vbox_main);
    gtk_widget_show (win_main);
  
    menubar = gtk_menu_bar_new ();
    gtk_box_pack_start (GTK_BOX (vbox_main), menubar, FALSE, TRUE, 0);
    gtk_widget_show (menubar);

    menu = CreateBarSubMenu (menubar, "File");
    menuitem = CreateMenuItem (menu, NULL, NULL, NULL, NULL, NULL);
    menuitem = CreateMenuItem (menu, "Quit", "", "Quit CIF Driver Setup and Test Program", 
                     GTK_SIGNAL_FUNC (QuitFunc), "quit");
    menuitem = CreateMenuItem (menu, "About", "", "", 
                     GTK_SIGNAL_FUNC (DlgAbout), "About");
    CreateNotebook (vbox_main);
}

void ButtonClicked (GtkWidget *widget, gpointer data)
{
    int nState = GTK_TOGGLE_BUTTON (widget)->active;

    SetMenuButton ((char *) data, nState);
}

void SelectMenu (GtkWidget *widget, gpointer data)
{
    GtkToggleButton *button = NULL;
    char *szButton;

    int nState = GTK_CHECK_MENU_ITEM (widget)->active;
 
    szButton = (char *) data;
    //printf ("select menu %s - %d\n", szButton, nState);

    if (!strcmp (szButton, "bold")) {
        button = GTK_TOGGLE_BUTTON (tool_bold);
    } else if (!strcmp (szButton, "italics")) {
        button = GTK_TOGGLE_BUTTON (tool_italics);
    } else if (!strcmp (szButton, "underline")) {
        button = GTK_TOGGLE_BUTTON (tool_underline);
    } 
    if (button) {
        gtk_toggle_button_set_state (button, nState);
    }
}

void SetMenuButton (char *szButton, int nState) 
{
    GtkCheckMenuItem *check = NULL;

    //printf ("check_menu_set_state - %d\n", nState);

    if (!strcmp (szButton, "bold")) {
        check = GTK_CHECK_MENU_ITEM(menuBold);
    } else if (!strcmp (szButton, "italics")) {
        check = GTK_CHECK_MENU_ITEM(menuItalics);
    } else if (!strcmp (szButton, "underline")) {
        check = GTK_CHECK_MENU_ITEM(menuUnderline);
    }
    if (check) {
        gtk_check_menu_item_set_state (check, nState);
    }
}

