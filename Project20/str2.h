#pragma once
int str_to_num(char *str)
{
	int i = 0, result = 0, neg = 1;
	if (str[0] == '-')
	{
		neg = -1;
		i++;
	}
	for (; str[i] != '\0'; i++)
		result = result * 10 + str[i] - '0';
	return result*neg;
}

char* to_str(int val)
{
	char *result = (char*)malloc(sizeof(char) * 10);
	int temp, sizeOfValTemp, i = 0, sizeOfVal = 0;
	temp = val;
	if (val == 0)
		sizeOfVal = 1;
	else
		while (temp != 0)
		{
			temp /= 10;
			sizeOfVal++;
		}
	sizeOfValTemp = sizeOfVal;
	do{
		result[sizeOfVal - 1] = '0' + (val % 10);
		val /= 10;
		sizeOfVal--;
	} while (sizeOfVal != 0);
	result[sizeOfValTemp] = '\0';
	return result;
}

void str_cpy(char *source, char *destination)
{
	int i = 0;
	do
	{
		destination[i] = source[i];
	} while (source[i++] != '\0');
}


char** splitStr(char *str, char delimiter, int *num)
{
	int i, j = 0, size = 10, numberOfStrings = 0;// openParenthasis = 0;
	char **strings;
	strings = (char**)malloc(sizeof(char*) * 10);
	for (i = 0; i < size; i++)
		strings[i] = (char*)malloc(sizeof(char) * 30);
	for (i = 0; str[i] != '\0'; i++)
	{
		if ((str[i] == delimiter) || str[i] == '\n')
		{
			if (j == 0)
			{
				continue;
			}
			strings[numberOfStrings][j] = '\0';
			j = 0;
			if (numberOfStrings == size - 1)
			{
				size += 5;
				strings = (char**)realloc(strings, sizeof(char*)*size);
				for (j = numberOfStrings + 1; j < size; j++)
				{
					strings[j] = (char*)malloc(sizeof(char) * 30);
				}
				j = 0;
			}
			numberOfStrings++;
			continue;
		}
		strings[numberOfStrings][j] = str[i];
		j++;
	}
	if (j == 0)
		numberOfStrings--;
	else
		strings[numberOfStrings][j] = '\0';
	*num = numberOfStrings + 1;
	size--;
	for (; size != numberOfStrings; size--)
		free(strings[size]);
	return strings;
}

int str_concat(char *destination, char *str2)
{
	int i = 0, j = 0;
	for (i = 0; destination[i] != '\0'; i++);
	for (j = 0; str2[j] != '\0'; j++, i++)
		destination[i] = str2[j];
	destination[i] = '\0';
	return i;
}

int str_cmp(char *str1, char *str2)
{
	int i = 0;
	while ((str1[i] == str2[i] || str1[i] - 'a' == str2[i] - 'A' || str1[i] - 'A' == str2[i] - 'a') && (str1[i] != '\0') && (str2[i] != '\0')){
		i++;
	}
	if ((str1[i] != '\0') && (str2[i] != '\0'))
	{
		if (str1[i] > str2[i])
			return -1;
		else
			return 1;
	}
	if (str1[i] == '\0' &&str2[i] != '\0')
	{
		return 1;
	}
	else if (str2[i] == '\0'&&str1[i] != '\0')
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
