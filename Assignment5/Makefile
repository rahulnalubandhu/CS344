all:
	gcc -std=gnu99 -o enc_server enc_server.c utility.c 
	gcc -std=gnu99 -o enc_client enc_client.c utility.c 
	gcc -std=gnu99 -o dec_server dec_server.c utility.c 
	gcc -std=gnu99 -o dec_client dec_client.c utility.c 
	gcc -std=gnu99 -o keygen keygen.c

test:
	bash compileall
	gnome-terminal -- ./enc_server 2224
	./enc_client plaintext1 keytest 2224 > output
	pkill -9 enc_server
	diff output outtest

clean:
	rm -f enc_server enc_client
	rm -f dec_server dec_client
	rm -f keygen
