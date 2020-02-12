#ifndef MATH_H
#define MATH_H
// Biar shell dan kernel bisa #include
int mod(int x, int y) { 
    while (x>=y) {
        x-=y;
    }return x;
}

