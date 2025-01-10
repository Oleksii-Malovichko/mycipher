#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t g_ack_received = 0;
volatile int tab[128];

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
	wait_for_ack();
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

int main(int n, char **args)
{
	int pid = atoi(args[1]);
	srand(time(NULL));
	int size = 128;
	struct sigaction sa;
	printf("Server PID: %d\n", getpid());

	sa.sa_handler = handle_ack;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGUSR1, &sa, NULL) == -1 || sigaction(SIGUSR2, &sa, NULL) == -1)
		return (EXIT_FAILURE);
	make_random(tab, size);
	for (int i = 0; i < size; i++)
	{
		send_number(pid, tab[i]);
		// wait_for_ack();
		printf("%d: %d\n", i, tab[i]);
		usleep(300);
	}
}
/* после создания массива мы передаем все эти числа клиенту, это и будет симметричный ключ сдвига*/
// printf("\n");
// printf("%d\n", find_couple(tab, size));
