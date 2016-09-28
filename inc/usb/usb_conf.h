#pragma once
#include "stm32f4xx.h"

#define USE_USB_OTG_FS
#define USE_EMBEDDED_PHY
  
#ifdef USE_USB_OTG_FS 
 #define USB_OTG_FS_CORE
 #define USB_OTG_CORE_ID USB_OTG_FS_CORE_ID
#endif

#ifdef USE_USB_OTG_HS 
 #define USB_OTG_HS_CORE
 #define USB_OTG_CORE_ID USB_OTG_HS_CORE_ID
#endif

#ifndef USB_SUPPORT_USER_STRING_DESC
#define USB_SUPPORT_USER_STRING_DESC
#endif

#ifdef USB_OTG_HS_CORE
 #define RX_FIFO_HS_SIZE                           	512
 #define TX0_FIFO_HS_SIZE                          	64
 #define TX1_FIFO_HS_SIZE                          	64
 #define TX2_FIFO_HS_SIZE                          	64
 #define TX3_FIFO_HS_SIZE                          	64
 #define TX4_FIFO_HS_SIZE                          	128
 #define TX5_FIFO_HS_SIZE                   		64
 #define TXH_NP_HS_FIFOSIZ							128
 #define TXH_P_HS_FIFOSIZ							128

 #ifdef USE_EMBEDDED_PHY 
   #define USB_OTG_EMBEDDED_PHY_ENABLED
   #define USB_OTG_HS_LOW_PWR_MGMT_SUPPORT
 #endif
 #ifdef USE_I2C_PHY
  #define USB_OTG_I2C_PHY_ENABLED
 #endif
 /* #define USB_OTG_HS_INTERNAL_DMA_ENABLED */ /* Be aware that enabling DMA mode will result in data being sent only by
                                                  multiple of 4 packet sizes. This is due to the fact that USB DMA does
                                                  not allow sending data from non word-aligned addresses.
                                                  For this specific application, it is advised to not enable this option
                                                  unless required. */
// #define USB_OTG_HS_DEDICATED_EP1_ENABLED
#endif

#ifdef USB_OTG_FS_CORE
 #define RX_FIFO_FS_SIZE                          64
 #define TX0_FIFO_FS_SIZE                         64
 #define TX1_FIFO_FS_SIZE                         64
 #define TX2_FIFO_FS_SIZE                         64
 #define TX3_FIFO_FS_SIZE                         32

 #define TXH_NP_FS_FIFOSIZ							128
 #define TXH_P_FS_FIFOSIZ							128
 //#define USB_OTG_FS_LOW_PWR_MGMT_SUPPORT
// #define USB_OTG_FS_SOF_OUTPUT_ENABLED
#endif

#define USE_HOST_MODE
#define USE_DEVICE_MODE


/****************** C Compilers dependant keywords ****************************/
/* In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */    
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined   (__GNUC__)        /* GNU Compiler */
    #define __ALIGN_END    __attribute__ ((aligned (4)))
    #define __ALIGN_BEGIN         
  #else                           
    #define __ALIGN_END
    #if defined   (__CC_ARM)      /* ARM Compiler */
      #define __ALIGN_BEGIN    __align(4)  
    #elif defined (__ICCARM__)    /* IAR Compiler */
      #define __ALIGN_BEGIN 
    #elif defined  (__TASKING__)  /* TASKING Compiler */
      #define __ALIGN_BEGIN    __align(4) 
    #endif /* __CC_ARM */  
  #endif /* __GNUC__ */ 
#else
  #define __ALIGN_BEGIN
  #define __ALIGN_END   
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* __packed keyword used to decrease the data type alignment to 1-byte */
#if defined (__CC_ARM)         /* ARM Compiler */
  #define __packed    __packed
#elif defined (__ICCARM__)     /* IAR Compiler */
  #define __packed    __packed
#elif defined   ( __GNUC__ )   /* GNU Compiler */                        
#ifdef __packed
#else
#define __packed    __attribute__ ((__packed__))
#endif
#elif defined   (__TASKING__)  /* TASKING Compiler */
  #define __packed    __unaligned
#endif /* __CC_ARM */



