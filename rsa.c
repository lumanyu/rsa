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

int main()
{
    huge e, d, n, m, c;

    set_huge(&e, 79);
    set_huge(&d, 1019);
    set_huge(&n, 3337);

    set_huge(&m, 668);
    rsa_compute(&m, &e, &n, &c);
    printf("Encrypted to: %d\n", c.rep[0]);

    set_huge(&m, 0);
    rsa_compute(&c, &d, &n, &m);
    printf("Decrypted to: %d\n", m.rep[0]);
}
