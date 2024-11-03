#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "UDP.h"

//--------------------------------------------------------------------------------------------------------------------------

#define MAX_MESSAGE_SIZE 40960

//--------------------------------------------------------------------------------------------------------------------------

int UDP_server(int port)
{
	int server_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	char message[MAX_MESSAGE_SIZE];
	fd_set read_fds;  // Множество дескрипторов для select()
	int max_fd;       // Максимальный дескриптор

    // Создание UDP сокета
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        perror("Не удалось создать сокет");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Привязка сокета к адресу и порту
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка привязки");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("UDP-сервер запущен и ожидает сообщения на порту %d...\n", port);

    while (1) {
        // Настройка множества дескрипторов для `select`
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);  // Добавляем стандартный ввод
        FD_SET(server_fd, &read_fds);     // Добавляем сокет сервера

        // Устанавливаем максимальный дескриптор для `select`
        max_fd = (STDIN_FILENO > server_fd) ? STDIN_FILENO : server_fd;

        // Ожидание событий на любом из дескрипторов
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Ошибка select");
            break;
        }

        // Проверка, есть ли данные от клиента
        if (FD_ISSET(server_fd, &read_fds)) {
            int bytes_received = recvfrom(server_fd, message, MAX_MESSAGE_SIZE, 0,
                                          (struct sockaddr *)&client_addr, &client_addr_len);
            if (bytes_received < 0) {
                perror("Ошибка при получении сообщения");
                continue;
            }

            message[bytes_received] = '\0';
            printf("Сообщение от клиента: %s\n", message);

            // Проверка на команду завершения "exit" от клиента
            if (strcmp(message, "exit") == 0) {
                printf("Клиент завершил сеанс.\n");
                continue;
            }

            // Отправка ответа клиенту
            printf("Отправка ответа клиенту...\n");
            if (sendto(server_fd, message, bytes_received, 0,
                       (struct sockaddr *)&client_addr, client_addr_len) < 0) {
                perror("Ошибка отправки сообщения");
            }
        }

        // Проверка, есть ли команда от сервера
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            fgets(message, MAX_MESSAGE_SIZE, stdin);
            message[strcspn(message, "\n")] = '\0';

            // Проверка на команду завершения сервера
            if (strcmp(message, "server_exit") == 0) {
                printf("Сервер завершает работу.\n");
                break;  // Выход из основного цикла для завершения работы сервера
            }

            // Отправка сообщения последнему клиенту, если он известен
            if (client_addr_len > 0) {
                if (sendto(server_fd, message, strlen(message), 0,
                           (struct sockaddr *)&client_addr, client_addr_len) < 0) {
                    perror("Ошибка отправки сообщения клиенту");
                }
            } else {
                printf("Нет подключённых клиентов для отправки сообщения.\n");
            }
        }
    }

    // Закрытие сервера
    close(server_fd);
    printf("Сервер завершил работу.\n");

	return 0;
}

//--------------------------------------------------------------------------------------------------------------------------

int UDP_client(const char* address, int port)
{
    int sock_fd;
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    char message[MAX_MESSAGE_SIZE];
    fd_set read_fds;
    int max_fd;

    // Создание UDP сокета
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Не удалось создать сокет");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0) {
        perror("Неверный адрес или ошибка преобразования");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Настройка множества дескрипторов для `select`
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);  // Ввод пользователя
        FD_SET(sock_fd, &read_fds);       // Сокет для данных от сервера

        // Устанавливаем максимальный дескриптор для `select`
        max_fd = (STDIN_FILENO > sock_fd) ? STDIN_FILENO : sock_fd;

        // Ожидание события на любом из дескрипторов
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Ошибка select");
            break;
        }

        // Проверка, есть ли данные от сервера
        if (FD_ISSET(sock_fd, &read_fds)) {
            int bytes_received = recvfrom(sock_fd, message, MAX_MESSAGE_SIZE, 0,
                                          (struct sockaddr *)&server_addr, &server_addr_len);
            if (bytes_received < 0) {
                perror("Ошибка при получении ответа от сервера");
            } else {
                message[bytes_received] = '\0';
                printf("Ответ от сервера: %s\n", message);
            }
        }

        // Проверка, есть ли ввод с клавиатуры
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            printf("Введите сообщение для отправки (или 'exit' для завершения): ");
            fgets(message, MAX_MESSAGE_SIZE, stdin);
            message[strcspn(message, "\n")] = '\0';

            // Проверка на команду завершения клиента
            if (strcmp(message, "exit") == 0) {
                printf("Клиент завершает сеанс.\n");
                break;
            }

            // Отправка сообщения серверу
            if (sendto(sock_fd, message, strlen(message), 0,
                       (struct sockaddr *)&server_addr, server_addr_len) < 0) {
                perror("Ошибка отправки сообщения");
            }
        }
    }

    // Закрытие клиента
    close(sock_fd);
    printf("Клиент завершил работу.\n");

	return 0;
}

//--------------------------------------------------------------------------------------------------------------------------
