/*
 * BUFFERS FOR OPERATIONS ON POLYNOMIALS
 */

/*
 * Polynomials are represented as sums of pairs <coeff, pp>
 * - the coeff is a rational number.
 * - pp is a power product (cf. power_products.h)
 *
 * In normal form, polynomials have the following properties:
 * - the coefficients are all non zero
 * - the monomials are stored in deg-lex order: lower degree
 *   monomials appear first. Monomials of equal degree are
 *   sorted in lexicographic order.
 */

#ifndef __ARITH_BUFFERS_H
#define __ARITH_BUFFERS_H

#include <stdint.h>

#include "rationals.h"
#include "object_stores.h"
#include "pprod_table.h"


/*
 * An arithmetic buffer stores a polynomial as a list of monomials
 * - list = list of pairs <coeff, pp> sorted in increasing order
 * - some of the coefficients may be zero.
 * - zero coefficients are removed by the normalize operation
 * - the list contains an end marker (with pp = end_pp),
 *   which is always the last element in the list.
 * - nterms = number of monomials in the list, excluding end marker.
 */

// element in a list of monomials
typedef struct mlist_s {
  struct mlist_s *next;
  rational_t coeff;
  pprod_t *prod;
} mlist_t;

// buffer
typedef struct arith_buffer_s {
  uint32_t nterms;        // length of the list (excluding the end marker)
  mlist_t *list;          // start of the list 
  object_store_t *store;  // for allocation of list elements
  pprod_table_t *ptbl;    // for creation of power products
} arith_buffer_t;


/*
 * Block size of an arith_buffer store
 */
#define MLIST_BANK_SIZE 64



/***********************
 * BUFFER  OPERATIONS  *
 **********************/

/*
 * Initialize store s for list elements
 */
extern void init_mlist_store(object_store_t *s);


/*
 * Delete store s: free all attached memory and clear all rationals.
 * Must not be called unless all buffers with store s are deleted.
 */
extern void delete_mlist_store(object_store_t *s);

/*
 * Initialize buffer b to the zero polynomial
 * - ptbl = attached power product table
 * - s = attached store
 */
extern void init_arith_buffer(arith_buffer_t *b, pprod_table_t *ptbl, object_store_t *s);


/*
 * Delete b and free all attached memory
 */
extern void delete_arith_buffer(arith_buffer_t *b);


/*
 * Normalize: remove any zero monomials from b
 */
extern void arith_buffer_normalize(arith_buffer_t *b);



/*************
 *  QUERIES  *
 ************/

/*
 * Size = number of terms.
 */
static inline uint32_t arith_buffer_size(arith_buffer_t *b) {
  return b->nterms;
}


/*
 * Check whether b is zero
 * - b must be normalized
 */
static inline bool arith_buffer_is_zero(arith_buffer_t *b) {
  return b->nterms == 0;
}


/*
 * Check whether b is constant
 * - b must be normalized
 */
extern bool arith_buffer_is_constant(arith_buffer_t *b);


/*
 * Check whether b is constant and nonzero
 * - b must be normalized
 */
extern bool arith_buffer_is_nonzero(arith_buffer_t *b);


/*
 * Check whether b is constant and positive, negative, etc.
 * - b must be normalized
 */
extern bool arith_buffer_is_pos(arith_buffer_t *b);
extern bool arith_buffer_is_neg(arith_buffer_t *b);
extern bool arith_buffer_is_nonneg(arith_buffer_t *b);
extern bool arith_buffer_is_nonpos(arith_buffer_t *b);


/*
 * Get degree of polynomial in buffer b.
 * - b must be normalized
 * - returns 0 if b is zero
 */
extern uint32_t arith_buffer_degree(arith_buffer_t *b);


/*
 * Degree of variable x in b
 * - return largest d such that x^d occurs in b
 * - return 0 if x does not occur in b
 */
extern uint32_t arith_buffer_var_degree(arith_buffer_t *b, int32_t x);


/*
 * Main term = maximal power product of b in the deg-lex ordering.
 * - b must be normalized and non zero
 */
extern pprod_t *arith_buffer_main_term(arith_buffer_t *b);


/*
 * Main monomial = monomial whose pp is the main term
 * - b must be normalized and non zero
 * - this returns the last element in b's monomial list
 */
extern mlist_t *arith_buffer_main_mono(arith_buffer_t *b);


/*
 * Check whether b1 and b2 are equal
 * - both must be normalized and use the same ptbl
 */
extern bool arith_buffer_equal(arith_buffer_t *b1, arith_buffer_t *b2);



/******************************
 *  POLYNOMIAL CONSTRUCTION   *
 *****************************/

/*
 * All operations update the first argument b.
 * They do not ensure that b is normalized.
 *
 * Some operations have a power product r as argument.
 * The power product r must be defined in b's internal 
 * power-product table: either r is empty_pp, or r is
 * a tagged variable, or r occurs in b->ptbl.
 *
 * Some operations use another buffer b1. In such cases,
 * b and b1 must have the same power-product table.
 * Unless otherwise indicated, the operations work correctly 
 * if b1 is equal to b (but this use is not recommended).
 */

/*
 * Reset b to the zero polynomial
 */
extern void arith_buffer_reset(arith_buffer_t *b);


/*
 * Multiply b by -1
 */
extern void arith_buffer_negate(arith_buffer_t *b);


/*
 * Multiply b by constant a
 */
extern void arith_buffer_mul_const(arith_buffer_t *b, rational_t *a);


/*
 * Divide b by constant a
 * - a must be non-zero
 */
extern void arith_buffer_div_const(arith_buffer_t *b, rational_t *a);


/*
 * Multiply b by power product r
 */
extern void arith_buffer_mul_pp(arith_buffer_t *b, pprod_t *r);


/*
 * Multiply b by (- r)
 */
extern void arith_buffer_mul_negpp(arith_buffer_t *b, pprod_t *r);


/*
 * Multiply b by a * r
 */
extern void arith_buffer_mul_mono(arith_buffer_t *b, rational_t *a, pprod_t *r);



/*
 * Add constant a to b
 */
extern void arith_buffer_add_const(arith_buffer_t *b, rational_t *a);


/*
 * Add constant (-a) to b
 */
extern void arith_buffer_sub_const(arith_buffer_t *b, rational_t *a);


/*
 * Add r to b
 */
extern void arith_buffer_add_pp(arith_buffer_t *b, pprod_t *r);


/*
 * Add -r to b
 */
extern void arith_buffer_sub_pp(arith_buffer_t *b, pprod_t *r);


/*
 * Add a * r to b
 */
extern void arith_buffer_add_mono(arith_buffer_t *b, rational_t *a, pprod_t *r);


/*
 * Add -a * r to b
 */
extern void arith_buffer_sub_mono(arith_buffer_t *b, rational_t *a, pprod_t *r);


/*
 * Add b1 to b
 */
extern void arith_buffer_add_buffer(arith_buffer_t *b, arith_buffer_t *b1);


/*
 * Add (-b1) to b
 */
extern void arith_buffer_sub_buffer(arith_buffer_t *b, arith_buffer_t *b1);


/*
 * Multiply b by b1
 * - b1 must be different from b
 */
extern void arith_buffer_mul_buffer(arith_buffer_t *b, arith_buffer_t *b1);


/*
 * Compute the square of b
 */
extern void arith_buffer_square(arith_buffer_t *b);


/*
 * Add a * b1 to b
 */
extern void arith_buffer_add_const_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, rational_t *a);


/*
 * Add (-a) * b1 to b
 */
extern void arith_buffer_sub_const_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, rational_t *a);


/*
 * Add r * b1 to b
 */
extern void arith_buffer_add_pp_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, pprod_t *r);


/*
 * Add - r * b1 to b
 */
extern void arith_buffer_sub_pp_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, pprod_t *r);


/*
 * Add a * r * b1 to b
 */
extern void arith_buffer_add_mono_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, rational_t *a, pprod_t *r);

/*
 * Add -a * r * b1 to b
 */
extern void arith_buffer_sub_mono_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, rational_t *a, pprod_t *r);


/*
 * Add b1 * b2 to b
 * - b1 and b2 must be distinct from b (but b1 may be equal to b2)
 */
extern void arith_buffer_add_buffer_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, arith_buffer_t *b2);


/*
 * Add - b1 * b2 to b
 * - b1 and b2 must be distinct from b (but b1 may be equal to b2)
 */
extern void arith_buffer_sub_buffer_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, arith_buffer_t *b2);




/*
 * SHORT CUTS
 */

/*
 * All operations that take a power product r have a variant that takes a single
 * variable x as argument.
 */

/*
 * Multiply b by x
 */
static inline void arith_buffer_mul_var(arith_buffer_t *b, int32_t x) {
  arith_buffer_mul_pp(b, var_pp(x));
}


/*
 * Multiply b by (- x)
 */
static inline void arith_buffer_mul_negvar(arith_buffer_t *b, int32_t x) {
  arith_buffer_mul_negpp(b, var_pp(x));
}


/*
 * Add x to b
 */
static inline void arith_buffer_add_var(arith_buffer_t *b, int32_t x) {
  arith_buffer_add_pp(b, var_pp(x));
}


/*
 * Add -x to b
 */
static inline void arith_buffer_sub_var(arith_buffer_t *b, int32_t x) {
  arith_buffer_sub_pp(b, var_pp(x));
}


/*
 * Add a * x to b
 */
static inline void arith_buffer_add_varmono(arith_buffer_t *b, rational_t *a, int32_t x) {
  arith_buffer_add_mono(b, a, var_pp(x));
}


/*
 * Add -a * x to b
 */
static inline void arith_buffer_sub_varmono(arith_buffer_t *b, rational_t *a, int32_t x) {
  arith_buffer_sub_mono(b, a, var_pp(x));
}


/*
 * Add x * b1 to b
 */
static inline void arith_buffer_add_var_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, int32_t x) {
  arith_buffer_add_pp_times_buffer(b, b1, var_pp(x));
}


/*
 * Add - x * b1 to b
 */
static inline void arith_buffer_sub_var_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, int32_t x) {
  arith_buffer_sub_pp_times_buffer(b, b1, var_pp(x));
}


/*
 * Add a * x * b1 to b
 */
static inline void 
arith_buffer_add_varmono_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, rational_t *a, int32_t x) {
  arith_buffer_add_mono_times_buffer(b, b1, a, var_pp(x));
}

/*
 * Add -a * x * b1 to b
 */
static inline void
arith_buffer_sub_varmono_times_buffer(arith_buffer_t *b, arith_buffer_t *b1, rational_t *a, int32_t x) {
  arith_buffer_sub_mono_times_buffer(b, b1, a, var_pp(x));
}




#endif /* __ARITH_BUFFERS_H */
