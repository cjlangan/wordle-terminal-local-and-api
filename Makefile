all: wordle

.PHONY: clean api

wordle: wordle.c
	clang -Wall wordle.c -o wordle

api:
	clang -Wall api-wordle.c -o api-wordle -lcurl -ljson-c

clean:
	rm -f wordle api-wordle
