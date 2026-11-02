/**
 * @file y2r_Y2r.h
 * @brief Nintendo 3DS YUV-to-RGB Hardware Conversion (`nn::y2r::CTR`).
 */
#ifndef NN_Y2R_CTR_Y2R_Y2R_H_
#define NN_Y2R_CTR_Y2R_Y2R_H_

#include "ctr/y2r.h"

#ifdef __cplusplus
extern "C" {
#endif

void func_00112AA8(Y2rDevice *dev);
void *func_00112D8C(void *self);
u16 func_00112E60(const Y2rObject *y2rObject);
void func_00112EC8(void);
void func_00112EE4(u32 arg0);

#ifdef __cplusplus
}
#endif

#endif /* NN_Y2R_CTR_Y2R_Y2R_H_ */
