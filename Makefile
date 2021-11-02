.PHONY: all clean run
CC=g++
CFLAGS=-std=c++11
LDFLAGS=-lm
TARGET=train test
TRAIN_ITER=100

all: $(TARGET)

train: src/train.cpp src/utils.cpp
	$(CC) -o train src/train.cpp $(CFLAGS) $(LDFLAGS) -Iinc

test: src/test.cpp src/utils.cpp
	$(CC) -o test src/test.cpp $(CFLAGS) $(LDFLAGS) -Iinc

unitest: src/unitest.cpp src/utils.cpp
	$(CC) -o unitest src/unitest.cpp $(CFLAGS) $(LDFLAGS) -Iinc

run_train_all: train
	./train 100 model_init.txt data/train_seq_01.txt model_01.txt
	./train 100 model_init.txt data/train_seq_02.txt model_02.txt
	./train 100 model_init.txt data/train_seq_03.txt model_03.txt
	./train 100 model_init.txt data/train_seq_04.txt model_04.txt
	./train 100 model_init.txt data/train_seq_05.txt model_05.txt

run_test: test
	./test modellist.txt data/test_seq.txt result.txt

run_train_1: train
	./train 100 model_init.txt data/train_seq_01.txt model_01.txt

run_unitest: unitest
	./unitest

clean:
	rm -f $(TARGET)
