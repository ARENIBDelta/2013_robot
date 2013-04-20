/*
 * Layer2P
 * 2013-02-23 Erwan Martin <erwan@martin.tl>
 */

#include "layer2p.h"
#include "stdint.h"

/* http://www.ccsinfo.com/forum/viewtopic.php?t=37015 */
int8_t crc8_step(int8_t crc, int8_t crc_data)  {
	int8_t i;
    i = (crc_data ^ crc) & 0xff;
    crc = 0;
    if(i & 1)
       crc ^= 0x5e;
    if(i & 2)
       crc ^= 0xbc;
    if(i & 4)
       crc ^= 0x61;
    if(i & 8)
       crc ^= 0xc2;
    if(i & 0x10)
       crc ^= 0x9d;
    if(i & 0x20)
       crc ^= 0x23;
    if(i & 0x40)
       crc ^= 0x46;
    if(i & 0x80)
       crc ^= 0x8c;
 return(crc);
 }

void layer2p_ctx_init(layer2p_ctx *ctx) {
	ctx->recv_callback = 0;
	ctx->error_callback = 0;
	layer2p_reset(ctx);
}

void layer2p_reset(layer2p_ctx *ctx) {
	ctx->recv_pointer = ctx->recv_buffer;
	ctx->state = 0;
}

void layer2p_receive(layer2p_ctx *ctx, unsigned char data) {
	uint8_t had_data = 0;

	switch(data) {
	case LAYER2P_START_BYTE:
		if (ctx->state == 0) {
			ctx->state = 1;
		}
		else if (ctx->state == 2) {
			*ctx->recv_pointer = data;
			ctx->state = 1;
			had_data = 1;
		}
		else {
			LAYER2P_DOERROR(ctx);
		}
		break;
	case LAYER2P_STOP_BYTE:
		if (ctx->state == 1 && (ctx->recv_pointer - ctx->recv_buffer > 1)) {

			int8_t data_crc = 0;
			unsigned char *p = ctx->recv_buffer;
			while(p < ctx->recv_pointer - 1)
				data_crc = crc8_step(data_crc, *(p++));

			int8_t recv_crc = *(ctx->recv_pointer - 1);
			if (recv_crc != data_crc) {
				LAYER2P_DOERROR(ctx);
			}
			else {
				if (ctx->recv_callback) {
					ctx->recv_callback(ctx->recv_buffer, ctx->recv_pointer - ctx->recv_buffer);
					layer2p_reset(ctx);
				}
				ctx->state = 0;
			}
			return;
		}
		else if (ctx->state == 2) {
			*ctx->recv_pointer = data;
			ctx->state = 1;
			had_data = 1;
		}
		else {
			LAYER2P_DOERROR(ctx);
		}
		break;
	case LAYER2P_ESCAPE_BYTE:
		if (ctx->state == 2) {
			*ctx->recv_pointer = data;
			ctx->state = 1;
			had_data = 1;
		}
		else if (ctx->state == 1) {
			ctx->state = 2;
		}
		else {
			LAYER2P_DOERROR(ctx);
		}
		break;
	default:
		if (ctx->state == 1) {
			*ctx->recv_pointer = data;
			had_data = 1;
		}
		else {
			LAYER2P_DOERROR(ctx);
		}
		break;
	}

	if (had_data) {
		ctx->recv_pointer++;
		if (ctx->recv_pointer - ctx->recv_buffer >= LAYER2P_RECV_BUFFER_SIZE) {
			ctx->state = 0;
			LAYER2P_DOERROR(ctx);
		}
	}
}

void layer2p_set_callback(layer2p_ctx *ctx, layer2p_received cb) {
	ctx->recv_callback = cb;
}

void layer2p_set_error_callback(layer2p_ctx *ctx, layer2p_errored cb) {
	ctx->error_callback = cb;
}

void layer2p_set_linksend_callback(layer2p_ctx *ctx, layer2p_linksend cb) {
	ctx->linksend_callback = cb;
}

void layer2p_send(layer2p_ctx *ctx, unsigned char *data, unsigned int size) {
	unsigned int i;

	if (!ctx->linksend_callback)
		return;
	int8_t crc = 0;
	ctx->linksend_callback(LAYER2P_START_BYTE, 0);
	for(i=0; i<size; i++) {
		switch(data[i]) {
		case LAYER2P_START_BYTE:
		case LAYER2P_STOP_BYTE:
		case LAYER2P_ESCAPE_BYTE:
			ctx->linksend_callback(LAYER2P_ESCAPE_BYTE, 0);
		default:
			crc = crc8_step(crc, data[i]);
			ctx->linksend_callback(data[i], 0);
		}
	}
	ctx->linksend_callback(crc, 0);
	ctx->linksend_callback(LAYER2P_STOP_BYTE, 1);
}
