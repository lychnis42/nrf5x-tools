#ifndef NRF_NVIC_H__
#define NRF_NVIC_H__ 
#include <stdint.h>
#include "nrf.h"
#include "nrf_error_soc.h"
#ifdef __cplusplus
extern "C" {
#endif
#define __NRF_NVIC_NVMC_IRQn (30)
#ifdef NRF51
  #define __NRF_NVIC_ISER_COUNT (1)
  #define __NRF_NVIC_SD_IRQS_0 ((uint32_t)( \
        (1U << POWER_CLOCK_IRQn) \
      | (1U << RADIO_IRQn) \
      | (1U << RTC0_IRQn) \
      | (1U << TIMER0_IRQn) \
      | (1U << RNG_IRQn) \
      | (1U << ECB_IRQn) \
      | (1U << CCM_AAR_IRQn) \
      | (1U << TEMP_IRQn) \
      | (1U << __NRF_NVIC_NVMC_IRQn) \
      | (1U << (uint32_t)SWI4_IRQn) \
      | (1U << (uint32_t)SWI5_IRQn) \
    ))
  #define __NRF_NVIC_APP_IRQS_0 (~__NRF_NVIC_SD_IRQS_0)
#endif
#ifdef NRF52
  #define __NRF_NVIC_ISER_COUNT (2)
  #define __NRF_NVIC_SD_IRQS_0 ((uint32_t)( \
        (1U << POWER_CLOCK_IRQn) \
      | (1U << RADIO_IRQn) \
      | (1U << RTC0_IRQn) \
      | (1U << TIMER0_IRQn) \
      | (1U << RNG_IRQn) \
      | (1U << ECB_IRQn) \
      | (1U << CCM_AAR_IRQn) \
      | (1U << TEMP_IRQn) \
      | (1U << __NRF_NVIC_NVMC_IRQn) \
      | (1U << (uint32_t)SWI4_EGU4_IRQn) \
      | (1U << (uint32_t)SWI5_EGU5_IRQn) \
    ))
  #define __NRF_NVIC_SD_IRQS_1 ((uint32_t)0)
  #define __NRF_NVIC_APP_IRQS_0 (~__NRF_NVIC_SD_IRQS_0)
  #define __NRF_NVIC_APP_IRQS_1 (~__NRF_NVIC_SD_IRQS_1)
#endif
typedef struct
{
  uint32_t volatile __irq_masks[__NRF_NVIC_ISER_COUNT];
  uint32_t volatile __cr_flag;
} nrf_nvic_state_t;
extern nrf_nvic_state_t nrf_nvic_state;
static inline int __sd_nvic_irq_disable(void)
{
  int pm = __get_PRIMASK();
  __disable_irq();
  return pm;
}
static inline void __sd_nvic_irq_enable(void)
{
  __enable_irq();
}
static inline uint32_t __sd_nvic_app_accessible_irq(IRQn_Type IRQn)
{
  if (IRQn < 32)
  {
    return ((1UL<<IRQn) & __NRF_NVIC_APP_IRQS_0) != 0;
  }
#ifdef NRF52
  else if (IRQn < 64)
  {
    return ((1UL<<(IRQn-32)) & __NRF_NVIC_APP_IRQS_1) != 0;
  }
#endif
  else
  {
    return 1;
  }
}
static inline uint32_t __sd_nvic_is_app_accessible_priority(uint32_t priority)
{
  if(priority >= (1 << __NVIC_PRIO_BITS))
  {
    return 0;
  }
#ifdef NRF51
  if( priority == 0
     || priority == 2
     )
  {
    return 0;
  }
#endif
#ifdef NRF52
  if( priority == 0
     || priority == 1
     || priority == 4
     || priority == 5
     )
  {
    return 0;
  }
#endif
  return 1;
}
static inline uint32_t sd_nvic_EnableIRQ(IRQn_Type IRQn)
{
  if (!__sd_nvic_app_accessible_irq(IRQn))
  {
    return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
  }
  if (!__sd_nvic_is_app_accessible_priority(NVIC_GetPriority(IRQn)))
  {
    return NRF_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED;
  }
  if (nrf_nvic_state.__cr_flag)
  {
    nrf_nvic_state.__irq_masks[(uint32_t)((int32_t)IRQn) >> 5] |= (uint32_t)(1 << ((uint32_t)((int32_t)IRQn) & (uint32_t)0x1F));
  }
  else
  {
    NVIC_EnableIRQ(IRQn);
  }
  return NRF_SUCCESS;
}
static inline uint32_t sd_nvic_DisableIRQ(IRQn_Type IRQn)
{
  if (!__sd_nvic_app_accessible_irq(IRQn))
  {
    return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
  }
  if (nrf_nvic_state.__cr_flag)
  {
    nrf_nvic_state.__irq_masks[(uint32_t)((int32_t)IRQn) >> 5] &= ~(1UL << ((uint32_t)(IRQn) & 0x1F));
  }
  else
  {
    NVIC_DisableIRQ(IRQn);
  }
  return NRF_SUCCESS;
}
static inline uint32_t sd_nvic_GetPendingIRQ(IRQn_Type IRQn, uint32_t * p_pending_irq)
{
  if (__sd_nvic_app_accessible_irq(IRQn))
  {
    *p_pending_irq = NVIC_GetPendingIRQ(IRQn);
    return NRF_SUCCESS;
  }
  else
  {
    return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
  }
}
static inline uint32_t sd_nvic_SetPendingIRQ(IRQn_Type IRQn)
{
  if (__sd_nvic_app_accessible_irq(IRQn))
  {
    NVIC_SetPendingIRQ(IRQn);
    return NRF_SUCCESS;
  }
  else
  {
    return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
  }
}
static inline uint32_t sd_nvic_ClearPendingIRQ(IRQn_Type IRQn)
{
  if (__sd_nvic_app_accessible_irq(IRQn))
  {
    NVIC_ClearPendingIRQ(IRQn);
    return NRF_SUCCESS;
  }
  else
  {
    return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
  }
}
static inline uint32_t sd_nvic_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if (!__sd_nvic_app_accessible_irq(IRQn))
  {
    return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
  }
  if (!__sd_nvic_is_app_accessible_priority(priority))
  {
    return NRF_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED;
  }
  NVIC_SetPriority(IRQn, (uint32_t)priority);
  return NRF_SUCCESS;
}
static inline uint32_t sd_nvic_GetPriority(IRQn_Type IRQn, uint32_t * p_priority)
{
  if (__sd_nvic_app_accessible_irq(IRQn))
  {
    *p_priority = (NVIC_GetPriority(IRQn) & 0xFF);
    return NRF_SUCCESS;
  }
  else
  {
    return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
  }
}
static inline uint32_t sd_nvic_SystemReset(void)
{
  NVIC_SystemReset();
  return NRF_ERROR_SOC_NVIC_SHOULD_NOT_RETURN;
}
static inline uint32_t sd_nvic_critical_region_enter(uint8_t * p_is_nested_critical_region)
{
  int was_masked = __sd_nvic_irq_disable();
  if (!nrf_nvic_state.__cr_flag)
  {
    nrf_nvic_state.__cr_flag = 1;
    nrf_nvic_state.__irq_masks[0] = ( NVIC->ICER[0] & __NRF_NVIC_APP_IRQS_0 );
    NVIC->ICER[0] = __NRF_NVIC_APP_IRQS_0;
    #ifdef NRF52
    nrf_nvic_state.__irq_masks[1] = ( NVIC->ICER[1] & __NRF_NVIC_APP_IRQS_1 );
    NVIC->ICER[1] = __NRF_NVIC_APP_IRQS_1;
    #endif
    *p_is_nested_critical_region = 0;
  }
  else
  {
    *p_is_nested_critical_region = 1;
  }
  if (!was_masked)
  {
    __sd_nvic_irq_enable();
  }
  return NRF_SUCCESS;
}
static inline uint32_t sd_nvic_critical_region_exit(uint8_t is_nested_critical_region)
{
  if (nrf_nvic_state.__cr_flag && (is_nested_critical_region == 0))
  {
    int was_masked = __sd_nvic_irq_disable();
    NVIC->ISER[0] = nrf_nvic_state.__irq_masks[0];
    #ifdef NRF52
    NVIC->ISER[1] = nrf_nvic_state.__irq_masks[1];
    #endif
    nrf_nvic_state.__cr_flag = 0;
    if (!was_masked)
    {
      __sd_nvic_irq_enable();
    }
  }
  return NRF_SUCCESS;
}
#ifdef __cplusplus
}
#endif
#endif
