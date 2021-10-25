.PHONY: all clean run
CC=gcc
CFLAGS=-std=c99 -O2
LDFLAGS=-lm
TARGET=train test
TRAIN_ITER=100

all: $(TARGET)

train: src/train.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc

test: src/test.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc

test_hmm: src/test_hmm.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc

run_train: train
	./train 100 model_init.txt data/train_seq_01.txt model_01.txt

clean:
	rm -f $(TARGET)

