#ifndef _DMA_OPERATIONS
#define _DMA_OPERATIONS

#define DMA_BASE_ADDR       0x1E000000
#define DMA_SRC_REG         0x00
#define DMA_DEST_REG        0x04
#define DMA_LEN_REG         0x08
#define DMA_CTL_REG         0x0C
#define DMA_STS_REG         0x10

#define STS_MASK_BUSY           0x00000001
#define STS_MASK_DONE           0x00000002
#define STS_MASK_ERROR          0x00000004

#endif
