/* C program to print a given number in words. The program
handles numbers from 0 to 9999 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

void convert_to_words(char* num);

/* Driver program to test above function */
int main(int argc, char ** argv)
{
    int i, start, end;
    char numstr[8];

    start = 1;
    end = 2499;

    if (argc == 3)
    {
        start = atoi(argv[1]);
        end = atoi(argv[2]);
    }

    for (i=start; i<=end; i++)
    {
        sprintf(numstr, "%d", i);
        convert_to_words(numstr);
    }

    return 0;
}

/* A function that prints a number as words */
void convert_to_words(char* num)
{
    // Get number of digits in given number
    int len = strlen(num);

    /* Base cases */
    if (len == 0)
    {
        fprintf(stderr, "empty string\n");
        return;
    }
    if (len > 4)
    {
        fprintf(stderr, "Length more than 4 is not supported\n");
        return;
    }

    /* first string not used, but makes array indexing simple */
    char* single_digits[] = { "zero", "one", "two", "three", "four",
                              "five", "six", "seven", "eight", "nine" };

    /* first string not used, but makes array indexing simple */
    char* two_digits[] = { "",		 "ten",	 "eleven", "twelve",
                           "thirteen", "fourteen", "fifteen", "sixteen",
                           "seventeen", "eighteen", "nineteen" };

    /* first two string not used, but make array indexing simple */
    char* tens_multiple[] = { "",	 "",	 "twenty",
                              "thirty", "forty", "fifty",
                              "sixty", "seventy", "eighty",
                              "ninety" };

    char* tens_power[] = { "hundred", "thousand" };

    /* for debugging purposes
    printf("\n%s: ", num);
    */

    /* single digit number */
    if (len == 1)
    {
        printf("%s\n", single_digits[*num - '0']);
        return;
    }

    /* iterate while num is not '\0' */
    while (*num != '\0')
    {
        /* Code path for first 2 digits */
        if (len >= 3)
        {
            if (*num - '0' != 0)
            {
                printf("%s ", single_digits[*num - '0']);
                // here length can be 3 or 4
                printf("%s ", tens_power[len - 3]);
            }
            --len;
        }
        /* Code path for last 2 digits */
        else
        {
            /* explicitly handle 10-19. Sum of the
               two digits is used as index of "two_digits"
               array of strings */
            if (*num == '1')
            {
                int sum = *num - '0' + *(num + 1) - '0';
                printf("%s\n", two_digits[sum]);
                return;
            }
            /* explicitly handle 20 */
            else if (*num == '2' && *(num + 1) == '0')
            {
                printf("twenty\n");
                return;
            }
            /* other two digit numbers i.e., 21 to 99
            */
            else
            {
                int i = *num - '0';
                printf("%s ", i ? tens_multiple[i] : "");
                ++num;
                if (*num != '0')
                {
                    printf("%s ", single_digits[*num - '0']);
                }
            }
        }
        ++num;
    }
    printf("\n");
}
