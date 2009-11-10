/*====================================================================

  This software was written by Ian L. Kaplan, Chief Fat Bear, Bear Products
  International.  Use of this software, for any purpose, is granted on two
  conditions: 

   1. This copyright notice must be included with the software
      or any software derived from it.

   2. The risk of using this software is accepted by the user.  No
      warranty as to its usefulness or functionality is provided
      or implied.  The author and Bear Products International provides
      no support.

 ====================================================================*/
//#define TESTING
#include "processviewserver.h"
#include "dxf.h"

// FACCESS.CPP
/* File access routines.

   FileAccess constructor - open the file
   FileAccess destructor  - close the file
   get_line - return a line from the file as a null terminated string
*/
FileAccess::FileAccess(const char *FileName )
{
  FilePtr = fopen(FileName, "r");  // open file for reading
  FileOpenOK = FALSE;
  if (FilePtr != NULL)
    FileOpenOK = TRUE;
  else
    perror(FileName);    // print system error message
} // FileAccess constructor

FileAccess::~FileAccess(void)
{
  if (FileOpenOK)
    fclose(FilePtr);
} // FileAccess destructor

void FileAccess::file_error(const char *msg )
{
  printf("%s\n", msg );
}

//
// Get a line from the file pointed to by the class variable FilePtr.
// When fgets reads the line, it includes the cariage return character.
// This function overwrites this with a null (like the gets() function).
//
BOOLEAN FileAccess::get_line(char *LineBuf, const int BufSize)
{
  BOOLEAN result;

  result = FALSE;
  LineBuf[0] = '\0';
  if (fgets(LineBuf, BufSize, FilePtr)) {
    int len = strlen(LineBuf);

    if (LineBuf[len-1] = '\n')
      LineBuf[len-1] = '\0';
    result = TRUE;
  }
  else {
    if (feof(FilePtr))
      file_error("Unexpected end of file");
    else
      perror("File error:");
  }
  return result;
}

//GLFACE.CPP
/* A 3D graphic object can be described by a list of polygon faces.  Each
   face is itself a list of 3D points (e.g., x, y, z coordinates).
   This class supports the construction of polygon faces for a 3D object
   that is read from a DXF file.  Two DXF polygon types are supported:

    - 3DFACE
    - POLYLINE

   A 3DFACE polygon is composed of either three points (e.g., its a triangle)
   or four points (its a rectangle).

   A POLYLINE object is composed of one or more polygons (what AutoDesk
   calls a polygon mesh).  These may have a variable number of points,
   although the number of points per polygon may be specified in the
   POLYLINE definition.

*/

// add_point
//
// Add a point to a polygon.  If the point being added is the same
// as the last point (which can occur in 3DFACE polygons), skip it.
//
void FaceList::add_point(face *cur_face,
			 float x, float y, float z)
{
  int PointCnt = cur_face->point_cnt;
  float *points = cur_face->f[PointCnt].v;
  float *LastPts = cur_face->f[ PointCnt-1 ].v;

  // If the point being added is not the same as the last point,
  // then add it to the point list.
  if (cur_face->point_cnt == 0 ||
      (LastPts[Xcoord] != x || LastPts[Ycoord] != y || LastPts[Zcoord] != z)) {
    points[Xcoord] = x;
    points[Ycoord] = y;
    points[Zcoord] = z;
    cur_face->point_cnt++;
  }
} // add_point

//
// add_3DFACE_point
//
// This function adds a 3D point to a 3DFACE polygon.  The number of
// points in a 3DFACE object is defined when the polygon face is allocated
// and is fixed at no more than four.
//
void FaceList::add_3DFACE_point(float x, float y, float z )
{

  if (ListTail != NULL) {
    face *cur_face = &ListTail->cur_face;
    int PointCnt = cur_face->point_cnt;

    if (PointCnt >= cur_face->point_max) {
      printf("FaceList::add_3DFACE_point: Attempted to add too many points\n");
    }
    else {
      add_point(cur_face, x, y, z);
    }
  }
  else
    printf("FaceList::add_3DFACE_point: Can't add point to unalloced list\n");
} // add_3DFACE_point

/*
  add_poly_point

  A DXF POLYLINE object is composed of a "polygon mesh", which
  contains one or more polygons.  These polygons may have a variable
  number of points.  According to the spec, the "71" marker following
  the POLYLINE marker gives the number of polygons and the "72" marker
  gives the number of points.  But the spec also says that these
  numbers are not required to be correct (got to love it).  Apparently
  the DXF reader is supposed to figure it out.  How this is done, is
  not stated (at least so that I could understand).

  So the code below depends on the "72" marker being correct for the
  "point maximum" of an object.  This value set when the polygon
  face is allocated.

  A POLYLINE object consists of a sequence of VERTEXs.  When "point_max"
  VERTEXs are read, a new POLYLINE face is allocated (get_new_poly_face).

 */
void FaceList::add_poly_point(float x, float y, float z )
{
  if (ListTail != NULL) {
    face *cur_face = &ListTail->cur_face;
    int PointCnt = cur_face->point_cnt;

    // Hit the end of the current polygon, start a new one
    if (cur_face->point_cnt >= cur_face->point_max) {
      get_new_poly_face( cur_face->point_max );
      cur_face = &ListTail->cur_face;
    }

    add_point(cur_face, x, y, z);
  }
  else
    printf("FaceList::add_poly_point: Can't add point to unalloced list\n");
} // add_poly_point

//
// get_new_3DFACE
//
// Allocate storage for a new 3DFACE (e.g., a polygon face consisting
// of three or four points).
//
void FaceList::get_new_3DFACE(void)
{
  FaceElem *tmp;
  vect *VectPtr;

  tmp = (FaceElem *)mem.page_alloc( sizeof( FaceElem ) );
  VectPtr = (vect *)mem.page_alloc( sizeof( vect ) * MinSize );
  tmp->next_face = NULL;
  tmp->cur_face.point_cnt = 0;
  tmp->cur_face.f = VectPtr;
  tmp->cur_face.point_max = MinSize;
  if (ListHead == NULL) {
    ListHead = tmp;
    ListTail = tmp;
  }
  else {
    ListTail->next_face = tmp;
    ListTail = tmp;
  }
}

//
// get_new_poly_face
//
// Allocate memory for a polygon face from a POLYLINE object.  The
// number of points is given by the polypoint argument.
//
void FaceList::get_new_poly_face(int polypoint)
{
  FaceElem *tmp;
  vect *VectPtr;

  tmp = (FaceElem *)mem.page_alloc( sizeof( FaceElem ) );
  VectPtr = (vect *)mem.page_alloc( sizeof( vect ) * polypoint );
  tmp->next_face = NULL;
  tmp->cur_face.point_cnt = 0;
  tmp->cur_face.f = VectPtr;
  tmp->cur_face.point_max = polypoint;
  if (ListHead == NULL) {
    ListHead = tmp;
    ListTail = tmp;
  }
  else {
    ListTail->next_face = tmp;
    ListTail = tmp;
  }
}

#ifdef DYNAMIC_ALLOC
/*
  GrowFace

  GrowFace was originally written so that the number of points in a
  POLYLINE object could be dynamically increased.  This is necessary
  because the number of VERTEXs (e.g., 3D points) that follow the
  POLYLINE header varies.  However, the number of points per POLYLINE
  polygon are supposed to be specified in the header.  So, assuming
  that this number is correct, storage for the polygon can be allocated
  once.  This avoids the complicated memory allocation, deallocation and
  reallocation of freed memory that is supported by this function.
  As a result, this function is currently unused.  It is included in this
  object in case it is necessary to support dynamically growable polygon
  faces.

  This function calls mem.mem_alloc, from the memory allocation object.
  If this function is used, the point vector should have been allocated
  with mem_alloc, instead of page_alloc.  This will allow the old
  memory to be deallocated and recovered for later allocation.

*/
void FaceList::GrowFace( face *cur_face )
{
  const int MaxSize = 1024 * 4;
  int new_size;

  if (cur_face->point_cnt < cur_face->point_max)
    printf("FaceList::GrowFace called too soon\n");
  else {
    new_size = cur_face->point_max * 2;
    if (new_size > MaxSize) {
      printf("FaceList::GrowFace: too many points allocated\n");
      exit(1);
    }
    else {
      vect *VectPtr, *tmp;
      int i;

      VectPtr = (vect *)mem.mem_alloc( sizeof(vect) * new_size );
      for (i = 0; i < cur_face->point_cnt; i++) {
        VectPtr[i] = cur_face->f[i];
      }
      tmp = cur_face->f;
      // Free the old memory
      mem.mem_free( tmp );
      cur_face->f = VectPtr;
      cur_face->point_max = new_size;
    }
  }
}  // GrowFace
#endif

//
// print
//
// Print out the face list for a 3D object.  This function is supplied for
// debugging.
//
void FaceList::print(void)
{
  FaceElem *cur_elem = ListHead;

  while (cur_elem != NULL) {
    int i, j;
    face *cur_face = &(cur_elem->cur_face);

    printf("{ ");
    for (j = 0; j < cur_face->point_cnt; j++) {
      if (j > 0)
        printf("  ");
      printf("[");
      for (i = Xcoord; i <= Zcoord; i++) {
        printf("%f", cur_face->f[j].v[i] );
        if (i < Zcoord)
          printf(", ");
      }
      printf("]");
      if (j < cur_face->point_cnt-1)
        printf("\n");
    } // j
    printf("}\n");
    cur_elem = cur_elem->next_face;
  }
}

//GLOBJ.CPP
static void print_token(gl_token token, GL_scanner &GL_scan);

/* The GL_object class supplies the class functions to read a DXF file
   consisting of 3DFACE and POLYLINE "entities".  These "entities"
   describe the polygon faces that make up a 3D object.  The GL_object
   class was written to support the display of DXF files using openGL
   under windows NT.

   DXF files are text files that describe 3D models built with
   AutoCAD, a program from AutoDesk.  Since AutoCAD was one of the
   first modelers that ran on PC, the DXF file format that it used
   became a defacto standard among the programs that later compeated
   with AutoDesk.  Also, many models are provided in DXF format.  

   The DXF file format has all the hallmarks of something designed by
   a bunch of Fortran programmers.  The format is complicated and only
   partially documented.  A DXF file consists of a number of sections,
   not all of which may be used by a given application.  For example,
   many DXF files contain color information, but GL_object does not
   use it.  Since a given application is likely to ignore at least
   some of the information that may be present in a DXF file, a DXF
   reader must be written so that it skips over sections that it does
   not need.  This design is followed by GL_object too.

   The structure of a DXF file is defined by text strings (which
   this program turns into enumeration "tokens") and numeric values.
   For example, all sections start with a zero (a DXF_start, below) and
   the token "SECTION".  There may also be names.  For example, each
   object or object section has a name (e.g., "Cube3" or "ENGINE").
   GL_object reads the file and breaks it up into "tokens", which are
   special words in the DXF file, and special integers (for example,
   DXF_name).  Points are described by single precision "float" values.

   A copy of the enumeration describing the "special integer values"
   is included for reference below (this is from dxfvals.h).
*/

//
// GL_GetFaceList is called to process the DXF file and return a pointer 
// to a FaceList object.
//
// The DXF file format was not designed so that it could be easily
// described as a grammer and/or a finite state machine.  In fact, one
// almost suspects that the format is kept purposely obscure, in an
// attemp to stifle any competition for AutoDesk.  An attempt is made
// below to describe the DXF file format in terms of a grammer.  Truth
// to tell, it seems rather dicey.  John Walker and his colleagues may
// have been geniuses, but they also had clay feet.  Below is an attempt
// at describing the file format in grammerical form.  As anyone who has
// read the DXF file format description can see, this is only a fraction
// of the sections that can be in a DXF file.  Since some DXF file
// information is application dependent and is not needed by all
// applications, unneeded sections are simply skipped.
//
//   Notes: Words that are in capitols are tokens (see tokens.h),
//          that is, values returned by the scanner.
//          Words that start with "DXF_" are DXF integer values
//          with special meanings.  Curley braces ("{" and "}")
//          mean "one or more occurances".
//
// DXF_Section = DXF_start SECTION DXF_name SecName SecData SecEnd
// SecName = HEADER | TABLES | ENTITIES
// SecData = DXF_start SecStuff 
// SecEnd = DXF_start ENDSEC
// SecStuff = TableSec | EntitiesSec
// TableSec = TABLE TableGroup DXF_start ENDTAB
// TableGroup = LayerGroup | OtherGroup
// OtherGroup = DXF_name GroupName GroupData
// LayerGroup = DXF_name LAYER DXF_SeventyFlag MaxItems {LayerDesc}
// LayerDesc = DXF_start LAYER DXF_name LayerName 
//             DXF_SeventyFlag FlagVals DXF_colornum
//             ColorNum DXF_line_type LineType
// LineType = CONTINUOUS
// EntitiesSec = {DXF_start EntType DXF_layer_name EntName EntData}
// EntTyp = THREE_D_FACE 
// EntData = ThreeD_data
// ThreeD_data = {Face}
// Face = Set1 Set2 Set3 Set4 EndSet
// EndSet = 62 0 
//
//
FaceList *GL_object::GL_GetFaceList(void)
{
  FaceList *CurObj;
  gl_token token;

  CurObj = NULL;
  token = GL_scan->get_next_token();
  while (token != end_of_file) {
    switch (token) {
    case header:
      // Skip the header section
      SkipSection();
      break;
    case tables:
      // This program skips the table section.  Some of the
      // grammar for tables is outlined in the comment above.
      while (IsTable()) {
        if (IsName()) {
          // Following the DXF_name token there will be the
          // table name.  Currently all table information is
          // skipped by reading tokens until the ENDTAB token
          // is encounterd.
          // Get the table name
          token = GL_scan->get_next_token();
          while (token != endtab && token != end_of_file) {
            token = GL_scan->get_next_token();
          }
          if (token == end_of_file) {
            printf("GL_GetFaceList: unexpected end of file\n");
          }
        } // IsName
      } // while table
      break;
    case blocks:     // terminated by an endblk
    case block:
    case entities: { // ended by an endsec
      token = GL_scan->get_next_token();
      while (token != endsec && token != endblk && 
             token != end_of_file) {

        switch (token) {
        case three_d_face:
          Get3DFACE( CurObj );
          break;
        case polyline:
          GetPolyLine( CurObj );
          break;
        } // switch

        if (token != end_of_file)
          token = GL_scan->get_next_token();
      } // while
    } // case blocks && entities
    } // end switch
    if (token != end_of_file)
      token = GL_scan->get_next_token();
  } // while
  return CurObj;
} // GL_GetFaceList

/* GetPolyLine

   Process a DXF file POLYLINE section and enter the points in the
   polygon face list.

   An attempt at explaining the POLYLINE format is shown below:

     PolyLineHeader PolyVert MeshVals
     PolyLineHeader = DXF_start POLYLINE DXF_name EntName
     PolyVert = DXF_entities_flg PolyElev
     PolyElev = 10 0 20 0 30 Elevation
     MeshVals = 70 0 71 PolygonCount 72 PointCount
     EntName = string

   The code below skips the POLYLINE header and the POLYLINE vertices
   section.  It looks for the "72" marker, which indicates the number
   of points per polygon.  Or at least that is what I think it means
   and what this object uses it for.

   Following the 71 and 72 groups is a sequence of VERTEX objects, where
   each VERTEX describes one 3D point.  It is these points that are
   entered into the polygon face list.

   I have not been able to get POLYLINE objects to display properly, so
   there is clearly something wrong with my interpretation of the
   DXF file format spec.
 */
void GL_object::GetPolyLine(FaceList *&CurObj)
{
  gl_token token;
  int polypoint = 3;

  if (CurObj == NULL)
    CurObj = new FaceList;

  token = GL_scan->get_next_token();
  // skip down to the first vertex in the poly line
  while (token != vertex && token != seqend && token != end_of_file) {
    token = GL_scan->get_next_token();
    // Look for the 72 marker - the number of points in the polygon
    // If this is not found, then the default is three
    if (token == integer &&
        GL_scan->get_gl_int() == DXF_SeventyTwoFlag) {
      token = GL_scan->get_next_token();
      if (token == integer) {
        polypoint = GL_scan->get_gl_int();
      }
      else {
        printf("GetPolyLine: int value expected after '72' polycount mark\n");
      }
    }
  }
  if (token == vertex) {

    // allocate storage for one polygon in the polyline object
    CurObj->get_new_poly_face(polypoint);  

    while (token != seqend && token != end_of_file) {

      token = GL_scan->get_next_token(); // get the token following the vertex
      if (GL_scan->get_gl_int() == DXF_layer_name) {
        token = GL_scan->get_next_token(); // read the layer name

        token = GL_scan->get_next_token(); // try for colornum
        if (GL_scan->get_gl_int() == DXF_colornum) {
          token = GL_scan->get_next_token(); // read the color value
        }
        else // it not the layer name, so put it back in the stack.
          undo(token);

      }
      else { // it not the layer name, but it the start of a point list,
             // push it back on the stack
        if (GL_scan->get_gl_int() == 10)
          undo(token);
      }

      // We should now be at the start of the point description
      get_poly_point( CurObj);  // get points {10, 20, 30}
      token = GL_scan->get_next_token(); // get the token following point desc.
      while (token != seqend && 
             token != end_of_file &&
             GL_scan->get_gl_int() != DXF_start) {
        token = GL_scan->get_next_token();
      }
      if (token != seqend && token != end_of_file)
        token = GL_scan->get_next_token(); // should be either vertex or seqend
    } // while
  } // if
} // GetPolyLine

/* Get3DFACE

   Process a DXF file 3DFACE section and enter the face into the face
   list.

   A 3DFACE section consists of either three or four 3D points.  Each
   "face" is read in and added to the face list for the object.

   DXF files that consist entirely of 3DFACE "entities" will be interpreted
   properly and will display properly using openGL.
 */
void GL_object::Get3DFACE(FaceList *&CurObj)
{
  gl_token token;

  // The last token fetched should be 3DFACE.  This is followed by some
  // optional information about the face (which this program ignores),
  // followed by the point descriptions.
  //
  // The points are preceeded by the integers {10, 20,  30}, {11, 21, 31},
  // {12, 22, 32},  {13, 23, 33}.  There will always be four points.
  // if the polygon is a triangle, the third and fourth points will be
  // the same.
  //
  // Skip down to the start of the first set:
  //
  token = GL_scan->get_next_token();
  while (token != integer || (token == integer && GL_scan->get_gl_int() != 10))
    token = GL_scan->get_next_token();
  // push the "10" back into the stack
  GL_scan->put_token_in_stack( integer, GL_scan->get_gl_int() );

  // We should not be at the start of the point list
  if (CurObj == NULL)
    CurObj = new FaceList;

  CurObj->get_new_3DFACE();  // allocate storage for a new face
  get_3DFACE_point( CurObj );    // get points {10, 20, 30}
  get_3DFACE_point( CurObj );    // get points {11, 21, 31}
  get_3DFACE_point( CurObj );    // get points {12, 22, 32}
  get_3DFACE_point( CurObj );    // get points {13, 23, 33}
}  // Get3DFACE

/* GetCoord

   Read in a 3D point coordinate.  Each coordiniate is a "float" and
   will be preceeded by an integer value.  For example, the X
   coordinate for the first point will be preceeded by a 10, the Y
   coordinate by a 20, and the Z coordinate by a 30.
*/
float GL_object::GetCoord(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  if (token != integer)
    printf("GL_object::GetCoord: integer point marker expected\n");

  token = GL_scan->get_next_token();
  if (token != real)
    printf("GL_object::GetCoord: real expected\n");
  return GL_scan->get_gl_float();
}

/* get_3DFACE_point

   Read in the three coordinates that make up a 3D point.  The object
   keeps track of the min and max dimensions so that the object can
   be properly displated by openGL.
 */
void GL_object::get_3DFACE_point(FaceList *FaceObjPtr)
{
  float x, y, z;

  x = GetCoord();
  y = GetCoord();
  z = GetCoord();

  FaceObjPtr->add_3DFACE_point(x, y, z );

  PointRange.SetMinMax( x );
  PointRange.SetMinMax( y );
  PointRange.SetMinMax( z );
}

/* get_poly_point

   Read in a 3D pont form a POLYLINE object.
 */
void GL_object::get_poly_point(FaceList *FaceObjPtr)
{
  float x, y, z;

  x = GetCoord();
  y = GetCoord();
  z = GetCoord();

  FaceObjPtr->add_poly_point(x, y, z);

  PointRange.SetMinMax( x );
  PointRange.SetMinMax( y );
  PointRange.SetMinMax( z );
 }

/* ---------------------------------------------------------

   The functions below are used by the DXF file scanner to recognize
   various parts of the DXF file.  An input token stack is used
   so that the scanner can be "backed up", when an item is not found
   and a try can be made for another item.

   --------------------------------------------------------- */
BOOLEAN GL_object::IsEndSet(void)
{
  gl_token token;
  BOOLEAN retval = FALSE;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == 62) {
    if (IsStart())
      retval = TRUE;
  }
  else
    printf("IsEndSet: value 62 expected\n");
  return retval;
}

BOOLEAN GL_object::IsEndSec(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  if (token == endsec)
    return TRUE;
  else {
    printf("ENDSEC token expected\n");
    return FALSE;
  }
}

void GL_object::undo(gl_token token)
{
  if (token == integer)
    GL_scan->put_token_in_stack( integer,  GL_scan->get_gl_int());
  else
    GL_scan->put_token_in_stack( token );
}

BOOLEAN GL_object::Is3DFace(void)
{
  gl_token token;
  int retval = FALSE;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == DXF_start) {
    token = GL_scan->get_next_token();
    if (token == three_d_face) {
      retval = TRUE;
    }
    else {
      GL_scan->put_token_in_stack( token );
      GL_scan->put_token_in_stack( integer, DXF_start );
    }
  }
  else {
    undo(token);
  }

  return retval;
}

BOOLEAN GL_object::IsTable(void)
{
  gl_token token;
  int retval = FALSE;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == DXF_start) {
    token = GL_scan->get_next_token();
    if (token == table) {
      retval = TRUE;
    }
    else {
      GL_scan->put_token_in_stack( token );
      GL_scan->put_token_in_stack( integer, DXF_start );
    }
  }
  else {
    undo(token);
  }

  return retval;
}

BOOLEAN GL_object::IsLayerName(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == DXF_layer_name)
    return TRUE;
  else {
    printf("DXF_layer_name expected\n");
    return FALSE;
  }
}

BOOLEAN GL_object::IsStart(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == DXF_start)
    return TRUE;
  else {
    printf("DXF_start expected\n");
    return FALSE;
  }
}

BOOLEAN GL_object::IsName(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == DXF_name)
    return TRUE;
  else {
    printf("DXF_name expected\n");
    return FALSE;
  }
}

//
// SkipSection
//
// Not all DXF sections are relevant to a given application and some
// sections are AutoDesk application specific.  This function will skip
// a section.  The function reads tokens until it finds the end of section
// sequence.  It then backs up, leaving the end of section sequence in place.
// If the EOF marker is found first, an error is printed.
//
void GL_object::SkipSection(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  while (token != endsec && token != end_of_file) {
    token = GL_scan->get_next_token();
  }
  // put the end section marker back in the stack
  if (token == end_of_file) {
    printf("GL_object::SkipSection: unexpected end of file\n");
    exit(1);
  }
} // SkipSection

static void print_token(gl_token token, GL_scanner &GL_scan)
{
  GL_scan.print_token( token );
  if (token == integer)
    printf(" %d", GL_scan.get_gl_int());
  else
    if (token == real)
      printf(" %f", GL_scan.get_gl_float());
    else
      if (token == name)
        printf(" %s", GL_scan.get_gl_name());
  printf("\n");  
}

//GLSCAN.CPP
//
// This file contains the GL_scanner class functions for the DXF file scanner.
// These functions are:
//
//   print_token     print the associated text string for a token
//   gl_name_lookup  check if the word is a DXF reserved word
//   scan_word       pick out words from the input line and catagorized them
//   can_numer       scan words starting with numeric characters, integers
//                   and reals.
//   get_token_from_stack  pop a token and possibly its value from the token
//                         stack
//   put_token_in_stack    push the current token and possibly its value
//                         into the token stack.
//   get_next_token  The main scanner routine, called to return the
//                   next token from the input stream.
//
// There is a "hole" in C++.  You are not allowed to
// initialize a static variable inside of a class definition,
// even though at static class member is truely static -
// that is, there is only one copy, for all instances of
// that class.  So the variable is included here.
//
typedef struct {
  const char *GLname;
  gl_token token;
} table_rec;

static table_rec res_word_table[] = {
       {"bad_gl_token", bad_gl_token},
       {"SECTION", section},
       {"HEADER", header },
       {"ENDSEC", endsec },
       {"SEQEND", seqend },
       {"BLOCKS", blocks },
       {"BLOCK",  block  },
       {"ENDBLK", endblk },
       {"VERTEX", vertex },
       {"TABLES", tables },
       {"TABLE", table },
       {"LAYER", layer },
       {"CONTINUOUS", contin },
       {"ENDTAB", endtab },
       {"ENTITIES", entities},
       {"3DFACE", three_d_face},
       {"EOF", end_of_file },
       {"POLYLINE", polyline },
       {"integer", integer},
       {"real", real},
       {"name", name},
       { NULL, bad_gl_token }};

//
// print_token
//
// Print the token name, as a character string.
//
void GL_scanner::print_token(const gl_token token)
{
  const char *str = "unknown";

  if (token < last_gl_token)
    str = res_word_table[token].GLname;
  printf("%s", str );
} // print_token

//
// gl_name_lookup
//
// gl_name_lookup looks up a word in the reserved word table.  If the
// word is found in the table, the associated enumeration value is
// returned.  Otherwise the function returns "name", meaning that
// the word is an identifier.
//
gl_token GL_scanner::gl_name_lookup(void)
{

  int ix = 0;
  BOOLEAN found = FALSE;
  gl_token return_token;

  return_token = name;
  while (res_word_table[ix].GLname != NULL && !found) {
    if (strcmp(WordBuf, res_word_table[ix].GLname) == 0) {
      return_token = res_word_table[ix].token;
      found = TRUE;
    }
    else
      ix++;
  }
  return return_token;
} // gl_name_lookup

//
// scan_word
//
// Find a word (e.g., a set of alpha-numeric characters bounded by spaces).
// The function is only called if the first character is an alphabetic char.
// The word will either be a reserved word (e.g., header or layer) or
// an identifier.  This function returns either "name" for an identifier
// or the associated enumeration value for a reserved word.
//
gl_token GL_scanner::scan_word(char *cptr)
{
  char *WordBufPtr = WordBuf;
  gl_token return_token;

  while (isalnum(*cptr))
    *WordBufPtr++ = *cptr++;
  *WordBufPtr = '\0';
  return_token = gl_name_lookup();

  return return_token;
} // scan_word

//
// scan_number
//
// DXF files may contain reserved words (or perhaps identifiers) that
// start with integer values.  For example, the reserved word 3DFACE.
// This function scans the word and returns either "name" or the
// associated enumeration for an identifier that starts with a number.
// The function returns integer or real for numeric values and bad_gl_token
// if there is an error.
//
// The token is either a name or reserved word begining with a number
// (e.g., like the reserved word 3DFACE) or its an integer or a float.
// The grammar is:
//
//    {digit}alpha ==> name or reserved word
//    {digit}(space|null) ==> integer
//    [+|-]{digit}.{digit} ==> float
//
gl_token GL_scanner::scan_number(char *cptr)
{
  char *WordBufPtr = WordBuf;
  gl_token return_token = bad_gl_token;

  if (*cptr == '+')
    cptr++;
  if (*cptr == '-') {
    *WordBufPtr++ = *cptr++;
  }
  while (isdigit(*cptr))
    *WordBufPtr++ = *cptr++;
  if (isalpha(*cptr)) {
    // its a reserved word or a name
    while (isalnum(*cptr))
      *WordBufPtr++ = *cptr++;
    *WordBufPtr = '\0';    
    return_token = gl_name_lookup();
  }
  else {
    if (*cptr == '.') {
      return_token = real;
      *WordBufPtr++ = *cptr++;  // put the '.' into the string
      while (isdigit(*cptr))
        *WordBufPtr++ = *cptr++;
      *WordBufPtr = '\0';
      sscanf(WordBuf, "%f", &gl_float);
    }
    else { // its and integer
      return_token = integer;
      *WordBufPtr = '\0';
      sscanf(WordBuf, "%d", &gl_int);
    }
  }
  return return_token;
} // scan_number

//
// get_token_from_stack
//
// The scanner uses two token look ahead when checking for things like
// the EOF token sequence.  If EOF is not found then the tokens are
// pushed back into the token stack.  This function pops a token and
// possibly an integer or real value from the token stack and puts the
// values in the scanner object.  This allows the rest of the scanner
// to behave as if the token were fetched from the input stream.
//
gl_token GL_scanner::get_token_from_stack()
{
  gl_token token;

  token = bad_gl_token;
  if (token_in_stack()) {
    token_stack::token_stack_type t;

    t = stack.pop_token();
    token = t.token;
    if (token == integer)
      gl_int = t.val.intval;
    else
      if (token == real)
        gl_float = t.val.realval;
  }
  else
    printf("Error: get_token_from_stack called on empty stack\n");
  return token;
}

//
// put_token_in_stack
//
// This function pushs the current token into the token stack.  This is
// needed to support scanner look ahead and back the token stream up
// (just as ungetc() does in UNIX).  The push_token function is
// overloaded to support either a token, a token and an integer or a
// token and a real.
//
void GL_scanner::put_token_in_stack(const gl_token token)
{
  if (token == integer)
    stack.push_token(token, gl_int);
  else
    if (token == real)
      stack.push_token(token, gl_float);
    else
      stack.push_token(token);
}

//
// get_next_token
//
// Scan the DXF file for tokens.  This function assume that there is one 
// DXF token per line.  This is the main scanner routine which is called
// to return the next token.
//
// The functions that call the scanner need a way to check for the
// end of file token sequence and, if the token sequence is not the end 
// of file sequence to push the tokens back into the input stream, 
// so that they will be gotten the next time get_next_token is called.  
// This is supported by the token stack object.
//
gl_token GL_scanner::get_next_token(void)
{
  const int LineSize = 80;
  char line[LineSize];
  gl_token return_token;

  return_token = bad_gl_token;
  gl_int = -1;
  if (token_in_stack()) {
    return_token = get_token_from_stack();
  }
  else {
    if (MyFileObj->get_line( line, sizeof(line) )) {
      char *cptr = line;

      while (isspace(*cptr))
        cptr++;
      // if its an alphabetic character, then it is either a DXF "reserved
      // word" (e.g., a DXF token) or it a name.
      if (isalpha(*cptr))
        return_token = scan_word(cptr);
      else {
        // Its either a token or name starting with a number or its a
        // floating point or an integer.
        return_token = scan_number(cptr);
      }
    }
  }

  return return_token;
}

//GLTSTACK.CPP
//
// push_token : integer version
//
void token_stack::push_token( gl_token token, int intval )
{
  if (token_sp > 0) {
    token_sp--;
    token_stack_buf[ token_sp ].token = token;
    token_stack_buf[ token_sp ].val.intval = intval;
  }
  else
    printf("push_token: stack overflow\n");
}

//
// push_token : float version
//
void token_stack::push_token( gl_token token, float realval )
{
  if (token_sp > 0) {
    token_sp--;
    token_stack_buf[ token_sp ].token = token;
    token_stack_buf[ token_sp ].val.realval = realval;
  }
  else
    printf("push_token: stack overflow\n");
}

//
// pop_token
//
token_stack::token_stack_type token_stack::pop_token(void)
{
  token_stack_type t;

  t.token = bad_gl_token;
  t.val.intval = 0;
  if (token_sp < MaxTokens)
    return token_stack_buf[ token_sp++ ];
  else {
    printf("pop_token: stack underflow\n");
      return t;
  }
}

//PAGEPOOL.CPP
//
// Get the system page size by calling the Windows NT function GetSystemInfo.
// This function is system dependant and must be rewritten for a non-Win32
// platform, like UNIX (live free or die).
//
// This function is used to initialize the static class variable page_size.
//
/*
unsigned int page_pool::GetSysPageSize( void )
{
  SYSTEM_INFO sys_info;  // system info structure, defined in windows.h

  GetSystemInfo( &sys_info );
  return sys_info.dwPageSize;
} // page_pool::GetSysPageSize

unsigned int page_pool::page_size = page_pool::GetSysPageSize();
*/
unsigned int page_pool::page_size = 4096; //rl

//
// page_pool object constructor.  Set up the page list pointers and allocate
// an initial page of memory.
//
page_pool::page_pool(void)
{
  page_chain *new_link;

   new_link = new_block( page_size );
   page_list_head = new_link;
   current_page = new_link;
} // page_pool constructor

//
// page_pool destructor
//
// This function moves through the link list of memory pages and
// deallocates them.
//
page_pool::~page_pool(void)
{
  page_chain *tmp;
  while (page_list_head != NULL) {
    free( page_list_head->block );
    tmp = page_list_head;
    page_list_head = page_list_head->next_page;
    free( tmp );
  }
}

//
// virtual function add_to_list
//
// User defined ordered list function.  This function is optional.
// If the memory allocater built on top of the page_pool object supports
// allocation, deallocation and reallocation, a free list can be added 
// to reduce fragementation in the memory allocated by the page_pool
// allocator.
//
void page_pool::add_to_list(void *addr, unsigned int size)
{
  // Don't do anything...
} // add_to_free_list

//
// new_block
//
// The new_block function is the "root" memory allocator for the
// page_pool object.  The amount of memory allocated is rounded up
// to the next "page_size" boundary.
//
page_pool::page_chain *page_pool::new_block( unsigned int block_size )
{
  page_chain *new_link = NULL;
  int alloc_amt;

  // round to the nearest page size
  alloc_amt = ((block_size + (page_size-1)) / page_size) * page_size;
  if (alloc_amt <= max_block_size) {
    new_link = (page_chain *)malloc( sizeof( page_chain ) );
    if (new_link != NULL) {
      new_link->block = (void *)malloc( alloc_amt );
      new_link->bytes_used = 0;
      new_link->block_size = alloc_amt;
      new_link->next_page = NULL;
    }
    else {
      printf("page_pool::page_chain: malloc memory allocation error\n");
    }
  }
  else {
    printf("page_pool::new_block: allocation request too large\n");
  }
  return new_link;
} // page_chain::new_block

//
// add_block
//
// This function is called when the amount of memory requested by page_alloc
// will not fit in the current block.  If the amount of free memory in
// the current block is larger than a minimum allocation value
// (min_block_size) this free memory is place in the free list
// by calling add_to_list, which is a virtual function.
//
void *page_pool::add_block( unsigned int block_size )
{
  page_chain *new_page = NULL;
  page_chain *last_page;
  int bytes_free;

  last_page = current_page;
  new_page = new_block( block_size );
  current_page->next_page = new_page;
  current_page = new_page;
  bytes_free = last_page->block_size - last_page->bytes_used;
  // if there is unused memory in the block and it is larger than the minimum
  // allocation block, put it in the free list, to avoid fragmentation.
  if (bytes_free >= min_block_size) {
    void *addr;

    addr = (void *)((unsigned long)last_page->block + last_page->bytes_used);
    last_page->bytes_used += bytes_free;
    add_to_list(addr, bytes_free);  // Add to the ordered free list
  }
  return (void *)new_page;
} // page_chain::add_block

//
// page_alloc
//
// This function is called to allocate memory from the page_pool object
// memory pool.  If there is enough free memory in the current block to
// satisify the memory request, memory is allocated from the current
// block and the amount of free memory is updated.  If the current block
// does not have enough memory, add_block is called to allocate a new memory
// block which will be large enough.
//
void *page_pool::page_alloc( unsigned int num_bytes )
{
  void *addr = NULL;
  unsigned int amt_free = current_page->block_size - current_page->bytes_used;

  if (num_bytes > amt_free) {
    if (add_block( num_bytes ) != NULL) {
      amt_free = current_page->block_size;
    }
  }
  if (amt_free >= num_bytes) {

    addr = (void *)((unsigned long)current_page->block + current_page->bytes_used);
    current_page->bytes_used += num_bytes;
  }
  else {
    printf("page_pool::page_alloc: allocation error\n");
    exit(1);
  }
  return addr;
} // page_pool::page_alloc

//
// print_page_pool_info
//
// Print information about the page pool
//
void page_pool::print_page_pool_info(void)
{
  int total_allocated = 0;
  int total_unused = 0;
  page_chain *ptr = page_list_head;

  printf("[block size, bytes_used]\n");
  while (ptr != NULL) {
    printf("[%4d, %4d]", ptr->block_size, ptr->bytes_used);
    total_allocated += ptr->bytes_used;
    total_unused += (ptr->block_size - ptr->bytes_used);
    if (ptr->next_page != NULL) {
      printf(", ");
    }
    else {
      printf("\n");
    }
    ptr = ptr->next_page;
  } // while
  printf("Total allocated = %5d, total unused = %3d\n", total_allocated,
         total_unused );
}

//
// Feed the point list to openGL as a polygon
//
#ifdef TESTING
static void disp_gl_face(FaceList::face *face_ptr, int type)
{
  int NumPts = face_ptr->point_cnt;
  int i;

  //glBegin( GL_POLYGON );
  for (i = 0; i < NumPts; i++) {
    //glVertex3fv(face_ptr->f[i].v);
    printf("glVertex3fv(%f,%f,%f)\n",
            face_ptr->f[i].v[0],
            face_ptr->f[i].v[1],
            face_ptr->f[i].v[2]);
  }
  //glEnd();
} // disp_gl_face
#else
static void disp_gl_face(FaceList::face *face_ptr, int type)
{
  int NumPts = face_ptr->point_cnt;
  int i;

  glBegin( type );
  for (i = 0; i < NumPts; i++) {
    glVertex3fv(face_ptr->f[i].v);
    //printf("glVertex3fv(%f,%f,%f)\n",
    //        face_ptr->f[i].v[0],
    //        face_ptr->f[i].v[1],
    //        face_ptr->f[i].v[2]);
  }
  glEnd();
} // disp_gl_face
#endif

static void disp_gl_facelist(FaceList *CurObj, int type)
{
    FaceList::ListHandle FaceList = CurObj->GetListStart();
    while (FaceList != NULL) {
      disp_gl_face( CurObj->GetFace(FaceList), type);
      FaceList = CurObj->GetNextFace( FaceList );
    } // while
}

#ifdef TESTING
int main()
{
  FileAccess FileObj("triangles.dxf");

  if (FileObj.FileOK())
  {
    GL_object GL_Obj( &FileObj );
    FaceList *CurObj = GL_Obj.GL_GetFaceList();  // Get the DXF object
    disp_gl_facelist(CurObj, 1);
    delete CurObj;
  }
  return 0;
}
#else
int pvglReadDXF(const char *filename, int type)
{
  FileAccess FileObj(filename);

  if (FileObj.FileOK())
  {
    GL_object GL_Obj( &FileObj );
    FaceList *CurObj = GL_Obj.GL_GetFaceList();  // Get the DXF object
    disp_gl_facelist(CurObj, type);
    delete CurObj;
    return 0;
  }
  return -1;
}
#endif

