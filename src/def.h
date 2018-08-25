#ifndef _DEF_H_
#define _DEF_H_

typedef union  VALUE  VALUE;

union VALUE {
    int     i;  //: type integer
    float   f;  //: type float
    char    *s; //: type pointer of char
    void    *p; //: type pointer
};

#endif // ! _DEF_H_
