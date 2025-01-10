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
    if (diff != start)
    {
        swap(&str[start], &str[diff]);
    }
}

void	get_big_arr(int **arr, int *size, int *ascii_arr, int size_ascii, char str[])
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

int main()
{
	char str[] = "mamaqpicacho. hello world, how are you?";
	int str_len = ft_strlen(str);

	int *to_move = (int *)malloc(sizeof(int) * 3);
	to_move[0] = 5;
	to_move[1] = 1;
	to_move[2] = 10;
	int size = 3;

	int *big_arr;
	int size_big = 0;
	if (size < str_len)
	{	
		get_big_arr(&big_arr, &size_big, to_move, size, str);
		free(to_move);
		to_move = big_arr;
		size = size_big;
	}
	// мешалка
	int i = 0;
	while (i < size && str[i] != '\0')
	{
		move_letter(str, i, to_move[i]);
		i++;
	}
	printf("Shuffled: %s\n", str);

    // Восстановление
    i = str_len - 1;
    while (i >= 0 && str[i] != '\0')
	{
        move_letter(str, i, to_move[i]);
        i--;
    }
    printf("Restored: %s\n", str);
}

