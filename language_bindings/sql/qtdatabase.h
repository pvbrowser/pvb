/***************************************************************************
                          qtdatabase.h  -  description
                             -------------------
    begin                : Fri May 28 2010
    http://pvbrowser.org
    TODO: customize / extend this class for your own purposes

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <qstring.h>
#include "processviewserver.h"

/**
The currently available driver types are:
dbtype :=  Description
"QDB2"     IBM DB2, v7.1 and higher
"QIBASE"   Borland InterBase Driver
"QMYSQL"   MySQL Driver
"QOCI"     Oracle Call Interface Driver
"QODBC"    ODBC Driver (includes Microsoft SQL Server)
"QPSQL"    PostgreSQL v6.x and v7.x Driver
"QSQLITE"  SQLite version 3 or above
"QSQLITE2" SQLite version 2
"QTDS"     Sybase Adaptive Server
*/

class qtDatabase
{
  public:
    qtDatabase();
    ~qtDatabase();
    int open(const char *dbtype, const char *hostname, const char *dbname, const char *user, const char *pass);
    int close();
    int query(PARAM *p, const char *sqlcommand);
    int populateTable(PARAM *p, int id);
    const char *recordFieldValue(PARAM *p, int x);
    int nextRecord();

    QSqlDatabase *db;
    QSqlQuery    *result;
    QSqlError    *error;
};

