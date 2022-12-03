all: thready

thready: thready.c
	gcc thready.c -o thready

clean:
	rm -f thready
