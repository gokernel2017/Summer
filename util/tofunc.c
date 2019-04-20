//-------------------------------------------------------------------
//
// Disasm to Function:
//
//-------------------------------------------------------------------
//
#include <stdio.h>

int main (int argc, char **argv) {
    FILE *fp;
    if (argc >= 2 && (fp = fopen (argv[1], "r")) != NULL) {
        char string [1024];
        printf ("asm func_name {\n");
        while(fgets(string, sizeof(string), fp) != NULL) {
            char *s = string;
            if (s[0] > 32 && s[0]=='0') {
                if (s[4]==':') {
                    int i = 0;
                    s += 5;
                    // remove: '|' >>>
                    //-----------------------------------------------
                    while (s[i]) {
                        if (s[i]=='|') {
                            while (i) {
                                i--;
                                if (s[i]>32)
                                    break;
                            }
                            i++;
                            s[i] = '\n';
                            s[i+1] = 0;
                            break;
                        }
                        i++;
                    }
                    //-----------------------------------------------
                }
                printf("%s",s);
            }
        }
        printf ("}\n");
        fclose(fp);
        printf ("Exiting With Sucess:\n");
    }
    else printf ("USAGE:\n  %s <file_disasm.txt>\n", argv[0]);
    return 0;
}

