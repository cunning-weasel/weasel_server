# weasel_server.c

compile: gcc -o output_weasel_server weasel_server.c index_html_string.c

compile for perf optimization: gcc -o output_weasel_server weasel_server.c -O3

debugging:
-Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined 

i.e: gcc -o output_weasel_server weasel_server.c -Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined

additional flags? : -O1 -fno-omit-frame-pointer

executable(for prod):
-Wall -Wextra -Wpedantic -Werror -O2 i.e: gcc -o output_weasel_server weasel_server.c index_html_string.c -Wall -Wextra -Wpedantic -Werror -O2 -lssl -lcrypto

run: ./output_weasel_server
run with GDB: gdb ./output_weasel_server

i.e:
run GDB:
gdb ./output_weasel_server

set a breakpoint at arena_allocate:
break arena_allocate

run server:
run

inspect local vars in arena_allocate:
info locals

examine content of a var (replace var_name with the actual var name):
p var_name

examine mem around specific address (replace address with the actual address):
x/8x address

set a watchpoint on the 'arena' variable in main:
watch arena

run server:
run

valgrind mem checks:
run Valgrind for mem leak detection:
valgrind --leak-check=full ./output_weasel_server