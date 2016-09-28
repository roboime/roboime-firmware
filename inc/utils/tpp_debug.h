
#include "stdio.h"

/** lower two bits indicate debug level
 * - 0 all
 * - 1 warning
 * - 2 serious
 * - 3 severe
 */
#define TPP_DBG_LEVEL_ALL     0x00
#define TPP_DBG_LEVEL_OFF     TPP_DBG_LEVEL_ALL /* compatibility define only */
#define TPP_DBG_LEVEL_WARNING 0x01 /* bad checksums, dropped packets, ... */
#define TPP_DBG_LEVEL_SERIOUS 0x02 /* memory allocation failures, ... */
#define TPP_DBG_LEVEL_SEVERE  0x03
#define TPP_DBG_MASK_LEVEL    0x03

/** flag for TPP_DEBUGF to enable that debug message */
#define TPP_DBG_ON            0x80U
/** flag for TPP_DEBUGF to disable that debug message */
#define TPP_DBG_OFF           0x00U

/** flag for TPP_DEBUGF indicating a tracing message (to follow program flow) */
#define TPP_DBG_TRACE         0x40U
/** flag for TPP_DEBUGF indicating a state debug message (to follow module states) */
#define TPP_DBG_STATE         0x20U
/** flag for TPP_DEBUGF indicating newly added code, not thoroughly tested yet */
#define TPP_DBG_FRESH         0x10U
/** flag for TPP_DEBUGF to halt after printing this debug message */
#define TPP_DBG_HALT          0x08U


/* ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/
/**
 * TPP_DBG_MIN_LEVEL: After masking, the value of the debug is
 * compared against this value. If it is smaller, then debugging
 * messages are written.
 */
#ifndef TPP_DBG_MIN_LEVEL
#define TPP_DBG_MIN_LEVEL              TPP_DBG_LEVEL_ALL
#endif

/**
 * TPP_DBG_TYPES_ON: A mask that can be used to globally enable/disable
 * debug messages of certain types.
 */
#ifndef TPP_DBG_TYPES_ON
#define TPP_DBG_TYPES_ON               TPP_DBG_ON
#endif


/* Plaform specific diagnostic output */
#define TPP_PLATFORM_DIAG2(...) printf(__VA_ARGS__)
#define TPP_PLATFORM_DIAG(x) TPP_PLATFORM_DIAG2 x
#include <stm32f4xx.h>



#define TPP_DEBUG TPP_DBG_ON
#define SI446X_DEBUG TPP_DBG_OFF
#define PACKER_DEBUG TPP_DBG_OFF
#define RADIO_DEBUG TPP_DBG_OFF
#define RNDIS_DEBUG TPP_DBG_OFF


#if TPP_DEBUG
#define TPP_DEBUGF(debug, message) \
		if(((ITM->TCR & ITM_TCR_ITMENA_Msk)&&(ITM->TER & (1UL << 0)))) { \
			if (((debug) & TPP_DBG_ON) && ((debug) & TPP_DBG_TYPES_ON) && (((debug) & TPP_DBG_MASK_LEVEL) >= TPP_DBG_MIN_LEVEL)) { \
				TPP_PLATFORM_DIAG(message); \
				if ((debug) & TPP_DBG_HALT) { \
					while(1); \
				} \
			} \
		}
#else
#define TPP_DEBUGF(debug, message)
#endif
