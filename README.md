# weasel_server.c

compile: gcc -o output_weasel_server weasel_server.c index_html_string.c

run: ./output_weasel_server

ssh into aws instance:
ssh -i /path/to/key-pair.pem ec2-user@your-public-ipv4-dns

wip
