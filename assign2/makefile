all: producer consumer producer_no_me consumer_no_me producer_timed consumer_timed producer_no_me_timed consumer_no_me_timed

producer: producer.o
	gcc -o producer producer.c

consumer: consumer.o
	gcc -o consumer consumer.c

producer_without_me: producer_without_me.o
	gcc -o	producer_without_me	producer_without_me.c
	
consumer_without_me: consumer_without_me.o
	gcc -o	consumer_without_me	consumer_without_me.c

producer_timed: producer_timed.o
	gcc -o producer_timed producer_timed.c

consumer_timed: consumer_timed.o
	gcc -o consumer_timed consumer_timed.c

producer_without_me_timed: producer_without_me_timed.o
	gcc -o	producer_without_me_timed	producer_without_me_timed.c
	
consumer_without_me_timed: consumer_without_me_timed.o
	gcc -o	consumer_without_me_timed	consumer_without_me_timed.c
