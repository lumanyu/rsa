#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KRESET "\x1b[0m"

typedef struct
{
    unsigned char *rep;
    unsigned int size;
} huge;

void add(huge *h1, huge *h2); 
void subtract(huge *h1, huge *h2);
void multiply (huge *h1, huge *h2);
void divide(huge *dividend, huge *divisor, huge *quotient);

int compare(huge *h1, huge *h2);

void copy_huge(huge *tgt, huge *src);
void free_huge(huge *h);
void set_huge(huge *h, unsigned int val);
void print_huge (huge *h);
