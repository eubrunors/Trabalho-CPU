#define hlt 0b00000;
#define nop 0b00001;
#define add 0b00010;
#define sub 0b00011;
#define mul 0b00100;
#define div 0b00101;
#define cmp 0b00110;
#define xchg 0b00111;
#define and 0b01000;
#define or 0b01001;
#define xor 0b01010;
#define not 0b01011;
#define jeMX 0b01100;
#define jneMX 0b01101;
#define jleMX 0b01111;
#define jgMX 0b10000;
#define jgeMX 0b10001;
#define jmpMX 0b10010;
#define ldaMX 0b10011; // Provavelmente é melhor tirar o M[X] e deixar só lda e ldb
#define ldbMX 0b10100;
#define staMX 0b10101;
#define stbMX 0b10110;
#define ldrb 0b10111;
#define movialimm 0b11000;
#define moviahimm 0b11001;
#define addiaimm 0b11010;
#define subiaimm 0b11011;
#define muliaimm 0b11100;
#define diviaimm 0b11101;
#define lshimm 0b11110;
#define rshimm 0b11111;