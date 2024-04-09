#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ecode.h>
#include <munit.h>

#if !defined(NO_HARDWARE_BUILD)
static unsigned char __attribute__((section(".cpu2_data"))) __attribute__((used)) cb_cpu2_data[(ECODE_SHM_SIZE_BYTES / ECODE_CPU2_MAU)];
#else
static unsigned char __attribute__((section(".cpu2_data"))) __attribute__((used)) cb_cpu2_data[ECODE_SHM_SIZE_BYTES];
#endif

static cb_t cb_cpu2;

MunitResult cpu2_cb_read_ofst_test(const MunitParameter params[], void* user_data) {

	cb_t *cb = (cb_t *)user_data;
	unsigned char data = 0;

	(void)params;
	munit_assert_ptr_not_equal(cb, NULL);

	if (cb_read(cb, &data) != MUNIT_OK) {
		munit_assert_int(ECODE_GET_READ_OFFSET(cb), <=, ECODE_GET_WRITE_OFFSET(cb));
		munit_assert_int((ECODE_GET_WRITE_OFFSET(cb) ==
				((ECODE_GET_READ_OFFSET(cb) + 1) % ECODE_SHM_DATA_SIZE_BYTES)), ||, \
				(ECODE_GET_READ_OFFSET(cb) == ECODE_GET_WRITE_OFFSET(cb)));
	}

	munit_assert_int(ECODE_GET_READ_OFFSET(cb), <, ECODE_SHM_DATA_SIZE_BYTES);
	munit_assert_int(ECODE_GET_WRITE_OFFSET(cb), <, ECODE_SHM_DATA_SIZE_BYTES);
	munit_assert_int(ECODE_GET_READ_OFFSET(cb), >=, 0);
	munit_assert_int(ECODE_GET_WRITE_OFFSET(cb), >=, 0);

	usleep(10 * 1000);
	return MUNIT_OK;
}

MunitResult cpu2_cb_read(const MunitParameter params[], void* user_data) {

	cb_t *cb = (cb_t *)user_data;
	unsigned char data = 0;

	munit_assert_ptr_not_equal(cb, NULL);

	(void)params;

	if (cb_read(cb, &data) != MUNIT_OK) {
		munit_log(MUNIT_LOG_WARNING, "unable to read any more data\n");
		munit_assert_int(ECODE_GET_READ_OFFSET(cb), <=, ECODE_GET_WRITE_OFFSET(cb));
		return MUNIT_SKIP;
	} else {
		printf(" data: %d\n", data);
	}

	return MUNIT_OK;
}

MunitResult cpu2_cb_show_only(const MunitParameter params[], void* user_data) {

	cb_t *cb = (cb_t *)user_data;

	munit_assert_ptr_not_equal(cb, NULL);

	(void)params;

	debug_print(cb, 0);

	return MUNIT_OK;
}

MunitResult cpu2_cb_show_clear(const MunitParameter params[], void* user_data) {

	cb_t *cb = (cb_t *)user_data;

	munit_assert_ptr_not_equal(cb, NULL);

	(void)params;

	debug_print(cb, 1);

	return MUNIT_OK;
}

static void* cpu2_setup(const MunitParameter params[], void* user_data) {

	int shm_id;
	key_t shm_key;
	unsigned char *shm_ptr;

	(void)params;
	(void)user_data;

	shm_key = ftok("/dev/null", 'R');

	shm_id = shmget(shm_key, ECODE_SHM_SIZE_BYTES, 0); 

#if !defined(NO_HARDWARE_BUILD)
	shm_ptr = (unsigned char*) shmat(shm_id, cb_cpu2_data, SHM_REMAP);
	CB_INIT(cb_cpu2, shm_ptr, ECODE_CPU2_MAU);
#else
	shm_ptr = (unsigned char*) shmat(shm_id, 0, 0);
	CB_INIT(cb_cpu2, shm_ptr, 2);
#endif

	return &cb_cpu2;
}

static void cpu2_teardown(void* user_data) {

	cb_t *cb = (cb_t *)user_data;
	shmdt(cb->rwidx);
}

MunitTest cpu2_tests[] = {
	{
		"/cpu2-cb-read", /* name */
		cpu2_cb_read, /* test */
		cpu2_setup, /* setup */
		cpu2_teardown, /* tear_down */
		MUNIT_TEST_OPTION_NONE, /* options */
		NULL /* parameters */
	},
	{
		"/cpu2-cb-read-ofst-test", /* name */
		cpu2_cb_read_ofst_test, /* test */
		cpu2_setup, /* setup */
		cpu2_teardown, /* tear_down */
		MUNIT_TEST_OPTION_NONE, /* options */
		NULL /* parameters */
	},
	{
		"/cpu2-cb-show-only", /* name */
		cpu2_cb_show_only, /* test */
		cpu2_setup, /* setup */
		cpu2_teardown, /* tear_down */
		MUNIT_TEST_OPTION_NONE, /* options */
		NULL /* parameters */
	},
	{
		"/cpu2-cb-show-clear", /* name */
		cpu2_cb_show_clear, /* test */
		cpu2_setup, /* setup */
		cpu2_teardown, /* tear_down */
		MUNIT_TEST_OPTION_NONE, /* options */
		NULL /* parameters */
	},

	{ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite cpu2_suite = {
	"/cpu2-tests", /* name */
	cpu2_tests, /* tests */
	NULL, /* suites */
	1, /* iterations */
	MUNIT_SUITE_OPTION_NONE /* options */
};

int main(int argc, char *argv[])
{
	return munit_suite_main(&cpu2_suite, NULL, argc, argv);
}
