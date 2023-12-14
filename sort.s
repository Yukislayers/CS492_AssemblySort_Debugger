# KAU Anthony 20226189

.intel_syntax noprefix
.global sort

sort:

# void sort (int arr[], int count)
# that is the function we want to implement
# We know that the stack grows backward
# so we will have the address of the array at ebp + 8,
# count (size of the array) at ebp + 12
# when the function will be called
# one element has a size of 4
# We want it in descending order so
# 12, 5, 160 = 160, 12, 5 (after our sort algo)
# To understand array in assembly, I watched this document : https://www.cs.uaf.edu/2017/fall/cs301/lecture/09_15_strings_arrays.html
# prolog is a routine to make sure the stack is in the right order

        push ebp

# top of the stack is pointed by esp
# so know ebp will have the value of the top of the stack

        mov ebp, esp

# we are going to use available registers
# to store value instead of using temp variables

        push ebx
	push edx
        push esi


# settings one register to be our number of items, edx

        mov eax, [ebp + 12] # at the memory address ebp + 12 we have the size of the array
        mov edx, 4 # We put 4 to edx, 4 is the size of one element
        mul edx # mul = eax * edx, so eax know how much items were in the array
                # mul always multiplies eax by a value
        mov edx, eax # edx is our numbers of items so we put back the value of eax into edx

# we need to choose which registers will be our iter
# and which registers will store the address of the array

        mov eax, 4 # eax will be our i, we start at i + 1
        mov ebx, 0 # ebx will be our j, j = i - 1
        mov esi, [ebp + 8] # We put in esi the array element 0

forloop:

# if i >= size of the array, then we can
# exit the loop, all elements will
# have been sorted

        cmp eax, ecx # eax, is our i, and ecx is the number of items in the array
        jge endloop # Jump to the end of the program


# ecx will be our temp value
# we want to compare it to the other ones, ecx = arr[i]
# In the first iter, i = 1 so the second value of the array
# Our array is made of int and an int is 4 bytes (DWORD)

        mov ecx, DWORD [esi + eax]

# j = i - 1

        mov ebx, eax # j = i
        sub ebx, 4 # j = i - 1 (4 is the size of an element)

whileloop: # whileloop because it is the while loop for the insertion sort

	# while (j >= 0 && temp > array[j]), this is the condition for the sorting algorithm

	cmp ebx, 0 # One of the conditions of the while loop for an insertion sort
	jl afterwhile # if j < 0 we exit the while

	cmp DWORD [esi + ebx], ecx # We are comparing array[j] and temp
	jge afterwhile # if array[j] > temp then it is sorted, since we want it in descending order

	# This is the part array[j+1] = array[1]

	push DWORD [esi + ebx] # We push on the stack array[j]
	pop DWORD [esi + ebx + 4] # We pop on the stack array [j+1]

	sub ebx, 4 # we are decrementing j, to continue to compare it with previous value

	jmp whileloop # Going back at the beginning of this label to continue in the while loop

afterwhile:

	# We are here when we exit the while, that's the part array[j+1]=key

	mov DWORD [esi + ebx + 4], ecx

	# i ++ to go to the second iter of the for loop
	# with the jump instructions below

	add eax, 4

	# go back to continue the for loop with the next value

	jmp forloop

endloop:

# epilog (same goal as prolog)

        mov esp, ebp
        pop esi
	pop edx
        pop ebx
        pop ebp
        ret

# Tried to use it with a C file but it didn't seem to work :(