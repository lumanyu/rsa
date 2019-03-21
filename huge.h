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
