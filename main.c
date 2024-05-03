// POONYAWATT's
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define SEM_FLAG_NAME1 "/fortytwo"
#define SEM_FLAG_NAME2 "/fortythree"
#define SEM_FLAG_NAME3 "/fortyfour"

#define SEM_FLAG_NAME4 "/fortyfive"
#define SEM_FLAG_NAME5 "/fortysix"
#define SEM_FLAG_NAME6 "/fortyseven"

#define SEM_FLAG_NAME7 "/fortyeight"
#define SEM_FLAG_NAME8 "/fortynine"

#define B 300
#define R 100

char storage[R][B];
int indexStorageIn = 0;
char word[R][B];
char editedWord[R][B];
int indexWordIn = 0;
int indexWordOut = 0;

int indexEditedWordIn = 0;
int indexEditedWordOut = 0;
int indexEditedWordWriterOut = 0;

int noOfChar = 0;
int noOfWord = 0;
int noOfMisspelled = 0;
int turn = 0;

sem_t *empty, *full, *mutex;
sem_t *empty2, *full2, *mutex2;
sem_t *writer_turn, *counter_turn;

void *readFile(void *arg)
{
	char temp[B];
	while (1)
	{
		if (scanf("%s", temp) != EOF)
		{
			sem_wait(empty);
			sem_wait(mutex);
			strcpy(word[indexWordIn], temp);
			sem_post(mutex);
			sem_post(full);
			indexWordIn = (indexWordIn + 1) % R;

			// printf("%s\n", word[indexWordIn]);
		}
		else
		{
			break;
		}
	}
	for (int i = 0; i < 1; i++)
	{
		sem_wait(empty);
		sem_wait(mutex);
		strcpy(word[indexWordIn], "<end>");
		sem_post(mutex);
		sem_post(full);
		indexWordIn = (indexWordIn + 1) % R;
	}
	printf("ReadFile died\n");
}
void *spellCheck(void *arg)
{
	FILE *file;
	char filename[] = "list.txt";
	char buffer[100];
	char wordTemp[100];
	char wordTemp1[100];
	int tmp = 0;
	while (tmp != -1)
	{
		sem_wait(full);
		sem_wait(mutex);
		strcpy(wordTemp, word[indexWordOut]);
		printf("%s\n", wordTemp);
		fflush(stdin);
		indexWordOut = (indexWordOut + 1) % R;
		sem_post(mutex);
		sem_post(empty);
		if (strcmp(wordTemp, "<end>") == 0)
		{
			sem_wait(empty2);
			sem_wait(mutex2);
			strcpy(editedWord[indexEditedWordIn], "<end>");
			indexEditedWordIn = (indexEditedWordIn + 1) % R;
			sem_post(mutex2);
			sem_post(full2);
			break;
		}
		int flag = 0;
		file = fopen(filename, "r");
		if (file == NULL)
		{
			printf("Error opening the file.\n");
		}
		while (fgets(buffer, sizeof(buffer), file) != NULL)
		{
			size_t newline_pos = strcspn(buffer, "\n");
			if (buffer[newline_pos] == '\n')
			{
				buffer[newline_pos] = '\0';
			}
			if (strcmp(wordTemp, buffer) == 0)
			{
				strcpy(wordTemp1, wordTemp);
				flag = 1;
				break;
			}
		}
		if (flag == 0)
		{
			strcpy(wordTemp1, "[MS]");
			strcat(wordTemp1, wordTemp);
			strcat(wordTemp1, "[/MS]");
		}
		sem_wait(empty2);
		sem_wait(mutex2);
		strcpy(editedWord[indexEditedWordIn], wordTemp1);
		indexEditedWordIn = (indexEditedWordIn + 1) % R;
		sem_post(mutex2);
		sem_post(full2);
		printf("%d %s %s\n", flag, wordTemp, wordTemp1);
	}
	fclose(file);
	printf("spellCheck died\n");
}
void *counter(void *arg)
{
	while (1)
	{
		printf("Waiting for Counter turn\n");
		sem_wait(counter_turn);
		printf("Counter turn\n");
		fflush(stdout);
		// get item from editedWord
		sem_wait(full2);
		sem_wait(mutex2);
		char temp[B];
		strcpy(temp, editedWord[indexEditedWordOut]);

		if (strcmp(temp, "<end>") == 0)
		{
			sem_post(writer_turn);
			break;
		}
		// check if temp start with [MS]
		if (temp[0] == '[' && temp[1] == 'M' && temp[2] == 'S' && temp[3] == ']')
		{
			noOfMisspelled++;
			// count no of char without [MS] and [/MS]
			noOfChar += strlen(temp) - 9;
		}
		else
		{
			noOfChar += strlen(temp);
		}
		noOfWord++;
		sem_post(writer_turn);
	}
	printf("COunter died\n");
}
void *writer(void *arg)
{
	FILE *file_pointer, *file_pointer2;
	char data[100];
	file_pointer = fopen("output.txt", "w");
	while (1)
	{
		printf("Waiting for Writer turn\n");
		sem_wait(writer_turn);
		printf("Writer turn\n");
		fflush(stdout);
		// get item from editedWord
		// sem_wait(full2);
		// sem_wait(mutex2);
		char temp[B];
		strcpy(temp, editedWord[indexEditedWordOut]);
		indexEditedWordOut = (indexEditedWordOut + 1) % R;
		sem_post(mutex2);
		sem_post(empty2);

		if (strcmp(temp, "<end>") == 0)
		{
			break;
		}
		fprintf(file_pointer, "%s\n", temp);
		fflush(file_pointer);
		file_pointer2 = fopen("stats.txt", "w");

		fprintf(file_pointer2, "No of word: %d\n", noOfWord);
		fprintf(file_pointer2, "No of char: %d\n", noOfChar);
		fprintf(file_pointer2, "No of misspelled: %d\n", noOfMisspelled);
		fflush(file_pointer2);
		fclose(file_pointer2);

		sem_post(counter_turn);
	}

	fclose(file_pointer);
	printf("Writer died\n");
}

int main()
{
	empty = sem_open(SEM_FLAG_NAME1, O_CREAT, 0666, R);
	full = sem_open(SEM_FLAG_NAME2, O_CREAT, 0666, 0);
	mutex = sem_open(SEM_FLAG_NAME3, O_CREAT, 0666, 1);

	empty2 = sem_open(SEM_FLAG_NAME4, O_CREAT, 0666, R);
	full2 = sem_open(SEM_FLAG_NAME5, O_CREAT, 0666, 0);
	mutex2 = sem_open(SEM_FLAG_NAME6, O_CREAT, 0666, 1);

	writer_turn = sem_open(SEM_FLAG_NAME7, O_CREAT, 0666, 0);
	counter_turn = sem_open(SEM_FLAG_NAME8, O_CREAT, 0666, 1);

	sem_close(empty);
	sem_close(full);
	sem_close(mutex);
	sem_close(empty2);
	sem_close(full2);
	sem_close(mutex2);
	sem_close(writer_turn);
	sem_close(counter_turn);
	sem_unlink(SEM_FLAG_NAME1);
	sem_unlink(SEM_FLAG_NAME2);
	sem_unlink(SEM_FLAG_NAME3);
	sem_unlink(SEM_FLAG_NAME4);
	sem_unlink(SEM_FLAG_NAME5);
	sem_unlink(SEM_FLAG_NAME6);
	sem_unlink(SEM_FLAG_NAME7);
	sem_unlink(SEM_FLAG_NAME8);

	empty = sem_open(SEM_FLAG_NAME1, O_CREAT, 0666, R);
	full = sem_open(SEM_FLAG_NAME2, O_CREAT, 0666, 0);
	mutex = sem_open(SEM_FLAG_NAME3, O_CREAT, 0666, 1);

	empty2 = sem_open(SEM_FLAG_NAME4, O_CREAT, 0666, R);
	full2 = sem_open(SEM_FLAG_NAME5, O_CREAT, 0666, 0);
	mutex2 = sem_open(SEM_FLAG_NAME6, O_CREAT, 0666, 1);

	writer_turn = sem_open(SEM_FLAG_NAME7, O_CREAT, 0666, 0);
	counter_turn = sem_open(SEM_FLAG_NAME8, O_CREAT, 0666, 1);

	pthread_t pid;
	pthread_create(&pid, NULL, readFile, NULL);
	pthread_t pid1;
	pthread_create(&pid1, NULL, spellCheck, NULL);
	pthread_t pid2;
	pthread_create(&pid2, NULL, counter, NULL);
	pthread_t pid3;
	pthread_create(&pid3, NULL, writer, NULL);
	pthread_join(pid, NULL);
	pthread_join(pid1, NULL);
	pthread_join(pid2, NULL);
	pthread_join(pid3, NULL);
	printf("Edited word\n");
	for (int i = 0; i < indexWordIn; i++)
	{
		printf("%s\n", editedWord[i]);
	}
	printf("No of word: %d\n", noOfWord);
	printf("No of char: %d\n", noOfChar);
	printf("No of misspelled: %d\n", noOfMisspelled);
	sem_close(empty);
	sem_close(full);
	sem_close(mutex);
	sem_close(empty2);
	sem_close(full2);
	sem_close(mutex2);
	sem_close(writer_turn);
	sem_close(counter_turn);
	sem_unlink(SEM_FLAG_NAME1);
	sem_unlink(SEM_FLAG_NAME2);
	sem_unlink(SEM_FLAG_NAME3);
	sem_unlink(SEM_FLAG_NAME4);
	sem_unlink(SEM_FLAG_NAME5);
	sem_unlink(SEM_FLAG_NAME6);
	sem_unlink(SEM_FLAG_NAME7);
	sem_unlink(SEM_FLAG_NAME8);
	return 0;
}
