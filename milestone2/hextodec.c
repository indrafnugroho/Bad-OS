#include <stdio.h>
#include <math.h>
#include <string.h>

int main()
{
    long long dec, p;
    int i = 0, val, len;
    char hex[17];

    p = 1;
    dec = 0;

    /* input */
    printf("Masukkan hexadecimal: ");
    gets(hex);

    /* cari length */
    len = strlen(hex);
    len--;

    /* Iterasi tiap digit hex */
    while(hex[i] != '\0') {
        /* cari decimal untuk tiap hex[i] */
        if(hex[i] >= '0'&&hex[i] <= '9') {
            val = hex[i]-48;
        } else if(hex[i] >= 'A'&&hex[i] <= 'F') {
            val = hex[i]-65+10;
        } else if(hex[i] >= 'a'&&hex[i] <= 'f') {
            val = hex[i]-97+10;
        }
        i++;
        dec += val*pow(16,len);
        len--;
    }
    printf("Decimal = %lld", dec);
    return 0;
}