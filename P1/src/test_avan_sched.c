/* FEEL FREE TO EDIT THIS FILE
 * - test_schedule.c (Trilby VM)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
// Local Includes
#include "vm_support.h" // Gives abort_error, print_warning, print_status, print_debug commands
#include "avan_sched.h" // Your header for the functions you're testing.

int debug_mode = 1; // Hardcodes debug on for the custom print functions

// Local Prototypes
void test_scheduler_create();

int main() {
  print_status("Test 1: Testing Scheduler Create");
  test_scheduler_create();

  return 0;
}

void test_scheduler_create() {
  print_debug("...Calling avan_create()");
  MARK("I can be used anywhere, even if debug mode is off.\n");
  MARK("I work just like printf! %s %d %lf\n", "Cool!", 42, 3.14);
  avan_header_t *header = avan_create();
  if(header == NULL) {
    abort_error("...avan_create returned NULL!", __FILE__);
  }
  if(header->ready_queue == NULL || header->suspended_queue == NULL || header->terminated_queue == NULL) {
    abort_error("...avan_create returned at least one NULL queue.", __FILE__);
  }
  if(header->ready_queue->count != 0) {
    print_warning("...The count on Ready Queue is not 0!");
  }

  print_status("...Printing the Schedule");
  print_avan_debug(header);
  print_status("...avan_create is looking good so far.");
}

void test_scheduler_create() {
  	print_debug("...Calling avan_create()");
  	MARK("I can be used anywhere, even if debug mode is off.\n");
 	MARK("I work just like printf! %s %d %lf\n", "Cool!", 42, 3.14);
  	avan_header_t *header = avan_create();
  	if(header == NULL) {
    		abort_error("...avan_create returned NULL!", __FILE__);
  	}
  	if(header->ready_queue == NULL || header->suspended_queue == NULL || header->terminated_queue == NULL) {
    		abort_error("...avan_create returned at least one NULL queue.", __FILE__);
  	}
  	if(header->ready_queue->count != 0) {
   		 print_warning("...The count on Ready Queue is not 0!");
  	}

  	print_status("...Printing the Schedule");
  	print_avan_debug(header);
  	print_status("...avan_create is looking good so far.");
}

