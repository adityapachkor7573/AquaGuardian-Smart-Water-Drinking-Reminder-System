//defines.h
/* ---- BIT OPERATIONS ---- */
#define SETBIT(WORD, BITPOS) (WORD |= ((u32)1 << (BITPOS)))

#define CLRBIT(WORD, BITPOS) (WORD &= ~((u32)1 << (BITPOS)))

#define CPLBIT(WORD, BITPOS) (WORD ^= ((u32)1 << (BITPOS)))

#define READBIT(WORD, BITPOS) (((WORD) >> (BITPOS)) & 1)

#define WRITEBIT(WORD, BITPOS, BITLEVEL) \
				WORD = (((WORD) & ~((u32)1 << (BITPOS))) | ((u32)(BITLEVEL) << (BITPOS)))

#define READWRITEBIT(WORD, DBIT, SBIT) \
				WORD = (((WORD) & ~((u32)1 << (DBIT))) | ((((WORD) >> (SBIT)) & 1) << (DBIT)))

/* ---- NIBBLE OPERATIONS (4 bits) ---- */
#define READNIBBLE(WORD, STARTBITPOS) \
				(((WORD) >> (STARTBITPOS)) & 0x0F)

#define WRITENIBBLE(WORD, STARTBITPOS, NIBBLEVAL) \
				WORD = (((WORD) & ~((u32)0x0F << (STARTBITPOS))) | \
				((u32)((NIBBLEVAL) & 0x0F) << (STARTBITPOS)))

/* ---- BYTE OPERATIONS (8 bits) ---- */

#define READBYTE(WORD, STARTBITPOS) \
			(((WORD) >> (STARTBITPOS)) & 0xFF)

#define WRITEBYTE(WORD, STARTBITPOS, BYTEVAL) \
			WORD = (((WORD) & ~((u32)0xFF << (STARTBITPOS))) | \
			((u32)((BYTEVAL) & 0xFF) << (STARTBITPOS)))
