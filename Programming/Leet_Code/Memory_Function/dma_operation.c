#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>

#include "dma_operation.h"

uint32_t read_reg(const uint32_t BASE_ADDR, const uint32_t REG_OFFSET) {
    uint32_t value = 0;
    uint32_t *ptr = BASE_ADDR;
    value = *(ptr + (REG_OFFSET/sizeof(uint32_t)));
    return value;
}

void write_reg(const uint32_t BASE_ADDR, const uint32_t REG_OFFSET,
               const uint32_t value) {
    uint32_t *ptr = BASE_ADDR;
    *(ptr + (REG_OFFSET/sizeof(uint32_t))) = value;
}

bool dma_busy(void) {
    if (read_reg(DMA_BASE_ADDR, DMA_STS_REG) & STS_MASK_BUSY) {
        return true;
    } else {
        return false;
    }
}

bool dma_done(void) {
    if (read_reg(DMA_BASE_ADDR, DMA_STS_REG) & STS_MASK_DONE) {
        return true;
    } else {
        return false;
    }
}
bool dma_error(void) {
    if (read_reg(DMA_BASE_ADDR, DMA_STS_REG) & STS_MASK_ERROR) {
        return true;
    } else {
        return false;
    }
}

int dma_trx(const uint32_t dest, const uint32_t src, const uint32_t num_bytes) {
    // Checking if the value is in range
    if (num_bytes > USHRT_MAX) {
        return -EINVAL;
    }

    // TODO: DMA is in error state
    if (dma_error()) {
        return -EBADARCH;
    }

    // If DMA is busy it will continuoulsy poll untill it
    // is not busy
    while (dma_busy() == true) {
        // Poll until busy
        // TODO: Poll with timeout
    }

    // Writing to the registers
    write_reg(DMA_BASE_ADDR, DMA_SRC_REG, src);
    write_reg(DMA_BASE_ADDR, DMA_DEST_REG, dest);
    write_reg(DMA_BASE_ADDR, DMA_LEN_REG, num_bytes);

    // When it will not be busy
    // Operation will  start
    write_reg(DMA_BASE_ADDR, DMA_CTL_REG, 1);   // MACRO    what is 1?

    //// Do I need to show the actual transfer of data

    // Poll untill done bit is 1
    while (dma_done() == false) {
        // ADD TIMEOUT
    }

    if (dma_error() == true) {
        return -EBADF;
    }

    return 0;
}

int main(void) {
    int rc;

    rc = dma_trx(0x1200, 0x800, 0x100);
    if (rc) {
        printf("ERROR: %d %s", rc, strerror(-rc));
    }

    return rc;
}

