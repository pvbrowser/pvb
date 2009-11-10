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
#ifndef DXF2GL_H
#define DXF2GL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// These rather convoluted ifndef's are required because both FALSE, TRUE
// and BOOLEAN are defined by various windows and windows/nt headers.  When
// these headers are included, a redefine error results without these ifndefs.
//
typedef enum {
#ifndef FALSE
                 FALSE = 0, 
#endif
#ifndef TRUE
                 TRUE = 1,
#endif
                 BOGUS
} BoolVals;

#ifndef _WIN32
  typedef int BOOLEAN;
#endif

/* FileAccess - a class for opening and reading ASCII files.

   Public functions:
     FileAccess constructor - The argument to this constructor is
                              the file name.  The constructor will
                              open the file and set the variable
                              FileOpenOK (which can be accessed via
                              FileOK).
     FileAccess destructor  - Close the file
     get_line               - get a line for the file and return it as a null
                              terminated string.
*/
class FileAccess {

  // Variables
  private:
    FILE *FilePtr;
    BOOLEAN FileOpenOK;

  protected:

  public:

  // Class Functions
  private:
    //OpenFile(char *FileName);
    void file_error(const char *msg);
  protected:
  public:
    FileAccess(const char *FileName = "" );
    ~FileAccess(void);
    BOOLEAN FileOK(void) { return FileOpenOK; }
    BOOLEAN get_line(char *LineBuf, const int BufSize);
}; // FileAccess

/*
 The page_pool object is designed to support rapid memory allocation
 and deallocation of all allocated memory, once the object is no
 longer needed.  By using a block allocation scheme, the overhead of
 memory allocation is greatly reduced, compared to "malloc" (or new).
 Deallocation of large blocks of memory is also much faster than
 calling free for every allocated block of memory.

 The page_pool allocator allocates blocks of memory in "page_size"
 increments.  The page_size variable is set at start-up and contains
 the system virtual memory page size.

 The page_pool object contains the virtual function "add_to_list"
 which can be implemented by the inherited function
 ordered_list.  Ordered list is a class that supports ordered list
 insertion and deletion.  In this case, this is used to support an
 ordered list of free memory blocks.  The ordered list is only
 needed if the page_pool is used as the base for a memory allocater
 that can allocate from a free list.
*/

class page_pool {
private: // typedefs and variables
  typedef struct page_chain_struct 
  { void *block;
    unsigned int bytes_used;
    unsigned int block_size;
    page_chain_struct *next_page;
  } page_chain;

  // The min_block_size is the smallest block that will be returned by the
  // page_pool allocater.  The max_block_size is the largest memory block 
  // that can be requested. Ideally, this should be a multiple of the page 
  // size.
  typedef enum { min_block_size = 96, max_block_size = 1024 * 8 } bogus;
  page_chain *page_list_head;
  page_chain *current_page;
  static unsigned int page_size;  // system page size, shared by all instances

private: // class functions
  static unsigned int GetSysPageSize(void);
  page_chain *new_block( unsigned int block_size );
  void *add_block( unsigned int block_size );

public: // class functions
  page_pool(void);
  ~page_pool(void);
  void *page_alloc( unsigned int block_size );
  void print_page_pool_info(void);
  virtual void add_to_list(void *addr, unsigned int size);
}; // class page_pool

/*
  A 3D graphic object can be described as a list of polygons.  Each
  of these polygons consists of three or more points in 3D space
  (e.g., x, y, z coordinates).

  The FaceList object will construct a list of polygon faces read from
  a DXF file.  Two DXF file polygon formats are supported:

    - 3DFACE
    - POLYLINE

  The "face" structure member "f" is a pointer to an array of points.
  This point array is dynamically allocated.  The minimum number of
  points that can be allocated for a polygon is four (a 3DFACE polygon
  may only consist of three points, in which case the last point is
  unused).

  Although during creating there is a distinction made between DXF 
  3DFACE objects and POLYLINE objects, the class represents both as
  polygon faces and no such distinction exists when the polygon
  list is read.

  The object supplies the following public functions for creating a
  polygon list:

    add_3DFACE_point  - Add a 3D point to a 3DFACE polygon 
    add_poly_point    - Add a 3D point to a POLYLINE polygon

    get_new_3DFACE    - Create a new 3DFACE polygon
    get_new_poly_face - Create a new POLYLINE polygon 

  Once the polygon list is created, it can be read using the following
  functions:

    GetListStart      - Return a handle for the start of the polygon face
                        list
    GetFace           - Get a polygon face, given a list handle
    GetNextFace       - Get the next polygon face in the list

*/
class FaceList {
  public:
    typedef enum {MinSize = 4} bogus;  // minimum number of points
    typedef enum {Xcoord, Ycoord, Zcoord} bogus2;

    typedef struct { float v[3]; } vect;   // a point in the 3D plane

    typedef struct { vect *f;          // an array of 3D points
                     int point_cnt;    // number of points currently alloc'ed
                     int point_max;    // max points
                   } face;

    typedef void *ListHandle;

  private:
    typedef struct FaceListStruct { face cur_face;
                                    struct FaceListStruct *next_face;
                                  } FaceElem;
    page_pool mem;
    FaceElem *ListHead;
    FaceElem *ListTail;

  private:
    void add_point(face *cur_face, float x, float y, float z);
    // void GrowFace( face *cur_face );

  public:
    FaceList(void) { ListHead = NULL; 
                     ListTail = NULL;
                   }
    void add_3DFACE_point(float x, float y, float z );
    void add_poly_point(float x, float y, float z);
    void get_new_3DFACE(void);
    void get_new_poly_face(int polypont);
    ListHandle GetListStart(void) { return (ListHandle)ListHead; }
    face *GetFace( ListHandle handle)
      { return &((FaceElem *)handle)->cur_face; }
    ListHandle GetNextFace( ListHandle handle ) 
      { return (ListHandle)((FaceElem *)handle)->next_face; }
    void print(void);
};

//
// The gl_token enumeration defines the tokens that are returned by the
// scanner and are used by the parser in attempting to recognize the
// components of a DXF file.
//
typedef enum { bad_gl_token,
               section,
               header,
               endsec,
               seqend,
               blocks,
               block,
               endblk,
               vertex,
               tables,
               table,
               layer,
               contin,
               endtab,
               entities,
               three_d_face,
               end_of_file,
               polyline,
               integer,
               real,
               name,
               last_gl_token } gl_token;

/*
   This file contains the stack object definition.  The scanner
   requires two token look ahead to resolve things like the EOF token
   sequence.  If this sequence is checked for, but not found, it is
   necessary to "back up" the input stream.  This is done via the token
   stack.

   Note that a DXF file may contain both integer and float values.
   Two versions of "push_token" are implemented, one with an integer
   value and one with a float value.  The integer version may be used
   with just a token value, in which case the default value will be
   used for the integer.
*/
class token_stack {
  public:
    typedef union { int intval;
                    float realval;
                  } token_union;

    typedef struct { gl_token token;
                     token_union val;
                   } token_stack_type;

 private:
    typedef enum {MaxTokens = 10} bogus1;
    token_stack_type token_stack_buf[ MaxTokens ];
    int token_sp;

  public:
    token_stack(void) { token_sp = MaxTokens; }
    BOOLEAN StackHasItems(void) { return token_sp < MaxTokens; }
    void push_token( gl_token token, int intval = 0 );
    void push_token( gl_token token, float realval );
    token_stack_type pop_token(void);
};

/*
  This file contains the scanner object that tokenizes the input
  stream.  The scanner either returns a reserved word (e.g., section
  or three_d_face), "name" (an identifier, such as a layer name or
  face name), integer or real.  The various DXF markers (e.g., the 70
  marker or 0 for start) are returned as integers.  This scanner is
  designed as a front end for a program that reads a DXF file and
  displays it as a 3D wire frame, using openGL (hense then "GL"
  naming).

  Scanning and parsing a DXF file is difficult, because the DXF file
  format seems to have grown over time in an ad hoc manner.  Further,
  the documentation is obscure and/or incomplete.  While parsing the
  file, it is sometimes necessary to "back up".  To support this, a
  stack is maintained.  Input tokens and values can be "pushed" onto
  this stack and re-read.  The token_stack class supports this feature.

*/
class GL_scanner {
  // variables
  private:
    token_stack stack;
    float gl_float;
    int gl_int;
    FileAccess *MyFileObj;

    typedef enum {WordBufSize = 40} bogus2;
    char WordBuf[ WordBufSize ];
  protected:
  public:

  // class functions
  private:
    gl_token scan_number(char *cptr);
    gl_token scan_word(char *cptr);
    gl_token gl_name_lookup(void);
  protected:
  public:
    GL_scanner( FileAccess *FileObj ) { MyFileObj = FileObj; }
    float get_gl_float() { return gl_float; }
    int get_gl_int() { return gl_int; }
    char *get_gl_name() { return WordBuf; }
    gl_token get_next_token(void);
    BOOLEAN token_in_stack(void) { return stack.StackHasItems(); }
    gl_token get_token_from_stack();
    void put_token_in_stack(const gl_token token);
    void put_token_in_stack(const gl_token token, int DX_val)
      { stack.push_token(token, DX_val); }
    void print_token(const gl_token token);
};


/* This file contains the parser object that processes the DXF file.
   The class function GL_GetFaceList returns the Face list for the
   3D DXF object.
 */
class MinMax {
 private:
   float min;
   float max;
 public:
   typedef enum {MAXVAL = 1024*1000} bogus;
   // The initial values of max and min should be way beyond any reasonable
   // range.
   MinMax(void) { min = (float)MAXVAL;  max = (float)(-(MAXVAL)); }
   void SetMinMax( float val ) { if (val < min)
                                   min = val;
                                 if (val > max)
                                   max = val;
                               }
   float GetMin(void) { return min; }
   float GetMax(void) { return max; }
};

class GL_object {
 // variables
 private:
   GL_scanner *GL_scan;
 protected:
 public:
   MinMax PointRange;

 // class functions
 private:
   void set_min_max();
   void undo(gl_token token);
   BOOLEAN IsEndSec(void);
   BOOLEAN IsStart(void);
   BOOLEAN IsName(void);
   BOOLEAN IsTable(void);
   BOOLEAN IsLayerName(void);
   BOOLEAN Is3DFace(void);
   BOOLEAN IsEndSet(void);
   float   GetCoord(void);
   void get_3DFACE_point(FaceList *FaceObjPtr);
   void get_poly_point(FaceList *FaceObjPtr);
   void SkipSection(void);
   void Get3DFACE(FaceList *&CurObj);
   void GetPolyLine(FaceList *&CurObj);
 protected:
 public:
   GL_object(FileAccess *FileObj ) { GL_scan = new GL_scanner( FileObj ); };
   ~GL_object(void) { delete GL_scan; }
   FaceList *GL_GetFaceList(void);
}; // GL_object

typedef enum { DXF_start         = 0, 
               DXF_textval       = 1,
               DXF_name          = 2,
               DXF_othername1    = 3,
               DXF_othername2    = 4,
               DXF_entity_handle = 5,
               DXF_line_type     = 6,
               DXF_text_style    = 7,
               DXF_layer_name    = 8,
               DXF_var_name      = 9,
               DXF_primary_X     = 10,
               DXF_other_X_1     = 11,
               DXF_primary_Y     = 20,
               DXF_other_Y_1     = 21,
               DXF_other_Z       = 31,
               DXF_elevation     = 38,
               DXF_thickness     = 39,
               DXF_floatvals     = 40,
               DXF_repeat        = 49,
               DXF_angle1        = 50,  DXF_angle2        = 51,
               DXF_angle3        = 52,  DXF_angle4        = 53,
               DXF_angle5        = 54,  DXF_angle6        = 55,
               DXF_angle7        = 56,  DXF_angle8        = 57,
               DXF_angle9        = 58,
               DXF_colornum      = 62,
               DXF_entities_flg  = 66,
               DXF_ent_ident     = 67,
               DXF_SeventyFlag   = 70,
               DXF_SeventyOneFlag= 71,
               DXF_SeventyTwoFlag= 72,
               DXF_view_state    = 69,
               DXF_comment       = 999 } DXF_values;

#endif

