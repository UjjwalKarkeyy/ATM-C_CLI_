#include<stdio.h>
#include<stdlib.h>

int main()
{
    float a = 600.1;

    if((a == (int)a))
        printf("I work: %f\n", (a / 100.0));

    else
        printf("I don't work: %f\n", (a/100.0));
}