TARGET = SignalProxy
SOURCE = main.c
FLAGS = -Wall

all:
ifdef FILE
	@if [ ! -f "$(FILE)" ]; then \
		echo "start" > "$(FILE)"; \
	fi
	@gcc $(FLAGS) $(SOURCE) -o $(TARGET);
else
	@echo "?ERROR" >&2;
endif

clean:
ifdef FILE
	@if [ -f $(FILE) ]; then \
		rm $(FILE); \
	fi
else
	@echo "?ERROR" >&2;
endif
