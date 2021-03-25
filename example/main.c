/**
 * Copyright 2021 <Alessandro Morniroli>
 */

#include <inttypes.h>
// cppcheck-suppress misra-c2012-21.6; for testing purpose
#include <stdio.h>
#include <stdlib.h>

#include "RingBuffer.h"

static uint32_t arg;

typedef struct
{
    uint8_t  dummy;
    uint32_t x;
} Item_t;

void
RingBuffer_ProtectCallback (uint8_t op, void* ptrArg)
{
    uint32_t* ptrTemp;

    // cppcheck-suppress misra-c2012-11.5; for testing purpose
    ptrTemp = ptrArg;

    switch (op)
    {
        case RING_BUFFER_UNLOCK:
        {
            *ptrTemp = 0;
        }
        break;

        case RING_BUFFER_LOCK:
        {
            *ptrTemp = 1;
        }
        break;

        default:
        {
            assert (0);
        }
        break;
    }
}

int
main (void)
{
    RingBuffer_Create(, Item_t, testBuffer, 8, RingBuffer_ProtectCallback, &arg);

    uint32_t i;
    Item_t   item;
    Item_t*  ptrTemp;
    uint32_t usedHalf;
    uint32_t used;
    uint32_t available;
    uint32_t length;
    int32_t  res;

    length = RingBuffer_GetLength (testBuffer);

    for (i = 0UL; i < (length - 1UL); i++)
    {
        (void) memset (&item, 0x00, sizeof (item));

        item.x = i;

        RingBuffer_Push (testBuffer, item);

        used = RingBuffer_GetUsed (testBuffer);

        available = RingBuffer_GetFree (testBuffer);

        res = memcmp (&testBuffer.array[testBuffer.tail + i], &item, sizeof (item));

        (void) printf ("push %"PRIu32" count %"PRIu32"\n", item.x, used);

        assert ((res == 0) && (used == (i + 1UL)) && (available == ((length - 1UL) - used)));
    }

    usedHalf = (RingBuffer_GetUsed (testBuffer) / ((uint32_t) 2));

    // cppcheck-suppress misra-c2012-11.5; for testing purpose
    // cppcheck-suppress misra-c2012-21.3; for testing purpose
    ptrTemp = calloc (usedHalf, sizeof (*ptrTemp));

    assert (ptrTemp != NULL);

    RingBuffer_Empty (ptrTemp, testBuffer, usedHalf);

    (void) printf ("count after empty %"PRIu32"\n", RingBuffer_GetUsed (testBuffer));

    used = RingBuffer_GetUsed (testBuffer);

    available = RingBuffer_GetFree (testBuffer);

    assert ((used == ((length - 1UL) - usedHalf)) && (available == usedHalf));

    for (i = 0; i < usedHalf; i++)
    {
        (void) memset (&item, 0x00, sizeof (item));

        item.x = i;

        res = memcmp (&ptrTemp[i], &item, sizeof (item));

        assert (res == 0);
    }

    RingBuffer_Fill (testBuffer, ptrTemp, usedHalf);

    (void) printf ("count after fill %"PRIu32"\n", RingBuffer_GetUsed (testBuffer));

    used = RingBuffer_GetUsed (testBuffer);

    available = RingBuffer_GetFree (testBuffer);

    assert ((used == (length - 1UL)) && (available == 0UL));

    item.x = (usedHalf - 1UL);

    while (RingBuffer_GetUsed (testBuffer) > 1UL)
    {
        Item_t popped;

        item.x = ((item.x + 1UL) % (length - 1UL));

        RingBuffer_Pop (testBuffer, popped);

        res = memcmp (&popped, &item, sizeof (popped));

        (void) printf ("pop %"PRIu32" count %"PRIu32"\n", item.x, RingBuffer_GetUsed (testBuffer));

        assert (res == 0);
    }

    RingBuffer_Reset (testBuffer);

    used = RingBuffer_GetUsed (testBuffer);

    available = RingBuffer_GetFree (testBuffer);

    assert ((used == 0UL) && (available == (length - 1UL)));

    // cppcheck-suppress misra-c2012-21.3; for testing purpose
    free (ptrTemp);

    return 0;
}
