#include "huge.h"
#include <stdio.h>
#include <stdlib.h>
#include <jemalloc/jemalloc.h>
//#include <bits/string.h>
#include <string.h>

/**
 * Extend the space for h by 1 and set the LSB of that int
 * to 1
 */
void expand(huge *h)
{
    unsigned char *tmp = h->rep;
    h->size++;
    h->rep = (unsigned char*) calloc (h->size, sizeof(unsigned char));
    memcpy (h->rep + 1, tmp, (h->size - 1) * sizeof(unsigned char));
    h->rep[0] = 0x01;
    free(tmp);
}

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

/**
 * Go through h and see how many of the left-most bytes anr unused.
 * Remove them and resize h appropriately
 */

void contract (huge *h)
{
    int i = 0;
    while (!(h->rep[i]) && (i < h->size)) {
        i++;
    }
    if (i && i < h->size) {
        unsigned char *tmp = &h->rep[i];
        h->rep = (unsigned char*) calloc(h->size -i, sizeof(unsigned char));
        memcpy(h->rep, tmp, h->size - i);
        h->size -= i;
    }
}

void subtract(huge *h1, huge *h2)
{
    int i = h1->size;
    int j = h2->size;
    int difference;//signed int - important!
    unsigned int borrow = 0;

    do {
        i--;
        if(j) {
            j--;
            difference = h1->rep[i] - h2->rep[j] - borrow;
        } else {
            difference = h1->rep[i] - borrow;
        }

        borrow = (difference < 0);
        h1->rep[i] = difference;
    } while (i);

    if (borrow && i) {
        if (!(h1->rep[i-1])) {// don't borrow i
            printf("Error, subtraction result is negative\n");
            exit(0);
        }
        h1->rep[i-1] --;
    }
    contract(h1);
}

void copy_huge(huge *tgt, huge *src)
{
    if (tgt->rep) {
        free(tgt->rep);
    }

    tgt->size = src->size;
    tgt->rep = (unsigned char*)calloc(src->size, sizeof(unsigned char));
    memcpy(tgt->rep, src->rep, (src->size * sizeof(unsigned char)));
}

void free_huge(huge *h)
{
    if (h->rep) {
        free(h->rep);
    }
}

void left_shift (huge *h1)
{
    int i;
    int old_carry, carry = 0;

    i = h1->size;
    do {
        i--;
        old_carry = carry;
        carry = (h1->rep[i] & 0x80) == 0x80;
        h1->rep[i] = (h1->rep[i] << 1) | old_carry;
    } while(i);

    if (carry) {
        expand(h1);
    }
}

void set_huge(huge *h, unsigned int val)
{
    unsigned int mask, i, shift;
    h->size = 4;

    //Figure out the minimum amount of space this "val" will take
    //up in char* (leave at least one byte, though, if "val" is 0)
    for (mask = 0xFF000000; mask > 0x000000FF; mask >>8) {
        if (val & mask) {
            break;
        }
        h->size--;
    }

    h->rep = (unsigned char*) malloc (h->size);
    //Now work backward through the int, masking off each 8-bit
    //byte (up to first 0 byte) and copy it into the "huge"
    //array in big-endian format
    mask = 0X000000FF;
    shift = 0;
    for (i = h->size; i; i--) {
        h->rep[i-1] = (val &mask) >> shift;
        mask <<= 8;
        shift += 8;
    }
}

/**
 * Multiply h1 by h2, overwriting the value of h1
 */

void multiply (huge *h1, huge *h2)
{
    unsigned char mask;
    unsigned int i;
    huge temp;
    set_huge(&temp, 0);
    copy_huge(&temp, h1);

    set_huge(h1, 0);

    i = h1->size;
    do {
        i--;
        for (mask = 0x01; mask; mask <<=1) {
            if (mask && h2->rep[i]) {
                add(h1, &temp);
            }
            left_shift(&temp);
        }
    } while (i);
}

static void right_shift(huge *h1)
{
    int i;
    unsigned int old_carry, carry = 0;

    i = 0;
    do {
        old_carry = carry;
        carry = (h1->rep[i] & 0x01) << 7;//xxxx xxx1 ->  1xxx xxxx
        h1->rep[i] = (h1->rep[i] >> 1) | old_carry;
    } while (++i < h1->size);

    contract (h1);
}

/**
 * compare h1 to h2. Return:
 * 0 if h1 == h2
 * a positive number if h1>h2
 * a negative number if h1<h2
 */

int compare(huge *h1, huge *h2)
{
    int i, j;

    if (h1->size > h2->size) {
        return 1;
    }

    if (h1->size < h2->size) {
        return -1;
    }

    //otherwise, sizes are equal, have to actually commpare
    //only have to compare "hi-int", since the lower ints
    //can't change the comparsion.
    i = j =0;

    //ohterwise, keep searching through the representational integers
    //until one is bigger than another - once we've found one, it's
    //safe to stop, since the "lower order bytes" can't affect the
    //comparsion
    while (i < h1->size && j < h2->size){
        if (h1->rep[i] < h2->rep[j]) {
            return -1;
        } else if (h1->rep[i] > h2->rep[j]) {
            return 1;
        }
        i++;
        j++;
    }

    //If we got all the way to the end without a comparison, the
    //two are equal
    return 0;
}


/**
 * dividend = numerator, divisor = denominator
 * 
 * Note that this process destroys divisor (and, of course,
 * overwrites quotient). The dividend is the remainder of the
 * division (if that's important to the caller). The divisor will
 * be modified by this routine, but it will end up back where it
 * "started".
 */

void divide(huge *dividend, huge *divisor, huge *quotient)
{
    int bit_size, bit_position;

    //"bit_position" keeps track of which bit, of the quotient,
    //is being set or cleared on the current operation.
    bit_size = bit_position = 0;

    //First, left-shift divisor until it's >= than the divideend
    while (compare(divisor, dividend) < 0) {
        left_shift(divisor);
        bit_size++;
    }

    //overestimates a bit in some
    if (quotient) {
        quotient->size = (bit_size / 8) + 1;
        quotient->rep = (unsigned char*) calloc(quotient->size, sizeof(unsigned char));
        memset(quotient->rep, 0, quotient->size);
    }

    bit_position = 8 - (bit_size%8) - 1;

    do {
        if (compare(divisor, dividend) <= 0) {
            if (quotient) {
                subtract(dividend, divisor); //dividend -= divisor
                quotient->rep[(int)(bit_position/8)] |=
                    (0x80 >> (bit_position%8));
            }
        }

        if (bit_size) {
            right_shift(divisor);
        }
        bit_position++;
    } while (bit_size--);

}

int huge_main(void) {
    char h1[100] = {0x01, 0xFF, 0xFE};
    char h2[100] = {0x01, 0xFF, 0xFE};
    huge hh1 = {h1, 100};
    huge hh2 = {h2, 100};
    //big endian: x h1->rep, p h1->size
    add(&hh1, &hh2);
    subtract(&hh1, &hh2);
    //printf();

    return 0;
}
