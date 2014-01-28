/*
 * scope_exit.c
 *
 *  Created on: Jan 27, 2014
 *      Author: nathan
 *
 *  Function implementations.
 */

#include "csope_impl.h"

#include <stdlib.h>
#include <unistd.h>

static SE_ScopeData** se_global_scope_data()
{
	static __thread SE_ScopeData* global = NULL;
	return &global;
}

static inline SE_ScopeData* se_make_scope_data(void* new_scope_data_block,
		SE_Action action)
{
	SE_ScopeData** scope_data_ptr = se_global_scope_data();

	SE_ScopeData* new_scope_data = (SE_ScopeData*)new_scope_data_block;
	SE_ScopeData* old_scope_data = *scope_data_ptr;

	new_scope_data->prev = old_scope_data;
	new_scope_data->action = action;
	new_scope_data->top = false;

	*scope_data_ptr = new_scope_data;

	return new_scope_data;
}

void se_scope_begin()
{
	SE_ScopeData* top = *se_global_scope_data();
	if(top) top->top = true;
}

void* se_add_delete(void* alloca_block, void* ptr,
		void (*deleter)(void*), SE_Action action)
{
	SE_ScopeData* new_scope_data = se_make_scope_data(alloca_block, action);

	new_scope_data->deleter = deleter;
	new_scope_data->ptr = ptr;
	return ptr;
}

int se_add_clean(void* alloca_block, int fd,
		void (*cleaner)(int), SE_Action action)
{
	SE_ScopeData* new_scope_data = se_make_scope_data(alloca_block, action);

	new_scope_data->fd = fd;
	new_scope_data->cleaner = cleaner;
	return fd;
}

void se_scope_end()
{
	SE_ScopeData** scope_data = se_global_scope_data();
	SE_ScopeData* next = *scope_data;

	for(; next && !next->top; next = next->prev)
	{
		switch(next->action)
		{
		case se_action_free:
		case se_action_delete:
			if(next->deleter)
				next->deleter(next->ptr);
			break;
		case se_action_close:
		case se_action_clean:
			if(next->cleaner)
				next->cleaner(next->fd);
			break;
		}

		switch(next->action)
		{
		case se_action_free:
			free(next->ptr);
			break;
		case se_action_close:
			close(next->fd);
			break;
		default:
			break;
		}
	}

	if(next) next->top = false;
	*scope_data = next;
}
