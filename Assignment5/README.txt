so we usually type "make" in terminal 
2. now do ./enc_server <portnumber> then do enter
3. now open new bash do enc_client <plaintext_filename> <key> <portnumber> > <cipherfilename> (here the port number should be same as the server port number).
4. now open an another bash do ./dec_server <portnumber> then do enter.
5. now open new bash do dec_client <cipher_filename> <key> <portnumber> > <plaintextfilename> (here the port number should be same as the server port number).

in order to run grading script do ./p5testscript <portnumber_1> <portnumber_2> > mytestresults 2 > &1.