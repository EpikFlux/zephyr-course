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
	zassert_equal(rb_push(99), 0, "push should succeed");
	zassert_equal(rb_count(), 1, "count should be 1 after push");

	rb_init(4);

	zassert_true(rb_is_empty(), "Buffer must be empty after reinit");
	zassert_equal(rb_count(), 0, "Count must be 0 after reinit");
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
	int v = 0;

	zassert_equal(rb_push(42), 0, "push should succeed");
	zassert_equal(rb_pop(&v), 0, "pop should succeed");
	zassert_equal(v, 42, "popped value should be 42, got %d", v);
	zassert_true(rb_is_empty(), "Buffer must be empty after pop");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	int v = 0;

	zassert_equal(rb_push(1), 0, "push(1) should succeed");
	zassert_equal(rb_push(2), 0, "push(2) should succeed");
	zassert_equal(rb_push(3), 0, "push(3) should succeed");

	zassert_equal(rb_pop(&v), 0, "first pop should succeed");
	zassert_equal(v, 1, "first pop should yield 1, got %d", v);
	zassert_equal(rb_pop(&v), 0, "second pop should succeed");
	zassert_equal(v, 2, "second pop should yield 2, got %d", v);
	zassert_equal(rb_pop(&v), 0, "third pop should succeed");
	zassert_equal(v, 3, "third pop should yield 3, got %d", v);

	zassert_true(rb_is_empty(), "Buffer must be empty after three pops");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	for (int i = 1; i <= 4; i++) {
		zassert_equal(rb_push(i), 0, "push(%d) should succeed", i);
	}
	zassert_true(rb_is_full(), "Buffer must be full after 4 pushes");

	zassert_equal(rb_push(99), -ENOSPC, "push on full buffer must return -ENOSPC");
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
	int v = 0;

	zassert_equal(rb_push(7), 0, "push should succeed");

	zassert_equal(rb_peek(&v), 0, "first peek should succeed");
	zassert_equal(v, 7, "first peek should yield 7, got %d", v);

	v = 0; /* make sure the second peek really writes the value */
	zassert_equal(rb_peek(&v), 0, "second peek should succeed");
	zassert_equal(v, 7, "second peek should yield 7, got %d", v);

	zassert_equal(rb_count(), 1, "peek must not consume the element");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	zassert_equal(rb_pop(NULL), -EINVAL, "pop(NULL) must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	for (int i = 1; i <= 4; i++) {
		zassert_equal(rb_push(i), 0, "push(%d) should succeed", i);
	}
	zassert_true(rb_is_full(), "Buffer must be full");
	zassert_equal(rb_count(), 4, "Count must be 4");
}
