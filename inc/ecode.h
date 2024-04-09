#ifndef ECODE_H
#define ECODE_H

/* Minimum addressable unit */
#define ECODE_CPU1_MAU		(1U)
#define ECODE_CPU2_MAU		(2U)

#if !defined(NO_HARDWARE_BUILD)
#define ECODE_SHM_SIZE_BYTES		(64U)
#else
#define ECODE_SHM_SIZE_BYTES		(14U)
#endif
#define ECODE_SHM_RWIDX_OFF_BYTES	(0U)
#define ECODE_SHM_LOCK_OFF_BYTES	(2U)
#define ECODE_SHM_START_OFF_BYTES	(4U)
#define ECODE_SHM_DATA_SIZE_BYTES	(ECODE_SHM_SIZE_BYTES - ECODE_SHM_START_OFF_BYTES)

#define SPIN_LOCK_SHM(cb) 		({							\
						while(*(volatile unsigned char *)cb->lock);	\
						*(volatile unsigned char *)cb->lock = 1U;	\
					})

#define SPIN_UNLOCK_SHM(cb)		({							\
						*(volatile unsigned char *)cb->lock = 0U;	\
					})

#define ECODE_GET_READ_OFFSET(cb)	({								\
						unsigned char result = 0;				\
						SPIN_LOCK_SHM(cb);					\
						result = (*(unsigned short *)(cb->rwidx) & 0xFF);	\
						SPIN_UNLOCK_SHM(cb);					\
						result;							\
					})

#define ECODE_GET_WRITE_OFFSET(cb)	({									\
						unsigned char result = 0;					\
						SPIN_LOCK_SHM(cb);						\
						result = ((*(unsigned short *)(cb->rwidx) >> 8U) & 0xFF);	\
						SPIN_UNLOCK_SHM(cb);						\
						result;								\
					})

#define ECODE_SET_READ_OFFSET(cb, ofst)		({									\
							SPIN_LOCK_SHM(cb);						\
							*(unsigned short *)(cb->rwidx) =				\
							((*(unsigned short *)(cb->rwidx) & 0xFF00) | ((ofst) & 0xFF));	\
							SPIN_UNLOCK_SHM(cb);						\
						})

#define ECODE_SET_WRITE_OFFSET(cb, ofst)	({										\
							SPIN_LOCK_SHM(cb);							\
							(*(unsigned short *)(cb->rwidx) =					\
							((*(unsigned short *)(cb->rwidx) & 0xFF) | (((ofst) & 0xFF) << 8U)));	\
							SPIN_UNLOCK_SHM(cb);							\
						})

typedef struct {
	void *rwidx;
	void *lock;
	void *start;
	unsigned char mau;
} cb_t;

#if !defined(NO_HARDWARE_BUILD)
#define CB_INIT(cb_name, cb_start_addr, ecode_mau)			\
	cb_name = (cb_t){						\
		.rwidx	= cb_start_addr,				\
		.lock	= (unsigned char *)cb_start_addr +		\
		 	  (ECODE_SHM_LOCK_OFF_BYTES / ecode_mau),	\
		.start	= (unsigned char *)cb_start_addr +		\
		 	  (ECODE_SHM_START_OFF_BYTES / ecode_mau),	\
		.mau	= ecode_mau ? ecode_mau : 1U,			\
	}
#else
#define CB_INIT(cb_name, cb_start_addr, ecode_mau)			\
	cb_name = (cb_t){						\
		.rwidx	= cb_start_addr,				\
		.lock	= (unsigned char *)cb_start_addr +		\
		 	  (ECODE_SHM_LOCK_OFF_BYTES),			\
		.start	= (unsigned char *)cb_start_addr +		\
		 	  (ECODE_SHM_START_OFF_BYTES),			\
		.mau	= ecode_mau ? ecode_mau : 1U,			\
	}
#endif
extern int cb_write(cb_t *cb, unsigned char data);
extern int cb_read(cb_t *cb, unsigned char *data);
extern void debug_print(cb_t *cb, unsigned char mem_clear);

#endif
