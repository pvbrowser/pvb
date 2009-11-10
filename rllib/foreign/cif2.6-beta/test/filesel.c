/* <St> *******************************************************************

   filesel.c 
  
  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver setup und test program
  =========================================================================

  DISCRIPTION
    File dialog               .
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

static   GtkWidget     *filew;
static   char        *sFilename = NULL; 
    
void file_ok_sel (GtkWidget *w, GtkFileSelection *fs)
{
    char *sTempFile;

    sTempFile = gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs));

    sFilename = malloc (sizeof (char) * (strlen (sTempFile) + 1));
    strcpy (sFilename, sTempFile);

    gtk_widget_destroy (filew);
}

int CloseDialog (GtkWidget *widget, gpointer *data)
{
    return (FALSE);
}

int DestroyDialog (GtkWidget *widget, gpointer *data)
{
    gtk_grab_remove (widget);
    gtk_main_quit ();
    return (FALSE);
}

char *GetFilename (char *sTitle)
{ 
    if (sFilename) {
        free (sFilename);
        sFilename = NULL;
    }

    filew = gtk_file_selection_new (sTitle);

    gtk_signal_connect (GTK_OBJECT (filew), "delete_event",
            (GtkSignalFunc) CloseDialog, &filew);

    gtk_signal_connect (GTK_OBJECT (filew), "destroy",
            (GtkSignalFunc) DestroyDialog, &filew);

    gtk_signal_connect (
            GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
            "clicked", (GtkSignalFunc) file_ok_sel, filew );
    
    gtk_signal_connect_object (
             GTK_OBJECT (GTK_FILE_SELECTION (filew)->cancel_button),
             "clicked", (GtkSignalFunc) gtk_widget_destroy,
             GTK_OBJECT (filew));
    
    gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew), "");
    
    gtk_widget_show (filew);

    gtk_grab_add (filew);

    gtk_main ();

    return (sFilename);
}



