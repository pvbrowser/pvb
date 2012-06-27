/***************************************************************************
                          rlcanopen.cpp  -  description
                             -------------------
    begin     : Tue March 03 2004
    copyright : (C) 2004 by Marc Bräutigam, Christian Wilmes, R. Lehrig
    email     : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/


#ifndef OBJDIR
#define OBJDIR

#include <iostream>
    using std::cin;
    using std::cout;

#include "rlinifile.h"
#include <stdlib.h>
#include <qstring.h>
#include <qptrvector.h>

#define ACSII_CODE_CARRIAGE_RETURN 13

//! class which handles the object directory of a node
class ObjDir 
{
  public:
  //! enum contains all values saved in one objdir entry
  enum Obj_Parameters{ 
    OBJNUMBER,      
    SUBNUMBER,      
    PARAMETERNAME,  
    OBJECTTYPE,     
    DATATYPE,       
    ACCESSTYPE,     
    DEFAULTVALUE,   
    PDOMAPPING
  };

  //! empty constructor not yet implemented
  ObjDir();
  
  //! nothing to do. QPtrVector Destructor performs memory cleanup.
  ~ObjDir();
  
  /*! main constructor builds object directory. It recieves a pointer to a 
      rlIniFile object which is allready initialized with an EDS file. Calls
      countobj to resize QPtrVector.  Calls extraktobj to extrakt all objects 
      from an eds-object category (MandatoryObjects or OptionalObjects). */
  ObjDir(rlIniFile* _ini);
  
  /*! returns a specific parameter identified by _paramtype from an object 
      directory entry, identified by objectindex and subindex. */
  QString get_objparameter( Obj_Parameters _paramtype, int _obj, int _sub );

  //! iterates through whole objectdir and returns 1 when given object exists
  int OVAdressExists(  int _obj, int _sub );
    
  private:
  /*! this class contains all data of an object diretory entry*/
  class ObjItem
  { 
    public:
    /*! constructor initializes item-data */
    ObjItem( QString _objnumber,
             QString _subnumber,
             QString _parametername,
             QString _objecttype,
             QString _datatype,
             QString _accesstype,
             QString _defaultvalue,
             QString _pdomapping ):
             objnumber     (_objnumber),
             subnumber     (_subnumber),    
             parametername (_parametername),
             objecttype    (_objecttype),
             datatype      (_datatype),
             accesstype    (_accesstype),
             defaultvalue  (_defaultvalue),
             pdomapping    (_pdomapping) {};
              
    QString objnumber;
    QString subnumber;
    QString parametername;
    QString objecttype;
    QString datatype;
    QString accesstype;
    QString defaultvalue;
    QString pdomapping;
  };
  
  //! object which manages a dynamic list of all objects in memory
  QPtrVector<ObjItem> objstorage;
  
  //! pointer to inifile, previously initialized by cannode object
  rlIniFile* ini;
  
  //! output function for debbuging purposes only
  void dout(const char* _str);
  
  /*! creates a new ObjItem and adds it to the objstorage. iniheadline must 
      be generated previously from objectindex and subindex number */
  int additem(QString _objname, QString _iniheadline, int _subint);
  
  /*! iterates through all items of given category. extracts objnames. 
      iterates through all subindexes of objname and calls additem. */
  int extraktobj(QString _categoryname, int _objcount);
  
  /*! similar to extraktobj but only counts number of available objects */
  int countobj(QString _categoryname, int _objcount);
  
  int itemcount;
};


#endif
