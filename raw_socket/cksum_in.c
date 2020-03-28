
unsigned short cksum_in(unsigned short *addr, int len)
{
	unsigned long sum = 0;
	unsigned short answer = 0;
	unsigned short *w = addr;
	int nleft = len;

	while (nleft > 1) {
		sum += *w++; // sum 2 bytes number
		if (sum & 0x80000000) {
			// it will overflow 
			sum = (sum & 0xffff) + (sum >> 16);
		}
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *) (&answer) = *(unsigned char*) w;
		sum += answer;
	}

	while (sum >> 16) {
		// if first 16 bits has value, sum this with last 16 bits
		sum = (sum & 0xffff) + (sum >> 16);
	}

	// if ~sum is zero, return sum
	return (sum==0xffff)? sum:~sum;
}
