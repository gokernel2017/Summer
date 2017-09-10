//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
// FILE:
//   def.h
//
//-------------------------------------------------------------------
//

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define UCHAR       unsigned char
#define GVAR_SIZE   255

enum { // variable type:
    TYPE_LONG = 0,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_POINTER,
    TYPE_STRUCT,
    TYPE_PSTRUCT  // struct data *p;
};

//-----------------------------------------------
//-------------------  STRUCT  ------------------
//-----------------------------------------------
//
typedef union  TValue     TValue;
typedef struct TVar       TVar;

union TValue {
    long    l;  //: type long integer
    float   f;  //: type float
    char    *s; //: type pointer of char
    void    *p; //: type pointer
};
struct TVar {
    char    *name;
    int     type;
    TValue  value;
    void    *info;  // any information ... struct type use this
};

//-----------------------------------------------
//---------------  GLOBAL VARIABLE  -------------
//-----------------------------------------------
//
extern TVar Gvar[GVAR_SIZE];
extern int  return_type;
