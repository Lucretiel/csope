/*
 * scope_exit.h
 *
 *  Created on: Jan 27, 2014
 *      Author: nathan
 *
 *  This library allows for basic scope semantics in C. This file contains the
 *  whole interface intended to be used by the client.
 */

#pragma once

#include <stdlib.h>

#include "csope_impl.h"

/* SCOPE DECLARATIONS */
/* Place this at the beginning of the function you wish to scope */
void se_scope_begin();

/* Place this wherever the function may exit */
void se_scope_end();

/* Return this to return a value; ie return se_scope_return(value); */
#define se_scope_return(VALUE) ({ \
	typeof(VALUE) _ret = (VALUE); \
	se_scope_end(); \
	_ret; })

/* POINTER CLEANERS */
/* Mark a pointer to be free'd */
#define se_free(PTR) SE_CALL_DELETE(PTR, NULL, se_action_free)

/* Mark a pointer to be cleaned up */
#define se_delete(PTR, DELETER) SE_CALL_DELETE(PTR, DELETER, se_action_delete)

/* Mark a pointer to be cleaned up and freed */
#define se_free_delete(PTR, DELETER) SE_CALL_DELETE(PTR, DELETER, se_action_free)

/* FD CLEANERS */
/* Mark an fd to be closed */
#define se_close(FD) SE_CALL_CLEAN(FD, NULL, se_action_close)

/* Mark an fd to be cleaned */
#define se_clean(FD, CLEANER) SE_CALL_CLEAN(FD, CLEANER, se_action_clean)

/* Mark an fd to be cleaned and closed */
#define se_close_clean(FD, CLEANER) SE_CALL_CLEAN(FD, CLEANER, se_action_close)

/* Allocators. Mark their pointers to be free'd */
/* Basic malloc */
#define se_malloc(SIZE) se_free(malloc(SIZE))

/* All other allocators zero-initialize */
/* Allocates for a type */
#define se_make(TYPE) (TYPE*)se_free(calloc(1, sizeof(TYPE)))

/* Allocate for type, assign destructor */
#define se_new(TYPE, DESTRUCTOR) (TYPE*)se_free_delete( \
		calloc(1, sizeof(TYPE)), (DESTRUCTOR))
