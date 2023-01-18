/* This is the only file you will be editing.
 * - Copyright of Starter Code: Prof. Kevin Andrea, George Mason University.  All Rights Reserved
 * - Copyright of Student Code: You!  
 * - Restrictions on Student Code: Do not post your code on any public site (eg. Github).
 * -- Feel free to post your code on a PRIVATE Github and give interviewers access to it.
 * - Date: Aug 2022
 */

/* Fill in your Name and GNumber in the following two comment fields
 * Name: Jed
 * GNumber:00846927
 */

// System Includes
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>
// Local Includes
#include "avan_sched.h"
#include "vm_support.h"
#include "vm_process.h"

/* Feel free to create any helper functions you like! */
process_node_t *remove_process_node_t(queue_header_t *queue, pid_t pid);
int insert_process_node_t(queue_header_t *queue, process_node_t *node);

/* Initialize the avan_header_t Struct
 * Follow the specification for this function.
 * Returns a pointer to the new avan_header_t or NULL on any error.
 */
avan_header_t *avan_create() {
	avan_header_t *p_header;

	/*
	 *	avan_header
	 *		ready_queue
	 *		suspended_queue
	 *		terminated_queue
	 * */
	p_header = (struct avan_header *) malloc(sizeof(struct avan_header));
	if(p_header) {
		p_header->ready_queue = (struct queue_header *) malloc(sizeof(struct queue_header));
		p_header->ready_queue->count = 0;
		p_header->ready_queue->head = NULL;

		p_header->suspended_queue = (struct queue_header *) malloc(sizeof(struct queue_header));
		p_header->suspended_queue->count = 0;
	        p_header->suspended_queue->head = NULL;

		p_header->terminated_queue = (struct queue_header *) malloc(sizeof(struct queue_header));
		p_header->terminated_queue->count = 0;
        	p_header->terminated_queue->head = NULL;
	}

	return p_header; 
}

/* Adds a process into the appropriate singly linked list.
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error.
 */
int avan_insert(avan_header_t *header, process_node_t *process) {
  	if(header == NULL || process ==  NULL) 
		return -1;
	
	//IF x = 0b0001 THEN x|1 is 0b0001
	process->flags &= 1;

	return insert_process_node_t(header->ready_queue, process);
}
 
/* Move the process with matching pid from Ready to Suspended queue.
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error (such as process not found).
 */
int avan_suspend(avan_header_t *header, pid_t pid) {
	if(header == NULL) 
		return -1;

	process_node_t *process = remove_process_node_t(header->ready_queue, pid);
	
	//if process is found, remove its pointer to next and update its flag as suspended
	if(process != NULL) {
		process->next = NULL;
		//x * 2 = (x<<1)
		process->flags <<= 1;
	}	

	return insert_process_node_t(header->suspended_queue, process);
}

/* Move the process with matching pid from Suspended to Ready queue.
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error (such as process not found).
 */
int avan_resume(avan_header_t *header, pid_t pid) {
  	if(header == NULL) return -1;

        process_node_t *process = remove_process_node_t(header->suspended_queue, pid);
	
	if(process != NULL) {
		process->next = NULL;
		process->flags >>= 1;
	}

	return insert_process_node_t(header->ready_queue, process);
}

/* Insert the process in the Terminated Queue and add the Exit Code to it.
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error.
 */
int avan_quit(avan_header_t *header, process_node_t *node, int exit_code) {
 	if(header == NULL || node == NULL) 
		return -1;
	
	//x & 0 == 0b0000 | 4 == 0b0100	
	node->flags &= 0;
	node->flags |= 4;
	//leave the bits 0-3 and set the upper bits to exit code
	node->flags |= (exit_code << 4);

	return insert_process_node_t(header->terminated_queue, node);	
}


/* Move the process with matching pid from Ready to Terminated and add the Exit Code to it.
 * Follow the specification for this function.
 * Returns its exit code (from flags) on success or a -1 on any error.
 */
int avan_terminate(avan_header_t *header, pid_t pid, int exit_code) {
	//remove from ready queue
	process_node_t *process = remove_process_node_t(header->ready_queue, pid);
	//if not found in ready queue, remove from terminated queue
	if(process == NULL) 
		process = remove_process_node_t(header->terminated_queue, pid);
	//if found, terminate the process
	if(process != NULL) {
		process->next = NULL;
		avan_quit(header, process, exit_code);	
	}

	return avan_quit(header, process, exit_code); 
}

/* Create a new process_node_t with the given information.
 * - Malloc and copy the command string, don't just assign it!
 * Follow the specification for this function.
 * Returns the process_node_t on success or a NULL on any error.
 */
process_node_t *avan_new_process(char *command, pid_t pid, int priority, int critical) {
	process_node_t *process = (struct process_node *) malloc(sizeof(struct process_node));
	
	process->flags = 0x00000001;
	process->priority = priority;

	if(critical) 
		process->flags |= 0x00000008;  
	
	process->flags &= 0x0000000F;
	process->pid = pid;
	process->skips = 0;
	
	if(command != NULL) 
		strcpy(process->cmd, command);
	else return NULL;

	return process;	
}

/* Schedule the next process to run from Ready Queue.
 * Follow the specification for this function.
 * Returns the process selected or NULL if none available or on any errors.
 */
process_node_t *avan_select(avan_header_t *header) {
	if(header->ready_queue->count == 0) return NULL;
	
	process_node_t *critical = NULL;
	process_node_t *starving = NULL;  
	process_node_t *process = header->ready_queue->head;
	
	while(process != NULL) {
		if((process->flags & 0x08) == 0x08) {
			critical = process;
			break;
		}

		if(process->skips >= MAX_SKIPS) {
			if (starving == NULL) starving = process;
			else if (process->pid < starving->pid) starving = process;
		}

		process = process->next;
	}
	
	if(critical != NULL) process = critical;
	else if(starving != NULL) process = starving;
	else process = header->ready_queue->head;

	process = remove_process_node_t(header->ready_queue, process->pid);
	process->skips = 0;
	process->next = NULL;

	process_node_t *inc_node = header->ready_queue->head;
	
	while(inc_node != NULL) {
		inc_node->skips++;
		inc_node = inc_node->next;
	}

  	return process;
}

/* Returns the number of items in a given queue_header_t
 * Follow the specification for this function.
 * Returns the number of processes in the list or -1 on any errors.
 */
int avan_get_size(queue_header_t *ll) {
  	return ll->count;
}

/* Frees all allocated memory in the avan_header_tr */
void avan_cleanup(avan_header_t *header) {
	process_node_t *process;
	process_node_t *free_this_node;

 	process = header->ready_queue->head;
	while(process != NULL) {
		free_this_node = process;
		process = process->next;
		free(free_this_node);
	}
	
	process = header->suspended_queue->head;
	while(process != NULL) {
		free_this_node = process;
                process = process->next;
                free(free_this_node);
        }
	
	process = header->terminated_queue->head;
	while(process != NULL) {
		free_this_node = process;
                process = process->next;
                free(free_this_node);
        }

	free(header->ready_queue);
	free(header->suspended_queue);
	free(header->terminated_queue);
	free(header);

	return;
}

/*returns the address of node that matches the pid else returns NULL*/
process_node_t *remove_process_node_t(queue_header_t *queue, pid_t pid) {
	if(queue->count == 0) return NULL;
	
	process_node_t *prev_node = NULL;
	process_node_t *address = queue->head;
	pid_t address_pid = address->pid;
		
	//find address
	while (address != NULL && address_pid != pid) {
		prev_node = address;
		address = address->next;
		
		if (address != NULL) {
			address_pid = address->pid;
		}	
	}	
	
	if(address != NULL) {
		if (queue->count == 1) queue->head = NULL;			//remove the only node 
		else if(queue->head == address) queue->head = address-> next;	//remove from head
		else if(prev_node != NULL) prev_node->next = address->next;	//remove from middle or tail

		queue->count--;
	}

	return address;
}

/*inserts a process node in the queue returns 0 else -1*/
int insert_process_node_t(queue_header_t *queue, process_node_t *node) {
	if(queue == NULL || node == NULL) {
		return -1;
	}
	
	//insert first element at head
	if(queue->count == 0) {
               queue->head = node;
        }
        else {
                process_node_t *comp_process_node = queue->head;
                pid_t comp_pid = comp_process_node->pid;
                pid_t node_pid = node->pid;

                //head insert
                if (node_pid < comp_pid) {
                        queue->head = node;
                        node->next = comp_process_node;
                }
                //within and tail insert
                else {
                        process_node_t *prev_process_node = NULL;

                        while(comp_process_node != NULL && node_pid > comp_pid ) {
                                prev_process_node = comp_process_node;
                                comp_process_node = comp_process_node->next;

                                if(comp_process_node != NULL) {
                                        comp_pid = comp_process_node->pid;
                                }
                        }
			
			//fix ordering	
			if(prev_process_node != NULL) {
                        	prev_process_node->next = node;
                        	node->next = comp_process_node;
			}
                }
        }

       	queue->count++;
	
	return 0;
}
