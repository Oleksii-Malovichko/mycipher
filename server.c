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

void send_str(int pid, char ch)
{
    int bit_index;
    int bit;

    bit_index = 0;
    while (bit_index < 8)
    {
        bit = (ch >> bit_index) & 1;
		send_bit(pid, bit);
        bit_index++;
    }
}

void	cipher_string(char *str, siginfo_t *info)
{
	int str_len = ft_strlen(str);
	int size = 128;

	volatile int *big_arr;
	int size_big = 0;
	big_arr = arr;
	if (size < str_len)
	{	
		get_big_arr(&big_arr, &size_big, arr, size, str);
		size = size_big;
	}
	int i = 0;
	while (i < size && str[i] != '\0')
	{
		move_letter(str, i, big_arr[i]);
		i++;
	}
	printf("Shuffled: %s\n", str);
	printf("Pid to send: %d\n", info->si_pid);
	i = 0;
	while (str[i] != '\0')
	{
		send_str(info->si_pid, str[i]);
		i++;
	}
	send_str(info->si_pid, '\0');
	// i = str_len - 1;
    // while (i >= 0 && str[i] != '\0')
	// {
    //     move_letter(str, i, big_arr[i]);
    //     i--;
    // }
    // printf("Restored: %s\n", str);
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
	// if (kill(client_pid, SIGUSR2) == -1)
	// 	exit(EXIT_FAILURE);
	if (data->index == 128)
	{
		printf("write smth\n");
		for (int i = 0; i < 128; i++)
		{
			printf("%d, ",arr[i]);
		}
		printf("\n");
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		while ((read = getline(&line, &len, stdin)) != -1) // получение строки
		{
			printf("line: %s", line);
			cipher_string(line, info);
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