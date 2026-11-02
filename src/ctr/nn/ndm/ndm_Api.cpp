/**
 * @file ndm_Api.cpp
 * @brief Nintendo 3DS Network Daemon Manager (`nn::ndm`) implementation.
 */

#include <nn/ndm/ndm_Api.h>

extern "C" {

/**
 * @brief func_00127EE0
 * @address 0x00127EE0
 * nn::ndm global state offset reader via literal pool (0x00865F18)
 */
void *func_00127EE0(void)
{
    return (void *)&unk_00865F18;
}

/**
 * @brief func_00127EEC
 * @address 0x00127EEC
 * nn::ndm global state pointer getter via literal pool (0x00865F10)
 */
void *func_00127EEC(void)
{
    return &unk_00865F10;
}

/**
 * @brief func_001280A8
 * @address 0x001280A8
 * nn::ndm bounds-checked struct array element indexer (stride 8, offset +0x0C)
 */
NdmElement8 *func_001280A8(NdmContainer *ctx, u32 index)
{
    if (ctx->count <= index)
    {
        return NULL;
    }
    return &ctx->items[index];
}

} /* extern "C" */
