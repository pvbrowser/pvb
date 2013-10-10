/***************************************************************************
                          qtdatabase.cpp  -  description
                             -------------------
    begin                : Fri May 28 2010
    http://pvbrowser.org
 ***************************************************************************/

#include "qtdatabase.h"

qtDatabase::qtDatabase()
{
  sprintf(connectionName,"%p",this); 
  db = NULL;
  result = new QSqlQuery();
  error  = new QSqlError();
}

qtDatabase::~qtDatabase()
{
  delete result;
  delete error;
  if(db != NULL)
  {
    close();
  }
#if QT_VERSION < 0x050000
  if(QSqlDatabase::contains(QString::fromAscii(connectionName))) 
  {
    QSqlDatabase::removeDatabase(QString::fromAscii(connectionName));
  }
#else
  if(QSqlDatabase::contains(QString::fromLatin1(connectionName))) 
  {
    QSqlDatabase::removeDatabase(QString::fromLatin1(connectionName));
  }
#endif  
}

int qtDatabase::open(const char *dbtype, const char *hostname, const char *dbname, const char *user, const char *pass)
{
  if(db != NULL) return -1;
  db = new QSqlDatabase;

#if QT_VERSION < 0x050000
  *db = QSqlDatabase::addDatabase(dbtype, QString::fromAscii(connectionName));
#else
  *db = QSqlDatabase::addDatabase(dbtype, QString::fromLatin1(connectionName));
#endif
  db->setHostName(hostname);
  db->setDatabaseName(dbname);
  db->setUserName(user);
  db->setPassword(pass);
  if(db->open())
  {
    return 0;
  }
  else
  {
    delete db;
    db = NULL;
    return -1;
  }
}

int qtDatabase::close()
{
  if(db == NULL) 
  {
    return -1;
  }  
  db->close();
  delete db;
  db = NULL;
  return 0;
}

int qtDatabase::query(PARAM *p, const char *sqlcommand)
{
  if(db == NULL) return -1;
  QString qsqlcommand = QString::fromUtf8(sqlcommand);
  *result = db->exec(qsqlcommand);
  *error = db->lastError();
  if(error->isValid())
  {
    QString e = error->databaseText();
    printf("qtDatabase::query ERROR: %s\n", (const char *) e.toUtf8());
    pvStatusMessage(p,255,0,0,"ERROR: qtDatabase::query(%s) %s", sqlcommand, (const char *) e.toUtf8());
    return -1;
  }
  return 0;
}

int qtDatabase::populateTable(PARAM *p, int id)
{
  int x,y,xmax,ymax;
  
  if(db == NULL)
  {
    pvStatusMessage(p,255,0,0,"ERROR: qtDatabase::populateTable() db==NULL");
    return -1;
  }  

  // set table dimension
  xmax = result->record().count();
  //ymax = result->size();
  ymax = result->numRowsAffected();
  pvSetNumRows(p,id,ymax);
  pvSetNumCols(p,id,xmax);

  // populate table
  QSqlRecord record = result->record();
  if(record.isEmpty())
  {
    pvStatusMessage(p,255,0,0,"ERROR: qtDatabase::populateTable() record is empty");
    return -1;
  }

  for(x=0; x<xmax; x++)
  { // write name of fields
    pvSetTableText(p, id, x, -1, (const char *) record.fieldName(x).toUtf8());
  }
  result->next();
  for(y=0; y<ymax; y++)
  { // write fields
    QSqlRecord record = result->record();
    for(x=0; x<xmax; x++)
    {
      QSqlField f = record.field(x);
      if(f.isValid())
      {
        QVariant v = f.value();
        pvSetTableText(p, id, x, y, (const char *) v.toString().toUtf8());
      }
      else
      {
        pvSetTableText(p, id, x, y, "ERROR:");
      }
    }
    result->next();
  }

  return 0;
}

const char *qtDatabase::recordFieldValue(PARAM *p, int x)
{
  QSqlRecord record = result->record();
  if(record.isEmpty())
  {
    pvStatusMessage(p,255,0,0,"ERROR: qtDatabase::recordFieldValue(%d) record is empty", x);
    return "ERROR:";
  }
  QSqlField f = record.field(x);
  if(f.isValid())
  {
    QVariant v = f.value();
    return v.toString().toUtf8();
  }
  else
  {
    pvStatusMessage(p,255,0,0,"ERROR: qtDatabase::recordFieldValue(%d) field is invalid", x);
    return "ERROR:";
  }
}

const char *qtDatabase::dbQuery(const char *sqlcommand)
{
  if(db == NULL) return "ERROR: database is NULL";
  QString qsqlcommand = QString::fromUtf8(sqlcommand);
  *result = db->exec(qsqlcommand);
  *error = db->lastError();
  if(error->isValid())
  {
    QString e = error->databaseText();
    printf("qtDatabase::query ERROR: %s\n", (const char *) e.toUtf8());
    return "ERROR: qtDatabase::query()";
  }
  return "ERROR: result is not valid";
}

const char *qtDatabase::dbRecordFieldValue(int x)
{
  QSqlRecord record = result->record();
  if(record.isEmpty())
  {
    return "ERROR: qtDatabase::recordFieldValue record is empty";
  }
  QSqlField f = record.field(x);
  if(f.isValid())
  {
    QVariant v = f.value();
    return v.toString().toUtf8();
  }
  else
  {
    return "ERROR: qtDatabase::recordFieldValue field is invalid";
  }
}

int qtDatabase::nextRecord()
{
  bool res;             // may 2013 evaluate res due to report by user mhe_fr from our forum
  res = result->next(); // may 2013 evaluate res  due to report by user mhe_fr from our forum
  if(!res) return -1;   // may 2013 evaluate res due to report by user mhe_fr from our forum
  QSqlRecord record = result->record();
  if(record.isEmpty())
  if(record.isEmpty()) return -1;
  return 0;
}
