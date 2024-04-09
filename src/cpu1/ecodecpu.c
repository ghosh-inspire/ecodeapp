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
static unsigned char __attribute__((section(".cpu1_data"))) __attribute__((used)) cb_cpu1_data[(ECODE_SHM_SIZE_BYTES / ECODE_CPU1_MAU)];
#else
static unsigned char __attribute__((section(".cpu1_data"))) __attribute__((used)) cb_cpu1_data[ECODE_SHM_SIZE_BYTES];
#endif

static cb_t cb_cpu1;

static char* cpu1_cb_write_params[] = {
	"11", "22", "33", "44", "55", "66", "77", "88", "99", NULL
};

static MunitParameterEnum cpu1_params[] = {
	{ "cpu1-cb-write-params", cpu1_cb_write_params },
	{ NULL, NULL },
};

MunitResult cpu1_cb_write_ofst_test(const MunitParameter params[], void* user_data) {

	cb_t *cb = (cb_t *)user_data;
	unsigned char rand_num = (unsigned char)munit_rand_int_range(0, 255);

	(void)params;
	munit_assert_ptr_not_equal(cb, NULL);

	if (cb_write(cb, rand_num) != MUNIT_OK) {
		munit_assert_int(ECODE_GET_READ_OFFSET(cb), ==, (ECODE_GET_WRITE_OFFSET(cb) + 1) % ECODE_SHM_DATA_SIZE_BYTES);
	}

	munit_assert_int(ECODE_GET_READ_OFFSET(cb), <, ECODE_SHM_DATA_SIZE_BYTES);
	munit_assert_int(ECODE_GET_WRITE_OFFSET(cb), <, ECODE_SHM_DATA_SIZE_BYTES);
	munit_assert_int(ECODE_GET_READ_OFFSET(cb), >=, 0);
	munit_assert_int(ECODE_GET_WRITE_OFFSET(cb), >=, 0);

	usleep(10 * 1000);
	return MUNIT_OK;
}

MunitResult cpu1_cb_write(const MunitParameter params[], void* user_data) {

	cb_t *cb = (cb_t *)user_data;
	const char *cpu1_param = munit_parameters_get(params, "cpu1-cb-write-params");

	munit_assert_ptr_not_equal(cb, NULL);

	if (cb_write(cb, atoi(cpu1_param)) != MUNIT_OK) {
		munit_log(MUNIT_LOG_WARNING, "unable to write any more data\n");
		return MUNIT_SKIP;
	}

	return MUNIT_OK;
}

MunitResult cpu1_cb_show_only(const MunitParameter params[], void* user_data) {

	cb_t *cb = (cb_t *)user_data;

	munit_assert_ptr_not_equal(cb, NULL);

	(void)params;

	debug_print(cb, 0);

	return MUNIT_OK;
}

MunitResult cpu1_cb_show_clear(const MunitParameter params[], void* user_data) {

	cb_t *cb = (cb_t *)user_data;

	munit_assert_ptr_not_equal(cb, NULL);

	(void)params;

	debug_print(cb, 1);

	return MUNIT_OK;
}

static void* cpu1_setup(const MunitParameter params[], void* user_data) {

	int shm_id;
	key_t shm_key;
	unsigned char *shm_ptr;

	(void)params;
	(void)user_data;

	shm_key = ftok("/dev/null", 'R');

	shm_id = shmget(shm_key, ECODE_SHM_SIZE_BYTES, IPC_CREAT | S_IRUSR | S_IWUSR);

#if !defined(NO_HARDWARE_BUILD)
	shm_ptr = (unsigned char*) shmat(shm_id, cb_cpu1_data, SHM_REMAP);
	CB_INIT(cb_cpu1, shm_ptr, ECODE_CPU1_MAU);
#else
	shm_ptr = (unsigned char*) shmat(shm_id, 0, 0);
	CB_INIT(cb_cpu1, shm_ptr, 1);
#endif

	return &cb_cpu1;
}

static void cpu1_teardown(void* user_data) {

	cb_t *cb = (cb_t *)user_data;

	shmdt(cb->rwidx);
}

MunitTest cpu1_tests[] = {
	{
		"/cpu1-cb-write", /* name */
		cpu1_cb_write, /* test */
		cpu1_setup, /* setup */
		cpu1_teardown, /* tear_down */
		MUNIT_TEST_OPTION_NONE, /* options */
		cpu1_params /* parameters */
	},
	{
		"/cpu1-cb-write-ofst-test", /* name */
		cpu1_cb_write_ofst_test, /* test */
		cpu1_setup, /* setup */
		cpu1_teardown, /* tear_down */
		MUNIT_TEST_OPTION_NONE, /* options */
		NULL /* parameters */
	},
	{
		"/cpu1-cb-show-only", /* name */
		cpu1_cb_show_only, /* test */
		cpu1_setup, /* setup */
		cpu1_teardown, /* tear_down */
		MUNIT_TEST_OPTION_NONE, /* options */
		NULL /* parameters */
	},
	{
		"/cpu1-cb-show-clear", /* name */
		cpu1_cb_show_clear, /* test */
		cpu1_setup, /* setup */
		cpu1_teardown, /* tear_down */
		MUNIT_TEST_OPTION_NONE, /* options */
		NULL /* parameters */
	},

	{ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite cpu1_suite = {
	"/cpu1-tests", /* name */
	cpu1_tests, /* tests */
	NULL, /* suites */
	1, /* iterations */
	MUNIT_SUITE_OPTION_NONE /* options */
};

int main(int argc, char *argv[])
{
	return munit_suite_main(&cpu1_suite, NULL, argc, argv);
}
