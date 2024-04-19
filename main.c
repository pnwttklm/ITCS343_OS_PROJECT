#include <stdio.h>
#include <string.h>
#include <pthread.h>
#define B 300
#define R 100

char storage[R][B];
int indexStorageIn = 0;
char word[R][B];
int indexWordIn = 0;
int indexWordOut = 0;

void *readFile(void *arg)
{
	char temp[B];
	while (1)
	{
		if (scanf("%s", temp) != EOF)
		{
			strcpy(word[indexWordIn], temp);
			indexWordIn = (indexWordIn + 1) % R;
		}
		else
		{
			break;
		}
	}
}
void getWord() {
	char temp[B];
	while (1)
	{
		if (scanf("%s", temp) != EOF)
		{
			strcpy(word[indexStorageIn], temp);
			indexStorageIn = (indexStorageIn + 1) % R;
		}
		else
		{
			break;
		}
	}
}
void *spellCheck(void *arg)
{
	while (1)
	{
		if (indexWordOut != indexWordIn)
		{
			word[indexWordOut];
			indexWordOut = (indexWordOut + 1) % R;
		}else{
			break;
		}
	}
}
void counter() {}
void writer() {}

int main()
{
	pthread_t pid;
	pthread_create(&pid, NULL, readFile, NULL);
	pthread_join(pid, NULL);
	for (int i = 0; i < indexWordIn; i++)
	{
		printf("%s\n", word[i]);
	}
	return 0;
}