//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
//   Main Core Language.
//
// TANKS TO:
// ----------------------------------------------
//
//   01: God the creator of the heavens and the earth in the name of Jesus Christ.
//
//   02 - Fabrice Bellard: www.bellard.org
//
// ----------------------------------------------
//
// FILE:
//   core.c
//
// START DATE:
//   27/08/2017 - 08:35
//
// BY:  Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "summer.h"

PARSE *coreNewParse (unsigned long size) {
    PARSE *parse = (PARSE*)malloc(sizeof(PARSE));
    ASM *a;

    if (parse && (a = asm_new(size)) != NULL) {
        parse->text = NULL;
        parse->token[0] = 0;
        parse->tok = 0;
        parse->i = 0;
        parse->a = a;
        return parse;
    }
    return NULL;
}

char * coreFileOpen (const char *FileName) {
    FILE *fp;

    if ((fp = fopen (FileName, "rb")) != NULL) {
        char *str;
        int size, i;

        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        str = (char *)malloc (size + 5);
        if(!str){
            fclose (fp);
            return NULL;
        }
        i = fread(str, 1, size, fp);
        fclose(fp);
        str[i] = 0;
        str[i+1] = 0;

        return str;
    }
    else printf ("File Not Found: '%s'\n", FileName);

    return NULL;
}
