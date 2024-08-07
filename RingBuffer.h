/*
 * RingBuffer.h
 *
 *  Created on: May 13, 2024
 *      Author: Arshia Keshvari
 */

/*
 * RingBuffer.h
 *
 *  Created on: 23 Jan 2022
 *      Author: Harris Shallcross
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
