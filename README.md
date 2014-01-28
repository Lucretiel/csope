csope
=====

Basic scope management for C.

Summary
-------

### Overview

- Call `se_scope_begin()` to create a new scope
- Call `se_scope_end()` to clean all the relevant data since the call to
`se_scope_begin`
- Alternatively, call `return se_scope_return(value)` to get a value and then
clean.
- Once a scope is created, use the registration functions (see below) to
create various cleanup actions. These are called in reverse order that they
are created. Currently supported cleanup actions include freeing memory, calling
functions, and closing linux file descriptors.

### Setting memory to be cleaned

- Call `se_free(ptr)` to mark a pointer to be `free`d
- Call `se_delete(ptr, deleter)` to register a deleter function, which is a
function pointer of type `void(*)(void*)`.
- Call `se_delete_free(ptr, deleter)` to register a deleter function. The ptr is
`free`d after this function is called
- Call `se_close(fd)` to mark a linux file descriptor to be `close`d.
- Call `se_clean(fd, cleaner)` to register a cleaner function, which is a function
pointer of type `void(*)(int)`. 
- Call `se_clean_close(fd, cleaner)` to register a cleaner function. The
descriptor is `close`d after this function is called.

### Helpers

- `se_malloc(size)` allocates some memory and marks it to be `free`d
- `se_make(type)` allocates some memory for a type, zero-initializes it, and
marks it to be `free`d
- `se_new(type, destructor)` allocates some memory for a type, zero initializes
it, registers a destructor function to be called at scope end, and marks it to
be `free`d after the destructor is called.

Example
-------

```C
#include <stdio.h>
#include "csope.h"

void array_2d_stuff(int dx, int dy)
{
    /*create a new scope*/
	se_scope_begin();
	
	/*allocate a pointer, and mark it to be free'd at the end*/
    int** backbone = se_free(malloc(dx * sizeof(int*)));
    for(int x = 0; x < dx; ++x)
    {
        /*se_malloc is a convenience macro for se_free(malloc(...))*/
        backbone[x] = se_malloc(dy * sizeof(int));
        for(int y = 0; y < dy; ++y)
        {
            backbone[x][y] = rand() % 10;
        }
    }
    
    for(int x = 0; x < dx; ++x)
    {
        for(int y = 0; y < dy; ++y)
        {
            printf("%d", backbone[x][y]);
        }
        printf("\n");
    }
    
    /*
     * call se_scope_return (with return value) or se_scope_end (no return
     * value) to end the scope, freeing any memory and calling any
     * registered destructors.
     */
    return se_scope_return(backbone[x][y]);
}

int main()
{
    int value = array_2d_stuff(4, 5);
    printf("Got %d\n", value);
    return 0;
}
```

_Output:_

```
36753
56291
27093
60626
Got 3
```

TODO
----

- Platform independence. Currently depends on GCC compiler extensions.
- Move linux-specific stuff (close, etc) to a separate header, or check a macro
to only compile on linux.
- Equivelent helpers for opening files
- Add a `void(*)()` (function with no arguments) as a cleanup action.