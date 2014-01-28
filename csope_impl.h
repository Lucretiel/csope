/*
 * scope_exit_impl.h
 *
 *  Created on: Jan 27, 2014
 *      Author: nathan
 *
 *  Internal helper macros and other things not part of the published client
 *  interface
 */

#pragma once

#include <alloca.h>
#include <stdbool.h>
/* Types and typedefs */

/*
 * se_action_delete: Call the deleter function only
 * se_action_free: Call the deleter and free
 * se_action_clean: Call the cleaner function only
 * se_action_close: Call the cleaner and close
 *
 * The deleter/cleaner is ignored if it is NULL
 */

typedef enum {
	se_action_delete,
	se_action_free,
	se_action_clean,
	se_action_close}
SE_Action;

typedef struct SE_ScopeData_ {
	struct SE_ScopeData_* prev;
	SE_Action action;
	bool top; //TODO: optimize this into a bit in SE_Action action
	union {
		struct {
			void (*deleter)(void*);
			void* ptr;
		};
		struct {
			void (*cleaner)(int);
			int fd;
		};
	};
} SE_ScopeData;

/* Functions */
void* se_add_delete(void* alloca_block, void* ptr, void (*deleter)(void*),
		SE_Action action);

int se_add_clean(void* alloca_block, int fd, void (*cleaner)(int),
		SE_Action action);

/* Helper Macros */

#define SE_CALL(SE_FUNC, OBJ, FUNC, ACTION) SE_FUNC( \
		alloca(sizeof(SE_ScopeData)), (OBJ), (FUNC), (ACTION))

#define SE_CALL_DELETE(PTR, DELETER, ACTION) SE_CALL( \
		se_add_delete, (PTR), (DELETER), (ACTION))

#define SE_CALL_CLEAN(FD, CLEANER, ACTION) SE_CALL( \
		se_add_clean, (FD), (CLEANER), (ACTION))
