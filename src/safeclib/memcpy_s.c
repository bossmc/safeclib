/*------------------------------------------------------------------
 * memcpy_s
 *
 * October 2008, Bo Berry
 *
 * Copyright (c) 2008-2011 Cisco Systems
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *------------------------------------------------------------------
 */

#include "safeclib_private.h"
#include "safe_mem_constraint.h"
#include "mem_primitives_lib.h"
#include "safe_mem_lib.h"


/**
 * @brief 
 *    This function copies at most smax bytes from src to dest, up to
 *    dmax.
 *
 * @remark SPECIFIED IN
 *    ISO/IEC JTC1 SC22 WG14 N1172, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * @param[out] dest  pointer to the memory that will be replaced by src.
 * @param[in]  dmax  maximum length of the resulting dest, in bytes
 * @param[in]  src   pointer to the memory that will be copied to dest
 * @param[in]  smax  maximum number bytes of src that can be copied
 *
 * @pre  Neither dest nor src shall be a null pointer.
 * @pre  Neither dmax nor smax shall be 0.
 * @pre  dmax shall not be greater than RSIZE_MAX_MEM.
 * @pre  smax shall not be greater than dmax.
 * @pre  Copying shall not take place between regions that overlap.
 *    
 * @return  If there is a runtime-constraint violation, the memcpy_s function
 *          stores zeros in the ﬁrst dmax bytes of the region pointed to
 *          by dest if dest is not a null pointer and smax is valid.
 * @retval  EOK         when operation is successful
 * @retval  ESNULLP     when dest/src is NULL POINTER
 * @retval  ESZEROL     when dmax/smax = ZERO
 * @retval  ESLEMAX     when dmax > RSIZE_MAX_MEM
 * @retval  ESNOSPC     when dmax < smax
 * @retval  ESOVRLP     when src memory overlaps dst
 *
 * @see 
 *    memcpy16_s(), memcpy32_s(), memmove_s(), memmove16_s(), memmove32_s()
 *
 */
errno_t
memcpy_s (void * restrict dest, rsize_t dmax, const void * restrict src, rsize_t smax)
{
    uint8_t *dp;
    const uint8_t  *sp;

    dp = (uint8_t*) dest;
    sp = (uint8_t*) src;

    if (dp == NULL) {
        invoke_safe_mem_constraint_handler("memcpy_s: dest is NULL",
                   NULL, ESNULLP);
        return RCNEGATE(ESNULLP);
    }

    if (dmax == 0) {
        invoke_safe_mem_constraint_handler("memcpy_s: dmax is 0",
                   NULL, ESZEROL);
        return RCNEGATE(ESZEROL);
    }

    if (dmax > RSIZE_MAX_MEM) {
        invoke_safe_mem_constraint_handler("memcpy_s: dmax exceeds max",
                   NULL, ESLEMAX);
        return RCNEGATE(ESLEMAX);
    }

    if (smax == 0) {
        mem_prim_set(dp, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy_s: smax is 0",
                   NULL, ESZEROL);
        return RCNEGATE(ESZEROL);
    }

    if (smax > dmax) {
        mem_prim_set(dp, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy_s: smax exceeds dmax",
                   NULL, ESNOSPC);
        return RCNEGATE(ESNOSPC);
    }

    if (sp == NULL) {
        mem_prim_set(dp, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy_s: src is NULL",
                   NULL, ESNULLP);
        return RCNEGATE(ESNULLP);
    }


    /*
     * overlap is undefined behavior, do not allow
     */
    if( ((dp > sp) && (dp < (sp+smax))) ||
        ((sp > dp) && (sp < (dp+dmax))) ) {
        mem_prim_set(dp, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy_s: overlap undefined",
                   NULL, ESOVRLP);
        return RCNEGATE(ESOVRLP);
    }

    /*
     * now perform the copy
     */
    mem_prim_move(dp, sp, smax);

    return RCNEGATE(EOK);
}
EXPORT_SYMBOL(memcpy_s)
