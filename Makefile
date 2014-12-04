all:
	gcc -o s ftp_server.c -w -g
	gcc -o c ftp_client.c -w -g
	

clean:
	rm -r s
	rm -r c
