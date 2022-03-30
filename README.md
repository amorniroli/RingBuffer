# RingBuffer
[![Build Status](https://api.travis-ci.com/amorniroli/RingBuffer.svg?branch=main)](https://app.travis-ci.com/github/amorniroli/RingBuffer) [![codecov](https://codecov.io/gh/amorniroli/RingBuffer/branch/main/graph/badge.svg)](https://codecov.io/gh/amorniroli/RingBuffer)

Header-only ring buffer C util.

# Features
- single header module;
- extra configuration file to enable features such as custom assert & thread-safe protect callback;
- malloc free;
- generic type supported;
## Usage
The module must be configured by supplying a project specific `RingBuffer_Conf.h`. `RingBuffer_ConfTemplate.h` can be copied from `template` folder, renamed and modified to supply compile time options.
#### Create ring buffer
```c
#include "RingBuffer.h"

/* Generic type. */
typedef struct
{
    uint8_t  dummy;
    uint32_t x;
} Item_t;

static void* ptrMutex;

/* Thread-safe custom protect callback. */
void
ProtectCallback (uint8_t op, void* ptrArg)
{
    switch (op)
    {
        case RING_BUFFER_UNLOCK:
        {
            /* Exit critical section / unlock mutex / suspend scheduler. */
        }
        break;

        case RING_BUFFER_LOCK:
        {
            /* Enter critical section / lock mutex / suspend scheduler. */
        }
        break;

        default:
        break;
    }
}

/*
 * Create a static Item_t ring buffer named testBuffer w/ 7 slots using
 * ProtectCallback w/ custom argument for thread-safe protection.
 */
RingBuffer_Create (static , Item_t, testBuffer, 7, ProtectCallback, ptrMutex);
```
#### Push item into ring buffer
```c
Item_t item;

item.x = 10;

RingBuffer_Push (testBuffer, item);
```
#### Pop item from ring buffer
```c
Item_t item;

RingBuffer_Pop (testBuffer, item);
```
#### Fill ring buffer with multiple items
```c
uint8_t i;
Item_t  item[4];

for (i = 0; i < 4; i++)
{
    item.x = i;
}

RingBuffer_Fill (testBuffer, item, 4);
```
#### Empty multiple items from ring buffer
```c
Item_t item[2];

RingBuffer_Empty (item, testBuffer, 2);
```
#### Get number of used slots
```c
uint32_t used;

used = RingBuffer_GetUsed (testBuffer);
```
#### Get number of free slots
```c
uint32_t free;

free = RingBuffer_GetFree (testBuffer);
```
#### Reset head & tail indexes
```c
RingBuffer_Reset (testBuffer);
```
