#include <FreeRTOS.h>
#include <task.h>
#include <malloc.h>

// Define the 'new' operator for C++ to use the freeRTOS memory management
// functions. THIS IS NOT OPTIONAL!
//
#define USE_FREERTOS
void *operator new(size_t size){
	void *p;
#ifdef USE_FREERTOS
	if(1 || uxTaskGetNumberOfTasks())
		p=pvPortMalloc(size);
	else
		p=malloc(size);
#else
		p=malloc(size);
#endif

#ifdef __EXCEPTIONS
	if (p==0) // did pvPortMalloc succeed?
		throw std::bad_alloc(); // ANSI/ISO compliant behavior
#endif
	return p;
}



//
// Define the 'delete' operator for C++ to use the freeRTOS memory management
// functions. THIS IS NOT OPTIONAL!
//
void operator delete(void *p){
#ifdef USE_FREERTOS
	if(1 || uxTaskGetNumberOfTasks())
		vPortFree( p );
	else
		free( p );
#else
		free( p );
#endif
	p = NULL;
}


void *operator new[](size_t size){
	void *p;
#ifdef USE_FREERTOS
	if(1 || uxTaskGetNumberOfTasks())
		p=pvPortMalloc(size);
	else
		p=malloc(size);
#else
		p=malloc(size);
#endif
#ifdef __EXCEPTIONS
	if (p==0) // did pvPortMalloc succeed?
		throw std::bad_alloc(); // ANSI/ISO compliant behavior
#endif
	return p;
}


// Define the 'delete' operator for C++ to use the freeRTOS memory management
// functions. THIS IS NOT OPTIONAL!
//
void operator delete[](void *p){
#ifdef USE_FREERTOS
	if(1 || uxTaskGetNumberOfTasks())
		vPortFree( p );
	else
		free( p );
#else
		free( p );
#endif
	p = NULL;
}



/* Optionally you can override the 'nothrow' versions as well.
   This is useful if you want to catch failed allocs with your
   own debug code, or keep track of heap usage for example,
   rather than just eliminate exceptions.
 */
//
//
//void* operator new(std::size_t size, const std::nothrowt&) {
//	return malloc(size);
//}
//
//
//
//void* operator new {
//	return malloc(size);
//}
//
//
//
//void operator delete(void* ptr, const std::nothrow_t&) {
//	free(ptr);
//}
//
//
//
//void operator delete {
//	free(ptr);
//}
//
