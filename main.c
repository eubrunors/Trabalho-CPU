#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"

Registrador registrador;
unsigned char MEM[0x99];
int flagControle = 0;

void inicializaRegistrador()
{
    registrador.PC = 0;
    registrador.MBR = 0;
    registrador.MAR = 0;
    registrador.IR = 0;
    registrador.IMM = 0;
    registrador.IBR = 0;
    registrador.AC = 0;
    registrador.E = 0;
    registrador.L = 0;
    registrador.G = 0;
    registrador.LR = 0;
    registrador.A = 0;
    registrador.B = 0;
    registrador.T = 0;
}

void inicializaMemoria()
{
    for (int i = 0; i < tamanhoMemoria; i++)
    {
        MEM[i] = hlt;
    }
}

void imprimeMemoria()
{
    const int colunasPorLinha = 6;

    printf("\nMEMORIA:\n");
    for (int endereco = 0; endereco < tamanhoMemoria; endereco++)
    {
        printf("%04X: 0x%04X\t", endereco, MEM[endereco]);
        if ((endereco + 1) % colunasPorLinha == 0)
        {
            printf("\n");
        }
    }
}

void imprimeEstadoCPU()
{
    printf("\n\nCPU:\n");
    printf("A:\t0x%02X \tB:\t0x%02X \tT:\t0x%02X \n", registrador.A, registrador.B, registrador.T);
    printf("MBR:\t0x%04X \tMAR:\t0x%04X \tIMM:\t0x%02X \n", registrador.MBR, registrador.MAR, registrador.IMM);
    printf("PC:\t0x%04X \tIR:\t0x%02X \tLR:\t0x%02X \n", registrador.PC, registrador.IR, registrador.LR);
    printf("E:\t0x%01X \tL:\t0x%01X \tG:\t0x%01X \n", registrador.E, registrador.L, registrador.G);
}

void mudaLR()
{
    registrador.LR = !registrador.LR;
}

void busca()
{
    if (registrador.LR == 0)
    {
        registrador.MAR = registrador.PC;
        for (int i = 0; i < 4; i++)
        {
            registrador.MBR <<= 8;
            registrador.MBR |= MEM[registrador.MAR + i];
        }
    }
}
void decodifica()
{
    if (registrador.LR == 0)
    {  
        registrador.IBR = registrador.MBR & MASK_16BIT;

        registrador.MBR = registrador.MBR >> 16;
        registrador.IR = registrador.MBR >> 11;

        // VERIFICA SE É IMEDIATO
        if (registrador.IR >= moviahImm && registrador.IR <= rshImm)
        { 
            registrador.IMM = registrador.MBR & MASK_11BIT;
        }
        else
        {
            registrador.MAR = registrador.MBR & MASK_11BIT;
        }
    }
    else
    {
        registrador.IR = registrador.IBR >> 11;

        // VERIFICA SE É IMEDIATO
        if (registrador.IR >= moviahImm && registrador.IR <= rshImm)
        { 
            registrador.IMM = registrador.IBR & MASK_11BIT;
        }
        else
        {
            registrador.MAR = registrador.IBR & MASK_11BIT;
        }

        if (registrador.IR != hlt)
            registrador.PC += 4;
    }
}
void executa()
{   
    if (registrador.IR == hlt)
    {   
        flagControle = 1;
    }
    else if (registrador.IR == nop)
    {
        if (registrador.LR == 1)
        {
            registrador.PC += 4;
            mudaLR();
        }
        else
        {
            mudaLR();
        }
    }
    else if (registrador.IR == add)
    {
        registrador.A = registrador.A + registrador.B;
        mudaLR();
    }
    else if (registrador.IR == sub)
    {
        registrador.A = registrador.A - registrador.B;
        mudaLR();
    }
    else if (registrador.IR == mul)
    { // mul
        registrador.A = registrador.A * registrador.B;
        mudaLR();
    }
    else if (registrador.IR == div)
    {
        registrador.A = registrador.A / registrador.B;
        mudaLR();
    }
    else if (registrador.IR == lda)
    { // LDA
        registrador.MBR = MEM[registrador.MAR];
        registrador.MBR = registrador.MBR << 8;
        registrador.MAR++;
        registrador.MBR = registrador.MBR | MEM[registrador.MAR];
        registrador.A = registrador.MBR & 0xFFFF;
        mudaLR();
    }
    else if (registrador.IR == ldb)
    { // LDB
        registrador.MBR = MEM[registrador.MAR];
        registrador.MBR = registrador.MBR << 8;
        registrador.MAR++;
        registrador.MBR = registrador.MBR | MEM[registrador.MAR];
        registrador.B = registrador.MBR & 0xFFFF;
        mudaLR();
    }
    else if (registrador.IR == cmp)
    {
        if (registrador.A == registrador.B)
        {
            registrador.E = 1;
        }
        else
            registrador.E = 0;
        if (registrador.A < registrador.B)
        {
            registrador.L = 1;
        }
        else
            registrador.L = 0;
        if (registrador.A > registrador.B)
        {
            registrador.G = 1;
        }
        else
            registrador.G = 0;
        mudaLR();
    }
    else if (registrador.IR == xchg)
    {
        registrador.T = registrador.A;
        registrador.A = registrador.B;
        registrador.B = registrador.T;
        mudaLR();
    }
    else if (registrador.IR == and)
    {
        registrador.A = registrador.A & registrador.B;
        mudaLR();
    }
    else if (registrador.IR == or)
    {
        registrador.A = registrador.A | registrador.B;
        mudaLR();
    }
    else if (registrador.IR == xor)
    {
        registrador.A = registrador.A ^ registrador.B;
        mudaLR();
    }
    else if (registrador.IR == not )
    {
        registrador.A = !registrador.A;
        mudaLR();
    }
    else if (registrador.IR == je)
    {
        if (registrador.E == 1)
            registrador.PC = registrador.MAR;
        mudaLR();
    }
    else if (registrador.IR == jne)
    {
        if (registrador.E == 0)
            registrador.PC = registrador.MAR;
        mudaLR();
    }
    else if (registrador.IR == jl)
    {
        if (registrador.L == 1)
            registrador.PC = registrador.MAR;
        mudaLR();
    }
    else if (registrador.IR == jle)
    {
        if (registrador.L == 1 || registrador.E == 1)
            registrador.PC = registrador.MAR;
        mudaLR();
    }
    else if (registrador.IR == jg)
    {
        if (registrador.G == 1)
            registrador.PC = registrador.MAR;
        mudaLR();
    }
    else if (registrador.IR == jge)
    {
        if (registrador.G == 1 || registrador.E == 1)
            registrador.PC = registrador.MAR;
        mudaLR();
    }
    else if (registrador.IR == jmp)
    {
        registrador.PC = registrador.MAR;
        mudaLR();
    }
    else if (registrador.IR == sta)
    { // STORE A
        MEM[registrador.MAR] = registrador.A >> 8;
        MEM[registrador.MAR + 1] = registrador.A & 0xFF;
        mudaLR();
    }
    else if (registrador.IR == stb)
    { // STORE B
        MEM[registrador.MAR] = registrador.B >> 8;
        MEM[registrador.MAR + 1] = registrador.B & 0xFF;
        mudaLR();
    }
    else if (registrador.IR == movialImm)
    { // MOVE IMMEDIATE TO LOWER A
        registrador.A = 0x0000;
        registrador.A = registrador.IMM & 0xFF;
        mudaLR();
    }
    else if (registrador.IR == moviahImm)
    { // MOVE IMMEDIATE TO HIGHER A
        registrador.A = registrador.A | registrador.IMM << 8;
        mudaLR();
    }
    else if (registrador.IR == addiaImm)
    {
        registrador.A = registrador.A + registrador.IMM;
        mudaLR();
    }
    else if (registrador.IR == subiaImm)
    {
        registrador.A = registrador.A - registrador.IMM;
        mudaLR();
    }
    else if (registrador.IR == muliaImm)
    {
        registrador.A = registrador.A * registrador.IMM;
        mudaLR();
    }
    else if (registrador.IR == diviaImm)
    {
        registrador.A = registrador.A / registrador.IMM;
        mudaLR();
    }
    else if (registrador.IR == lshImm)
    {
        registrador.A = registrador.A << registrador.IMM;
        mudaLR();
    }
    else if(registrador.IR == rshImm)
    {
        registrador.A = registrador.A >> registrador.IMM;
        mudaLR();
    }
    else
    {
        printf("\nOPCODE Invalido : %d \n", registrador.IR);
        printf("Instrução invalida. Por favor confira novamente a entrada");
        exit(1);
    }
}

int encontraInicio(char *nomeArquivo)
{
    FILE *arquivo;
    int num;
    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }
    num = fgetc(arquivo) - '0';
    fseek(arquivo, -1, SEEK_CUR);
    fclose(arquivo);
    return num;
}

void armazenaDado(char *palavraDado, int enderecoInstrucao)
{
    char *pointAuxiliar;
    unsigned short int dado = strtol(palavraDado, &pointAuxiliar, 16);
    MEM[enderecoInstrucao] = dado >> 8;
    MEM[enderecoInstrucao + 1] = dado & MASK_8BIT;
}

int decodificaInstrucao(char *str)
{
    if (strcmp(str, "hlt") == 0)
    {
        return 0b00000;
    }
    else if (strcmp(str, "nop") == 0)
    {
        return 0b00001;
    }
    else if (strcmp(str, "add") == 0)
    {
        return 0b00010;
    }
    else if (strcmp(str, "sub") == 0)
    {
        return 0b00011;
    }
    else if (strcmp(str, "mul") == 0)
    {
        return 0b00100;
    }
    else if (strcmp(str, "div") == 0)
    {
        return 0b00101;
    }
    else if (strcmp(str, "cmp") == 0)
    {
        return 0b00110;
    }
    else if (strcmp(str, "xchg") == 0)
    {
        return 0b00111;
    }
    else if (strcmp(str, "and") == 0)
    {
        return 0b01000;
    }
    else if (strcmp(str, "or") == 0)
    {
        return 0b01001;
    }
    else if (strcmp(str, "xor") == 0)
    {
        return 0b01010;
    }
    else if (strcmp(str, "not") == 0)
    {
        return 0b01011;
    }
    else if (strcmp(str, "je") == 0)
    {
        return 0b01100;
    }
    else if (strcmp(str, "jne") == 0)
    {
        return 0b01101;
    }
    else if (strcmp(str, "jl") == 0)
    {
        return 0b01110;
    }
    else if (strcmp(str, "jle") == 0)
    {
        return 0b01111;
    }
    else if (strcmp(str, "jg") == 0)
    {
        return 0b10000;
    }
    else if (strcmp(str, "jge") == 0)
    {
        return 0b10001;
    }
    else if (strcmp(str, "jmp") == 0)
    {
        return 0b10010;
    }
    else if (strcmp(str, "lda") == 0)
    {
        return 0b10011;
    }
    else if (strcmp(str, "ldb") == 0)
    {
        return 0b10100;
    }
    else if (strcmp(str, "sta") == 0)
    {
        return 0b10101;
    }
    else if (strcmp(str, "stb") == 0)
    {
        return 0b10110;
    }
    else if (strcmp(str, "ldrb") == 0)
    {
        return 0b10111;
    }
    else if (strcmp(str, "movial") == 0)
    {
        return 0b11000;
    }
    else if (strcmp(str, "moviah") == 0)
    {
        return 0b11001;
    }
    else if (strcmp(str, "addia") == 0)
    {
        return 0b11010;
    }
    else if (strcmp(str, "subia") == 0)
    {
        return 0b11011;
    }
    else if (strcmp(str, "mulia") == 0)
    {
        return 0b11100;
    }
    else if (strcmp(str, "divia") == 0)
    {
        return 0b11101;
    }
    else if (strcmp(str, "lsh") == 0)
    {
        return 0b11110;
    }
    else if (strcmp(str, "rsh") == 0)
    {
        return 0b11111;
    }
    else
    {
        printf("Erro ao decodificar instrucao!!");
        exit(1);
    }
}

void armazemaIntrucao( unsigned int posicaoPalavra, unsigned short int palavraInstrucao,  int palavraEndereco, int flagRightLeft)
{

    if (flagRightLeft == 0)
    {
        MEM[posicaoPalavra] = palavraInstrucao;
        MEM[posicaoPalavra + 1] = palavraEndereco & MASK_8BIT;
    }
    else if (flagRightLeft == 1)
    {
        MEM[posicaoPalavra + 2] = palavraInstrucao;
        MEM[posicaoPalavra + 3] = palavraEndereco & MASK_8BIT;
    }
    else
    {
        printf("Programa deu erro ao armazena instrucao em memoria!");
        exit(1);
    }
}

void processaInstrucao( unsigned int posicao, char* instrucao, int endereco, int flagRightLeft)
{   
    unsigned short int instrucaoDecodificada = decodificaInstrucao(instrucao);
    instrucaoDecodificada = instrucaoDecodificada << 3;
    instrucaoDecodificada |= (endereco & MASK_UPPER_3BIT);

    armazemaIntrucao(posicao, instrucaoDecodificada, endereco, flagRightLeft);
}

void separaInstrucao(char *palavraCompacta, unsigned int posicaoPalavra)
{

    char *textoInstrucaoEsquerda = strtok(palavraCompacta, "/");
    char *textoInstrucaoDireita = strtok(NULL, "/");

    char *instrucaoEsquerda = strtok(textoInstrucaoEsquerda, " ");
    int enderecoEsquerda = strtol(strtok(NULL, " "), NULL, 16);

    char *instrucaoDireita = strtok(textoInstrucaoDireita, " ");
    int enderecoDireita = strtol(strtok(NULL, " "), NULL, 16);

    
    processaInstrucao(posicaoPalavra, instrucaoEsquerda, enderecoEsquerda, 0);
    
    processaInstrucao(posicaoPalavra, instrucaoDireita, enderecoDireita, 1);

}

void lerArquivo(char *nomeArquivo)
{
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL)
    {
        printf("\nOcorreu um erro ao abrir o arquivo!\n");
        exit(1);
    }

    char linha[50];
    unsigned int enderecoInstrucao = 0;

    while (fgets(linha, sizeof(linha), arquivo))
    {
        char palavraCompacta[20];
        char tipoInstrucao;

        sscanf(linha, "%x;%c;%[^\n]s", &enderecoInstrucao, &tipoInstrucao, palavraCompacta);

        if (tipoInstrucao == 'i')
        {
            separaInstrucao(palavraCompacta, enderecoInstrucao);
        }
        else if (tipoInstrucao == 'd')
        {
            armazenaDado(palavraCompacta, enderecoInstrucao);
        }
        else
        {
            printf("Tipo de instrucao invalido. Use 'i' para instrucao ou 'd' para dado.\n");
            printf("\n %c \n", tipoInstrucao);
            exit(1);
        }
    }
    fclose(arquivo);
}

int main()
{
    char nomeArquivo1[] = "instrucoes.txt";

    inicializaRegistrador();
    inicializaMemoria();
    imprimeMemoria();
    imprimeEstadoCPU();

    printf("\nPressione Enter para LER o arquivo!\n");
    getchar();

    registrador.PC = encontraInicio(nomeArquivo1);
    lerArquivo(nomeArquivo1);

    system("cls");
    imprimeMemoria();
    imprimeEstadoCPU();

    printf("\nPressione Enter para prosseguir!\n");
    getchar();

    while (1)
    {
        busca();
        decodifica();
        executa();
        system("cls");
        imprimeMemoria();
        imprimeEstadoCPU();
        if(flagControle==0){
            printf("\nPressione Enter para prosseguir!\n");
        }else{
            printf("\nPressione Enter para prosseguir ou 'ctrl+c' para sair!\n");
        }
        getchar();
    }
    return 0;
}