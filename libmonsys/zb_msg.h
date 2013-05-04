#ifndef _ZB_MSG_H__
#define _ZB_MSG_H__
// #include <stdint.h>
#include "z_codec.h"



#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum {
	ZB_ID_HEARTBEAT = 0xFF,

	ZB_ID_REG_REQ = 0x01,
	ZB_ID_REG_RSP = 0x81,

	ZB_ID_GET_REQ = 0x02,
	ZB_ID_GET_RSP = 0x82,

	ZB_ID_SET_REQ = 0x03,
	ZB_ID_SET_RSP = 0x83,

	// to be continued
};

///////////////////////////////////////
// header
struct zb_header {
  uint8_t syn;
  uint16_t len; 			/* header is not included */
  uint8_t cmd;
};

struct zb_item_id_list {
	uint8_t count;
	uint8_t *id;
};

struct zb_item_pair {
	uint8_t id;
	uint16_t val;
};

struct zb_item_pair_list {
	uint8_t count;
	struct zb_item_pair *pairs;
};

// get
struct zb_get_req {
	struct zb_header hdr;
	struct zb_item_id_list ids;
};

struct zb_get_rsp {
	struct zb_header hdr;
	struct zb_item_pair_list pairs;
};

// set
struct zb_set_req {
	struct zb_header hdr;
	struct zb_item_pair_list pairs;
};

struct zb_set_rsp {
	struct zb_header hdr;
	uint8_t status;
};

int zb_encode_header(struct zb_header *val, char *buf, uint32_t buf_len);
int zb_encode_id_list(struct zb_item_id_list *val, char *buf, uint32_t buf_len);
int zb_decode_header(struct zb_header *val, char *buf, uint32_t buf_len);
uint32_t zb_getlen_header(struct zb_header *val);

// get_req
int zb_encode_get_req(struct zb_get_req *msg, char *buf, uint32_t buf_len);
int zb_decode_get_req(struct zb_get_req *val, char *buf, uint32_t buf_len);
uint32_t zb_getlen_get_req(struct zb_get_req *val);

int zb_encode_get_rsp(struct zb_get_rsp *val, char *buf, uint32_t buf_len);

int zb_encode_set_req(struct zb_set_req *msg,char *buf,uint32_t buf_len);
int zb_decode_set_rsp(struct zb_set_rsp *val,char *buf,uint32_t buf_len);
//int test_zb_decode_set_req();

//int zb_test(uint8_t *serial_data,uint8_t value,uint8_t len);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // _ZB_MSG_H__


