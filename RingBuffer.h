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
 * RingBuffer.h
 *
 *  Created on: May 13, 2024
 *      Author: Arshia Keshvari
 *
 * FIFO source: http://www.simplyembedded.org/tutorials/interrupt-free-ring-buffer/
 *
 */

#ifndef INC_RINGBUFFER_H_
#define INC_RINGBUFFER_H_

#include "main.h"

#define UART_FIFOBUFLEN		128
#define RING_BUFFER_MAX		128

typedef struct {
    size_t s_elem;
    size_t n_elem;
    void *buffer;
} rb_attr_t;

struct ring_buffer
{
    size_t s_elem;
    size_t n_elem;
    uint8_t *buf;
    volatile size_t head;
    volatile size_t tail;
};

typedef unsigned int rbd_t;

extern volatile uint8_t rb1buf[UART_FIFOBUFLEN];
extern volatile rbd_t rbd;

int ring_buffer_init(rbd_t *rbd, rb_attr_t *attr);
int ring_buffer_put(rbd_t rbd, const void *data);
int ring_buffer_get(rbd_t rbd, void *data);

#endif /* INC_RINGBUFFER_H_ */
