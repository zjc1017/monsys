#include "zb_msg.h"

// #include "z_codec.h"

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
  //val->id = malloc(sizeof(uint8_t) * val->count);
  static uint8_t ids[10];
  val->id = ids;
  for (i = 0; i < val->count; ++i) {
    DECODE_WITH_METHOD(z_decode_byte, (char*)&val->id[i], buf, buf_len, rv);
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

	if (val->syn != 0xFF) {
		return -1;
	}

	DECODE_WITH_METHOD(z_decode_integer16, &val->len, buf, buf_len, rv);
  //printf("val->len: %u\n", val->len);
	DECODE_WITH_METHOD(z_decode_byte, (char*)&val->cmd, buf, buf_len, rv);
	//printf("val->cmd: %u\n", val->cmd);

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
zb_encode_pair_list(struct zb_item_pair_list *val, char *buf, uint32_t buf_len)
{
	uint32_t orig_len = buf_len;
	int rv;
  int i;

	ENCODE_WITH_METHOD(z_encode_byte, val->count, buf, buf_len, rv);
  if (buf_len < sizeof(uint8_t) * val->count) {
    return -1;
  }

  for (i = 0; i < val->count; ++i) {
    ENCODE_WITH_METHOD(z_encode_byte, val->pairs[i].id, buf, buf_len, rv);
    ENCODE_WITH_METHOD(z_encode_integer16, val->pairs[i].val, buf, buf_len, rv);
  }

	return orig_len - buf_len;
}

uint32_t
zb_getlen_item_pair()
{
	return 1 + 2;
}

uint32_t
zb_getlen_pair_list(struct zb_item_pair_list *list)
{
	uint32_t len = 0;

	len += 1;
	len += list->count * zb_getlen_item_pair();
	return len;
}

uint32_t
zb_getlen_get_rsp(struct zb_get_rsp *val)
{
	uint32_t len = 0;

	len += zb_getlen_header(&val->hdr);
	len += zb_getlen_pair_list(&val->pairs);
	return len;
}

int
zb_encode_get_rsp(struct zb_get_rsp *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
	int rv;

	/* fixed values */
	val->hdr.len = zb_getlen_get_rsp(val) - zb_getlen_header(&val->hdr);
	if (val->hdr.len + zb_getlen_header(&val->hdr) > buf_len) {
    return -1;
	}

	val->hdr.cmd = ZB_ID_GET_RSP;

	ENCODE_WITH_METHOD(zb_encode_header, &val->hdr, buf, buf_len, rv);
	ENCODE_WITH_METHOD(zb_encode_pair_list, &val->pairs, buf, buf_len, rv);



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

int
zb_decode_pair_list(struct zb_item_pair_list *val, char *buf, uint32_t buf_len)
{
	uint32_t orig_len = buf_len;
	int rv;
  int i;
  DECODE_WITH_METHOD(z_decode_byte, (char*)&(val->count), buf, buf_len, rv);
  printf("val->count: %u\n", val->count);
  //0x22ff17 "\002\001\020\002\021\377"
  struct zb_item_pair pairs_buf[10];
  val->pairs = pairs_buf;
  for (i = 0; i < val->count; ++i) {
  DECODE_WITH_METHOD(z_decode_byte,(char*)&val->pairs[i].id, buf, buf_len, rv);
  printf("val->pairs[i].id: %u\n", val->pairs[i].id);
  DECODE_WITH_METHOD(z_decode_integer16, &val->pairs[i].val, buf, buf_len, rv);
  printf("val->pairs[i].val: %u\n", val->pairs[i].val);
  }

	return orig_len - buf_len;
}

int
zb_decode_set_req(struct zb_set_req *val, char *buf, uint32_t buf_len)
{
	uint32_t orig_len = buf_len;
	int rv;

	/* fixed values */
	DECODE_WITH_METHOD(zb_decode_header, &val->hdr, buf, buf_len, rv);
	//trace_bin(&val->hdr,4);
	DECODE_WITH_METHOD(zb_decode_pair_list, &val->pairs, buf, buf_len, rv);

	return orig_len - buf_len;
}


int
zb_test(uint8_t *serial_data,uint8_t value,uint8_t len)
{
	int i;
	for(i=0;i<len;i++)
	{
		*(serial_data+i)=value;
	}
	return 0;
}

