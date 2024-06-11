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

valgrind mem checks:
run Valgrind for mem leak detection:
valgrind --leak-check=full ./output_weasel_server

Note to self: slap this all in a .sh build file or something