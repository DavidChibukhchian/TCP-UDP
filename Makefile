# Название исполняемого файла
TARGET = build/network_example

# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -g -Iinclude

# Файлы исходного кода и заголовки
SRC = source/main.c source/tcp.c source/udp.c
OBJ = $(SRC:source/%.c=build/%.o)

# Правило по умолчанию - сборка всего проекта
all: build $(TARGET)

# Создаем исполняемый файл из объектных файлов
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Общее правило для компиляции всех .c файлов в .o файлы в папке build
build/%.o: source/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

# Создаем папку build, если её нет
build:
	mkdir -p build

# Очистка скомпилированных файлов
clean:
	rm -rf build

# Файлы, которые не являются реальными целями
.PHONY: all clean build
