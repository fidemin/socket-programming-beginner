
struct tcphdr
{
	u_int16_t source;
	u_int16_t dest;
	u_int32_t seq;
	u_int32_t ack_seq;
#if __BYTE_ORDER == __LITTLE_ENDIAN
	u_int16_t res1:4;
	u_int16_t hlen:4;

	u_int16_t fin:1;
	u_int16_t syn:1;
	u_int16_t rst:1;
	u_int16_t psh:1;
	u_int16_t ack:1;
	u_int16_t urg:1;
	u_int16_t res2:2;
#elif __BYTE_ORDER == __BIG_ENDIAN
	u_int16_t hlen:4;
	u_int16_t res1:4;

	u_int16_t urg:1;
	u_int16_t ack:1;
	u_int16_t psh:1;
	u_int16_t rst:1;
	u_int16_t syn:1;
	u_int16_t fin:1;
	u_int16_t res2:2;
#else
#error "check your <bits/endian.h>"
#endif
	u_int16_t window;
	u_int16_t check;
	u_int16_t urg_ptr;;
};
