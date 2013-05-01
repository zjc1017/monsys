#include "zb_msg.h"

// #include "z_codec.h"
#include <stdlib.h>
#include <stdio.h>


int
zb_encode_header(struct zb_header *val, char *buf, uint32_t buf_len)
{
	uint32_t orig_len = buf_len;
	int rv;

	/* fixed values */
	val->syn = 0xFF;

	ENCODE_WITH_METHOD(z_encode_byte, val->syn, buf, buf_len, rv);
	ENCODE_WITH_METHOD(z_encode_integer16, val->len, buf, buf_len, rv);
	ENCODE_WITH_METHOD(z_encode_byte, val->cmd, buf, buf_len, rv);

	return orig_len - buf_len;
}
int
zb_decode_id_list(struct zb_item_id_list *val, char *buf, uint32_t buf_len)
{
	uint32_t orig_len = buf_len;
	int rv;
  int i;
	DECODE_WITH_METHOD(z_decode_byte, (char*)&val->count, buf, buf_len, rv);
  // uint8_t *id;
  if (buf_len < sizeof(uint8_t) * val->count) {
    return -1;
  }

  // XXX: memory leak
  val->id = malloc(sizeof(uint8_t) * val->count);
  printf("val->count: %u\n", val->count);

  for (i = 0; i < val->count; ++i) {
    DECODE_WITH_METHOD(z_decode_byte, (char*)&val->id[i], buf, buf_len, rv);
    printf("val->id[%d]: %u\n", i, val->id[i]);
  }

	// ENCODE_WITH_METHOD(z_encode_byte, val->id, buf, buf_len, rv);
	return orig_len - buf_len;
}


int
zb_decode_header(struct zb_header *val, char *buf, uint32_t buf_len)
{
	uint32_t orig_len = buf_len;
	int rv;

	/* fixed values */
	DECODE_WITH_METHOD(z_decode_byte, (char*)&(val->syn), buf, buf_len, rv);
	printf("val->syn: 0x%02X\n", val->syn);

	if (val->syn != 0xFF) {
		return -1;
	}

	DECODE_WITH_METHOD(z_decode_integer16, &val->len, buf, buf_len, rv);
  printf("val->len: %u\n", val->len);
	DECODE_WITH_METHOD(z_decode_byte, (char*)&val->cmd, buf, buf_len, rv);
	printf("val->cmd: %u\n", val->cmd);

	return orig_len - buf_len;
}

uint32_t
zb_getlen_header(struct zb_header *val)
{
	uint32_t len = 0;

	len += z_getlen_byte();
	len += z_getlen_integer16();
	len += z_getlen_byte();

	return len;
}

int
zb_encode_get_req(struct zb_get_req *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
	int rv;

	/* fixed values */
	val->hdr.len = zb_getlen_get_req(val) - zb_getlen_header(&val->hdr);
	if (val->hdr.len + zb_getlen_header(&val->hdr) > buf_len) {
    return -1;
	}

	val->hdr.cmd = ZB_ID_GET_REQ;

	ENCODE_WITH_METHOD(zb_encode_header, &val->hdr, buf, buf_len, rv);
	//ENCODE_WITH_METHOD(zb_encode_id_list, &val->ids, buf, buf_len, rv);

	return orig_len - buf_len;
}


int
zb_decode_get_req(struct zb_get_req *val, char *buf, uint32_t buf_len)
{
	uint32_t orig_len = buf_len;
	int rv;

	/* fixed values */
	DECODE_WITH_METHOD(zb_decode_header, &val->hdr, buf, buf_len, rv);
	//trace_bin(&val->hdr,4);
	DECODE_WITH_METHOD(zb_decode_id_list, &val->ids, buf, buf_len, rv);

	return orig_len - buf_len;
}

uint32_t
zb_getlen_id_list(struct zb_item_id_list *ids)
{
  return 1 + ids->count;
}

uint32_t
zb_getlen_get_req(struct zb_get_req *val)
{
  uint32_t len = 0;

  len += zb_getlen_header(&val->hdr);
  len += zb_getlen_id_list(&val->ids);

  return len;
}


