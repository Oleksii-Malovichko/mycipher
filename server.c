#include "server.h"

volatile int arr[128];

int ft_strlen(char *str)
{
	int i = 0;
	while (str[i] != '\0')
	{
		i++;
	}
	return i;
}

void	swap(char *c1, char *c2)
{
	char temp = *c1;

	*c1 = *c2;
	*c2 = temp;
}

void	move_letter(char str[], int start, int to_move)
{
	int str_len;
	int diff;

	str_len = ft_strlen(str) - 1;
	if (start < 0 || start >= str_len) { // Проверяем границы индекса
        return;
    }
	if (to_move < 0)
		to_move = -to_move;
	diff = (start + to_move) % str_len;
	if (diff < 0) {
        diff += str_len;
    }
	if (diff != start) { // Меняем местами только если индексы различны
        swap(&str[start], &str[diff]);
    }
	// printf("diff: %d\n", diff);
	// swap(&str[start], &str[diff]);
	// printf("start %d; diff %d\n", start, diff);
}

char *cipher_string(char *line, int str_len)
{
	int size = sizeof(arr) / sizeof(arr[0]);
	// мешалка
	int i = 0;
	while (i < size && line[i] != '\0')
	{
		move_letter(line, i, arr[i]);
		i++;
	}
	printf("Shuffled: %s\n", line);

    // Восстановление
    i = size - 1;
    while (i >= 0) {
        move_letter(line, i, -arr[i]); // Используем обратный сдвиг
        i--;
    }
    printf("Restored: %s\n", line);
}

void	handle_signal_bytes(int sig, siginfo_t *info, void *context) // получение ключа
{
	(void)context;
	int client_pid;
	static s_data *data = NULL;
	if (!data)
	{
		data = malloc(sizeof(s_data));
		if (!data)
			exit(1);
		data->bit_index = 0;
		data->current_value = 0;
		data->index = 0;
	}
	client_pid = info->si_pid;
	if (sig == SIGUSR2)
		data->current_value |= (1 << data->bit_index);
	data->bit_index++;
	if (data->bit_index == 32)
	{
		arr[data->index] = data->current_value;
		data->bit_index = 0;
		data->current_value = 0;
		data->index++;		
	}
	if (kill(client_pid, SIGUSR2) == -1)
		exit(EXIT_FAILURE);
	if (data->index == 128)
	{
		// printf("write smth\n");
		for (int i = 0; i < 128; i++)
		{
			printf("%d, ",arr[i]);
		}
		printf("\n");
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		while ((read = getline(&line, &len, stdin)) != -1)
		{
			cipher_string(line, ft_strlen(line));
			// printf("\n");
		}
		free(line);
		free(data);
		data = NULL;
	}
}

int main(void)
{
	struct sigaction sa;
	printf("Server PID: %d\n", getpid());

	sa.sa_sigaction = handle_signal_bytes;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGUSR1, &sa, NULL) == -1 || sigaction(SIGUSR2, &sa, NULL) == -1)
		return (EXIT_FAILURE);
	while (1)
		pause();
}