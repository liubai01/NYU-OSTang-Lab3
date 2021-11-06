#include <stdio.h>


int main(int argc, char* argv[])
{
    FILE *fp = NULL;
    char c;
    int accChar = -1; // accmulated character
    int cnt = 0; // counter of occurence of character

    for (int fidx = 1; fidx < argc; ++fidx)
    {
        fp = fopen(argv[fidx], "r");

        if (!fp) {
            printf("File %s not exists!", argv[fidx]);
        }
        
        // read character by character
        while ((c = fgetc(fp)) != EOF)
        {
          if (accChar != c) {
            if (cnt) {
                printf("%c%c", accChar, cnt);
            }
            accChar = c;
            cnt = 1;
          } else {
            ++cnt;
            // reset counter when reach 16
            if (cnt == 16)
            {
                printf("%c%c", accChar, cnt);
                accChar = -1;
                cnt = 0;
            }

          }
        }

        fclose(fp);
    }

    // output the tail
    if (cnt) {
        printf("%c%c", accChar, cnt);
    }
    
    return 0;
}