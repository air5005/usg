#include "usg_common.h"
#include <stdint.h>
#include <stdlib.h>

#define RING_SIZE 4096
#define MAX_BULK 32

static usg_atomic32_t synchro;

#define	TEST_RING_VERIFY(exp)						\
	if (!(exp)) {							\
		printf("error at %s:%d\tcondition " #exp " failed\n",	\
		    __func__, __LINE__);				\
		usg_ring_dump(stdout, r);				\
		return -1;						\
	}

#define	TEST_RING_FULL_EMTPY_ITER	8
#define LINE_LEN 128

static inline void
usg_srand(uint64_t seedval)
{
	srand48((long)seedval);
}

static inline uint64_t
usg_rand(void)
{
	uint64_t val;
	val = (uint64_t)lrand48();
	val <<= 32;
	val += (uint64_t)lrand48();
	return val;
}

void
test_hexdump(FILE *f, const char * title, const void * buf, unsigned int len)
{
    unsigned int i, out, ofs;
    const unsigned char *data = buf;
    char line[LINE_LEN];    /* space needed 8+16*3+3+16 == 75 */

    fprintf(f, "%s at [%p], len=%u\n", (title)? title  : "  Dump data", data, len);
    ofs = 0;
    while (ofs < len) {
        /* format the line in the buffer, then use printf to output to screen */
        out = snprintf(line, LINE_LEN, "%08X:", ofs);
        for (i = 0; ((ofs + i) < len) && (i < 16); i++)
            out += snprintf(line+out, LINE_LEN - out, " %02X", (data[ofs+i] & 0xff));
        for(; i <= 16; i++)
            out += snprintf(line+out, LINE_LEN - out, " | ");
        for(i = 0; (ofs < len) && (i < 16); i++, ofs++) {
            unsigned char c = data[ofs];
            if ( (c < ' ') || (c > '~'))
                c = '.';
            out += snprintf(line+out, LINE_LEN - out, "%c", c);
        }
        fprintf(f, "%s\n", line);
    }
    fflush(f);
}

/*
 * helper routine for test_ring_basic
 */
static int
test_ring_basic_full_empty(struct usg_ring *r, void * const src[], void *dst[])
{
	unsigned i, rand;
	const unsigned rsz = RING_SIZE - 1;

	printf("Basic full/empty test\n");

	for (i = 0; TEST_RING_FULL_EMTPY_ITER != i; i++) {

		/* random shift in the ring */
		rand = USG_MAX(usg_rand() % RING_SIZE, 1UL);
		printf("%s: iteration %u, random shift: %u;\n",
		    __func__, i, rand);
		TEST_RING_VERIFY(usg_ring_enqueue_bulk(r, src, rand,
				NULL) != 0);
		TEST_RING_VERIFY(usg_ring_dequeue_bulk(r, dst, rand,
				NULL) == rand);

		/* fill the ring */
		TEST_RING_VERIFY(usg_ring_enqueue_bulk(r, src, rsz, NULL) != 0);
		TEST_RING_VERIFY(0 == usg_ring_free_count(r));
		TEST_RING_VERIFY(rsz == usg_ring_count(r));
		TEST_RING_VERIFY(usg_ring_full(r));
		TEST_RING_VERIFY(0 == usg_ring_empty(r));

		/* empty the ring */
		TEST_RING_VERIFY(usg_ring_dequeue_bulk(r, dst, rsz,
				NULL) == rsz);
		TEST_RING_VERIFY(rsz == usg_ring_free_count(r));
		TEST_RING_VERIFY(0 == usg_ring_count(r));
		TEST_RING_VERIFY(0 == usg_ring_full(r));
		TEST_RING_VERIFY(usg_ring_empty(r));

		/* check data */
		TEST_RING_VERIFY(0 == memcmp(src, dst, rsz));
		usg_ring_dump(stdout, r);
	}
	return 0;
}

static int
test_ring_basic(struct usg_ring *r)
{
	void **src = NULL, **cur_src = NULL, **dst = NULL, **cur_dst = NULL;
	int ret;
	unsigned i, num_elems;

	/* alloc dummy object pointers */
	src = malloc(RING_SIZE*2*sizeof(void *));
	if (src == NULL)
		goto fail;

	for (i = 0; i < RING_SIZE*2 ; i++) {
		src[i] = (void *)(unsigned long)i;
	}
	cur_src = src;

	/* alloc some room for copied objects */
	dst = malloc(RING_SIZE*2*sizeof(void *));
	if (dst == NULL)
		goto fail;

	memset(dst, 0, RING_SIZE*2*sizeof(void *));
	cur_dst = dst;

	printf("enqueue 1 obj\n");
	ret = usg_ring_sp_enqueue_bulk(r, cur_src, 1, NULL);
	cur_src += 1;
	if (ret == 0)
		goto fail;

	printf("enqueue 2 objs\n");
	ret = usg_ring_sp_enqueue_bulk(r, cur_src, 2, NULL);
	cur_src += 2;
	if (ret == 0)
		goto fail;

	printf("enqueue MAX_BULK objs\n");
	ret = usg_ring_sp_enqueue_bulk(r, cur_src, MAX_BULK, NULL);
	cur_src += MAX_BULK;
	if (ret == 0)
		goto fail;

	printf("dequeue 1 obj\n");
	ret = usg_ring_sc_dequeue_bulk(r, cur_dst, 1, NULL);
	cur_dst += 1;
	if (ret == 0)
		goto fail;

	printf("dequeue 2 objs\n");
	ret = usg_ring_sc_dequeue_bulk(r, cur_dst, 2, NULL);
	cur_dst += 2;
	if (ret == 0)
		goto fail;

	printf("dequeue MAX_BULK objs\n");
	ret = usg_ring_sc_dequeue_bulk(r, cur_dst, MAX_BULK, NULL);
	cur_dst += MAX_BULK;
	if (ret == 0)
		goto fail;

	/* check data */
	if (memcmp(src, dst, cur_dst - dst)) {
		test_hexdump(stdout, "src", src, cur_src - src);
		test_hexdump(stdout, "dst", dst, cur_dst - dst);
		printf("data after dequeue is not the same\n");
		goto fail;
	}
	cur_src = src;
	cur_dst = dst;

	printf("enqueue 1 obj\n");
	ret = usg_ring_mp_enqueue_bulk(r, cur_src, 1, NULL);
	cur_src += 1;
	if (ret == 0)
		goto fail;

	printf("enqueue 2 objs\n");
	ret = usg_ring_mp_enqueue_bulk(r, cur_src, 2, NULL);
	cur_src += 2;
	if (ret == 0)
		goto fail;

	printf("enqueue MAX_BULK objs\n");
	ret = usg_ring_mp_enqueue_bulk(r, cur_src, MAX_BULK, NULL);
	cur_src += MAX_BULK;
	if (ret == 0)
		goto fail;

	printf("dequeue 1 obj\n");
	ret = usg_ring_mc_dequeue_bulk(r, cur_dst, 1, NULL);
	cur_dst += 1;
	if (ret == 0)
		goto fail;

	printf("dequeue 2 objs\n");
	ret = usg_ring_mc_dequeue_bulk(r, cur_dst, 2, NULL);
	cur_dst += 2;
	if (ret == 0)
		goto fail;

	printf("dequeue MAX_BULK objs\n");
	ret = usg_ring_mc_dequeue_bulk(r, cur_dst, MAX_BULK, NULL);
	cur_dst += MAX_BULK;
	if (ret == 0)
		goto fail;

	/* check data */
	if (memcmp(src, dst, cur_dst - dst)) {
		test_hexdump(stdout, "src", src, cur_src - src);
		test_hexdump(stdout, "dst", dst, cur_dst - dst);
		printf("data after dequeue is not the same\n");
		goto fail;
	}
	cur_src = src;
	cur_dst = dst;

	printf("fill and empty the ring\n");
	for (i = 0; i<RING_SIZE/MAX_BULK; i++) {
		ret = usg_ring_mp_enqueue_bulk(r, cur_src, MAX_BULK, NULL);
		cur_src += MAX_BULK;
		if (ret == 0)
			goto fail;
		ret = usg_ring_mc_dequeue_bulk(r, cur_dst, MAX_BULK, NULL);
		cur_dst += MAX_BULK;
		if (ret == 0)
			goto fail;
	}

	/* check data */
	if (memcmp(src, dst, cur_dst - dst)) {
		test_hexdump(stdout, "src", src, cur_src - src);
		test_hexdump(stdout, "dst", dst, cur_dst - dst);
		printf("data after dequeue is not the same\n");
		goto fail;
	}

	if (test_ring_basic_full_empty(r, src, dst) != 0)
		goto fail;

	cur_src = src;
	cur_dst = dst;

	printf("test default bulk enqueue / dequeue\n");
	num_elems = 16;

	cur_src = src;
	cur_dst = dst;

	ret = usg_ring_enqueue_bulk(r, cur_src, num_elems, NULL);
	cur_src += num_elems;
	if (ret == 0) {
		printf("Cannot enqueue\n");
		goto fail;
	}
	ret = usg_ring_enqueue_bulk(r, cur_src, num_elems, NULL);
	cur_src += num_elems;
	if (ret == 0) {
		printf("Cannot enqueue\n");
		goto fail;
	}
	ret = usg_ring_dequeue_bulk(r, cur_dst, num_elems, NULL);
	cur_dst += num_elems;
	if (ret == 0) {
		printf("Cannot dequeue\n");
		goto fail;
	}
	ret = usg_ring_dequeue_bulk(r, cur_dst, num_elems, NULL);
	cur_dst += num_elems;
	if (ret == 0) {
		printf("Cannot dequeue2\n");
		goto fail;
	}

	/* check data */
	if (memcmp(src, dst, cur_dst - dst)) {
		test_hexdump(stdout, "src", src, cur_src - src);
		test_hexdump(stdout, "dst", dst, cur_dst - dst);
		printf("data after dequeue is not the same\n");
		goto fail;
	}

	cur_src = src;
	cur_dst = dst;

	ret = usg_ring_mp_enqueue(r, cur_src);
	if (ret != 0)
		goto fail;

	ret = usg_ring_mc_dequeue(r, cur_dst);
	if (ret != 0)
		goto fail;

	free(src);
	free(dst);
	return 0;

 fail:
	free(src);
	free(dst);
	return -1;
}

static int
test_ring_burst_basic(struct usg_ring *r)
{
	void **src = NULL, **cur_src = NULL, **dst = NULL, **cur_dst = NULL;
	int ret;
	unsigned i;

	/* alloc dummy object pointers */
	src = malloc(RING_SIZE*2*sizeof(void *));
	if (src == NULL)
		goto fail;

	for (i = 0; i < RING_SIZE*2 ; i++) {
		src[i] = (void *)(unsigned long)i;
	}
	cur_src = src;

	/* alloc some room for copied objects */
	dst = malloc(RING_SIZE*2*sizeof(void *));
	if (dst == NULL)
		goto fail;

	memset(dst, 0, RING_SIZE*2*sizeof(void *));
	cur_dst = dst;

	printf("Test SP & SC basic functions \n");
	printf("enqueue 1 obj\n");
	ret = usg_ring_sp_enqueue_burst(r, cur_src, 1, NULL);
	cur_src += 1;
	if (ret != 1)
		goto fail;

	printf("enqueue 2 objs\n");
	ret = usg_ring_sp_enqueue_burst(r, cur_src, 2, NULL);
	cur_src += 2;
	if (ret != 2)
		goto fail;

	printf("enqueue MAX_BULK objs\n");
	ret = usg_ring_sp_enqueue_burst(r, cur_src, MAX_BULK, NULL);
	cur_src += MAX_BULK;
	if (ret != MAX_BULK)
		goto fail;

	printf("dequeue 1 obj\n");
	ret = usg_ring_sc_dequeue_burst(r, cur_dst, 1, NULL);
	cur_dst += 1;
	if (ret != 1)
		goto fail;

	printf("dequeue 2 objs\n");
	ret = usg_ring_sc_dequeue_burst(r, cur_dst, 2, NULL);
	cur_dst += 2;
	if (ret != 2)
		goto fail;

	printf("dequeue MAX_BULK objs\n");
	ret = usg_ring_sc_dequeue_burst(r, cur_dst, MAX_BULK, NULL);
	cur_dst += MAX_BULK;
	if (ret != MAX_BULK)
		goto fail;

	/* check data */
	if (memcmp(src, dst, cur_dst - dst)) {
		test_hexdump(stdout, "src", src, cur_src - src);
		test_hexdump(stdout, "dst", dst, cur_dst - dst);
		printf("data after dequeue is not the same\n");
		goto fail;
	}

	cur_src = src;
	cur_dst = dst;

	printf("Test enqueue without enough memory space \n");
	for (i = 0; i< (RING_SIZE/MAX_BULK - 1); i++) {
		ret = usg_ring_sp_enqueue_burst(r, cur_src, MAX_BULK, NULL);
		cur_src += MAX_BULK;
		if (ret != MAX_BULK)
			goto fail;
	}

	printf("Enqueue 2 objects, free entries = MAX_BULK - 2  \n");
	ret = usg_ring_sp_enqueue_burst(r, cur_src, 2, NULL);
	cur_src += 2;
	if (ret != 2)
		goto fail;

	printf("Enqueue the remaining entries = MAX_BULK - 2  \n");
	/* Always one free entry left */
	ret = usg_ring_sp_enqueue_burst(r, cur_src, MAX_BULK, NULL);
	cur_src += MAX_BULK - 3;
	if (ret != MAX_BULK - 3)
		goto fail;

	printf("Test if ring is full  \n");
	if (usg_ring_full(r) != 1)
		goto fail;

	printf("Test enqueue for a full entry  \n");
	ret = usg_ring_sp_enqueue_burst(r, cur_src, MAX_BULK, NULL);
	if (ret != 0)
		goto fail;

	printf("Test dequeue without enough objects \n");
	for (i = 0; i<RING_SIZE/MAX_BULK - 1; i++) {
		ret = usg_ring_sc_dequeue_burst(r, cur_dst, MAX_BULK, NULL);
		cur_dst += MAX_BULK;
		if (ret != MAX_BULK)
			goto fail;
	}

	/* Available memory space for the exact MAX_BULK entries */
	ret = usg_ring_sc_dequeue_burst(r, cur_dst, 2, NULL);
	cur_dst += 2;
	if (ret != 2)
		goto fail;

	ret = usg_ring_sc_dequeue_burst(r, cur_dst, MAX_BULK, NULL);
	cur_dst += MAX_BULK - 3;
	if (ret != MAX_BULK - 3)
		goto fail;

	printf("Test if ring is empty \n");
	/* Check if ring is empty */
	if (1 != usg_ring_empty(r))
		goto fail;

	/* check data */
	if (memcmp(src, dst, cur_dst - dst)) {
		test_hexdump(stdout, "src", src, cur_src - src);
		test_hexdump(stdout, "dst", dst, cur_dst - dst);
		printf("data after dequeue is not the same\n");
		goto fail;
	}

	cur_src = src;
	cur_dst = dst;

	printf("Test MP & MC basic functions \n");

	printf("enqueue 1 obj\n");
	ret = usg_ring_mp_enqueue_burst(r, cur_src, 1, NULL);
	cur_src += 1;
	if (ret != 1)
		goto fail;

	printf("enqueue 2 objs\n");
	ret = usg_ring_mp_enqueue_burst(r, cur_src, 2, NULL);
	cur_src += 2;
	if (ret != 2)
		goto fail;

	printf("enqueue MAX_BULK objs\n");
	ret = usg_ring_mp_enqueue_burst(r, cur_src, MAX_BULK, NULL);
	cur_src += MAX_BULK;
	if (ret != MAX_BULK)
		goto fail;

	printf("dequeue 1 obj\n");
	ret = usg_ring_mc_dequeue_burst(r, cur_dst, 1, NULL);
	cur_dst += 1;
	if (ret != 1)
		goto fail;

	printf("dequeue 2 objs\n");
	ret = usg_ring_mc_dequeue_burst(r, cur_dst, 2, NULL);
	cur_dst += 2;
	if (ret != 2)
		goto fail;

	printf("dequeue MAX_BULK objs\n");
	ret = usg_ring_mc_dequeue_burst(r, cur_dst, MAX_BULK, NULL);
	cur_dst += MAX_BULK;
	if (ret != MAX_BULK)
		goto fail;

	/* check data */
	if (memcmp(src, dst, cur_dst - dst)) {
		test_hexdump(stdout, "src", src, cur_src - src);
		test_hexdump(stdout, "dst", dst, cur_dst - dst);
		printf("data after dequeue is not the same\n");
		goto fail;
	}

	cur_src = src;
	cur_dst = dst;

	printf("fill and empty the ring\n");
	for (i = 0; i<RING_SIZE/MAX_BULK; i++) {
		ret = usg_ring_mp_enqueue_burst(r, cur_src, MAX_BULK, NULL);
		cur_src += MAX_BULK;
		if (ret != MAX_BULK)
			goto fail;
		ret = usg_ring_mc_dequeue_burst(r, cur_dst, MAX_BULK, NULL);
		cur_dst += MAX_BULK;
		if (ret != MAX_BULK)
			goto fail;
	}

	/* check data */
	if (memcmp(src, dst, cur_dst - dst)) {
		test_hexdump(stdout, "src", src, cur_src - src);
		test_hexdump(stdout, "dst", dst, cur_dst - dst);
		printf("data after dequeue is not the same\n");
		goto fail;
	}

	cur_src = src;
	cur_dst = dst;

	printf("Test enqueue without enough memory space \n");
	for (i = 0; i<RING_SIZE/MAX_BULK - 1; i++) {
		ret = usg_ring_mp_enqueue_burst(r, cur_src, MAX_BULK, NULL);
		cur_src += MAX_BULK;
		if (ret != MAX_BULK)
			goto fail;
	}

	/* Available memory space for the exact MAX_BULK objects */
	ret = usg_ring_mp_enqueue_burst(r, cur_src, 2, NULL);
	cur_src += 2;
	if (ret != 2)
		goto fail;

	ret = usg_ring_mp_enqueue_burst(r, cur_src, MAX_BULK, NULL);
	cur_src += MAX_BULK - 3;
	if (ret != MAX_BULK - 3)
		goto fail;


	printf("Test dequeue without enough objects \n");
	for (i = 0; i<RING_SIZE/MAX_BULK - 1; i++) {
		ret = usg_ring_mc_dequeue_burst(r, cur_dst, MAX_BULK, NULL);
		cur_dst += MAX_BULK;
		if (ret != MAX_BULK)
			goto fail;
	}

	/* Available objects - the exact MAX_BULK */
	ret = usg_ring_mc_dequeue_burst(r, cur_dst, 2, NULL);
	cur_dst += 2;
	if (ret != 2)
		goto fail;

	ret = usg_ring_mc_dequeue_burst(r, cur_dst, MAX_BULK, NULL);
	cur_dst += MAX_BULK - 3;
	if (ret != MAX_BULK - 3)
		goto fail;

	/* check data */
	if (memcmp(src, dst, cur_dst - dst)) {
		test_hexdump(stdout, "src", src, cur_src - src);
		test_hexdump(stdout, "dst", dst, cur_dst - dst);
		printf("data after dequeue is not the same\n");
		goto fail;
	}

	cur_src = src;
	cur_dst = dst;

	printf("Covering usg_ring_enqueue_burst functions \n");

	ret = usg_ring_enqueue_burst(r, cur_src, 2, NULL);
	cur_src += 2;
	if (ret != 2)
		goto fail;

	ret = usg_ring_dequeue_burst(r, cur_dst, 2, NULL);
	cur_dst += 2;
	if (ret != 2)
		goto fail;

	/* Free memory before test completed */
	free(src);
	free(dst);
	return 0;

 fail:
	free(src);
	free(dst);
	return -1;
}

/*
 * it will always fail to create ring with a wrong ring size number in this function
 */
static int
test_ring_creation_with_wrong_size(void)
{
	struct usg_ring * rp = NULL;

	/* Test if ring size is not power of 2 */
	rp = usg_ring_create(RING_SIZE + 1, 0);
	if (NULL != rp) {
		return -1;
	}

	/* Test if ring size is exceeding the limit */
	rp = usg_ring_create((USG_RING_SZ_MASK + 1), 0);
	if (NULL != rp) {
		return -1;
	}
	return 0;
}

/*
 * Test to if a non-power of 2 count causes the create
 * function to fail correctly
 */
static int
test_create_count_odd(void)
{
	struct usg_ring *r = usg_ring_create(4097, 0);
	if(r != NULL){
		return -1;
	}
	return 0;
}

/*
 * it tests some more basic ring operations
 */
static int
test_ring_basic_ex(void)
{
	int ret = -1;
	unsigned i;
	struct usg_ring *rp = NULL;
	void **obj = NULL;

	obj = calloc(RING_SIZE, sizeof(void *));
	if (obj == NULL) {
		printf("test_ring_basic_ex fail to malloc\n");
		goto fail_test;
	}

	rp = usg_ring_create(RING_SIZE, RING_F_SP_ENQ | RING_F_SC_DEQ);
	if (rp == NULL) {
		printf("test_ring_basic_ex fail to create ring\n");
		goto fail_test;
	}

	if (usg_ring_empty(rp) != 1) {
		printf("test_ring_basic_ex ring is not empty but it should be\n");
		goto fail_test;
	}

	printf("%u ring entries are now free\n", usg_ring_free_count(rp));

	for (i = 0; i < RING_SIZE; i ++) {
		usg_ring_enqueue(rp, obj[i]);
	}

	if (usg_ring_full(rp) != 1) {
		printf("test_ring_basic_ex ring is not full but it should be\n");
		goto fail_test;
	}

	for (i = 0; i < RING_SIZE; i ++) {
		usg_ring_dequeue(rp, &obj[i]);
	}

	if (usg_ring_empty(rp) != 1) {
		printf("test_ring_basic_ex ring is not empty but it should be\n");
		goto fail_test;
	}

	/* Covering the ring burst operation */
	ret = usg_ring_enqueue_burst(rp, obj, 2, NULL);
	if (ret != 2) {
		printf("test_ring_basic_ex: usg_ring_enqueue_burst fails, ret:%d \n", ret);
		goto fail_test;
	}

	ret = usg_ring_dequeue_burst(rp, obj, 2, NULL);
	if (ret != 2) {
		printf("test_ring_basic_ex: usg_ring_dequeue_burst fails, ret:%d \n", ret);
		goto fail_test;
	}

	ret = 0;
fail_test:
	usg_ring_free(rp);
	if (obj != NULL)
		free(obj);

	return ret;
}

static int
test_ring_with_exact_size(void)
{
	struct usg_ring *std_ring = NULL, *exact_sz_ring = NULL;
	void *ptr_array[16];
	static const unsigned int ring_sz = (sizeof(ptr_array) / sizeof(ptr_array[0]));
	unsigned int i;
	int ret = -1;

	std_ring = usg_ring_create(ring_sz, RING_F_SP_ENQ | RING_F_SC_DEQ);
	if (std_ring == NULL) {
		printf("%s: error, can't create std ring\n", __func__);
		goto end;
	}
	exact_sz_ring = usg_ring_create(ring_sz,
        RING_F_SP_ENQ | RING_F_SC_DEQ | RING_F_EXACT_SZ);
	if (exact_sz_ring == NULL) {
		printf("%s: error, can't create exact size ring\n", __func__);
		goto end;
	}

	/*
	 * Check that the exact size ring is bigger than the standard ring
	 */
	if (usg_ring_get_size(std_ring) >= usg_ring_get_size(exact_sz_ring)) {
		printf("%s: error, std ring (size: %u) is not smaller than exact size one (size %u)\n",
				__func__,
				usg_ring_get_size(std_ring),
				usg_ring_get_size(exact_sz_ring));
		goto end;
	}
	/*
	 * check that the exact_sz_ring can hold one more element than the
	 * standard ring. (16 vs 15 elements)
	 */
	for (i = 0; i < ring_sz - 1; i++) {
		usg_ring_enqueue(std_ring, NULL);
		usg_ring_enqueue(exact_sz_ring, NULL);
	}
	if (usg_ring_enqueue(std_ring, NULL) != -ENOBUFS) {
		printf("%s: error, unexpected successful enqueue\n", __func__);
		goto end;
	}
	if (usg_ring_enqueue(exact_sz_ring, NULL) == -ENOBUFS) {
		printf("%s: error, enqueue failed\n", __func__);
		goto end;
	}

	/* check that dequeue returns the expected number of elements */
	if (usg_ring_dequeue_burst(exact_sz_ring, ptr_array,
			(sizeof(ptr_array) / sizeof(ptr_array[0])), NULL) != ring_sz) {
		printf("%s: error, failed to dequeue expected nb of elements\n",
				__func__);
		goto end;
	}

	/* check that the capacity function returns expected value */
	if (usg_ring_get_capacity(exact_sz_ring) != ring_sz) {
		printf("%s: error, incorrect ring capacity repousgd\n",
				__func__);
		goto end;
	}

	ret = 0; /* all ok if we get here */
end:
	usg_ring_free(std_ring);
	usg_ring_free(exact_sz_ring);
	return ret;
}

int main(int argc, char **argv)
{
	struct usg_ring *r = NULL;

    set_tsc_freq();
	usg_srand(usg_rdtsc());

	/* some more basic operations */
	if (test_ring_basic_ex() < 0)
		goto test_fail;

	usg_atomic32_init(&synchro);

	r = usg_ring_create(RING_SIZE, 0);
	if (r == NULL)
		goto test_fail;

	/* burst operations */
	if (test_ring_burst_basic(r) < 0)
		goto test_fail;

	/* basic operations */
	if (test_ring_basic(r) < 0)
		goto test_fail;

	/* basic operations */
	if ( test_create_count_odd() < 0){
		printf("Test failed to detect odd count\n");
		goto test_fail;
	} else
		printf("Test detected odd count\n");

	/* test of creating ring with wrong size */
	if (test_ring_creation_with_wrong_size() < 0)
		goto test_fail;

	if (test_ring_with_exact_size() < 0)
		goto test_fail;

	usg_ring_free(r);

	return 0;

test_fail:
	usg_ring_free(r);

	return -1;
}
