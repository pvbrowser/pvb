/* <St> *******************************************************************

   misc.c 
  
  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver setup und test program
  =========================================================================

  DISCRIPTION
    menu generation functions               .
  =========================================================================

  CHANGES
  version name      date        Discription                 
   		 Juli  2004     Copyright changed to GNU Lesser GPL		                
-------------------------------------------------------------------------
  V2.600

  ======================== Copyright =====================================

  Copyright (C) 2004  Hilscher GmbH

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
   
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

  ========================================================================
******************************************************************** <En> */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern GtkWidget           *win_main;
extern GtkAccelGroup       *accelerator_table;
extern GtkTooltips         *tooltips;

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

GtkWidget *CreateMenuItem (GtkWidget *menu, 
                           char *szName, 
                           char *szAccel,
                           char *szTip, 
                           GtkSignalFunc func,
                           gpointer data)
{
    GtkWidget *menuitem;
    if (szName && strlen (szName)) {
        menuitem = gtk_menu_item_new_with_label (szName);
        gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
                    GTK_SIGNAL_FUNC(func), data);
    } else {
        menuitem = gtk_menu_item_new ();
    }

    gtk_menu_append (GTK_MENU (menu), menuitem);
    gtk_widget_show (menuitem);

    if (szAccel && szAccel[0] == '^') {
        gtk_widget_add_accelerator (menuitem, 
                                        "activate", 
                                        accelerator_table,
                                        szAccel[1], 
                                        GDK_CONTROL_MASK,
                                        GTK_ACCEL_VISIBLE);
    }

    if (szTip && strlen (szTip)) {
        gtk_tooltips_set_tip (tooltips, menuitem, szTip, NULL);
    }

    return (menuitem);
}

GtkWidget *CreateMenuCheck (GtkWidget *menu, 
                            char *szName, 
                            GtkSignalFunc func, 
                            gpointer data)
{
    GtkWidget *menuitem;

    menuitem = gtk_check_menu_item_new_with_label (szName);

    gtk_menu_append (GTK_MENU (menu), menuitem);
    gtk_widget_show (menuitem);

    gtk_signal_connect (GTK_OBJECT (menuitem), "toggled",
                        GTK_SIGNAL_FUNC(func), data);

    return (menuitem);
}

GtkWidget *CreateMenuRadio (GtkWidget *menu, 
                            char *szName, 
                            GSList **group,
                            GtkSignalFunc func, 
                            gpointer data)
{
    GtkWidget *menuitem;

    menuitem = gtk_radio_menu_item_new_with_label (*group, szName);
    *group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (menuitem));

    gtk_menu_append (GTK_MENU (menu), menuitem);
    gtk_widget_show (menuitem);

    gtk_signal_connect (GTK_OBJECT (menuitem), "toggled",
                        GTK_SIGNAL_FUNC(func), data);

    return (menuitem);
}

GtkWidget *CreateSubMenu (GtkWidget *menubar, char *szName)
{
    GtkWidget *menuitem;
    GtkWidget *menu;
 
    menuitem = gtk_menu_item_new_with_label (szName);

    gtk_widget_show (menuitem);
    gtk_menu_append (GTK_MENU (menubar), menuitem);

    menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menu);

    return (menu);
}

GtkWidget *CreateBarSubMenu (GtkWidget *menu, char *szName)
{
    GtkWidget *menuitem;
    GtkWidget *submenu;
 
    menuitem = gtk_menu_item_new_with_label (szName);

    gtk_menu_bar_append (GTK_MENU_BAR (menu), menuitem);
    gtk_widget_show (menuitem);

    submenu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);

    return (submenu);
}
