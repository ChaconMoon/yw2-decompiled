#ifndef CTR_NEX_H
#define CTR_NEX_H

#include "types.h"

#ifdef __cplusplus

namespace nn
{

        namespace nex
        {
                class StationConnectionInfo;

                class NexElement
                {
                        friend class StationConnectionInfo;

                public:
                        /* nn::nex leaf functions */

                        // Function 00340CE8 (Operator asignament for getter of NexElement)
                        NexElement &operator=(const NexElement &other); /* 0x00340CE8 */

                        // Function 00340CFC (Constructor with params of NexElement)
                        NexElement(s32 arg1, s16 arg2);                 /* 0x00340CFC */

                        // Function 00340D08 (Constructor of NexElement)
                        NexElement();                                   /* 0x00340D08 */

                        // Function 00340D18 (Destructor of NexElement)
                        ~NexElement();                                  /* 0x00340D18 */

                        // Function 00340D1C (Constructor with copy of NexElement)
                        NexElement(const NexElement &other);            /* 0x00340D1C */

                private:
                        s32 unk_01;
                        s16 unk_02;
                };

                struct ResultData
                {
                        u32 unk_01;
                        u32 unk_02;
                };

                class qResult
                {
                public:
                        // Function 0031F1E8 (Operator asignament for getter of qResult)
                        qResult &operator=(const qResult &other); /* 0x0031F1E8 */

                        // Function 0031F1F4 (default constructor of qResult)
                        qResult();                                /* 0x0031F1F4 */

                        // Function 0031F204 (Constructor with copy of qResult)
                        qResult(const qResult &other);            /* 0x0031F204 */

                        // Function 0031f210 (Getter / Conversion?)
                        u64 getValue() const;                     /* 0x0031F210 */

                private:
                        ResultData m_data;
                };

                class StationConnectionInfo
                {
                public:
                        // Function 00340ECC (Resest connection and sets all values at zero)
                        void reset();                                         /* 0x00340ECC */

                        // Function 00340EAC (set Port)
                        u32 setPort(u16 port);                                /* 0x00340EAC */

                        // Function 00340EB8 (set Element of the connection)
                        u32 setElement(const NexElement &element);            /* 0x00340EB8 */

                        // Function 00340E5C (deserialize object)
                        u32 deserialize(const u8 *buf);                       /* 0x00340E5C */

                        // Function 00340EE8 (Three-way comparison returning -1, 0, or 1)
                        s32 compare(const StationConnectionInfo &other) const; /* 0x00340EE8 */

                private:
                        s32 unk_00;
                        NexElement m_element;
                        u16 m_port;
                };
        }
}
extern "C"
{
#else
typedef struct NexElement
{
        s32 unk_01;
        s16 unk_02;
} NexElement;

typedef struct ResultData
{
        u32 unk_01;
        u32 unk_02;
} ResultData;

typedef struct qResult
{
        ResultData m_data;
} qResult;

typedef struct StationConnectionInfo
{
        s32 unk_00;
        NexElement m_element;
        u16 m_port;
} StationConnectionInfo;
#endif
#ifdef __cplusplus
        void func_00301248(void);                                              /* 0x00301248 */
        s32 func_00340C9C(nn::nex::NexElement *element, const void *buf);      /* 0x00340C9C */
        u16 func_00341368(const void *buf);                                    /* 0x00341368 */
        u32 func_00341374(const void *buf);                                    /* 0x00341374 */
        s64 func_0063A6E8(const nn::nex::NexElement *element);                 /* 0x0063A6E8 */
#else
        void func_00301248(void);                                              /* 0x00301248 */
        s32 func_00340C9C(NexElement *element, const void *buf);               /* 0x00340C9C */
        u16 func_00341368(const void *buf);                                    /* 0x00341368 */
        u32 func_00341374(const void *buf);                                    /* 0x00341374 */
        s64 func_0063A6E8(const NexElement *element);                          /* 0x0063A6E8 */
#endif

#ifdef __cplusplus
}
#endif

#endif /* CTR_NEX_H */
