/*
 * Layer2P
 * 2013-02-23 Erwan Martin <erwan@martin.tl>
 * Add framing and error checking to a level 1 bus (typically I2C or SPI)
 */

#ifndef LAYER2P_H_
#define LAYER2P_H_

#include <stdint.h>

#define LAYER2P_RECV_BUFFER_SIZE 96
#define LAYER2P_ESCAPE_BYTE 0xAA
#define LAYER2P_START_BYTE 0x55
#define LAYER2P_STOP_BYTE 0x54

#define LAYER2P_DOERROR(ctx) \
	if (ctx->error_callback) \
		ctx->error_callback(); \
	layer2p_reset(ctx);

typedef void (*layer2p_linksend)(unsigned char data, unsigned char lastbyte);
typedef void (*layer2p_received)(unsigned char *data, unsigned int size);
typedef void (*layer2p_errored)(void);

typedef struct _layer2p_ctx {
	layer2p_linksend linksend_callback;
	layer2p_received recv_callback;
	layer2p_errored error_callback;
	uint8_t recv_buffer[LAYER2P_RECV_BUFFER_SIZE];
	uint8_t *recv_pointer;
	unsigned char state; //0 => reseted, 1 => waiting for data, 2 => previous was escape char
} layer2p_ctx;

void layer2p_ctx_init(layer2p_ctx *ctx);
void layer2p_reset(layer2p_ctx *ctx);
void layer2p_receive(layer2p_ctx *ctx, unsigned char data);
void layer2p_set_linksend_callback(layer2p_ctx *ctx, layer2p_linksend cb);
void layer2p_set_callback(layer2p_ctx *ctx, layer2p_received cb);
void layer2p_set_error_callback(layer2p_ctx *ctx, layer2p_errored cb);
void layer2p_send(layer2p_ctx *ctx, unsigned char *data, unsigned int size);

#endif /* LAYER2P_H_ */
