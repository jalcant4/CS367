/* Fill in your Name and GNumber in the following two comment fields
 * Name: Jed
 * GNumber: 00846927
 * Last edited: 2/22/23
 * 
 * The purpose of the the task controller is a responsible for
 *  maintaining a list of several tasks, which can be executed, inspected, 
 *  or otherwise managed (e.g. by killing or suspending a running task).
 * This project is an example of process management in a 
 *  Unix-like environment.
 */

#include <sys/wait.h>
#include "taskctl.h"
#include "parse.h"
#include "util.h"
/* Definitions */
#define DEBUG 1
#define true 1
#define false 0
#define NUM_PATHS 2
#define NUM_INSTRUCTIONS 10
#define MAX_NUM_TASKS 9001
/* Typedef and constants*/
typedef int boolean;
static boolean builtin_signal = false;
static char buffer[MAXLINE + 1] = {0};
/*
    task_path[0] is ./ current directory
    task_path[1] is the directory of sys/usr/bin
*/
static char *task_path[] = { "./", "/usr/bin/", NULL };
/*
    in order of instructions[n] where n starts with 0
    "quit", "help", "list", "purge", "exec", "bg", "kill", "suspend", "resume", "pipe", NULL
*/
static char *instructions[] = { "quit", "help", "list", "purge", "exec", "bg", "kill", "suspend", "resume", "pipe", NULL};
/*
    tasks represent the list of current tasks
        status: LOG_STATE_READY      0
                LOG_STATE_RUNNING    1
                LOG_STATE_SUSPENDED  2
                LOG_STATE_FINISHED   3
                LOG_STATE_KILLED     4
        pid should be 0 while the task is in the ready state
        ground stores LOG_FG or LOG_BG
        exit_code should be 0 it did not terminate normally
        cmd[MAXLINE] stores the parsed cmd
*/
typedef struct task_struct {
    int exit_code;
    int status;
    int ground;
    pid_t pid;
    char command[MAXLINE];
} task_t;
/*
    task_flags stores the state of the corresponding task
    task_pointer stores the addresses of the tasks
    task_counter is updated by makeTask and freeTask
*/
static boolean task_flags[MAX_NUM_TASKS] = {0};
static task_t tasks[MAX_NUM_TASKS] = {0};
static task_t *fg_pointer;
static task_t *bg_pointer;
static int task_counter = 0;
/* Task Methods*/
boolean initTask(task_t *task);
boolean freeTask(int task_num);
task_t *makeTask(int task_num, int stat, int exit, int ground, pid_t pid, char *cmd);
boolean is_valid_task_num(int task_num);
boolean is_busy(task_t *task);
boolean is_idle(task_t *task);
/* Controller Methods*/
void launch_process(Instruction *inst, int task_num, pid_t task_pid);
void launch_task(Instruction *inst, int task_num, int *pipe_fd[], int ground);
void wait_for_tasks();
boolean update_tasks(pid_t task_pid, int task_status);
/* Helper Functions*/
int contains(const char *needle, char *haystack[]);
boolean init_argv(char *argv[]);
int parse_cmd_to_argv(char *cmd, char *argv[]);
boolean free_arg(char *argv[]);
boolean setPath(char *arg, char *path1, char* path2);
boolean execv_w_paths(task_t *this, char *argv[]);
void reaper(int sig);
void terminator(int sig);
void suspender(int sig);
void resumer(int sig);
/* Built-in Methods*/
void list();
void task_purge(Instruction *inst);
void task_pipe(Instruction *inst);
void task_kill(Instruction *inst);
void task_suspend(Instruction *inst);
void task_resume(Instruction *inst);
/* The entry of your task controller program */
int main() {
    struct sigaction reap = {0};
    reap.sa_handler = reaper;
    sigaction(SIGCHLD, &reap, NULL);    //norm term
    struct sigaction term = {0};
    term.sa_handler = terminator;
    sigaction(SIGINT, &term, NULL);     //suspend
    struct sigaction sus = {0};
    sus.sa_handler = suspender;
    sigaction(SIGTSTP, &sus, NULL);     //suspend
    struct sigaction res = {0};
    res.sa_handler = resumer;
    sigaction(SIGCONT, &res, NULL);     //continue 
    /* Command line */
    char cmdline[MAXLINE];
    char *cmd = NULL;

    /* Intial Prompt and Welcome */
    log_kitc_intro();
    log_kitc_help();

    /* Shell looping here to accept user command and execute */
    while (1) {
        builtin_signal = false;
        /*
            Argument list
            argv should store a list of arguments when we want to execute a non builtin commands
        */
        char *argv[MAXARGS+1];
        /*
            Instruction structure: check parse.h
            inst stores the values for BUILTIN instructions
        */
        Instruction inst;

        /* Print prompt */
        log_kitc_prompt();

        /* Read a line */
        // note: fgets will keep the ending '\n'
	    errno = 0;
        if (fgets(cmdline, MAXLINE, stdin) == NULL) {
            if (errno == EINTR) {
                continue;
            }
            exit(-1);
        }

        if (feof(stdin)) {  /* ctrl-d will exit text processor */
          exit(0);
        }

        /* Parse command line */
        if (strlen(cmdline)==1)   /* empty cmd line will be ignored */
          continue;     

        cmdline[strlen(cmdline) - 1] = '\0';        /* remove trailing '\n' */

        cmd = malloc(strlen(cmdline) + 1);          /* duplicate the command line */
        snprintf(cmd, strlen(cmdline) + 1, "%s", cmdline);

        /* Bail if command is only whitespace */
        if(!is_whitespace(cmd)) {
            initialize_command(&inst, argv);    /* initialize arg lists and instruction */
            parse(cmd, &inst, argv);            /* call provided parse() */

            if (DEBUG) {  /* display parse result, redefine DEBUG to turn it off */
                debug_print_parse(cmd, &inst, argv, "main (after parse)");
	        }

            /* After parsing: your code to continue from here */
            /*
                initialize after 
            */
            /*================================================*/
            /*
                if not builtin instruction
            */
            if (!contains(inst.instruct, instructions)) {
                int task_number = -1;
                for (int i = 1; i < MAX_NUM_TASKS; i++) {
                    if (task_flags[i] == false)  {
                            task_flags[i] = true;
                            task_number = i;
                            break;
                    }
                }
                if (task_number == -1) {
                    log_kitc_task_num_error(task_number);
                    continue;
                }
                log_kitc_task_init(task_number, cmd);
                makeTask(task_number, LOG_STATE_READY, 0, 0, 0, cmd);
            }
            /*instruction is quit*/
            else if (strcmp(inst.instruct, instructions[0]) == 0) {
                log_kitc_quit();
                exit(0);
            }
            /*help*/
            else if (strcmp(inst.instruct, instructions[1]) == 0) {
                log_kitc_help();
            }
            /*list*/
            else if (strcmp(inst.instruct, instructions[2]) == 0) {
                list();
            }
            /*purge*/
            else if (strcmp(inst.instruct, instructions[3]) == 0) {
                task_purge(&inst);
            }
            /*exec*/
            else if (strcmp(inst.instruct, instructions[4]) == 0) {
                launch_task(&inst, inst.num, NULL, LOG_FG);
            }
            /*bg*/
            else if (strcmp(inst.instruct, instructions[5]) == 0) {
                launch_task(&inst, inst.num, NULL, LOG_BG);
            }
            /*kill*/
            else if (strcmp(inst.instruct, instructions[6]) == 0) {
                task_kill(&inst);
            }
            /*suspend*/
            else if (strcmp(inst.instruct, instructions[7]) == 0) {
                task_suspend(&inst);
            }
            /*resume*/
            else if (strcmp(inst.instruct, instructions[8]) == 0) {
                task_resume(&inst);
            }
            /*pipe*/
            else if (strcmp(inst.instruct, instructions[9]) == 0) {
                task_pipe(&inst);
            }
            /*NULL*/
            else {
    
            }
        }  // end if(!is_whitespace(cmd))
        else {

        } //is_whitespace
	    free(cmd);
	    cmd = NULL;
        free_command(&inst, argv);
    }  // end while(1)

    return 0;
}  // end main()
/*
    initTask intializes task_t
        INCREMENTS task_counter
        DO NOT CALL ALONE, RUN MAKE TASK
    returns false on failure
*/
boolean initTask(task_t *task) {
    task->status = LOG_STATE_READY;
    task->pid = 0;
    task->exit_code = 0;
    task->ground = -1;
    memset(task->command, 0, MAXLINE);
    return true;
}
/*
    free task_t
    returns true if freed
*/
boolean freeTask(int task_num) {
    if (task_num < 0 || task_num > MAX_NUM_TASKS || !task_flags[task_num]) 
        return false;
    task_counter--;
    task_flags[task_num] = false;
    return true;
}
/*
    makeTask stores the arg that was parsed
        params
            task_number is the index of the task
            status:
                LOG_STATE_READY      0
                LOG_STATE_RUNNING    1
                LOG_STATE_SUSPENDED  2
                LOG_STATE_FINISHED   3
                LOG_STATE_KILLED     4
            exit code should be 0 unlesss the process has already completed execution
            process id should be 0 while task is in the ready state
            cmd is the complete command line of the task
        returns address of the task else NULL
*/
task_t *makeTask(int task_num, int stat, int exit, int ground, pid_t pid, char *cmd) {
    //create the task and return it
    if (task_num == -1) {
        //failure to find a valid flag
        log_kitc_task_num_error(task_num);
    }
    task_flags[task_num] = true;
    task_t *this = &tasks[task_num];
    if (initTask(this) == false) {
        //if initTask failed to initialize properly
        return NULL;
    }
    task_counter++;
    //update task values
    this->status = stat;
    this->exit_code = exit;
    this->ground = ground;
    this->pid = pid;
    strncpy(this->command, cmd, strlen(cmd));
    return this;
}// end maketask
/*
    returns true if the task_num associated with the task flag is true
*/
boolean is_valid_task_num(int task_num) {
    return (task_num < 0 || task_num > MAX_NUM_TASKS) ? false: task_flags[task_num];
}
/*
    returns true if the status of the task is either running or suspended
*/
boolean is_busy(task_t *task) {
    return (task->status == LOG_STATE_RUNNING || task->status == LOG_STATE_SUSPENDED) ? true : false;
}
/*
    returns true if the status of the task is ready finish or killed
*/
boolean is_idle(task_t *task) {
    return (task->status == LOG_STATE_READY || 
        task->status == LOG_STATE_FINISHED ||
        task->status == LOG_STATE_KILLED) ? true : false;
}
/*
    Controller Methods
*/
/*
    launch_process is called in the exec, bg, pipe process
        responsible for calling execv
        you must create a fork before use
*/
void launch_process(Instruction *inst, int task_num, pid_t task_pid) {
    task_t *this = &tasks[task_num];
    /*
        argv
    */
    char *argv[MAXARGS+1];
    init_argv(argv);
    parse_cmd_to_argv(this->command, argv);
    /*
        fd[0]/fd[LOG_REDIR_IN] is read
        fd[1]/fd[LOG_REDIR_OUT] is write
        If a redirect file cannot be opened, call log_kitc_file_error(task_num, filename).
    */
    int fd[2] = {-1, -1};
    char *infile = inst->infile;
    char *outfile = inst->outfile;
    if (infile != NULL) {
        fd[LOG_REDIR_IN] = open(infile, O_RDWR, 0644);
        if (fd[LOG_REDIR_IN] == STDIN_FILENO)
            log_kitc_file_error(task_num, infile);
    }
    if (outfile != NULL) {
        fd[LOG_REDIR_OUT] = open(outfile, O_CREAT | O_RDWR | O_TRUNC, 0644);
        if (fd[LOG_REDIR_OUT] == STDOUT_FILENO)
            log_kitc_file_error(task_num, outfile);
    }
    /*
        redirection
        redir_type is LOG_REDIR_IN or LOG_REDIR_OUT depending on which type.
    */
    if (fd[LOG_REDIR_IN] > 0) {
        log_kitc_redir(task_num, LOG_REDIR_IN, infile);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
    }
    if (fd[LOG_REDIR_OUT] > 1) {
        log_kitc_redir(task_num, LOG_REDIR_OUT, outfile); 
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
    }
    /*
        update status values then execute
    */
    this->pid = task_pid;
    log_kitc_status_change(task_num, this->pid, this->ground, this->command, LOG_START);
    this->status = LOG_STATE_RUNNING;
    execv_w_paths(this, argv);
    free_arg(argv);
    //Terminate the child with exit 1.
    exit(1);
} // end launch_process
/*
    launch_task is called in exec bg and pipe is responsible for initializing the task environment
        inst
        task_num1
        pipe_fd is a working pipe
        ground determines whether the task is in the fore ground or the background
*/
void launch_task(Instruction *inst, int task_num, int *pipe_fd[], int ground) {
    //If the selected task does not exist, call log_kitc_task_num_error(task_num)
    if (is_valid_task_num(task_num) == false) {
        log_kitc_task_num_error(task_num);
        return;
    }
    /*
        If the task is currently busy, call log_kitc_status_error(task_num, status) instead
        A busy task is a Running or Suspended task; do not execute the task in this case.
    */
    task_t *this = &tasks[task_num];
    if (is_busy(this)) {
        log_kitc_status_error(task_num, this->status);
        return;
    }
    /*
        fix pipes
    */
    int in_pipe;
    int out_pipe;
    if (pipe_fd != NULL) {
        in_pipe = pipe_fd[LOG_REDIR_IN];
        out_pipe = pipe_fd[LOG_REDIR_OUT];
    }
    //LOG_BG is 1
    if (ground) {
        //update bg_pointer
        this->ground = LOG_BG;
        this->status = LOG_STATE_RUNNING;
        int bg_pid = fork();
        if (bg_pid == 0) {
            setpgid(0, 0);
            /*
                fix pipes
            */
            if (strcmp(inst->instruct, instructions[9]) ==0) {
                //A redirects its output to the write-end of the pipe and closes its read-end.
                dup2(out_pipe, STDOUT_FILENO);
                close(in_pipe);
            }
            launch_process(inst, task_num, getpid());
        }
        this->pid = bg_pid;
        //close the parent pipe
        if (strcmp(inst->instruct, instructions[9]) ==0) {
            close(in_pipe);
            close(out_pipe);
        }
    }
    //LOG_FG is 0
    else {
        //update fg_pointer
        fg_pointer = this;
        //update this
        this->ground = LOG_FG;
        this->status = LOG_STATE_RUNNING;
        int fg_pid = fork();
        if (fg_pid == 0) {
            //group pid becomes fg_pid
            setpgid(0, 0);
            /*
                fix pipes
            */
            if (strcmp(inst->instruct, instructions[9]) ==0) {
                //B redirects its input from the read-end of the pipe and closes its write-end.
                dup2(in_pipe, STDIN_FILENO);
                close(out_pipe);
            }
            launch_process(inst, task_num, getpid());
        }
        this->pid = fg_pid;
        //close the parent pipe
        if (strcmp(inst->instruct, instructions[9]) ==0) {
            close(in_pipe);
            close(out_pipe);
        }
        //tell the controller to wait for foreground process
        pause();
        return;
    }
}//end launch_task
/*
    wait for tasks waits for ANY TASK
*/
void wait_for_tasks() {
    pid_t pid;
    int status;
    do {
        pid = waitpid(WAIT_ANY, &status, WNOHANG | WUNTRACED);
        update_tasks(pid, status);
    } while (pid > 0);
    return;
}// end wait_for_tasks
/*
    updates the task with the associated pid
        return false if it fails
*/
boolean update_tasks(pid_t task_pid, int task_status) {
    task_t *this;
    for (int task_num = 1; task_num < MAX_NUM_TASKS; task_num++) {
        if (task_flags[task_num] && tasks[task_num].pid == task_pid) {
            this = &(tasks[task_num]);
            //normal termination
            if (WIFEXITED(task_status)) {
                this->exit_code = WEXITSTATUS(task_status);
                this->status = LOG_STATE_FINISHED;
                log_kitc_status_change(task_num, this->pid, this->ground, this->command, LOG_TERM);
            }
            //suspended process (ctrl + z and task_suspend)
            else if (WIFSTOPPED(task_status)) {
                this->exit_code = WSTOPSIG(task_status);
                this->status = LOG_STATE_SUSPENDED;
                log_kitc_status_change(task_num, this->pid, this->ground, this->command, LOG_SUSPEND);
            }
            //signal termination (ctrl + c and task_kill)
            else if (WIFSIGNALED(task_status)) {
                this->exit_code = WTERMSIG(task_status);
                this->status = LOG_STATE_KILLED;
                log_kitc_status_change(task_num, this->pid, this->ground, this->command, LOG_TERM_SIG);
            }
            if (fg_pointer == this) fg_pointer = NULL;
            else bg_pointer = NULL;
            return true;
        }
    }
    return false;
}//end update_tasks
/*
    Helper functions
*/

/* 
    contains returns true of false depending on whether the haystack contains the needle. 
*/
int contains(const char *needle, char *haystack[]) {
    // sanity check for valid input
    if (!needle || !haystack) return 0;

    // search through the haystack string-by-string until we find a match
    for (int i = 0;  haystack[i];  i++) {
        if (strncmp(needle, haystack[i], strlen(haystack[i]) + 1) == 0) return 1;
    }
    return 0;
}
/*
    initialize argv
    argv size is MAXARGS + 1
    returns true on success
*/
boolean init_argv(char *argv[]) {
    if (argv == NULL) return false;  
    for (int i = 0; i < MAXARGS + 1; i++) {
        argv[i] = NULL;
    }
    return true;
}
/*
    converts a command to a dynamically array of arguments
    remember to free argv from memory
    returns the number of arguments
*/
int parse_cmd_to_argv(char *cmd, char *argv[]) {
    //initialize p_tok and buffer
    char *p_tok = NULL;
    memset(buffer, 0, MAXLINE);
    strncpy(buffer, cmd, strlen(cmd));
    //the following initializes p_tok
    p_tok = strtok(buffer, " ");
    if (p_tok == NULL) return 0;
    // copy the tokens into argv
    int index = 0;
    int argc = 0;
    while(p_tok != NULL) {
        argv[index] = malloc(MAXLINE * sizeof(char));
        strcpy(argv[index], p_tok);
	    index++;
        argc++;
        p_tok = strtok(NULL, " ");
	    if (index >= MAXLINE) p_tok = NULL;
    }
    return argc;
}
/*
    frees an array of arguments
*/
boolean free_arg(char *argv[]) {
    if (argv == NULL) return false;
    for (int i = 0; i < MAXARGS + 1; i++) {
        if (argv[i] != NULL)
            free(argv[i]);
        argv[i] = NULL;
    }
    argv = NULL;
    return true;
}
/*
    path1 ./arg
    path2 /sys/usr/bin/arg
*/
boolean setPath(char *arg, char *path1, char* path2) {
    if (arg == NULL || path1 == NULL || path2 == NULL) return false;
    memset(path1, 0, MAXLINE);
    strncpy(path1, task_path[0], strlen(task_path[0]));
    strncat(path1, arg, strlen(arg));
    memset(path2, 0, MAXLINE);
    strncpy(path2, task_path[1], strlen(task_path[1]));
    strncat(path2, arg, strlen(arg));
    return true;
}
/*
    execv_w_paths calls the execv function twice
        using exec path1 and exec path2
        if not executed calls log_kitc_exec_error then returns false
*/
boolean execv_w_paths(task_t *this, char *argv[]) {
    /*
        exec_path1 ./ is current directory
        exec_path2 /sys/usr/bin
    */
    char exec_path1[MAXLINE], exec_path2[MAXLINE];
    setPath(argv[0], exec_path1, exec_path2);
    if (execv(exec_path1, argv) == -1) {
        //exec_path is set to sys/usr/bin
        if (execv(exec_path2, argv) == -1) {
            //If the command cannot be executed (exec failed), call log_kitc_run_error(cmd).
            log_kitc_exec_error(this->command);
            fg_pointer = NULL;
            return false;
        }
    }
    return true;
}
/*
    reaper handles SIGCHLD
*/
void reaper(int sig) {
    wait_for_tasks();
    return;
}// end reaper
/*
    terminator handles SIGINT
*/
void terminator(int sig) {
    int task_num;
    task_t *this = NULL;
    if (builtin_signal && bg_pointer != NULL) {
        for (task_num = 1; task_num < MAX_NUM_TASKS; task_num++) {
            if (task_flags[task_num] && bg_pointer == &tasks[task_num]) {
                this = &tasks[task_num];
                break;
            }
        }
        builtin_signal = false;
    }
    else if (fg_pointer != NULL) {
        for (task_num = 1; task_num < MAX_NUM_TASKS; task_num++) {
            if (task_flags[task_num] && fg_pointer == &tasks[task_num]) {
                this = &tasks[task_num];
                break;
            }
        }
    }
    if (this != NULL) {
        log_kitc_sig_sent(LOG_CMD_KILL, task_num, this->pid);
        this->status = LOG_STATE_KILLED;
        kill(-(this->pid), SIGINT);
    }
    return;
}// end terminator
/*
    suspender handles SIGTSTP
*/
void suspender(int sig) {
    int task_num;
    task_t *this = NULL;
    //signal handler returns to task controller
    if (builtin_signal && bg_pointer != NULL) {
        for (task_num = 1; task_num < MAX_NUM_TASKS; task_num++) {
            if (task_flags[task_num] && bg_pointer == &tasks[task_num]) {
                this = &tasks[task_num];
                break;
            }
        }
        builtin_signal = false;
    }
    else if (fg_pointer != NULL) {
        for (task_num = 1; task_num < MAX_NUM_TASKS; task_num++) {
            if (task_flags[task_num] && fg_pointer == &tasks[task_num]) {
                this = &tasks[task_num];
                break;
            }
        }
    }
     if (this != NULL) {
        log_kitc_sig_sent(LOG_CMD_SUSPEND, task_num, this->pid);
        this->status = LOG_STATE_SUSPENDED;
        kill(-(this->pid), SIGTSTP);
    }
    return;
}//end suspender
/*
    resumer handles SIGCONT
*/
void resumer(int sig) {
    //get the task
    int task_num;
    task_t *this = NULL;
    for (task_num = 1; task_num < MAX_NUM_TASKS; task_num++) {
        if (task_flags[task_num] && fg_pointer == &tasks[task_num]) {
            this = &tasks[task_num];
            break;
        }
    }
    log_kitc_sig_sent(LOG_CMD_RESUME, task_num, this->pid);
    //signal handler returns to task controller
    this->status = LOG_STATE_RUNNING;
    this->ground = LOG_FG;
    log_kitc_status_change(task_num, this->pid, this->ground, this->command, this->status);
    return;
}//end resumer
/*
    Built in functions
*/
/*
    list: lists all of the currently existing tasks
        includes the total number of tasks
*/
void list() {
    log_kitc_num_tasks(task_counter);
    for(int i = 0; i < MAX_NUM_TASKS; i++) {
        //print each task if true
        if(task_flags[i]) {
            task_t *this = &tasks[i];
            log_kitc_task_info(i, this->status, this->exit_code, this->pid, this->command);
        }
    }
}
/*
    purge task_num removes TASKUM from the list of tasks
*/
void task_purge(Instruction *inst) {
    int task_num = inst->num;
    //if DNE call log_kitc_num_error
    if (is_valid_task_num(task_num) == false) {
        log_kitc_task_num_error(task_num);
        return;
    }
    task_t *this = &tasks[task_num];
    //if the task is busy (RUNNING or SUSPENDED) call log_kitc_status_error
    if(is_busy(this)) {
        log_kitc_status_error(task_num, this->status);
        return;
    }
    //on successful delete call log_kitc_purge()
    if (!freeTask(task_num))
        log_kitc_task_num_error(inst->num);
    else log_kitc_purge(task_num);
}
/*
    pipe
*/
void task_pipe(Instruction *inst) {
    // int pipe_fd[] = {-1, -1};
    // if (pipe(pipe_fd)) {
    //     log_kitc_pipe_error(inst->num);
    //     return;
    // }
    //
    // launch_task(inst, inst->num, &pipe_fd, LOG_BG);
    // launch_task(inst, inst->num2, &pipe_fd, LOG_FG);
    //check num and num2
    int task_num1 = inst->num;
    if (is_valid_task_num(task_num1) == false) {
        log_kitc_task_num_error(task_num1);
        return;
    }
    int task_num2 = inst->num2;
    if (is_valid_task_num(task_num2) == false) {
        log_kitc_task_num_error(task_num2);
        return;
    }
    if (task_num1 == task_num2) {
        log_kitc_pipe_error(task_num1);
        return;
    }
    //check status of task_t a and task_t b
    task_t *a = &tasks[task_num1];
    if (is_busy(a)) {
        log_kitc_status_error(task_num1, a->status);
        return;
    }
    task_t *b = &tasks[task_num2];
    if (is_busy(b)) {
        log_kitc_status_error(task_num2, b->status);
        return;
    }
     /*
        Mario Time
        fd[0]/fd[LOG_REDIR_IN] is read
        fd[1]/fd[LOG_REDIR_OUT] is write
    */
    int fd[2] = {-1, -1};
    if (pipe(fd)) {
        perror(LOG_FILE_PIPE);
        return;
    }
    //P creates a pipe before forking A or B; thus, both A and B gain access to the pipe.
    log_kitc_pipe(task_num1, task_num2);
    int in_pipe = fd[LOG_REDIR_IN];
    int out_pipe = fd[LOG_REDIR_OUT];
    /*
        Parent process P has two children, A and B.
        A wants to send data to B.
        Now, any output from A gets sent as input to B.
    */
    bg_pointer = a;
    pid_t a_pid = fork();
    if (a_pid == 0) {
        /*
            argv is a list of arguments derived from command
        */
        char *argv[MAXARGS+1];
        init_argv(argv);
        parse_cmd_to_argv(a->command, argv);
        //A redirects its output to the write-end of the pipe and closes its read-end.
        dup2(out_pipe, STDOUT_FILENO);
        close(in_pipe);
        //Execute task
        log_kitc_status_change(task_num1, getpid(), LOG_BG, a->command, LOG_START);
        execv_w_paths(a, argv);
        free_arg(argv);
        //Terminate the child with exit 1.
        exit(1);
    }
    a->pid = a_pid;
    fg_pointer = b;
    int b_status;
    pid_t b_pid = fork();
    if (b_pid == 0) {
         /*
            argv is a list of arguments derived from command
        */
        char *argv[MAXARGS+1];
        init_argv(argv);
        parse_cmd_to_argv(b->command, argv);
        //A redirects its output to the write-end of the pipe and closes its read-end.
        dup2(in_pipe, STDIN_FILENO);
        close(out_pipe);
        //Execute task
        log_kitc_status_change(task_num2, getpid(), LOG_FG, b->command, LOG_START);
        execv_w_paths(b, argv);
        free_arg(argv);
        //Terminate the child with exit code 1.
        exit(1);
    }
    //P closes both its read-end and write-end, because it is not directly using either.
    close(in_pipe);
    close(out_pipe);
    b->pid = b_pid;
    waitpid(b_pid, &b_status, 0);
    //Task is finished
    b->status = LOG_STATE_FINISHED;
    log_kitc_status_change(task_num2, b->pid, LOG_FG, b->command, LOG_TERM);
    return;
}
/*
    task_kill sends a signal to a task
        inst specifies which task
        signal:
            kill- SIGINT
            suspend- SIGSTOP
            resume- SIGCONT
        log_cmd:
            LOG_CMD_KILL
            LOG_CMD_SUSPEND
            LOG_CMD_RESUME
*/
void task_kill(Instruction *inst) {
    int task_num = inst->num;
    //If DNE call log_kitc_num_error
    if (is_valid_task_num(task_num) == false) {
        log_kitc_task_num_error(task_num);
        return;
    }
    task_t *this = &tasks[task_num];
    //If the task is currently idle, call log_kitc_status_error(task_num, status) instead.
    if (is_idle(this)) {
        log_kitc_status_error(task_num, this->status);
        return;
    }
    /*
        When signaling the kill, call log_kitc_sig_sent(LOG_CMD_KILL, task_num, pid).
        As a side effect, the child may exit, which leads to additional logs; see 2.7.2.

        On success (at least one signal was sent), zero is returned.  On
        error, -1 is returned, and errno is set to indicate the error.
    */
    builtin_signal = true;
    bg_pointer = this;
    if (kill(-(this->pid), SIGINT)) {
        bg_pointer = NULL;
        return;
    }
    // log_kitc_sig_sent(LOG_CMD_KILL, task_num, this->pid);
    // this->status = LOG_STATE_KILLED;
    // log_kitc_status_change(task_num, this->pid, this->ground, this->command, this->status);
    return;
}
/*
    suspend a process with SIGTSTP
*/
void task_suspend(Instruction *inst) {
    int task_num = inst->num;
    //If DNE call log_kitc_num_error
    if (is_valid_task_num(task_num) == false) {
        log_kitc_task_num_error(task_num);
        return;
    }
    task_t *this = &tasks[task_num];
    //If the task is currently idle, call log_kitc_status_error(task_num, status) instead.
    if (is_idle(this)) {
        log_kitc_status_error(task_num, this->status);
        return;
    }
    /*
        When signaling the kill, call log_kitc_sig_sent(LOG_CMD_KILL, task_num, pid).
        As a side effect, the child may exit, which leads to additional logs; see 2.7.2.

        On success (at least one signal was sent), zero is returned.  On
        error, -1 is returned, and errno is set to indicate the error.
    */
    builtin_signal = true;
    bg_pointer = this;
    if (kill(-(this->pid), SIGTSTP)) {
        bg_pointer = NULL;
        return;
    }
    return;
}
/*
    resume a process with SIGCONT
        the resumed process will become a foreground process
*/
void task_resume(Instruction *inst) {
    int task_num = inst->num;
    //If DNE call log_kitc_num_error
    if (is_valid_task_num(task_num) == false) {
        log_kitc_task_num_error(task_num);
        return;
    }
    task_t *this = &tasks[task_num];
    //If the task is currently idle, call log_kitc_status_error(task_num, status) instead.
    if (is_idle(this)) {
        log_kitc_status_error(task_num, this->status);
        return;
    }
    /*
        When signaling the kill, call log_kitc_sig_sent(LOG_CMD_KILL, task_num, pid).
        As a side effect, the child may exit, which leads to additional logs; see 2.7.2.

        On success (at least one signal was sent), zero is returned.  On
        error, -1 is returned, and errno is set to indicate the error.
    */
    fg_pointer = this;
    this->ground = LOG_FG;
    if (kill(-(this->pid), SIGCONT)) {
        //kill failed
        fg_pointer = NULL;
        return;
    }
    return;
}
