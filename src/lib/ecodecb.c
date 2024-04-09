/*
 * MIT License
 *
 * Copyright (c) 2024 Ecode Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file ecodecb.c
 * \ref ecodecb.c
 * @brief ecode circular buffer implementation.
 */

#include <ecode.h>
#include <stdio.h>

/**
 * cb_write function used to write into the circular buffer.
 *
 * @param cb_t Circular buffer data structure.
 * @param data Data to be written.
 */
int cb_write(cb_t *cb, unsigned char data) {

	if (cb == NULL) return -1;

	unsigned char wofst	= ECODE_GET_WRITE_OFFSET(cb);
	unsigned char rofst	= (ECODE_GET_READ_OFFSET(cb) / cb->mau) * cb->mau;
	unsigned char wofstnext	= (wofst + cb->mau) % ECODE_SHM_DATA_SIZE_BYTES;
#if !defined(NO_HARDWARE_BUILD)
	unsigned char *wptr	= (unsigned char *)cb->start + (wofst / cb->mau);
#else
	unsigned char *wptr	= (unsigned char *)cb->start + wofst;
#endif

	/* read equal to next write */
	if (wofstnext == rofst) {
		return -1;
	}

	/* write data */
	*wptr = data;

	/* If required wrap around */
	ECODE_SET_WRITE_OFFSET(cb, wofstnext);

	return 0;
}

/**
 * cb_read function used to read from the circular buffer.
 *
 * @param cb_t Circular buffer data structure.
 * @param *data Pointer to data to be read into.
 */
int cb_read(cb_t *cb, unsigned char *data) {

	if (cb == NULL) return -1;

	unsigned char wofst	= (ECODE_GET_WRITE_OFFSET(cb) / cb->mau) * cb->mau;
	unsigned char rofst	= ECODE_GET_READ_OFFSET(cb);
	unsigned char rofstnext	= (rofst + cb->mau) % ECODE_SHM_DATA_SIZE_BYTES;
#if !defined(NO_HARDWARE_BUILD)
	unsigned char *rptr	= (unsigned char *)cb->start + (rofst / cb->mau);
#else
	unsigned char *rptr	= (unsigned char *)cb->start + rofst;
#endif

	/* read equal to write */
	if ((data == NULL) || (rofst == wofst)) {
		return -1;
	}

	/* read data */
	*data = *rptr;

	/* If required wrap around */
	ECODE_SET_READ_OFFSET(cb, rofstnext);

	return 0;
}

/**
 * debug_print helper function used to display the circular buffer.
 *
 * @param cb_t Circular buffer data structure.
 * @param mem_clear flag to clear the circular buffer.
 */
void debug_print(cb_t *cb, unsigned char mem_clear) {
	
	unsigned char wofst	= ECODE_GET_WRITE_OFFSET(cb);
	unsigned char rofst	= ECODE_GET_READ_OFFSET(cb);

	printf("\n\n");
	for (unsigned int i = 0; i < ECODE_SHM_SIZE_BYTES; i++) {
		if (i == ECODE_SHM_START_OFF_BYTES) printf("\n");
		printf("%p: %03d [%c] [%c] \n", ((unsigned char *)cb->rwidx + i),
				*((unsigned char *)cb->rwidx + i),
				(i == rofst + ECODE_SHM_START_OFF_BYTES) ? 'R':'*',
				(i == wofst + ECODE_SHM_START_OFF_BYTES) ? 'W':'*');
		if (mem_clear) {
			*((unsigned char *)cb->rwidx + i) = 0;
		}
	}
}
