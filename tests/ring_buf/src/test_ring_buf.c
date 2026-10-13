/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	/* TODO(l8-task1): Push a value, call rb_init(4) again, then
	 * verify the buffer is empty and count is 0.
	 * See TEST_SPEC.md "Suite ring_buf_init" #2.
	 */
	// ztest_test_skip();

	// l8-task1: a pushed value must be fully discarded by a second rb_init()
	zassert_equal(rb_push(99), 0, "push must succeed");

	rb_init(4);

	zassert_true(rb_is_empty(), "Reinit must empty the buffer");
	zassert_equal(rb_count(), 0, "Reinit must reset count to 0");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	/* TODO(l8-task1): rb_push(42), rb_pop(&v) -> v == 42, buffer empty after.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #1.
	 */
	// ztest_test_skip();

	// l8-task1: one value round-trips through push/pop and leaves the buffer empty
	int v = 0;

	zassert_equal(rb_push(42), 0, "push must succeed");
	zassert_equal(rb_pop(&v), 0, "pop must succeed");
	zassert_equal(v, 42, "popped value must be 42");
	zassert_true(rb_is_empty(), "Buffer must be empty after pop");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	/* TODO(l8-task1): rb_push(1), rb_push(2), rb_push(3); pop three times
	 * and verify the values come out as 1, 2, 3 in that order.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #2.
	 */
	// ztest_test_skip();

	// l8-task1: values must pop out in the same order they were pushed (FIFO)
	int v = 0;

	zassert_equal(rb_push(1), 0, "push(1) must succeed");
	zassert_equal(rb_push(2), 0, "push(2) must succeed");
	zassert_equal(rb_push(3), 0, "push(3) must succeed");

	zassert_equal(rb_pop(&v), 0, "1st pop must succeed");
	zassert_equal(v, 1, "1st pop must yield 1");
	zassert_equal(rb_pop(&v), 0, "2nd pop must succeed");
	zassert_equal(v, 2, "2nd pop must yield 2");
	zassert_equal(rb_pop(&v), 0, "3rd pop must succeed");
	zassert_equal(v, 3, "3rd pop must yield 3");

	zassert_true(rb_is_empty(), "Buffer must be empty after 3 pops");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	/* TODO(l8-task1): Fill the buffer to its capacity of 4, then push
	 * one more value -> -ENOSPC.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #3.
	 */
	// ztest_test_skip();

	// l8-task1: pushing into a full buffer fails with -ENOSPC and changes nothing
	for (int i = 1; i <= 4; i++) {
		zassert_equal(rb_push(i), 0, "push(%d) must succeed", i);
	}
	zassert_true(rb_is_full(), "Buffer must be full after 4 pushes");

	zassert_equal(rb_push(99), -ENOSPC, "push into full buffer must return -ENOSPC");
	zassert_equal(rb_count(), 4, "Rejected push must not change count");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	/* TODO(l8-task1): rb_push(7); rb_peek(&v) -> v == 7; rb_peek(&v) again
	 * -> v == 7; rb_count() still == 1.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #1.
	 */
	// ztest_test_skip();

	// l8-task1: peek returns the oldest value without removing it
	int v = 0;

	zassert_equal(rb_push(7), 0, "push must succeed");

	zassert_equal(rb_peek(&v), 0, "1st peek must succeed");
	zassert_equal(v, 7, "1st peek must yield 7");

	v = 0;  // clear so the 2nd peek has to write 7 again
	zassert_equal(rb_peek(&v), 0, "2nd peek must succeed");
	zassert_equal(v, 7, "2nd peek must yield 7");

	zassert_equal(rb_count(), 1, "Peek must not consume the element");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	/* TODO(l8-task1): rb_pop(NULL) -> -EINVAL.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #2.
	 */
	// ztest_test_skip();

	// l8-task1: pop with a NULL output pointer is rejected with -EINVAL
	zassert_equal(rb_pop(NULL), -EINVAL, "pop(NULL) must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	/* TODO(l8-task1): push 4 values -> rb_is_full() == true, rb_count() == 4.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #3.
	 */
	// ztest_test_skip();

	// l8-task1: filling to capacity (4) makes rb_is_full() true
	for (int i = 1; i <= 4; i++) {
		zassert_equal(rb_push(i), 0, "push(%d) must succeed", i);
	}

	zassert_true(rb_is_full(), "Buffer must be full after 4 pushes");
	zassert_equal(rb_count(), 4, "Count must be 4 after filling");
}
