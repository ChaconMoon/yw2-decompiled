#include "ctr/dsp.h"

/**
 * @brief Checks if a string starts with the DSP firmware prefix "FGONLYT".
 *
 * Computes the length of the reference prefix string s_FGONLYT_007132D4 ("FGONLYT")
 * via func_002FEEBC (strlen), and compares it against the input string using
 * func_0013EC88 (strncmp).
 *
 * Address: 0x001386EC
 * Size:    52 bytes (48 B code + 4 B literal pool)
 *
 * @param str Input null-terminated string to validate.
 * @return bool32 1 if the string matches prefix "FGONLYT", 0 otherwise.
 */
bool32 func_001386EC(const char *str)
{
        bool32 match = 0;
        u32    len = func_002FEEBC(s_FGONLYT_007132D4);

        if (func_0013EC88(str, s_FGONLYT_007132D4, len) == 0)
        {
                match = 1;
        }

        return match;
}
