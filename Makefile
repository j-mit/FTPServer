all:
	gcc -o s ftp_server.c -w
	gcc -o c ftp_client.c -w
	

clean:
	rm -r s
	rm -r c
