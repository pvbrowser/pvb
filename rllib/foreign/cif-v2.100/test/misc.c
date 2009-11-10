/*
 * Sample GUI application front end.
 *
 * Auth: Eric Harlow
 *
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

extern GtkWidget           *win_main;
extern GtkAccelGroup       *accelerator_table;
extern GtkTooltips         *tooltips;

/*
 * CreateWidgetFromXpm
 *
 * Using the window information and the string with the icon color/data, 
 * create a widget that represents the data.  Once done, this widget can
 * be added to buttons or other container widgets.
 */
GtkWidget *CreateWidgetFromXpm (GtkWidget *window, gchar **xpm_data)
{
    GdkBitmap *mask;
    GdkPixmap *pixmap_data;
    GtkWidget *pixmap_widget;

    pixmap_data = gdk_pixmap_create_from_xpm_d (
                                 win_main->window, 
                                 &mask,
                                 NULL,
                                 (gchar **) xpm_data);

    pixmap_widget = gtk_pixmap_new (pixmap_data, mask);
    gtk_widget_show (pixmap_widget);

    return (pixmap_widget);
}



/*
 * CreateMenuItem
 *
 * Creates an item and puts it in the menu and returns the item.
 *
 * menu - container menu
 * szName - Name of the menu - NULL for a separator
 * szAccel - Acceleration string - "^C" for Control-C
 * szTip - Tool tips
 * func - Call back function
 * data - call back function data
 *
 * returns new menuitem
 */
GtkWidget *CreateMenuItem (GtkWidget *menu, 
                           char *szName, 
                           char *szAccel,
                           char *szTip, 
                           GtkSignalFunc func,
                           gpointer data)
{
    GtkWidget *menuitem;

    /* --- If there's a name, create the item and put a
     * --- Signal handler on it.
     */
    if (szName && strlen (szName)) {
        menuitem = gtk_menu_item_new_with_label (szName);
        gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
                    GTK_SIGNAL_FUNC(func), data);
    } else {
        /* --- Create a separator --- */
        menuitem = gtk_menu_item_new ();
    }

    /* --- Add menu item to the menu and show it. --- */
    gtk_menu_append (GTK_MENU (menu), menuitem);
    gtk_widget_show (menuitem);

    /* --- If there was an accelerator --- */
    if (szAccel && szAccel[0] == '^') {
        gtk_widget_add_accelerator (menuitem, 
                                        "activate", 
                                        accelerator_table,
                                        szAccel[1], 
                                        GDK_CONTROL_MASK,
                                        GTK_ACCEL_VISIBLE);
    }

    /* --- If there was a tool tip --- */
    if (szTip && strlen (szTip)) {
        gtk_tooltips_set_tip (tooltips, menuitem, szTip, NULL);
    }

    return (menuitem);
}


/*
 * CreateMenuCheck
 *
 * Create a menu checkbox
 *
 * menu - container menu
 * szName - name of the menu
 * func - Call back function.
 * data - call back function data
 *
 * returns new menuitem
 */
GtkWidget *CreateMenuCheck (GtkWidget *menu, 
                            char *szName, 
                            GtkSignalFunc func, 
                            gpointer data)
{
    GtkWidget *menuitem;

    /* --- Create menu item --- */
    menuitem = gtk_check_menu_item_new_with_label (szName);

    /* --- Add it to the menu --- */
    gtk_menu_append (GTK_MENU (menu), menuitem);
    gtk_widget_show (menuitem);

    /* --- Listen for "toggled" messages --- */
    gtk_signal_connect (GTK_OBJECT (menuitem), "toggled",
                        GTK_SIGNAL_FUNC(func), data);

    return (menuitem);
}



/*
 * CreateMenuRadio
 *
 * Create a menu radio
 *
 * menu - container menu
 * szName - name of the menu
 * func - Call back function.
 * data - call back function data
 *
 * returns new menuitem
 */ 
GtkWidget *CreateMenuRadio (GtkWidget *menu, 
                            char *szName, 
                            GSList **group,
                            GtkSignalFunc func, 
                            gpointer data)
{
    GtkWidget *menuitem;

    /* --- Create menu item --- */
    menuitem = gtk_radio_menu_item_new_with_label (*group, szName);
    *group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (menuitem));

    /* --- Add it to the menu --- */
    gtk_menu_append (GTK_MENU (menu), menuitem);
    gtk_widget_show (menuitem);

    /* --- Listen for "toggled" messages --- */
    gtk_signal_connect (GTK_OBJECT (menuitem), "toggled",
                        GTK_SIGNAL_FUNC(func), data);

    return (menuitem);
}


/*
 * CreateSubMenu
 *
 * Create a submenu off the menubar.
 *
 * menubar - obviously, the menu bar.
 * szName - Label given to the new submenu
 *
 * returns new menu widget
 */
GtkWidget *CreateSubMenu (GtkWidget *menubar, char *szName)
{
    GtkWidget *menuitem;
    GtkWidget *menu;
 
    /* --- Create menu --- */
    menuitem = gtk_menu_item_new_with_label (szName);

    /* --- Add it to the menubar --- */
    gtk_widget_show (menuitem);
    gtk_menu_append (GTK_MENU (menubar), menuitem);

    /* --- Get a menu and attach to the menuitem --- */
    menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menu);

    /* --- Viola! --- */
    return (menu);
}


/*
 * CreateBarSubMenu
 *
 * Create a submenu within an existing submenu.  (In other words, it's not
 * the menubar.)
 *
 * menu - existing submenu
 * szName - label to be given to the new submenu off of this submenu
 *
 * returns new menu widget 
 */ 
GtkWidget *CreateBarSubMenu (GtkWidget *menu, char *szName)
{
    GtkWidget *menuitem;
    GtkWidget *submenu;
 
    /* --- Create menu --- */
    menuitem = gtk_menu_item_new_with_label (szName);

    /* --- Add it to the menubar --- */
    gtk_menu_bar_append (GTK_MENU_BAR (menu), menuitem);
    gtk_widget_show (menuitem);

    /* --- Get a menu and attach to the menuitem --- */
    submenu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);

    /* --- Viola! --- */
    return (submenu);
}
