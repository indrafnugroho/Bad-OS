#include <stdio.h>
 
int main()
{
    int i, j = 0;
    char hex[100];
    long dec, hasilbagi, sisa;

    printf("Masukkan angka decimal: ");
    scanf("%ld", &dec);

    hasilbagi = dec;

    while (hasilbagi!=0) {
        sisa=hasilbagi%16;
        if(sisa>=10){
            hex[j++] = 55+sisa;
            hasilbagi = hasilbagi/16;
        }else {
            hex[j++] = 48+sisa;
        }    
    }
 
    // display integer into character
    printf("Hexadecimalimal = 0x%c%c\n",hex[i],hex[i+1]);
    return 0;
}