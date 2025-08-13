#ifndef     ISR_H
#define     ISR_H

#ifdef _ISR_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

#define ISR_TIMER0_INTERVAL     1 // unit: 1ms
INTERFACE   void ms1MsHandler(void);
INTERFACE   void ISR_R2IRQ_NONPM_IRQ(void); // R2 IRQ #46, source is PM_IRQ
INTERFACE   void ISR_R2IRQ_AUX_IRQ(void);
INTERFACE   void ISR_R2IRQ_NONPM_FIQ(void); // R2 FIQ #2, source is PM_FIQ
INTERFACE   void ISR_R2IRQ_PM_CEC_IRQ(void);
INTERFACE   void ISR_U3_RT(void);
INTERFACE   void ISR_U3_WAKEUP(void);
INTERFACE   void MuteVideoAndAudio(void);
#if (CHIP_ID == CHIP_MT9701)
INTERFACE   void ISR_R2FIQ_PM_EXT_TIMER0(void); // R2 FIQ #0
INTERFACE   void ISR_R2IRQ_PM_SCDC_IRQ(void);
#endif
#endif //#ifndef     ISR_H

