# weasel_server.c

compile: gcc -o output_weasel_server weasel_server.c index_html_string.c
compile for perf optimization:gcc -o output_weasel_server weasel_server.c index_html_string.c -O3

debugging:
-Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined i.e: gcc -o output_weasel_server weasel_server.c index_html_string.c -Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined

debug w/ openssl:
gcc -o output_weasel_server weasel_server.c index_html_string.c -Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined -lssl -lcrypto

executable(for prod):
-Wall -Wextra -Wpedantic -Werror -O2 i.e: gcc -o output_weasel_server weasel_server.c index_html_string.c -Wall -Wextra -Wpedantic -Werror -O2 -lssl -lcrypto

run: ./output_weasel_server

hacktoberfest contributions welcome! 
To-dos:
1. read html, js and css files
2. test and make sure ssl is working as intended
3. blog articles + end-points (add .md files as the blog articles, and only associate the end-point with the title of the blog article)
4. back-end pagination (so the user can navigate to older articles - whether or not there is an end-point here is irrelevant)
   In summary, page paths should only be: home page, title or article + article, maybe a 404 - essentially only 3 page-renders/ views need to be worked with
