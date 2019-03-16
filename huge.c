#include <huge.h>
#include <stdio.h>

void
add (huge *h1, huge *h2)
{
    unsigned int i, j;
    unsigned sum;
    unsigned int carry = 0;

    //adding h2 to h1. If h2 is > h1 to begin with, reize h1
    if(h2->size > h1->size) {
        unsigned char *tmp = h1->rep;
        h1->rep = (unsigned char*) calloc(h2->size, sizeof(unsigned char));
        memcpy(h1->rep + (h2->size - h1->size), tmp, h1->size);
        h1->size = h2->size;
        free(tmp);
    }

    i = h1->size;
    j = h2->size;

    do {
        i--;
        if (j) {//has added varible
            j--;
            sum = h1->rep[i] + h2->rep[j] + carry;//every char type plus. including carried
        } else {//not has added varible
            sum = h1->rep[i] + carry;
        }

        carry = sum >  0xFF; //prepare for next carry
        h1->rep[i] = sum;

    } while (i);

    if (carry) {
        //still overflowed; allocate more space
        expand(h1);
    }
}