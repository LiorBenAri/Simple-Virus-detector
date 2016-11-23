
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int memcmp_impl(const void* str1, const void* str2, size_t n){
	char* str11 = (char*)str1;
	char* str22 = (char*)str2;
	int ans = 0; 
	int i;
	for (i = 0; i < n; ++i)
	{
		if (*(str11+i) != *(str22+i))
		{
			ans = -1;
			return ans;
		}
	}
	return ans;
}

typedef struct virus virus;

struct virus {
	unsigned short length;
	char name[16];
	char signature[];
};

typedef struct link link;

struct link {
	virus *v;
	link *next;
};

/*append to the head of the list*/
link* list_append(link* virus_list, virus* data)
{
	link* new_link = (link*) malloc(sizeof(*data) + sizeof(link));
	new_link->v = data;
	new_link->next = NULL;

	if (virus_list != NULL)
	{
		new_link->next = virus_list;
	}
	return new_link;
}

/* Free the memory allocated by the list. */
void list_free(link* virus_list)
{
	link* curr = virus_list;
	link* prev; 
	while(curr != NULL)
	{
		free(curr->v);
		prev = curr;
		curr = curr->next;
		free(prev);
	}
}

void detect_virus(char *buffer, link *virus_list, unsigned int size)
{
	if (virus_list == NULL)
		return;

	int buffspot;
	link* curr = virus_list;
	char* curr_sig;
	int sig_len;

	while(curr != NULL)
	{
		curr_sig = (curr->v)->signature;
		sig_len = (curr->v)->length;
		buffspot = 0;
		while(buffspot+sig_len<=size)
		{
			if(memcmp_impl(buffer+buffspot, curr_sig, sig_len)==0)
			{
				printf("Found Virus!\n");
				printf("Virus starting point on the file: %i\n", buffspot);
				printf("Virus name: %s\n", (curr->v)->name);
				printf("Virus signature size: %i\n", sig_len);
				printf("\n");
				buffspot = buffspot+sig_len-1;
			}
			buffspot++;
		}
		curr = curr->next;
	}	
}

int main(int argc, char **argv)
{
	int buffersize = 10*1024;
	char* buffer = (char*) malloc(buffersize);
	FILE* f;
	FILE* suspect_f;
	unsigned int size;
	int fsize;
	char len[2];
	unsigned int len_value;
	link* head = NULL;
	virus* first_virus = NULL;

	suspect_f = fopen("infected", "rb");
	if (suspect_f == NULL)
	{
		fprintf(stderr, "can't open file\n");
		exit(-1);
	}

	fseek(suspect_f, 0, SEEK_END);	
	size = ftell(suspect_f);
	if (size > buffersize)
	{
		size = buffersize;
	}
	fseek(suspect_f, 0, SEEK_SET);
	fread(buffer, 1, size, suspect_f);
	fclose(suspect_f);

	f = fopen("signatures", "rb");
	if (f == NULL)
	{
		fprintf(stderr, "can't open file\n");
		exit(-1);
	}

	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	fread(len, 1, 2, f);
	len_value = ((unsigned int)((len[0])*16*16) + (unsigned int)len[1]);
	first_virus = (virus*) malloc(len_value);
	first_virus->length = len_value-18;
	fread(first_virus->name, 1, len_value-2, f);

	head = (link*) malloc(len_value + sizeof(link));
	head->v = first_virus;
	head->next = NULL;

	while(ftell(f)!=fsize)
	{
		virus* curr_virus;
		fread(len, 1, 2, f);
		len_value = ((unsigned int)((len[0])*16*16) + (unsigned int)len[1]);
		curr_virus = (virus*) malloc(len_value);
		curr_virus->length = len_value-18;
		fread(curr_virus->name, 1, len_value-2, f);

		head = list_append(head, curr_virus);
	}

	detect_virus(buffer, head, size);

	free(buffer);

	list_free(head);

	fclose(f);

	return 0;
}

/*if(memcmp(buffer+buffspot, curr_sig, sig_len)==0)*/ 

