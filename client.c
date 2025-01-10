#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include "client.h"

volatile sig_atomic_t g_ack_received = 0;
volatile int arr[128];

void	handle_ack(int sig)
{
	(void)sig;
	g_ack_received = 1;
}

void	wait_for_ack(void)
{
	while (!g_ack_received)
		pause();
	g_ack_received = 0;
}

void send_bit(int pid, int bit)
{
    if (bit == 0)
    {
        if (kill(pid, SIGUSR1) == -1)
            exit(EXIT_FAILURE);
    }
    else
    {
        if (kill(pid, SIGUSR2) == -1)
            exit(EXIT_FAILURE);
    }
	// wait_for_ack();
	usleep(300);
}

void send_number(int pid, int length)
{
    int bit_index;
    int bit;

    bit_index = 0;
    while (bit_index < 32)
    {
        bit = (length >> bit_index) & 1;
		send_bit(pid, bit);
        bit_index++;
    }
}

int random_range(void)
{
	int min = -100;
    int max = 100;
	return (rand() % (max - min + 1) + min);
}

void	make_random(volatile int tab[], int size)
{
	int i = 0;
	int random;
	while (i < size)
	{
		random = random_range();
		tab[i] = random;
		if (i > 0)
		{
			random = random_range();
			if (tab[i - 1] != random)
			{
				tab[i] = random;
			}
			else
				tab[i] = random_range() + (random_range() + 10) / random_range();
		}
		i++;
	}
}

int	find_couple(int tab[], int size)
{
	int i = 0;
	int j;
	int result_of_couples = 0;
	while (i < size)
	{
		j = i + 1;
		while (j < size)
		{
			if (tab[i] == tab[j])
				result_of_couples++;
			j++;
		}
		i++;
	}
	return (result_of_couples);
}

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
	int str_len = ft_strlen(str);
    int diff;

	if (to_move < 0)
	{
		to_move = -to_move;
	}
    diff = (start + to_move) % str_len;
    if (diff < 0)
        diff += str_len;
    if (diff != start)
    {
        swap(&str[start], &str[diff]);
    }
}

void	get_big_arr(volatile int **arr, int *size, volatile int *ascii_arr, int size_ascii, char str[])
{
	int str_len = ft_strlen(str);
	int i = 0;
	int cur_index = 0;
	*size = 0;

	while (*size < str_len)
	{
		(*size)++;
	}
	printf("new size: %d\n", *size);
	*arr = (int *)malloc(sizeof(int *) * (*size));
	if (!*arr)
		exit(1);
	while (i < *size)
	{
		if (cur_index == size_ascii)
		{
			cur_index = 0;
		}
		(*arr)[i] = ascii_arr[cur_index];
		cur_index++;
		i++;
	}
}

void	uncipher_string(char *str)
{
	int str_len = ft_strlen(str);
	int size = 128;
	int i;
	volatile int *big_arr;
	int size_big = 0;

	big_arr = arr;
	if (size < str_len)
	{	
		get_big_arr(&big_arr, &size_big, arr, size, str);
		size = size_big;
	}
	i = str_len - 1;
    while (i >= 0 && str[i] != '\0')
	{
        move_letter(str, i, big_arr[i]);
        i--;
    }
    printf("Restored: %s\n", str);
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
	char *result = malloc(11);
	if (data->bit_index == 8)
	{
		// result[data->index] = data->current_value;
		// arr[data->index] = data->current_value;
		write(1, &data->current_value, 1);
		data->bit_index = 0;
		data->current_value = 0;
		data->index++;
	}
	// if (kill(client_pid, SIGUSR2) == -1)
	// 	exit(EXIT_FAILURE);
	// printf("index: %d\n", data->index);
	// if (data->current_value == '\0' || data->index == 10)
	// {
	// 	// printf("result: %s\n", result);
	// 	// uncipher_string(result);
	// 	exit(0) ;
	// }
	// if (data->index == 10)
	// {
	// 	// printf("write smth\n");
	// 	// for (int i = 0; i < 128; i++)
	// 	// {
	// 	// 	printf("%d, ",arr[i]);
	// 	// }
	// 	// printf("\n");
	// 	char *line = NULL;
	// 	size_t len = 0;
	// 	ssize_t read;
	// 	uncipher_string(line);
	// 	// while ((read = getline(&line, &len, stdin)) != -1) // получение строки
	// 	// {
	// 	// 	printf("line: %s", line);
	// 	// 	// printf("\n");
	// 	// }
	// 	free(line);
	// 	free(data);
	// 	data = NULL;
	// }
}

int main(int n, char **args)
{
	int pid = atoi(args[1]);
	srand(time(NULL));
	int size = 128;
	struct sigaction sa;
	printf("Server PID: %d\n", getpid());

	// sa.sa_handler = handle_ack
	sa.sa_sigaction = handle_signal_bytes;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGUSR1, &sa, NULL) == -1 || sigaction(SIGUSR2, &sa, NULL) == -1)
		return (EXIT_FAILURE);
	make_random(arr, size);
	for (int i = 0; i < size; i++)
	{
		send_number(pid, arr[i]);
		printf("%d: %d\n", i, arr[i]);
		usleep(300);
	} 
	while (1)
		pause();
}
/* после создания массива мы передаем все эти числа клиенту, это и будет симметричный ключ сдвига*/
// printf("\n");
// printf("%d\n", find_couple(tab, size));
