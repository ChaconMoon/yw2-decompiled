#include "ctr/dsp.h"

/**
 * @brief Destroys and cleans up a DSP object's child interface.
 *
 * Inspects the tagged child pointer at offset +0x08. If non-null, checks the tag
 * bit (bit 0) for error reporting via func_001371D4, invokes the virtual release
 * method func_00136420 with the untagged pointer, resets the child pointer to null,
 * and returns the object pointer.
 *
 * Address: 0x00136330
 * Size:    60 bytes
 *
 * @param self Pointer to the DspObject_00136318 instance.
 * @return DspObject_00136318* Pointer to the same instance (self).
 */
DspObject_00136318* func_00136330(DspObject_00136318 *self)
{
        uintptr_t *pChild = &self->child;
        uintptr_t  val = *pChild;

        if ((val & ~1) != 0)
        {
                if ((val & 1) != 0)
                {
                        func_001371D4();
                }

                func_00136420((DspInterface_00136420 *)(*pChild & ~1));
                *pChild = 0;
        }

        return self;
}
