/***************************************************************************
                          rlcanopen.cpp  -  description
                             -------------------
    begin     : Tue March 03 2004
    copyright : (C) 2004 by Marc Br�tigam, Christian Wilmes, R. Lehrig
    email     : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlcutil.h"
#include "objdir.h"

//! empty constructor not yet implemented
ObjDir::ObjDir(){
  rlDebugPrintf("YOUR OBJDIR CTOR CODE!!!\n");
  objstorage.setAutoDelete(true);
}

//! destructor has nothing to do. QPtrVector Destructor performs memory cleanup
ObjDir::~ObjDir(){
  rlDebugPrintf("ObjDir destructor\n");
}

//! main constructor builds object directory.
ObjDir::ObjDir(rlIniFile* _ini){
  itemcount=0;
  ini= _ini;
  objstorage.setAutoDelete(true);
  int mobj = atoi(_ini->text("MandatoryObjects","SupportedObjects"));
  int obobj = atoi(_ini->text("OptionalObjects","SupportedObjects"));
  
  // calculate sum off all objects and subobjects in EDS FILE
  int sumobj = 
     countobj("MandatoryObjects", mobj) + countobj("OptionalObjects", obobj);
     
  objstorage.resize(sumobj);
  // extrakt all objects and subobjects from EDS file into objstorage
  extraktobj("MandatoryObjects", mobj);
  extraktobj("OptionalObjects", obobj);
  rlDebugPrintf("  calculated count obj: %d\n", sumobj);
}

//! returns a specific parameter identified by _paramtype from a directory entry
QString ObjDir::get_objparameter( Obj_Parameters _paramtype, int _obj, int _sub ){
  //cout<<"ObjDir::get_objparameter\n";
  QString objbuffer, subbuffer;
  QString objstring, substring;
  substring.setNum(_sub);
  objstring.setNum(_obj, 16);
  for (int j=0; j<itemcount; j++){
    objbuffer = objstorage.at(j)->objnumber;
    subbuffer = objstorage.at(j)->subnumber;
    if ((objbuffer == objstring.upper())&&(subbuffer == substring)){
      switch(_paramtype) {
        case OBJNUMBER:     return objstorage.at(j)->objnumber;
                            break;
        case SUBNUMBER:     return objstorage.at(j)->subnumber;
                            break;
        case PARAMETERNAME: return objstorage.at(j)->parametername;
                            break;
        case OBJECTTYPE:    return objstorage.at(j)->objecttype;
                            break;
        case DATATYPE:      return objstorage.at(j)->datatype;
                            break;
        case ACCESSTYPE:    return objstorage.at(j)->accesstype;
                            break;
        case DEFAULTVALUE:  return objstorage.at(j)->defaultvalue;
                            break;
        case PDOMAPPING:    return objstorage.at(j)->pdomapping;
                            break;                                  
      }
    }
  }
  return "ERROR";
}

/*! iterates through whole objectdir and returns 1 when given object exists */
int ObjDir::OVAdressExists(  int _obj, int _sub )
{
  QString objbuffer, subbuffer;
  QString objstring, substring;
  substring.setNum(_sub);
  objstring.setNum(_obj, 16);
  for (int j=0; j<itemcount; j++)
  {
     objbuffer = objstorage.at(j)->objnumber;
     subbuffer = objstorage.at(j)->subnumber;
     if ((objbuffer == objstring.upper())&&(subbuffer == substring)) return 1;
  }
  return 0; //Adress not found

}

//! output function for debbuging purposes only
void ObjDir::dout(const char* _str){
  unsigned int j=0;
  while ((_str[j]!='\0')&&(j<=strlen(_str))){
    if ((_str[j]>33)&&(_str[j]<127)){
      printf("%d[%c] ", j, _str[j]);
    }
    else
    {
      printf("%d[S%d] ", j, _str[j]);
    }
    j++;
  }
  printf("\n");
}

//! creates a new ObjItem and adds it to the objstorage. 
int ObjDir::additem(QString _objname, QString _iniheadline, int _subint){
  QString subnumber;
  subnumber.setNum(_subint);
  QString parametername = ini->text(_iniheadline,"ParameterName");
  parametername.remove((char) ACSII_CODE_CARRIAGE_RETURN);
  QString objecttype = ini->text(_iniheadline,"ObjectType");
  objecttype.remove((char) ACSII_CODE_CARRIAGE_RETURN);
  QString datatype = ini->text(_iniheadline,"DataType");
  datatype.remove((char) ACSII_CODE_CARRIAGE_RETURN);
  QString accesstype = ini->text(_iniheadline,"AccessType");
  accesstype.remove((char) ACSII_CODE_CARRIAGE_RETURN);
  QString defaultvalue = ini->text(_iniheadline,"DefaultValue");
  defaultvalue.remove((char) ACSII_CODE_CARRIAGE_RETURN);
  QString pdomapping = ini->text(_iniheadline,"PDOMapping");
  pdomapping.remove((char) ACSII_CODE_CARRIAGE_RETURN);
  
  ObjItem* newitem = new ObjItem( _objname, subnumber,
                                  parametername, objecttype,
                                  datatype, accesstype,
                                  defaultvalue, pdomapping );

  objstorage.insert(itemcount, newitem);
  //printf("itemcount: %d\n", itemcount);
  itemcount++;
  return 0;
}

//! iterates through all items of given category and adds it's items
int ObjDir::extraktobj(QString _categoryname, int _objcount){
  //cout<<"ObjDir::extraktobj\n";
  int j;
  QString buffer, objname, subnum, subname;
  for (int i=1; i<=_objcount; i++){
    buffer.setNum(i);
    
    objname = ini->text(_categoryname, buffer);
    objname.remove((char) ACSII_CODE_CARRIAGE_RETURN);
    objname.remove("0x");
    subnum = ini->text(objname,"SubNumber");
    subnum.remove((char) ACSII_CODE_CARRIAGE_RETURN);
     
    if (atoi(subnum)!= 0){
      for (j=0; j<atoi(subnum); j++){
        subname = objname;
        subname.append("sub");
        buffer.setNum(j);
        subname.append(buffer);
        additem(objname, subname, j);
      }
    }
    else
    {
      additem(objname, objname, 0);
    }
  }

  return 0;
}

/*! similar to extraktobj but only counts number of available objects */
int ObjDir::countobj(QString _categoryname, int _objcount){
  int counter=0;
  QString buffer, objname, subnum, subname;
  for (int i=1; i<=_objcount; i++){
    buffer.setNum(i);
    objname = ini->text(_categoryname, buffer);
    objname.remove((char) ACSII_CODE_CARRIAGE_RETURN);
    objname.remove("0x");
    subnum = ini->text(objname,"SubNumber");
    subnum.remove((char) ACSII_CODE_CARRIAGE_RETURN);
    if (atoi(subnum)!= 0){
        counter+=atoi(subnum);
    }
    else
    {
      counter++;
    }
  }
  return counter;
}

