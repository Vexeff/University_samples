OBJECTS = 
CFLAGS = -g -Wall -O3 --std=c11
LDLIBS= -l criterion -lm
INCLUDES=./include
SOURCE= ./src
TESTS=./tests
BIN= ./bin
CC=clang

all: student_test_a_star test_a_star

student_test_a_star: $(SOURCE)/a_star.c $(SOURCE)/student_test_a_star.c $(SOURCE)/util.c
	$(CC) $(CFLAGS) $? -o $(BIN)/$@ -I $(INCLUDES) $(LDLIBS)

test_a_star: $(TESTS)/test_a_star.c $(SOURCE)/a_star.c $(SOURCE)/util.c
	$(CC) $(CFLAGS) $?  -o $(BIN)/$@ -I $(INCLUDES) $(LDLIBS)

gen_score: test_a_star
	-bin/test_a_star --json > results.log 2> results.json
	python3 tests/grader.py

clean:
	rm -f results.json results.log
	rm -f $(BIN)/student_test_a_star $(BIN)/test_a_star
	rm -rf $(BIN)/*.dSYM
	rm -rf *~ */*~
