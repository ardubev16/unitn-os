TARGET = filename
SOURCE = main.c
FLAGS = -Wall

all:
ifdef DEST
	@if [ -d $(DEST) ]; then \
		gcc $(FLAGS) $(SOURCE) -o $(DEST)/$(TARGET); \
	else \
		echo "?ERROR" >&2; \
	fi
else
	@gcc $(FLAGS) $(SOURCE) -o $(TARGET)
endif

