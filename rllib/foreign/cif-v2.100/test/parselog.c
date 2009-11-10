/*
 * File: parselog.c
 * Auth: Eric Harlow
 *
 * Parse a apache log file and summarize information
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "logtypes.h"

void Init ();
void TrackUsers (typHit *hit);
void TrackDates (typHit *hit);
void TrackFiles (typHit *hit);
void StartProgress ();
void UpdateProgress (long pos, long len);
void EndProgress ();
void UpdateStats (typHit *hit);


/*
 * Define data for the storage of hourly information
 */
long timeOfDayHits[24];
long timeOfDaySize[24];

/*
 * Define trees to store the different types of data.
 */
GTree *fileTree = NULL;
GTree *dateTree = NULL;
GTree *userTree = NULL;

void ParseLine (char *buffer);

#define MONTHS 12
char *sValidMonths[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


/*
 * Date2Str
 *
 * Convert the date to a string.
 * date - date to convert
 * buffer - char* big enough to hold the date/time
 */
void Date2Str (typTimeStamp *date, char *buffer)
{
    sprintf (buffer, "%d/%d/%d %d:%d:%d ", 
                            date->month,
                            date->day,
                            date->year,
                            date->hours,
                            date->minutes,
                            date->seconds);
}


/*
 * ConvertDate
 * 
 * Convert the date from a format of 30/Feb/1999:11:42:23 
 * which is how the date was stored in my apache log files.
 */
void ConvertDate (char *sDate, typTimeStamp *date)
{
    char sMonth[33];
    int i;

    if (sDate) {

        /* --- Break down the date into its components --- */
        sscanf (sDate, "%d/%3c/%d:%d:%d:%d", &date->day,
                                        sMonth,
                                        &date->year,
                                        &date->hours,
                                        &date->minutes,
                                        &date->seconds);

        /* --- Convert the string date into a numeric --- */
        for (i = 0; i < MONTHS; i++) {

            /* --- Is this the month? --- */
            if (!strncasecmp (sValidMonths[i], sMonth, strlen (sValidMonths[i]))) {
                date->month = i + 1;
                break;
            }
        }
    } else {

        /* --- Bad date --- */
        date->day = 0;
        date->year = 0;
        date->month = 0;
        date->hours = 0;
        date->minutes = 0;
        date->seconds = 0;
    }
}


/*
 * ParseLog
 *
 * Parse the log file organize the data in the log file in to
 * a format that can be interpreted by a user.
 *
 * sFile - File to read.
 */
void ParseLog (char *sFile)
{
    FILE *fp;
    char buffer[2000];
    long nFileLen = 0;
    struct stat fileStatus;

    /* --- Alloc trees and init data --- */   
    Init ();

    /* --- Get filename --- */
    stat (sFile, &fileStatus);
    nFileLen = fileStatus.st_size;

    /* --- Open le file <--- that's french, you know. --- */
    fp = fopen (sFile, "r");

    /* --- Make sure we opened *something* --- */
    if (fp) {

        /* --- Show the progress bar --- */
        StartProgress ();

        /* --- While we have data --- */
        while (!feof (fp)) {

            /* --- Update the progress bar with information --- */
            UpdateProgress (ftell (fp), nFileLen);

            /* --- Read a line from the file --- */
            fgets (buffer, sizeof (buffer), fp);

            /* --- if not end of file --- */
            if (!feof (fp)) {

                /* --- Parse the data --- */
                ParseLine (buffer);
            }
        }

        /* --- Done - don't need progress bar --- */
        EndProgress ();

        /* --- Close the file --- */
        fclose (fp);
    }
}


/*
 * ParseLine
 *
 * Parse the line retrieved from the log file and break it
 * down into it's components and update the stats being
 * kept.
 */
void ParseLine (char *buffer)
{
    
    char *sUnknown;
    char *sNull;
    char *sDate;
    char output[88];
    typHit hit;
    char *sTmp;

    /* --- Get basic information --- */
    hit.sIp = strtok (buffer, " ");

    sUnknown = strtok (NULL, " ");
    hit.sUser = strtok (NULL, " ");

    /* --- Extract the date --- */
    sDate = strtok (NULL, "]");

    if (sDate) sDate++;

    /* --- Convert the date string --- */
    ConvertDate (sDate, &hit.date);

    /* --- Get file, size, status --- */
    sNull = strtok (NULL, "\"");
    hit.sCmd = strtok (NULL, "\"");
 
    sTmp = strtok (NULL, " ");
    if (sTmp) {
        hit.nResult = atoi (sTmp);
    } else {
        hit.nResult = 0;
    }

    sTmp = strtok (NULL, " ");
    if (sTmp) {
        hit.nSize = atoi (sTmp);
    } else {
        hit.nSize = 0;
    }

    /* --- Page was an error --- */
    if (hit.nResult == 404) return;

    /* --- Convert back to string --- */
    Date2Str (&hit.date, output);

    sUnknown = strtok (hit.sCmd, " ");
    hit.sURL = strtok (NULL, " ");

    /* --- Valid URL--- */
    if (hit.sURL) {

        /* --- Update the stats --- */
        UpdateStats (&hit);
    }
}



/*
 * UpdateStats
 *
 * Update information about the "hit"
 * Currently keep stats on:
 *      The time of day that the hit occurred on.
 *      The traffic occurring on each of the days.
 *      The traffic associated with users web sites.
 */
void UpdateStats (typHit *hit)
{
    /* -- Update hourly stats --- */
    timeOfDayHits[hit->date.hours]++;
    timeOfDaySize[hit->date.hours] += hit->nSize;

    /* --- Update this information --- */
    TrackFiles (hit);
    TrackDates (hit);
    TrackUsers (hit);
}



/*
 * TrackUsers
 *
 * Track traffic to the users sites.
 */
void TrackUsers (typHit *hit)
{
    typStat *stat;
    char *sUser;
    
    /* --- Bad data --- */
    if (hit->sURL == NULL || strlen (hit->sURL) == 0) return;

    /* --- Get first part of the path --- */
    sUser = strtok (&hit->sURL[1], "/");

    if (sUser == NULL) return;

    /* --- If it's a user site --- */
    if (sUser[0] == '~') {
    
        /* --- Get the name --- */
        sUser = &sUser[1];

    /* --- If ~ was encoded as %7E --- */
    } else if (!strcmp (sUser, "%7E")) {

        /* --- Get the name --- */
        sUser = &sUser[3];
    } else {

        /* --- Not a user, belongs to main site --- */
        sUser = "*ROOT";
    }

    /* --- See if the user exists in the tree --- */
    stat = g_tree_lookup (userTree, sUser);

    /* --- If user does not exist --- */
    if (stat == NULL) {

        /* --- Make space for the user --- */
        stat = g_malloc (sizeof (typStat));

        /* --- Populate fields --- */
        stat->sURL = strdup (sUser);
        stat->nHits = 0;
        stat->nSize = 0;

        /* --- Insert user into tree --- */
        g_tree_insert (userTree, stat->sURL, stat);
    }

    /* --- Update hit count for user --- */
    stat->nHits ++;
    stat->nSize += hit->nSize;
}


/*
 * TrackFiles
 *
 * Keep track of hit counts on individual files 
 */
void TrackFiles (typHit *hit)
{
    typStat *stat;

    /* --- Lookup the URL in the tree --- */
    stat = g_tree_lookup (fileTree, hit->sURL);

    /* --- If the URL is not found --- */
    if (stat == NULL) {

        /* --- Create a node for the URL --- */
        stat = g_malloc (sizeof (typStat));

        /* --- Populate the node --- */
        stat->sURL = strdup (hit->sURL);
        stat->nHits = 0;
        stat->nSize = 0;

        /* --- Add the node to the tree --- */
        g_tree_insert (fileTree, stat->sURL, stat);
    }

    /* --- Update the file hit count --- */
    stat->nHits ++;
    stat->nSize = hit->nSize;
}


/*
 * TrackDates
 *
 * Track the traffic that occurs on any given date.
 */
void TrackDates (typHit *hit)
{
    typDate date;
    typDate *newdate;
    typDateInfo *info;
 
    /* --- Get the date of the hit --- */
    date.year = hit->date.year;
    date.month = hit->date.month;
    date.day = hit->date.day;

    /* --- Look up the date --- */
    info = g_tree_lookup (dateTree, &date);

    /* --- If no data for the date? --- */
    if (info == NULL) {

        /* --- Create field to hold date information --- */
        info = g_malloc (sizeof (typDateInfo));
        newdate = g_malloc (sizeof (typDate));

        /* --- Populate fields --- */
        *newdate = date;

        info->nHits = 0;
        info->nSize = 0;
        info->date = newdate;

        /* --- Add date info to the tree --- */
        g_tree_insert (dateTree, newdate, info);
    }

    /* --- Update date hit count --- */
    info->nHits ++;
    info->nSize += hit->nSize;
}


/*
 * CompareStrings 
 * 
 * Function to compare two strings for a tree callback.
 * Could have just passed in strcmp into the tree function, 
 * but may want to change the comparision in the future.
 */
gint CompareStrings (gpointer g1, gpointer g2)
{
    return (strcmp ((char *) g1, (char *) g2));
}


/*
 * CompareDates
 *
 * Compare two dates and return negative value if first date is 
 * less than the second date, positive value if first date is 
 * greater than the second date and zero if the two dates are 
 * the same.
 */
gint CompareDates (gpointer g1, gpointer g2)
{
    typDate *d1 = (typDate *) g1;
    typDate *d2 = (typDate *) g2;

    /* --- Year has highest priority --- */
    if (d1->year == d2->year) {

        /* --- Years are same, what about months? --- */
        if (d1->month == d2->month) {

            /* --- return difference between days --- */
            return (d1->day - d2->day);
        } else {

            /* --- Months are different - calculate delta --- */
            return (d1->month - d2->month);
        }
    } else {

        /* --- Years are different, calc delta. --- */
        return (d1->year == d2->year);
    }
}


/*
 * GetHitsForHour
 * 
 * Function to return the the number of hits for a given 
 * time of day.
 */
void GetHitsForHour (int nHours, long *hits, long *size)
{
    /* --- If the clock is out of range --- */
    if (nHours < 0 || nHours > 23) {

        *hits = 0;
        *size = 0;
  
    } else {
        *hits = timeOfDayHits[nHours];
        *size = timeOfDaySize[nHours];
    }
}

/*
 * Init
 *
 * Initialize data so that the log file can be read in.
 * Create trees necessary to store the log data.
 */
void Init ()
{
    int i;

    /* --- Create the trees that will store the information --- */
    fileTree = g_tree_new (CompareStrings);
    dateTree = g_tree_new (CompareDates);
    userTree = g_tree_new (CompareStrings);

    /* --- Clear out the hits by hour --- */
    for (i = 0; i < 24; i++) {
        timeOfDayHits[i] = 0;
        timeOfDaySize[i] = 0;
    }
}


/*
 * FreeURLs
 *
 * Free the memory associated with the URLs.
 * This is a traverse callback
 */
gint FreeURLs (gpointer key, gpointer value, gpointer data)
{
    typStat *info;

    info = (typStat *) value;   
    free (info->sURL);
    g_free (info);
    return (0);
}


/*
 * FreeDates
 *
 * Free information allocated to keep track of the
 * traffic by date.  This is a traverse callback.
 */
gint FreeDates (gpointer key, gpointer value, gpointer data)
{
    typDateInfo *info;

    info = (typDateInfo *) value;   
    g_free (info->date);
    g_free (info);
    return (0);
}



/*
 * FreeResources
 *
 * Free the memory used in the tree nodes and then free the
 * trees. 
 */
void FreeResources ()
{
    /* --- Free data stored in the tree --- */
    g_tree_traverse (userTree, FreeURLs, G_IN_ORDER, NULL);
    g_tree_traverse (dateTree, FreeDates, G_IN_ORDER, NULL);
    g_tree_traverse (fileTree, FreeURLs, G_IN_ORDER, NULL);

    /* --- Free the trees --- */
    g_tree_destroy (userTree);
    g_tree_destroy (dateTree);
    g_tree_destroy (fileTree);

    /* --- Clear pointers --- */
    userTree = NULL;
    dateTree = NULL;
    fileTree = NULL;
}
