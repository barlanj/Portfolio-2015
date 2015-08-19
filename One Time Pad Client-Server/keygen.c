/********************************************************************
 * Author: barlanj
 * Description:
 *      This is a utility program to create a key file with uppercase letters
 *      and space char.
 *************************************************************************************/


#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: '%s keyLength'\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
        //get a random seed
        srand(time(NULL));

        //generate random letter
        int length = atoi(argv[1]);
        int i = 0;
        for(i = 0; i < length; i++)
            printf("%c", "ABCDEFGHIJKLMNOPQRSTUVWXYZ "[random()%27]);
        printf("\n");

    }

    return EXIT_SUCCESS;
}
