/*

## Custom License Agreement

This project includes software, hardware, and system integration and is licensed under the following terms:

### License Overview

- **Non-Commercial Use**: You are free to use, modify, and distribute the software, hardware designs, and integration components for non-commercial purposes, provided that you comply with the conditions outlined below.
- **Commercial Use**: If you wish to use, modify, or distribute the software, hardware designs, or integration components for commercial purposes, including but not limited to incorporating them into products or services for sale, you must obtain explicit permission from the author.

### Terms for Non-Commercial Use

1. **Attribution**: You must provide appropriate credit, including a link to the original repository and a notice indicating any changes made.
2. **Non-Commercial Use**: The use, modification, and distribution of this project are permitted only for non-commercial purposes. Commercial use requires obtaining a separate license.

### Terms for Commercial Use

1. **Contact Requirement**: To use the software, hardware designs, or integration components for commercial purposes, you must contact the author at [arshiakeshvariasl@gmail.com] to obtain permission and discuss licensing terms.
2. **Additional Licensing**: Commercial use will be subject to additional terms agreed upon between you and the author. This may include financial arrangements or other conditions.

### Integration with Other Projects

1. **Commercial Integration**: If you wish to integrate the software, hardware designs, or other components with other commercial projects or products, you must contact the author for permission and to discuss any additional licensing terms.
2. **Non-Commercial Integration**: Integration with non-commercial projects is permitted under the same terms as outlined for non-commercial use.

### Disclaimer

This project is provided "as-is" without warranty of any kind. The author is not liable for any damages arising from the use of this project, whether commercial or non-commercial. By using this project, you acknowledge that you assume all risks associated with its use.

### How to Contact

For commercial use inquiries or any other questions regarding this license, please contact the author at [arshiakeshvariasl@gmail.com].

By using this project, you agree to abide by the terms set forth in this license.

*/

/*
 * RingBuffer.c
 *
 *  Created on: May 13, 2024
 *      Author: Arshia Keshvari
 */

#include "RingBuffer.h"

static struct ring_buffer _rb[3];

volatile uint8_t rb1buf[UART_FIFOBUFLEN] = {0};

volatile rbd_t rbd;

int ring_buffer_init(rbd_t *rbd, rb_attr_t *attr)
{
	static int idx = 0;
	int err = -1;

	if ((idx < RING_BUFFER_MAX) && (rbd != NULL) && (attr != NULL)) {
		if ((attr->buffer != NULL) && (attr->s_elem > 0)) {
			/* Check that the size of the ring buffer is a power of 2 */
			if (((attr->n_elem - 1) & attr->n_elem) == 0) {
				/* Initialise the ring buffer internal variables */
				_rb[idx].head = 0;
				_rb[idx].tail = 0;
				_rb[idx].buf = attr->buffer;
				_rb[idx].s_elem = attr->s_elem;
				_rb[idx].n_elem = attr->n_elem;

				*rbd = idx++;
				err= 0;
			}
		}
	}

	return err;
}

static int _ring_buffer_full(struct ring_buffer *rb)
{
	return ((rb->head - rb->tail) == rb->n_elem) ? 1 : 0;
}

static int _ring_buffer_empty(struct ring_buffer *rb)
{
	return ((rb->head - rb->tail) == 0U) ? 1 : 0;
}

int ring_buffer_put(rbd_t rbd, const void *data)
{
	int err = 0;

	if ((rbd < RING_BUFFER_MAX) && (_ring_buffer_full(&_rb[rbd]) == 0)) {
		const size_t offset = (_rb[rbd].head & (_rb[rbd].n_elem - 1)) * _rb[rbd].s_elem;
		memcpy(&(_rb[rbd].buf[offset]), data, _rb[rbd].s_elem);
		_rb[rbd].head++;
	} else {
		err = -1;
	}

	return err;
}

int ring_buffer_get(rbd_t rbd, void *data)
{
	int err = 0;

	if ((rbd < RING_BUFFER_MAX) && (_ring_buffer_empty(&_rb[rbd]) == 0)) {
		const size_t offset = (_rb[rbd].tail & (_rb[rbd].n_elem - 1)) * _rb[rbd].s_elem;
		memcpy(data, &(_rb[rbd].buf[offset]), _rb[rbd].s_elem);
		_rb[rbd].tail++;
	} else {
		err = -1;
	}

	return err;
}
