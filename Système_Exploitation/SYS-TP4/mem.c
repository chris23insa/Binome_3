#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

#include "mem.h"

void * heap_base = NULL;// first byte of the heap
void * heap_end  = NULL;// first byte beyond the heap

int mem_initialized = 0;

// initialize the memory manager
void mem_init(void)
{
    assert(mem_initialized == 0);

    // request memory from the kernel
    heap_base = mmap(NULL, 800, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    assert(heap_base != MAP_FAILED);

    heap_end = heap_base + 800 ;

    // create some free blocks: five of 80 bytes and one of 400 bytes
    /*void *block_ptr;
    for(int i=0; i<5;i++)
    {
        block_ptr = heap_base + 80*i;
        *( (int64_t*)block_ptr ) = 80;
        // On a mis la valeur 80 dans les 8 premiers octets du blocs libre
        // soit 80 = 0000...001010000
    }
    block_ptr = heap_base + 400;
    *( (int64_t*)block_ptr ) = 400;*/
    *((int64_t*)heap_base) = 800; // Partie 4 header
    *((int64_t*)(heap_end-8)) = 800; // Partie 4 footer
    mem_initialized = 1;
}

void * mem_alloc(int64_t length)
{
    assert(mem_initialized == 1);

    // compute actual size of block
    length = (length+7)/8*8 ; // round up to nearest multiple of 8
    length += 8;              // add space for the header
    printf("length : %ld\n", length);
    // heap traversal
    void *  block_ptr ;
    int64_t header ;
    int64_t size;
    char    flags;

    block_ptr = heap_base;
    while(block_ptr < heap_end)
    {
        // On va lire les 8 octets (contenant la longueur) aux adresses block_ptr-block_ptr+7
        // et on met le résultat dans header
        header = *( (int64_t*)block_ptr );

        flags  = header & 0b111;  // keep only three least significant bits
        size = header & ~0b111;   // discard the three least significant bits

        if( (flags == 0 ) &&      // if current block is free, and
            (size >= length))     // is also large enough, then we have a winner
            break;

        // otherwise we move on to the next block
        block_ptr += size;
    }

    // if the heap  traversal reached this far, then it  means we have
    // found no suitable block, so we should return NULL
    if(block_ptr >= heap_end)
    {
        return NULL;
    }

    int64_t marge = size-length; // marge entre le bloc que l'on veut occupé et
    // celui dans lequel on l'insère
    printf("marge du bloc : %ld\n", marge);
    printf("flags : %d \n", flags);
    printf("size : %ld \n", size);
    printf("header : %ld \n", header);
    printf("footer : %ld\n", *( (int64_t*)(block_ptr+(length-8)) ));
    flags = 0b001; // mark block as taken
    header = length | flags;
    *( (int64_t*)block_ptr ) = header ; // write header back into the block
    *( (int64_t*)(block_ptr+(length-8)) ) = header; // Partie 4 : On modifie aussi le footer
    if (marge >=16) {
        *((int64_t * )(block_ptr + length)) = marge; // Nouveau bloc libre si jamais le bloc est trop grand
        *((int64_t * )(
                block_ptr + length + marge - 8)) = marge; // PArtie 4 : on modifie le footer du bloc libre découpé
        printf("footer AFT nv bloc libre: %ld \n", *( (int64_t*)(block_ptr+length+marge-8)));
    }
    printf("header AFT: %ld \n", header);
    printf("footer AFT: %ld \n", *((int64_t*)(block_ptr+(length-8))));
    printf("header AFT nv bloc libre: %ld \n", *( (int64_t*)(block_ptr+length)));


    return block_ptr + 8 ; // skip header
}

void mem_release(void *ptr)
{
    assert( mem_initialized == 1);
    assert( ((int64_t)ptr % 8) == 0 ); // sanity check

    int64_t header;
    int64_t size;
    char    flags;
    // PArtie 1 a 4
    /*header = *( (int64_t*)(ptr-8));
    flags  = header & 0b111;  // keep only three least significant bits
    size = header & ~0b111;   // discard the three least significant bits

    if (flags == 1){
        flags  = header&0b000;
    }

    header = (size) | flags;
    *( (int64_t*)(ptr+(size-16)) ) = header; // On change aussi la valeur du footer
    *( (int64_t*)(ptr-8)) = header;
    printf("size du block mem_release: %ld\n", size);
    printf("flags du block mem_release: %d\n", flags);
    printf("header du block mem_release: %ld\n", *( (int64_t*)(ptr-8)));
    printf("footer du block mem_release: %ld\n", *( (int64_t*)(ptr+(size-16)) ) );*/

    //Partie 5
    // Variables pour stocker données bloc précédent
    int64_t footer_prec;
    int64_t size_prec;
    char    flags_prec;
    // Variables pour stocker données bloc suivant
    int64_t header_suiv;
    int64_t size_suiv;
    char    flags_suiv;

    header = *( (int64_t*)(ptr-8));
    flags  = header & 0b111;  // keep only three least significant bits
    size = header & ~0b111;   // discard the three least significant bits

    footer_prec = *( (int64_t*)(ptr-16)); // ON lit le footer du bloc precedent
    flags_prec  = footer_prec & 0b111;  // keep only three least significant bits
    size_prec = footer_prec & ~0b111;   // discard the three least significant bits

    header_suiv = *( (int64_t*)(ptr+size-8)); // ON lit l'header du bloc suivant
    flags_suiv  = header_suiv & 0b111;  // keep only three least significant bits
    size_suiv = header_suiv & ~0b111;   // discard the three least significant bits
    // On vérifie que la zone est allouée
    assert(flags==1);

    flags  = header&0b000;

    // 4 cas possible
    if (flags_prec == 1 && flags_suiv==1) {
        // Cas dans lequel il ne faut pas fusionner (cas traité précedemment)
        header = size | flags;
        *( (int64_t*)(ptr+(size-16)) ) = header; // On change aussi la valeur du footer
        *( (int64_t*)(ptr-8)) = header;
    }
    else if (flags_prec == 0 && flags_suiv == 0){
        // Cas où on doit fusionner tout les blocs
        int64_t newsize = size + size_prec + size_suiv;
        header = newsize | flags;
        // On met à jour le header du bloc précédent et le footer du bloc suivant
        *( (int64_t*)(ptr+(size+size_suiv-16)) ) = header; // footer du bloc suivant
        *( (int64_t*)(ptr-8-size_prec)) = header; // header du bloc précédent
        // On met à 0 les header et footer restant (car font partis du bloc libre désormais)
        *( (int64_t*)(ptr+(size-16)) ) = 0; // footer du bloc actuel
        *( (int64_t*)(ptr-8)) = 0; // header du bloc actuel
        *( (int64_t*)(ptr+size-8)) = 0; // header du bloc suivant
        *( (int64_t*)(ptr-16)) = 0; // footer du bloc precedent
    }
    else if (flags_prec == 1 && flags_suiv == 0){
        // Cas où le bloc suivant est libre mais pas le prédédent
        int64_t newsize = size + size_suiv;
        header = newsize | flags;
        // On met à jour le header actuel et le footer du bloc suivant
        *( (int64_t*)(ptr+(size+size_suiv-16)) ) = header; // footer du bloc suivant
        *( (int64_t*)(ptr-8)) = header; // header du bloc actuel
        // On met à 0 les header et footer restant (car font partis du bloc libre désormais)
        *( (int64_t*)(ptr+(size-16)) ) = 0; // footer du bloc actuel
        *( (int64_t*)(ptr+size-8)) = 0; // header du bloc suivant
    }
    else if (flags_prec == 0 && flags_suiv == 1){
        // Cas où le bloc precedent est libre mais pas le suivant
        int64_t newsize = size + size_prec;
        header = newsize | flags;
        // On met à jour le header du bloc précédent et le footer du bloc actuel
        *( (int64_t*)(ptr-8-size_prec)) = header; // header du bloc precedent
        *( (int64_t*)(ptr+(size-16)) ) = 0; // footer du bloc actuel
        // On met à 0 les header et footer restant (car font partis du bloc libre désormais)
        *( (int64_t*)(ptr-8)) = 0; // header du bloc actuel
        *( (int64_t*)(ptr-16)) = 0;  // footer bloc précédent
    }
}

void mem_show_heap(void)
{
    assert( mem_initialized == 1);
    
    void * block_ptr = heap_base;

    printf("heap_base = %p\n",heap_base);
    while(block_ptr < heap_end)
    {
        int64_t header = *( (int64_t*)block_ptr );
        char    flags  = header & 0b111;  //   keep only three least significant bits
        int64_t size   = header & ~0b111; // discard the three least significant bits

        int64_t footer = *( (int64_t*)(block_ptr +(size-8))); // On récupère le footer

        // On vérifie que footer = header
        //assert(footer == header);

        if( (size < 8) ||
            (size%8 != 0) )
        {
            printf("error: block at %p has incorrect size %ld\n",block_ptr,size);
            exit(1);
        }

        printf("  block at %p: header=0x%08lx footer=0x%08lx size=%ld flags=%d (%s)\n",
               block_ptr,header, footer, size,flags,
               (flags ? "taken" : "free")
               );

        block_ptr += size; // move on to next block
    }

    printf("heap_end = %p\n",heap_end);

    //sanity check: a full heap traversal should reach *exactly* the end
    assert( block_ptr == heap_end); 
}
