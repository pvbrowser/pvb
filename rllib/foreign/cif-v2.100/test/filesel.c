/*
 * File selection dialog
 */
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

static   GtkWidget     *filew;
static   char        *sFilename = NULL; 
    
void file_ok_sel (GtkWidget *w, GtkFileSelection *fs)
{
    char *sTempFile;

    /* --- Get the name --- */
    sTempFile = gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs));

    /* --- Allocate space and save it.--- */
    sFilename = malloc (sizeof (char) * (strlen (sTempFile) + 1));
    strcpy (sFilename, sTempFile);

    /* --- Destroy the file selection --- */
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



