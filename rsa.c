#include "huge.h"
#include <stdlib.h>

/**
 * Compute c = m^e mod n
 */
void rsa_compute (huge *m, huge *e, huge *n, huge *c)
{
    huge counter;
    huge one;

    copy_huge(c, m);
    set_huge(&counter, 1);
    set_huge(&one, 1);

    while (compare(&counter, e) < 0) {
        multiply(c, m);
        add(&counter, &one);
    }
    divide(c, n, NULL);

    free_huge(&counter);
    free_huge(&one);

    //Remainder (result) is now in c
}

void test_huge()
{
    huge c;
    huge fin;

    set_huge(&c, 100);
    set_huge(&fin, 100);

    //multiply(&fin, &c);
    //printf("test_huge size:%d, to: %x %x, %d\n", fin.size, fin.rep[0], fin.rep[1], fin.rep[0]*256+fin.rep[1]);
    printf("test_huge size:%d, to: %x %x, %d\n", fin.size, fin.rep[0], fin.rep[1], fin.rep[0]);

    add(&fin, &c);
    printf("test_huge size:%d, to: %x %x, %d\n", fin.size, fin.rep[0], fin.rep[1], fin.rep[0]);

    left_shift(&fin);
    printf("test_huge size:%d, to: %x %x, %d\n", fin.size, fin.rep[0], fin.rep[1], fin.rep[0]*256+fin.rep[1]);

    multiply(&fin, &c);
    printf("test_huge size:%d, to: %x %x, %d\n", fin.size, fin.rep[0], fin.rep[1], fin.rep[0]*256+fin.rep[1]);
    //printf("test_huge size:%d, to: %x %x %x, %d\n", fin.size, fin.rep[0], fin.rep[1], fin.rep[2], fin.rep[0]*256*256+fin.rep[1]*256+fin.rep[2]);

    
    //subtract(&fin, &c);
    //printf("test_huge size:%d, to: %x %x, %d\n", fin.size, fin.rep[0], fin.rep[1], fin.rep[0]*256+fin.rep[1]);


    right_shift(&fin);
    printf("test_huge size:%d, to: %x %x, %d\n", fin.size, fin.rep[0], fin.rep[1], fin.rep[0]*256+fin.rep[1]);

    divide(&fin, &c, NULL);
    printf("test_huge size:%d, to: %x %x, %d\n", fin.size, fin.rep[0], fin.rep[1], fin.rep[0]*256+fin.rep[1]);

    //multiply(&fin, &c);
    //printf("test_huge size:%d, to: %d %d\n", fin.size, fin.rep[0], fin.rep[1]);
}

int main()
{
    test_huge();

    huge e, d, n, m, c;

    set_huge(&e, 79);
    set_huge(&d, 1019);
    set_huge(&n, 3337);

    set_huge(&m, 688);
    rsa_compute(&m, &e, &n, &c);
    printf("Encrypted to size:%d,  %x%x, c: %d\n", c.size, c.rep[0], c.rep[1], c.rep[0]*256+c.rep[1]);

    set_huge(&m, 0);
    rsa_compute(&c, &d, &n, &m);
    printf("Decrypted to size:%d,  %x%x, c: %d\n", m.size, m.rep[0], m.rep[1], m.rep[0]*256+m.rep[1]);
}
