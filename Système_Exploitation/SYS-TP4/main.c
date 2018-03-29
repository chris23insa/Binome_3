#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "mem.h"

int main()
{
    // initialize the allocator
    mem_init();
    mem_show_heap();

    //int i;
    char* p = NULL;
    /* Test partie 1 & 2
    boucle d'allocation de 42 octets
    char *test = NULL;
    for (i=0; i<6; i++) {
        test = mem_alloc(42);
        assert(test != NULL); // check whether the allocation was successful
        printf("allocated 42 bytes at %p\n", test);
        mem_show_heap();
    }
     // On relache puis on réalloue 42 octets
    mem_release(test);
    printf("test relache\n");
    mem_show_heap()

    test = mem_alloc(42);
    assert(test != NULL); // check whether the allocation was successful
    printf("allocated 42 bytes at %p\n", test);
    mem_show_heap();*/

    // Test partie 1 & 2 : allocation de 200, déallocation puis réallocation de 300
    /*p = mem_alloc(200);
    assert(p != NULL); // check whether the allocation was successful
    printf("allocated 200 bytes at %p\n", p);
    mem_show_heap();

    mem_release(p);
    printf("p relache\n");
    mem_show_heap();

    char * point = NULL;
    point = mem_alloc(300);
    assert(point != NULL); // check whether the allocation was successful
    printf("allocated 300 bytes at %p\n", point);
    mem_show_heap();*/

    // Test partie 3 : Tentative d'allocation de 7 blocs de 42 octets
    /*for (i=0; i<7; i++) {
        p = mem_alloc(42);
        assert(p != NULL); // check whether the allocation was successful
        printf("allocated 42 bytes at %p\n", p);
        mem_show_heap();
    }*/

    // Test partie 3 : Nbre d'allocation de 42 octets possibles
    /*i=0;
    p = mem_alloc(42);
    while (p != NULL){
        i++;
        p = mem_alloc(42);
        printf("allocated 42 bytes at %p\n", p);
        mem_show_heap();
    }
    printf("nb possible : %d\n", i);*/

    // Test partie 3 & 4 : Tentative d'allocation de blocs de 60 octets
    /*for (i=0; i<2; i++) {
        p = mem_alloc(60);
        assert(p != NULL); // check whether the allocation was successful
        printf("allocated 60 bytes at %p\n", p);
        mem_show_heap();
    }
    printf("release p\n");
    mem_release(p);
    mem_show_heap();

    p = mem_alloc(60);
    assert(p != NULL); // check whether the allocation was successful
    printf("allocated 60 bytes at %p\n", p);
    mem_show_heap();*/

    // Test partie 5
    p = mem_alloc(60);
    assert(p != NULL); // check whether the allocation was successful
    printf("allocated 60 bytes at %p\n", p);
    mem_show_heap();

    char * pp = NULL;
    pp = mem_alloc(60);
    assert(pp != NULL); // check whether the allocation was successful
    printf("allocated 60 bytes at %p\n", pp);
    mem_show_heap();

    char * ppp = NULL;
    ppp = mem_alloc(60);
    assert(ppp != NULL); // check whether the allocation was successful
    printf("allocated 60 bytes at %p\n", ppp);
    mem_show_heap();

    char * pppp = NULL;
    pppp = mem_alloc(60);
    assert(pppp != NULL); // check whether the allocation was successful
    printf("allocated 60 bytes at %p\n", pppp);
    mem_show_heap();

    printf("release ppp\n");
    mem_release(ppp);
    mem_show_heap();

    printf("release pp\n");
    mem_release(pp);
    mem_show_heap();

    printf("release pppp\n");
    mem_release(pppp);
    mem_show_heap();
}
