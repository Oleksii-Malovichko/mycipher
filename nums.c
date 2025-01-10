#include <stdio.h>
#include <stdlib.h>

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
	printf("diff: %d\n", diff);
    if (diff != start)
    {
        swap(&str[start], &str[diff]);
    }
}

int main()
{
	char str[] = "mamaqpicacho";
	int str_len = ft_strlen(str);

	int to_move[] = {1, 7, -10, 1, 7, -10,1, 7, -10,}; // 
	int size = sizeof(to_move) / sizeof(to_move[0]);

	// мешалка
	int i = 0;
	int index = 0;
	while (i < size && str[i] != '\0')
	{
		if (index == 3)
		{
			index = 0;
		}
		move_letter(str, i, to_move[i]);
		i++;
		index++;
	}
	printf("Shuffled: %s\n", str);

    // Восстановление
    i = str_len - 1;
	index = size - 1;
    while (i >= 0 && str[i] != '\0')
	{
		if (index == -1)
		{
			index = size -1;
		}
        move_letter(str, i, to_move[i]); // Используем обратный сдвиг
        i--;
		index--;
    }
    printf("Restored: %s\n", str);
}

