#ifndef _MS_CPU_H_
#define _MS_CPU_H_

#define FIFO_ADDRESS(e)  (otgRegAddress + (e<<3) + M_FIFO_EP0)
#define FIFO_DATA_PORT  (otgRegAddress+M_REG_FIFO_DATA_PORT)

#ifdef WORD_SWOP
#define SWOP(X) ((X) = (((X)<<8)+((X)>>8)))
#else
#define SWOP(X)  (X = X)
#endif

#endif  /* _MS_CPU_H_ */
