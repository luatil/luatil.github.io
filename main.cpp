#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef unsigned int u32;

// stretchy buffer // init: NULL // free: sbfree() // push_back: sbpush() // size: sbcount() //
#define sbfree(a)         ((a) ? free(stb__sbraw(a)),0 : 0)
#define sbpush(a,v)       (stb__sbmaybegrow(a,1), (a)[stb__sbn(a)++] = (v))
#define sbcount(a)        ((a) ? stb__sbn(a) : 0)
#define sbadd(a,n)        (stb__sbmaybegrow(a,n), stb__sbn(a)+=(n), &(a)[stb__sbn(a)-(n)])
#define sblast(a)         ((a)[stb__sbn(a)-1])

#define stb__sbraw(a) ((int *) (a) - 2)
#define stb__sbm(a)   stb__sbraw(a)[0]
#define stb__sbn(a)   stb__sbraw(a)[1]

#define stb__sbneedgrow(a,n)  ((a)==0 || stb__sbn(a)+n >= stb__sbm(a))
#define stb__sbmaybegrow(a,n) (stb__sbneedgrow(a,(n)) ? stb__sbgrow(a,n) : 0)
#define stb__sbgrow(a,n)  stb__sbgrowf((void **) &(a), (n), sizeof(*(a)))

static void stb__sbgrowf(void **arr, int increment, int itemsize)
{
    int m = *arr ? 2*stb__sbm(*arr)+increment : increment+1;
    void *p = realloc(*arr ? stb__sbraw(*arr) : 0, itemsize * m + sizeof(int)*2);
    assert(p);
    if (p) {
        if (!*arr) ((int *) p)[1] = 0;
        *arr = (void *) ((int *) p + 2);
        stb__sbm(*arr) = m;
    }
}


char*
ReadFile(char* Filename)
{
    FILE* F = fopen(Filename, "rb");
    if(F)
    {
        fseek(F, 0, SEEK_END);
        u32 Filesize = ftell(F);
        fseek(F, 0, SEEK_SET);
        char* Buffer = (char*)malloc(Filesize+1);
        fread(Buffer, sizeof(char), Filesize, F);
        Buffer[Filesize] = 0;
        return Buffer;
    }
    return 0;
}


void
WriteFile(char* Buffer, char* Filename)
{
    FILE* F = fopen(Filename, "w");
    if(F)
    {
        u32 BufferSize = strlen(Buffer);
        fwrite(Buffer, BufferSize, sizeof(char), F);
    }
}

u32
PrintUntilNewline(char* OutputBuffer, char* InputBuffer)
{
    char* C = InputBuffer;
    u32 Count = 0;
    while(*C != '\r\n' && *C != '\n')
    {
        *OutputBuffer++ = *C++;
        Count++;
    }
    return Count;
}

u32 
PrintSimpleParagraph(char* OutputBuffer, char* InputBuffer)
{
    u32 Offset = 0;
    Offset = sprintf(OutputBuffer+Offset, "<p>");
    Offset = PrintUntilNewline(OutputBuffer+Offset, InputBuffer);
    Offset = sprintf(OutputBuffer+Offset, "</p>");
    return Offset;
}

enum token_type
{
    TOKEN_WORD,
    TOKEN_NEWLINE,
    TOKEN_EOF,
};

struct token
{
    token_type Type;
    char* Start;
};


char*
ClearWhitespace(char* Buffer)
{
    char* C = Buffer;
    while(*C == ' ' || *C == '\t') C++;
    return C;
}

inline bool
IsNewline(char C)
{
    return C == '\n' || C == '\r\n';
}


inline bool
IsWhitespace(char C)
{
    return C == ' ' || C == '\t';
}


inline bool
IsPrintableChar(char C)
{
    return !(IsWhitespace(C) || IsNewline(C));
}


void
PrintWord(token Token)
{
    char* C = Token.Start;
    printf("WORD: ");
    while(IsPrintableChar(*C)) putchar(*C++);
    puts("");
}

void
PrintNewline(token Token)
{
    printf("NEWLINE\n");
}


void
PrintEOF()
{
    printf("EOF\n");
}

void
PrintToken(token Token)
{
    switch(Token.Type)
    {
        case TOKEN_WORD:
        {
            PrintWord(Token);
        } break;
        case TOKEN_EOF:
        {
            PrintEOF();
        } break;
        default:
        {
            PrintNewline(Token);
        } break;
    }
}

token
Consume(char* Buffer)
{
    assert(!IsWhitespace(Buffer[0]));
    char* C = Buffer;
    while(!IsWhitespace(*C))
    {
        C++;
    }
    return {TOKEN_WORD, Buffer};
}


token* 
Tokenize(char* Buffer)
{
    token* Tokens = 0;
    Buffer = ClearWhitespace(Buffer);
    
    
    while(Buffer[0] != 0)
    {
        switch(Buffer[0])
        {
            case '\r':
            {
                Buffer++;
            }
            case '\n':
            case '\t':
            {
                token Token = {TOKEN_NEWLINE, Buffer};
                Buffer++;
                sbpush(Tokens, Token);
            } break;
            default:
            {
                token Token = {TOKEN_WORD, Buffer};
                sbpush(Tokens, Token);
                while(IsPrintableChar(*Buffer)) Buffer++;
            } break;
        }
        Buffer = ClearWhitespace(Buffer);
    }
    token FinalToken = {TOKEN_EOF, 0};
    sbpush(Tokens, FinalToken);
    return Tokens;
}

int
main()
{
    char* Buffer = ReadFile("test.md");
    token* Tokens = Tokenize(Buffer);
    for(int I = 0;
        I < sbcount(Tokens);
        I++)
    {
        PrintToken(Tokens[I]);
    }
}
