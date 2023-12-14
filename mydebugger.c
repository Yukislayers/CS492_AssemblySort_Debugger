// KAU Anthony - 20226189

#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    pid_t child_pid;

    if (argc < 2){
        fprintf(stderr, "You have to put a program name\n");
        return -1;
    }

    child_pid = fork();

    if (child_pid == 0){

        printf("We will run as tracee the file = '%s'\n", argv[1]);

        // Start the tracee
        ptrace(PTRACE_TRACEME, 0, 0, 0); // become a tracee

        execl(argv[1], argv[1], 0);

    } else if (child_pid > 0){ // I am the tracer

        int wait_status;
        struct user_regs_struct regs;
        printf("debugger started\n");

        wait(&wait_status); // this will return when the tracee is ready
        printf("Wait for tracee to start \n");

        // Obtain and show tracee instruction pointer
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
        printf("Tracee will start at EIP = 0x%08x\n", regs.eip);

        // Setting a breakpoint at 0x80491f1
        printf("Setting up a breakpoint at 0x80491f1 \n");
        unsigned breakpoint = 0x80491f1;
        unsigned data = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)breakpoint, 0);
        printf("Original byte value of the address 0x%08x: 0x%08x\n", breakpoint, data);

        // Replace the byte value by 0xcc
        unsigned sigtrap = 0xcc;
        ptrace(PTRACE_POKETEXT, child_pid, (void*)breakpoint, (void*)sigtrap);

        // Trapped data
        unsigned trapped_data = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)breakpoint, 0);
        printf("After trap, value at the address 0x%08x: 0x%08x\n", breakpoint, trapped_data);

        // The program run until it hit the breakpoint

        ptrace(PTRACE_CONT, child_pid, 0, 0);

        wait(&wait_status);

        // So now the tracee will run the program until it hits this breakpoint
        printf ("The breakpoint has been hit by the tracee ! \n");

        // See where the child process is
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
        printf("Child stopped at EIP = 0x%08x\n", regs.eip);

        // We are putting back the original data at the address of the breakpoint
        // And we want to jump to the next instruction to not enter the conditional branch
        ptrace(PTRACE_POKETEXT, child_pid, (void*)breakpoint, (void*)data);
        regs.eip += 1; // We set the EIP to that value because it is the address of the next instructions
        ptrace(PTRACE_SETREGS, child_pid, 0, &regs);

        printf("Child will start again at EIP = 0x%08x \n", regs.eip);

        // The child can run back

        ptrace(PTRACE_CONT, child_pid, 0, 0);
        
        wait(&wait_status);

		// Tried something but it is not working, don't understand how the program should restart
        // After it hit the breakpoint and how the tracee should start back from the changed value of EIP
        // Just put the same pattern and tried to execute it 

        // Setting up the second breakpoints
        printf("Setting up a breakpoint at 0x8049214 \n");
        unsigned breakpoint2 = 0x8049214;
        unsigned data2 = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)breakpoint2, 0);
        printf("Original data at 0x%08x: 0x%08x\n", breakpoint2, data2);

        // Sigtrap, replace the byte value by 0xcc
        unsigned sigtrap2 = 0xcc;
        ptrace(PTRACE_POKETEXT, child_pid, (void*)breakpoint2, (void*)sigtrap2);

        // Verify that the breakpoint has been set
        unsigned trapped_data2 = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)breakpoint2, 0);
        printf("After trap, data at 0x%08x: 0x%08x\n", breakpoint2, trapped_data2);

        printf("The breakpoint has been hit by the tracee \n");

        // The child can run back

        ptrace(PTRACE_CONT, child_pid, 0, 0);

        wait(&wait_status);

        // See where the child process is
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
        printf("Child stopped at EIP = 0x%08x\n", regs.eip);

        // We are putting back the original data at the address of the breakpoint
        ptrace(PTRACE_POKETEXT, child_pid, (void*)breakpoint2, (void*)data2);
        regs.eip += 1; // We set the EIP to EIP + 1 to ignore the conditional branch
        ptrace(PTRACE_SETREGS, child_pid, 0, &regs);

        printf("Chill will start again at EIP = 0x%08x \n", regs.eip);
		

        if (WIFEXITED(wait_status)) {
        printf("Child exited\n");
        }
        else {
            printf("Unexpected signal\n");
        }

    } else {
        perror("fork");
        return -1;
    }

    return 0;
}