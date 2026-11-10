#ifndef GAME_LOOPS_H
#define GAME_LOOPS_H

#include "types.h"
#include "engine/lfw.h"

/* ========================================================================
 * Yo-kai Watch 2 (yw2) — Game Loop Subclasses (ywCLoop*)
 *
 * Registered in the Level-5 loop descriptor table (0x00107478):
 *   - ywCLoopMenu        (0x008604A3)
 *   - lfwCLoopEmpty      (0x008602F8)
 *   - ywCLoopMain        (0x00860497)
 *   - ywCLoopRPG         (0x0086048C)
 *   - ywCLoopEvent       (0x008604BB)
 *   - ywCLoopBattle      (0x008604C8)
 *   - ywCLoopWatchmap    (0x00860556)
 *   - ywCLoopYokaispot   (0x008605AA)
 * Standalone loop classes:
 *   - ywCLoopRace        (0x008604AF)
 *   - ywCLoopCapsule     (0x008604D9)
 *   - ywCLoopOrgeTime    (0x00860546)
 *   - ywCLoopQR          (0x00860794)
 * ======================================================================== */

#ifdef __cplusplus

class ywCLoopRPG
{
public:
        static s32 getClassType();          /* 0x0010CEE8 */
        static const char *getClassName();  /* 0x0010CEF0 */
};

class ywCLoopMain
{
public:
        static s32 getClassType();          /* 0x0010CFF4 */
        static const char *getClassName();  /* 0x0010CFFC */
};

class ywCLoopMenu
{
public:
        static s32 getClassType();          /* 0x0010D134 */
        static const char *getClassName();  /* 0x0010D13C */
};

class ywCLoopRace
{
public:
        static s32 getClassType();          /* 0x0010D234 */
        static const char *getClassName();  /* 0x0010D23C */
};

class ywCLoopEvent
{
public:
        static s32 getClassType();          /* 0x0010D3A8 */
        static const char *getClassName();  /* 0x0010D3B0 */
};

class ywCLoopBattle
{
public:
        static s32 getClassType();          /* 0x0010D500 */
        static const char *getClassName();  /* 0x0010D508 */
};

class ywCLoopCapsule
{
public:
        static s32 getClassType();          /* 0x0010D63C */
        static const char *getClassName();  /* 0x0010D644 */
};

class ywCLoopOrgeTime
{
public:
        static s32 getClassType();          /* 0x0010DC40 */
        static const char *getClassName();  /* 0x0010DC48 */
};

class ywCLoopWatchmap
{
public:
        static s32 getClassType();          /* 0x0010DD40 */
        static const char *getClassName();  /* 0x0010DD48 */
};

class ywCLoopYokaispot
{
public:
        static s32 getClassType();          /* 0x0010E35C */
        static const char *getClassName();  /* 0x0010E364 */
};

class ywCLoopQR
{
public:
        static s32 getClassType();          /* 0x0010FEBC */
        static const char *getClassName();  /* 0x0010FEC4 */
};

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif

/* Loop Controller Globals */
extern s32 unk_0086C8CC;

typedef struct YwLoopControllerState
{
        u8    enabled;       /* +0x00: Flag booleano (1 = activo) */
        u8    mode;          /* +0x01: Flag / modo de control */
        u8    pad_02[0x02];  /* +0x02 - +0x03: Alineación a 4 bytes */
        void *target;        /* +0x04: Puntero de destino / contexto */
        void *callback;      /* +0x08: Puntero callback guardado por func_0010C870 */
        u32   pad_0C;        /* +0x0C - +0x0F: Alineación final */
} YwLoopControllerState;

extern YwLoopControllerState g_LoopState_00815C90;

void *func_0010C894(void);            /* 0x0010C894 */
void  func_0010C870(void *callback);  /* 0x0010C870 */
void  func_0010C880(void);            /* 0x0010C880 */
void  func_0010C8A0(u8 mode);         /* 0x0010C8A0 */
void  func_0010CA68(void *target);    /* 0x0010CA68 */

typedef struct YwLoopHeader
{
        union
        {
                struct
                {
                        u8 status;
                        u8 pad[3];
                };
                u32 unk_00;
        };
        u32 value;
} YwLoopHeader;

void func_0011054C(YwLoopHeader *ywLoopHeader); /* 0x0011054C */

#ifdef __cplusplus
}
#endif

#endif /* GAME_LOOPS_H */
