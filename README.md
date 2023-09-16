# weasel_server.c

compile: gcc -o output_weasel_server weasel_server.c index_html_string.c
compile for perf optimization:gcc -o output_weasel_server weasel_server.c index_html_string.c -O3

debugging:
-Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined i.e: gcc -o output_weasel_server weasel_server.c index_html_string.c -Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined

debug w/ openssl:
gcc -o output_weasel_server weasel_server.c index_html_string.c -Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined -lssl -lcrypto

actual executable(for prod):
-Wall -Wextra -Wpedantic -Werror -O2 i.e: gcc -o output_weasel_server weasel_server.c index_html_string.c -Wall -Wextra -Wpedantic -Werror -O2

run: ./output_weasel_server

ssh into aws instance:
ssh -i /path/to/key-pair.pem ec2-user@your-public-ipv4-dns

