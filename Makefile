CC = gcc
CFLAGS = -Wall -Wextra -std=c11

canvascii: canvascii.c
	$(CC) $(CFLAGS) canvascii.c -o canvascii

html: README.md sujet.md
	pandoc -s --metadata title="Readme" -c misc/github-pandoc.css -o README.html README.md
	pandoc -s --metadata title="sujet" -c misc/github-pandoc.css -o sujet.html sujet.md

test: canvascii unit_test
	env TEST_ENV=ci bats check.bats

clean:
	rm -f canvascii *.o *.html
