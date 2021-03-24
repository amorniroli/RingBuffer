#include <inttypes.h>
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
    (void) arg;
    uint32_t* ptrTemp;

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

    for (i = 0; i < (length - 1); i++)
    {
        memset (&item, 0x00, sizeof (item));

        item.x = i;

        RingBuffer_Push (testBuffer, item);

        used = RingBuffer_GetUsed (testBuffer);

        available = RingBuffer_GetFree (testBuffer);

        res = memcmp (&testBuffer.array[testBuffer.tail + i], &item, sizeof (item));

        printf ("push %"PRIu32" count %"PRIu32"\n", item.x, used);

        assert ((res == 0) && (used == (i + 1)) && (available == ((length - 1) - used)));
    }

    usedHalf = (RingBuffer_GetUsed (testBuffer) / ((uint32_t) 2));

    assert ((ptrTemp = calloc (usedHalf, sizeof (*ptrTemp))) != NULL);

    RingBuffer_Empty (ptrTemp, testBuffer, usedHalf);

    printf ("count after empty %"PRIu32"\n", RingBuffer_GetUsed (testBuffer));

    used = RingBuffer_GetUsed (testBuffer);

    available = RingBuffer_GetFree (testBuffer);

    assert ((used == ((length - 1) - usedHalf)) && (available == usedHalf));

    for (i = 0; i < usedHalf; i++)
    {
        memset (&item, 0x00, sizeof (item));

        item.x = i;

        res = memcmp (&ptrTemp[i], &item, sizeof (item));

        assert (res == 0);
    }

    RingBuffer_Fill (testBuffer, ptrTemp, usedHalf);

    printf ("count after fill %"PRIu32"\n", RingBuffer_GetUsed (testBuffer));

    used = RingBuffer_GetUsed (testBuffer);

    available = RingBuffer_GetFree (testBuffer);

    assert ((used == (length - 1)) && (available == 0));

    item.x = (usedHalf - 1);

    while (RingBuffer_GetUsed (testBuffer) > 1UL)
    {
        Item_t popped;

        item.x = ((item.x + 1) % (length - 1));

        RingBuffer_Pop (testBuffer, popped);

        res = memcmp (&popped, &item, sizeof (popped));

        printf ("pop %"PRIu32" count %"PRIu32"\n", item.x, RingBuffer_GetUsed (testBuffer));

        assert (res == 0);
    }

    RingBuffer_Reset (testBuffer);

    used = RingBuffer_GetUsed (testBuffer);

    available = RingBuffer_GetFree (testBuffer);

    assert ((used == 0) && (available == (length - 1)));

    free (ptrTemp);

    return 0;
}
