FLAGS = -g -Wall -Wextra -fsanitize=address

all: basic_test

serialize.o: serialize.c helpers.h
	gcc $(FLAGS) -c serialize.c

deserialize.o: deserialize.c helpers.h
	gcc $(FLAGS) -c deserialize.c

helpers.o: helpers.c helpers.h
	gcc $(FLAGS) -c helpers.c

serialize: serialize.o helpers.o
	gcc $(FLAGS) -o serialize serialize.o helpers.o

deserialize: deserialize.o helpers.o
	gcc $(FLAGS) -o deserialize deserialize.o helpers.o

basic_test: serialize deserialize
	@./serialize < test.dat > output.dat
	@./deserialize < output.dat
	@if [ $$? -eq 0 ]; then\
		echo "Basic test passed";\
	else \
		echo "Basic test failed";\
	fi

clean:
	rm -f serialize deserialize *.o output.dat
