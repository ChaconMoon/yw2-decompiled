/**
 * @file fnd.h
 * @brief Nintendo CTR-SDK Foundation Library (`nn::fnd`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"

#ifdef __cplusplus

namespace nn {
namespace fnd {

    /* Intrusive List Link Node */
    struct Link {
        void* prev;
        void* next;

        Link() : prev(NULL), next(NULL) {}
    };

    /* Intrusive Doubly-Linked List */
    class List {
    private:
        void* m_Head;
        void* m_Tail;
        u16   m_Count;
        u16   m_Offset;

    public:
        List() : m_Head(NULL), m_Tail(NULL), m_Count(0), m_Offset(0) {}

        void  Initialize(u16 offset) {
            m_Head = NULL;
            m_Tail = NULL;
            m_Count = 0;
            m_Offset = offset;
        }

        void* GetHead() const { return m_Head; }
        void* GetTail() const { return m_Tail; }
        u16   GetSize() const { return m_Count; }
        bool  IsEmpty() const { return m_Count == 0; }

        void  Append(void* object);
        void  Prepend(void* object);
        void  Remove(void* object);
    };

    /* Heap Base & Expanded Heap */
    class Heap {
    public:
        virtual ~Heap();
        virtual void* Allocate(u32 size, s32 alignment) = 0;
        virtual void  Free(void* ptr) = 0;
        virtual u32   GetTotalFreeSize() const = 0;
        virtual u32   GetAllocatableSize(s32 alignment) const = 0;
    };

    class ExpHeap : public Heap {
    public:
        static ExpHeap* Create(void* startAddress, u32 size, u16 optFlag);
        virtual ~ExpHeap();
        virtual void* Allocate(u32 size, s32 alignment);
        virtual void  Free(void* ptr);
        virtual u32   GetTotalFreeSize() const;
        virtual u32   GetAllocatableSize(s32 alignment) const;
        void  Destroy();
    };

    class FrameHeap : public Heap {
    public:
        static FrameHeap* Create(void* startAddress, u32 size, u16 optFlag);
        virtual ~FrameHeap();
        virtual void* Allocate(u32 size, s32 alignment);
        virtual void  Free(void* ptr);
        virtual u32   GetTotalFreeSize() const;
        virtual u32   GetAllocatableSize(s32 alignment) const;
        void  Destroy();
    };

    class UnitHeap : public Heap {
    public:
        static UnitHeap* Create(void* startAddress, u32 size, u32 blockSize, s32 alignment, u16 optFlag);
        virtual ~UnitHeap();
        virtual void* Allocate(u32 size, s32 alignment);
        virtual void  Free(void* ptr);
        virtual u32   GetTotalFreeSize() const;
        virtual u32   GetAllocatableSize(s32 alignment) const;
        void  Destroy();
    };

    /* Time & Date */
    struct DateTime {
        s16 year;
        s8  month;
        s8  day;
        s8  hour;
        s8  minute;
        s8  second;
        s16 millisecond;
    };

} // namespace fnd
} // namespace nn

#else /* !__cplusplus (Pure C) */

typedef struct nn_fnd_Link {
    void* prev;
    void* next;
} nn_fnd_Link;

typedef struct nn_fnd_List {
    void* m_Head;
    void* m_Tail;
    u16   m_Count;
    u16   m_Offset;
} nn_fnd_List;

typedef struct nn_fnd_DateTime {
    s16 year;
    s8  month;
    s8  day;
    s8  hour;
    s8  minute;
    s8  second;
    s16 millisecond;
} nn_fnd_DateTime;

#endif /* __cplusplus */
