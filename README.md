# Assignment-2

Modification of Assignment-1 with addition of pipes, TCP socket, file system and signal

Commands supported:

‘q’ : quit current program (whole program)

‘c’ : create a string buffer

‘s’ : insert user key in string into buffer (created by ‘c’)

‘g’ : print string buffer (user key in-in string using command ‘s’)

‘t’ : increment time by 1 second for Thread 1

'r' : decrement time by 1 second for Thread 1 (min 1s)

‘f’ : flag HIGH

'z' : flag LOW

Thread 1:

Perform the following function periodically (time control by ‘s’). Default to every seconds Increase the first character of string buffer : e.g. a → b, or A→ B
