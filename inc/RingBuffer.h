/**
 * Copyright 2021 <Alessandro Mornirolir>
 *
 * @file   RingBuffer.h
 * @author Alessandro Morniroli <alessandro.morniroli@gmail.com>
 * @date   Mon Mar 15 2021
 *
 * @brief  Header-only ring buffer util.
 */

/* ***************************************************************************************************************** */
#ifndef RING_BUFFER_H__
#define RING_BUFFER_H__
/* ***************************************************************************************************************** */

/* ***************************************************************************************************************** *
 *                                                  Includes section                                                 *
 * ***************************************************************************************************************** */

/* Standard includes. */
#include <stdint.h>
#include <string.h>
/* Module configuration. */
#include "RingBuffer_Conf.h"

/* ***************************************************************************************************************** *
 *                                             Global definitions section                                            *
 * ***************************************************************************************************************** */

/**
 * Ring buffer module major version.
 */
#define RING_BUFFER_VERSION_MAJOR 1

/**
 * Ring buffer module major version.
 */
#define RING_BUFFER_VERSION_MINOR 0

/**
 * Ring buffer module patch version.
 */
#define RING_BUFFER_VERSION_PATCH 1

/**
 * Ring buffer unlock operation.
 */
#define RING_BUFFER_UNLOCK 0

/**
 * Ring buffer lock operation.
 */
#define RING_BUFFER_LOCK 1

/**
 * Macro for creating a ring buffer.
 *
 * @param          storageClass Storage class for the ring buffer (e.g. static).
 * @param          type         Type of ring buffer elements (e.g. uint8_t).
 * @param          name         Ring buffer name.
 * @param          pLength      Ring buffer length.
 * @param[in]      pPtrCallback Protect callback (\ref RING_BUFFER_PROTECT must be eanbled).
 * @param[in, out] pPtrArg      Protect callback pointer to argument (if \ref RING_BUFFER_PROTECT is enabled).
 */
#if (RING_BUFFER_PROTECT == 1)
    // cppcheck-suppress misra-c2012-20.7; no way to enclose pStorageClass / pName due to X-macro
    #define RingBuffer_Create(pStorageClass, pType, pName, pLength, pPtrCallback, pPtrArg) \
        pStorageClass struct                                                               \
        {                                                                                  \
            pType    array[(pLength)];                                                     \
            uint32_t head;                                                                 \
            uint32_t tail;                                                                 \
            struct                                                                         \
            {                                                                              \
                void (*ptrCallback)(uint8_t, void*);                                       \
                void* ptrArg;                                                              \
            } protect;                                                                     \
        } pName =                                                                          \
        {                                                                                  \
            .head    = 0,                                                                  \
            .tail    = 0,                                                                  \
            .protect =                                                                     \
            {                                                                              \
                .ptrCallback = (pPtrCallback),                                             \
                .ptrArg      = (pPtrArg)                                                   \
            }                                                                              \
        }
#else
    // cppcheck-suppress misra-c2012-20.7; no way to enclose pStorageClass / pName due to X-macro
    #define RingBuffer_Create(pStorageClass, pType, pName, pLength, ...) \
        pStorageClass struct                                             \
        {                                                                \
            pType    array[(pLength)];                                   \
            uint32_t head;                                               \
            uint32_t tail;                                               \
        } pName =                                                        \
        {                                                                \
            .head = 0,                                                   \
            .tail = 0                                                    \
        }
#endif /* RING_BUFFER_PROTECT == 1 */

/**
 * Thread-safe protect callback (if \ref RING_BUFFER_PROTECT is enabled).
 *
 * @param          pOp          0-Unlock, 1-Lock operations.
 * @param[in]      pPtrCallback Protect callback.
 * @param[in, out] pPtrArg      Callback argument.
 */
#if (RING_BUFFER_PROTECT == 1)
    #define RingBuffer_Protect(pOp, pPtrCallback, pPtrArg) \
        ((pPtrCallback) ((pOp), (pPtrArg)))
#else
    #define RingBuffer_Protect(...)
#endif /* RING_BUFFER_PROTECT == 1 */

/**
 * Resets ring buffer indexes.
 *
 * @param pName Ring buffer variable name.
 */
#define RingBuffer_Reset(pName)                                                                   \
{                                                                                                 \
    RingBuffer_Protect (RING_BUFFER_LOCK, (pName).protect.ptrCallback, (pName).protect.ptrArg);   \
    (pName).head = 0;                                                                             \
    (pName).tail = 0;                                                                             \
    RingBuffer_Protect (RING_BUFFER_UNLOCK, (pName).protect.ptrCallback, (pName).protect.ptrArg); \
}

/**
 * Gets the ring buffer type size.
 *
 * @param pName Ring buffer variable name.
 *
 * @return The ring buffer type size.
 */
#define RingBuffer_GetTypeSize(pName) \
    (sizeof ((pName).array[0]))

/**
 * Gets the total slots number of the ring buffer.
 *
 * @param pName Ring buffer variable name.
 *
 * @return The slots number of the ring buffer.
 */
#define RingBuffer_GetLength(pName)                      \
    (sizeof ((pName).array) / sizeof ((pName).array[0]))

/**
 * @brief Increments index.
 *
 * @param pIndex  Index to increment.
 * @param pSize   Size to increment.
 * @param pLength Ring buffer length.
 */
#define RingBuffer_IncrementLinear(pIndex, pSize, pLength)                                                   \
    (pIndex) = (((pIndex) + (pSize)) >= (pLength)) ? ((pIndex) + (pSize) - (pLength)) : ((pIndex) + (pSize))

/**
 * @brief Increments the ring buffer tail index.
 *
 * @param pName Ring buffer variable name.
 * @param pSize Size to increment.
 */
#define RingBuffer_IncrementTail(pName, pSize)                                       \
    RingBuffer_IncrementLinear((pName).tail, (pSize), RingBuffer_GetLength((pName)))

/**
 * @brief Increments the ring buffer head index.
 *
 * @param pName Ring buffer variable name.
 * @param pSize Size to increment.
 */
#define RingBuffer_IncrementHead(pName, pSize)                                        \
    RingBuffer_IncrementLinear((pName).head, (pSize), RingBuffer_GetLength((pName))); \
    if ((pName).head == (pName).tail)                                                 \
    {                                                                                 \
        RingBuffer_IncrementTail((pName), 1U);                                        \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
    }

/**
 * Gets the number of the used slots in the ring buffer.
 *
 * @param pName Ring buffer variable name.
 *
 * @return The number of slots currently used.
 */
#define RingBuffer_GetUsed(pName)                                                                                    \
    ((uint32_t) (((pName).head >= (pName).tail) ? ((pName).head - (pName).tail)                                    : \
                                                  (RingBuffer_GetLength ((pName)) - (pName).tail + (pName).head)))

/**
 * Gets the free slots in the buffer.
 *
 * @param pName Ring buffer variable name.
 *
 * @return The number of slots currently available.
 */
#define RingBuffer_GetFree(pName)                                             \
    ((RingBuffer_GetLength ((pName)) - (RingBuffer_GetUsed ((pName)))) - 1UL)

/**
 * Checks if the ring buffer is empty.
 *
 * @param pName Ring buffer variable name.
 *
 * @return The ring buffer empty status (0-not empty, 1-empty).
 */
#define RingBuffer_IsEmpty(pName)         \
    (RingBuffer_GetUsed ((pName)) == 0UL)

/**
 * Checks if the ring buffer is full.
 *
 * @param pName Ring buffer variable name.
 *
 * @return The ring buffer full status (0-not full, 1-full).
 */
#define RingBuffer_IsFull(pName)          \
    (RingBuffer_GetFree ((pName)) == 0UL)

/**
 * Copies data from circular buffer to linear buffer w/o updating r/w indexes.
 *
 * @param[out]     pPtrDest Linear buffer to copy data to.
 * @param[in, out] pSource  Ring buffer to copy data from.
 * @param          pSize    Number of slots to empty.
 */
#define RingBuffer_Empty(pPtrDest, pSource, pSize)                                                             \
{                                                                                                              \
    uint32_t used;                                                                                             \
    uint32_t temp;                                                                                             \
    uint32_t size;                                                                                             \
    RingBuffer_Protect (RING_BUFFER_LOCK, (pSource).protect.ptrCallback, (pSource).protect.ptrArg);            \
    used = RingBuffer_GetUsed ((pSource));                                                                     \
    RingBuffer_Assert ((pSize) <= used);                                                                       \
    if ((pSize) > used)                                                                                        \
    {                                                                                                          \
        size = used;                                                                                           \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
        size = (pSize);                                                                                        \
    }                                                                                                          \
    if (size >= (RingBuffer_GetLength ((pSource)) - (pSource).tail))                                           \
    {                                                                                                          \
        temp = (RingBuffer_GetLength ((pSource)) - (pSource).tail);                                            \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
        temp = size;                                                                                           \
    }                                                                                                          \
    (void) memcpy ((pPtrDest), &(pSource).array[(pSource).tail], (temp * RingBuffer_GetTypeSize ((pSource)))); \
    (void) memcpy (&((uint8_t*) (pPtrDest))[(temp * RingBuffer_GetTypeSize ((pSource)))],                      \
                   ((uint8_t*) (pSource).array),                                                                            \
                   (size - temp) * RingBuffer_GetTypeSize ((pSource)));                                        \
    RingBuffer_IncrementTail ((pSource), (pSize));                                                             \
    RingBuffer_Protect (RING_BUFFER_UNLOCK, (pSource).protect.ptrCallback, (pSource).protect.ptrArg);          \
}

/**
 * Pushes single item into ring buffer.
 *
 * @warning In case ring buffer is full, oldest data is discarded.
 *
 * @param pName Ring buffer to push to.
 * @param pItem Item to push.
 */
#define RingBuffer_Push(pName, pItem)                                                             \
{                                                                                                 \
    RingBuffer_Protect (RING_BUFFER_LOCK, (pName).protect.ptrCallback, (pName).protect.ptrArg);   \
    (pName).array[(pName).head] = (pItem);                                                        \
    RingBuffer_IncrementHead ((pName), 1UL);                                                      \
    RingBuffer_Protect (RING_BUFFER_UNLOCK, (pName).protect.ptrCallback, (pName).protect.ptrArg); \
}

/**
 * Pops single item from ring buffer.
 * *
 * @param pName Ring buffer to push to.
 * @param pItem Item to copy to the popped item.
 */
#define RingBuffer_Pop(pName, pItem)                                                              \
{                                                                                                 \
    RingBuffer_Protect (RING_BUFFER_LOCK, (pName).protect.ptrCallback, (pName).protect.ptrArg);   \
    RingBuffer_Assert (RingBuffer_IsEmpty ((pName)) == 0UL);                                      \
    (pItem) = (pName).array[(pName).tail];                                                        \
    RingBuffer_IncrementTail ((pName), 1UL);                                                      \
    RingBuffer_Protect (RING_BUFFER_UNLOCK, (pName).protect.ptrCallback, (pName).protect.ptrArg); \
}

/**
 * Copies data from linear buffer to circular buffer.
 *
 * @param     pDest      Ring buffer to push to.
 * @param[in] pPtrSource Linear buffer to copy data from.
 * @param     pSize      Number of slots to fill.
 */
#define RingBuffer_Fill(pDest, pPtrSource, pSize)                                                  \
{                                                                                                  \
    uint32_t size;                                                                                 \
    RingBuffer_Assert ((pSize) < (RingBuffer_GetLength ((pDest)) - 1UL));                          \
    RingBuffer_Protect (RING_BUFFER_LOCK, (pDest).protect.ptrCallback, (pDest).protect.ptrArg);    \
    if ((pSize) >= (RingBuffer_GetLength ((pDest)) - (pDest).head))                                \
    {                                                                                              \
        size = (RingBuffer_GetLength ((pDest)) - (pDest).head);                                    \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        size = (pSize);                                                                            \
    }                                                                                              \
    (void) memcpy (&(pDest).array[((pDest).head * RingBuffer_GetTypeSize ((pDest)))], \
                   (pPtrSource),                                                                   \
                   (size * RingBuffer_GetTypeSize ((pDest))));                                     \
    (void) memcpy (((uint8_t*) (pDest).array),                                                     \
                   &((uint8_t*) (pPtrSource))[(size * RingBuffer_GetTypeSize ((pDest)))],          \
                   (((pSize) - size) * RingBuffer_GetTypeSize ((pDest))));                         \
    RingBuffer_IncrementHead ((pDest), (pSize));                                                   \
    RingBuffer_Protect (RING_BUFFER_UNLOCK, (pDest).protect.ptrCallback, (pDest).protect.ptrArg);  \
}

/* ***************************************************************************************************************** *
 *                                              Global variables section                                             *
 * ***************************************************************************************************************** */

/* ***************************************************************************************************************** *
 *                                              Global functions section                                             *
 * ***************************************************************************************************************** */

#endif /* RING_BUFFER_H__ */
