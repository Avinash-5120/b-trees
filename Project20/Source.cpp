#include<stdio.h>
#include<stdlib.h>
#include"str2.h"
#include<Windows.h>

#define PageSize 128
#define STUDENTRECSIZE 36
#define MARKSRECSIZE 20
#define StudentTable 1
#define MarksTable 2
#define StopPrint 555
#define SizeOfPool 10

struct page
{ 
	int inUse = 0;
	int counter=-1;
	char *allocated;
};

struct Table_page
{
	byte table_type = 3;
	byte table_id = 0;
	char unused[6];
	int root_page_id[10];
	char table_name[10][8];
};

struct Marks
{
	int id;
	int marks[4];
};

struct Student
{
	int id;
	char name[32];
};



//struct IndexPage
//{
//	int pageid[16];
//	int key[15];
//	char unused[4];
//};

//struct DataPage{
//	struct Student records[3];
//	char unused[20];
//};

struct Student_datapage
{
	byte page_type;
	byte table_id;
	char unused[18];
	Student records[3];
};

struct Marks_data_page
{
	byte page_type;
	byte table_id;
	char unused[6];
	Marks records[6];
};

struct Indexpage
{
	byte pagetype;
	byte table_id;
	byte pagePointingTo;
	char unused[1];
	int pageid[16];
	int key[15];
};

page pagePool[SizeOfPool];

void* search(Indexpage *, int , FILE *, int *);
void* goToDataPage(int , int , FILE *,int);
int computeQuery(char **, int , int , Student *, Marks *, int , char **);
void whereCon(int, int, FILE *, int *, int , char **, int , char **, int );
void** getRange(int , int , Indexpage *, FILE *, int *);

char* getPageFromPool(char *memoryVar,int initialiseMem)
{
	static char *memory;
	static int counter = 0;
	int i, j, oldFreePage=-1,min;
	if (initialiseMem == 1)
	{
		memory = memoryVar;
		return NULL;
	}
	min = 0;
	for (i = 0; i < SizeOfPool; i++)
	{
		if (min>pagePool[i].counter && pagePool[i].inUse == 0)
		{
			oldFreePage = i; min = pagePool[i].counter;
		}
	}
	if (oldFreePage == -1)
	{
		printf("please Check Memory\n");
		return NULL;
	}
	pagePool[oldFreePage].allocated = &memory[oldFreePage * 128];
	pagePool[oldFreePage].counter = ++counter;
	pagePool[oldFreePage].inUse = 1;
	return pagePool[oldFreePage].allocated;
}

void freeThePage(char *pageStart)
{
	int i = 0;
	for (i = 0; i < SizeOfPool; i++)
	{
		if (pageStart == pagePool[i].allocated)
		{
			pagePool[i].inUse = 0;
			break;
		}
	}
}

int copyStudentFromString(char **strings,Student *student)
{
	student->id = str_to_num(strings[0]);
	str_cpy(strings[1], student->name);
	return 0;
}

int copyMarksFromString(char **strings, Marks *marks)
{
	marks->id = str_to_num(strings[0]);
	for (int i = 0; i < 4; i++)
		marks->marks[i] = str_to_num(strings[i + 1]);
	return 0;
}

void initialiseDataPage(Student_datapage *dataPage,int maxRec)
{
	int recCount = 0;
	while (recCount != maxRec)
	{
		dataPage->records[recCount].id = -1;
		for (int i = 0; i < 32; i++)
			dataPage->records[recCount].name[i] = '\0';
		recCount++;
	}
	for (int i = 0; i < 18; i++)
		dataPage->unused[i] = '\0';
	dataPage->page_type = 0;//data
	dataPage->table_id = StudentTable;	
}

void initialiseMarksDataPage(Marks_data_page *dataPage, int maxRec)
{
	int recCount = 0;
	while (recCount != maxRec)
	{
		dataPage->records[recCount].id = -1;
		for (int i = 0; i < 4; i++)
			dataPage->records[recCount].marks[i]=-1;
		recCount++;
	}
	for (int i = 0; i < 16; i++)
		dataPage->unused[i] = '\0';
	dataPage->page_type = 0;//data
	dataPage->table_id = MarksTable;
}

void initialiseIndex(Indexpage *indexp,int TableId)
{
	indexp->pagetype = 1;//index
	for (int i = 0; i < 2; i++)
		indexp->unused[i] = '\0';
	indexp->table_id = TableId;
}

int copyStudentDataPageFromFile(FILE *fp, FILE *wt)
{
	char **strings, buf[250];
	Student_datapage *datapage;
	//IndexPage indePage;
	int i = 0, recCount, num = 0, maxRec, PageCount = 0, indexUpdateFlag = 0, PageIdCount = 0;
	datapage = (Student_datapage*)getPageFromPool(NULL, 0);
	recCount = 0;
	maxRec = PageSize / STUDENTRECSIZE;
	initialiseDataPage(datapage, maxRec);
	while (fgets(buf, 249, fp) != NULL)
	{
		if (recCount == maxRec)
		{
			recCount = 0;
			fwrite(datapage, 1, sizeof(Student_datapage), wt);
			initialiseDataPage(datapage, maxRec); indexUpdateFlag = 1;
			PageCount++; PageIdCount++;
		}
		strings = splitStr(buf, ',', &num);
		copyStudentFromString(strings, &datapage->records[recCount]);
		recCount++;
	}
	if (recCount != maxRec && recCount!=0)
	{
		while (recCount != maxRec)
		{
			datapage->records[recCount].id=-1;
			for (int i = 0; i < 32; i++)
				datapage->records[recCount].name[i] = '\0';
			recCount++;
		}
	}
	fwrite(datapage, 1, sizeof(Student_datapage), wt);
	freeThePage((char*)datapage);
	fclose(wt);
	return 0;
}

int copyMarksDataPageFromFile(FILE *fp, FILE *wt)
{
	char **strings, buf[250];
	Marks_data_page *datapage;
	//IndexPage indePage;
	int i = 0, recCount, num = 0, maxRec, PageCount = 0, indexUpdateFlag = 0, PageIdCount = 0;
	recCount = 0;
	datapage = (Marks_data_page*)getPageFromPool(NULL, 0);
	maxRec = PageSize / MARKSRECSIZE;
	initialiseMarksDataPage(datapage, maxRec);
	while (fgets(buf, 249, fp) != NULL)
	{
		if (recCount == maxRec)
		{
			recCount = 0;
			fwrite(datapage, 1, sizeof(Marks_data_page), wt);
			initialiseMarksDataPage(datapage, maxRec);
			indexUpdateFlag = 1;
			PageCount++; PageIdCount++;
		}
		strings = splitStr(buf, ',', &num);
		copyMarksFromString(strings, &datapage->records[recCount]);
		recCount++;
	}
	if (recCount != maxRec&&recCount!=0)
	{
		while (recCount != maxRec)
		{
			initialiseMarksDataPage(datapage, maxRec);
			recCount++;
		}
	}
	fwrite(datapage, 1, sizeof(Marks_data_page), wt);
	freeThePage((char*)datapage);
	fclose(wt);
	return 0;
}

long copyMarksIndexesToFile(FILE *wt,int *pageIdsCount)
{
	char **strings, buf[250];
	Marks_data_page *datapage;
	Indexpage *indePage, *BufindexPage;
	int i = 0, recCount, num = 0, maxRec, PageCount = 0, indexUpdateFlag = 0, PageIdCount = 0;
	long fileCurrent, fileEnd = 0, rootIndexFseek;
	int startFlag = 0;
	int indexPagesCounted = 0;
	recCount = 0;
	maxRec = PageSize / MARKSRECSIZE;
	datapage = (Marks_data_page*)getPageFromPool(NULL, 0);
	indePage = (Indexpage*)getPageFromPool(NULL, 0);
	BufindexPage = (Indexpage*)getPageFromPool(NULL, 0);
	initialiseMarksDataPage(datapage, maxRec);
	PageCount = 0;
	PageIdCount = *pageIdsCount;
	fileCurrent = ftell(wt);
	fseek(wt, 0, SEEK_END); 
	fileEnd = ftell(wt);
	fseek(wt, fileCurrent, SEEK_SET);
	initialiseIndex(indePage, MarksTable);
	indePage->pagePointingTo = 0;
	while (ftell(wt) != fileEnd)
	{
		//PageCount++;
		fread(datapage, 1, sizeof(Marks_data_page), wt);
		if (startFlag == 0)
		{
			startFlag++;
			PageIdCount++;
			PageCount++;
			continue;
		}
		if (PageCount == 1)
		{
			indePage->pageid[0] = PageIdCount - 1;
		}
		indePage->key[PageCount - 1] = datapage->records[0].id;
		indePage->pageid[PageCount] = PageIdCount;
		PageCount++;
		PageIdCount++;
		if (PageCount == 16)
		{
			indexPagesCounted++;
			PageCount = 0;
			fileCurrent = ftell(wt);
			fseek(wt, 0, SEEK_END);
			rootIndexFseek = ftell(wt);
			fwrite(indePage, sizeof(Indexpage), 1, wt);
			fseek(wt, fileCurrent, SEEK_SET);
		}
	}
	if (PageCount != 16&&PageCount!=0)
	{
		indexPagesCounted++;
		while (PageCount != 16)
		{
			indePage->pageid[PageCount] = -1;
			indePage->key[PageCount - 1] = -1;
			PageCount++;
		}
		fileCurrent = ftell(wt);
		fseek(wt, 0, SEEK_END);
		rootIndexFseek = ftell(wt);
		fwrite(indePage, sizeof(Indexpage), 1, wt);
		fseek(wt, fileCurrent, SEEK_SET);
	}
	if (indexPagesCounted == 1)
	{
		fclose(wt);
		freeThePage((char*)datapage);
		freeThePage((char*)indePage);
		freeThePage((char*)BufindexPage);
		return rootIndexFseek;
	}
	fseek(wt, 0, SEEK_END);
	fileEnd = ftell(wt);
	indePage->pagePointingTo = 1;
	fseek(wt, fileCurrent, SEEK_SET);
	while (1)
	{
		startFlag = 0;
		indexPagesCounted = 0;
		//PageIdCount = 0;
		PageCount = 0;
		while (ftell(wt) != fileEnd)
		{
			fread(BufindexPage, 1, sizeof(Indexpage), wt);
			//PageCount++;
			if (startFlag == 0)
			{
				startFlag++;
				PageIdCount++;
				PageCount++;
				continue;
			}
			if (PageCount == 1)
			{
				indePage->pageid[0] = PageIdCount - 1;
			}
			indePage->key[PageCount - 1] = BufindexPage->key[0];
			indePage->pageid[PageCount] = PageIdCount;
			PageCount++;
			PageIdCount++;
			if (PageCount == 16)
			{
				indexPagesCounted++;
				PageCount = 0;
				startFlag = 0;
				fileCurrent = ftell(wt);
				fseek(wt, 0, SEEK_END);
				rootIndexFseek = ftell(wt);
				fwrite(indePage, sizeof(Indexpage), 1, wt);
				fseek(wt, fileCurrent, SEEK_SET);
			}
		}
		if (PageCount != 16)
		{
			indexPagesCounted++;
			while (PageCount != 16)
			{
				indePage->pageid[PageCount] = -1;
				indePage->key[PageCount - 1] = -1;
				PageCount++;
			}
			fileCurrent = ftell(wt);
			fseek(wt, 0, SEEK_END);
			rootIndexFseek = ftell(wt);
			fwrite(indePage, sizeof(Indexpage), 1, wt);
			fseek(wt, fileCurrent, SEEK_SET);
		}
		fseek(wt, 0, SEEK_END);
		fileEnd = ftell(wt);
		fseek(wt, fileCurrent, SEEK_SET);
		if (indexPagesCounted == 1)
			break;
	}
	fclose(wt);
	freeThePage((char*)datapage);
	freeThePage((char*)indePage);
	freeThePage((char*)BufindexPage);
	return rootIndexFseek;
}

long copyStudentIndexesToFile(FILE *wt,int *pageIdsCount)
{
	char **strings, buf[250];
	Student_datapage *datapage;
	Indexpage *indePage,*BufindexPage;
	int i = 0, recCount, num = 0, maxRec, PageCount = 0, indexUpdateFlag = 0, PageIdCount = 0;
	long fileCurrent, fileEnd = 0,rootIndexFseek;
	int startFlag = 0;
	int indexPagesCounted = 0;
	datapage = (Student_datapage*)getPageFromPool(NULL, 0);
	indePage = (Indexpage*)getPageFromPool(NULL, 0);
	BufindexPage = (Indexpage*)getPageFromPool(NULL, 0);
	recCount = 0;
	maxRec = PageSize / STUDENTRECSIZE;
	initialiseDataPage(datapage, maxRec);
	PageCount = 0;
	//PageIdCount = 0;
	fseek(wt, 0, SEEK_END);
	fileEnd = ftell(wt);
	fseek(wt, 0, SEEK_SET);
	initialiseIndex(indePage, StudentTable);
	indePage->pagePointingTo = 0;//data
	while (ftell(wt)!=fileEnd)
	{
		//PageCount++;
		fread(datapage, 1, sizeof(Student_datapage), wt);
		if (startFlag == 0)
		{
			startFlag++;
			PageIdCount++;
			PageCount++;
			continue;
		}
		if (PageCount == 1)
		{
			indePage->pageid[0] = PageIdCount-1;
		}
		indePage->key[PageCount-1] = datapage->records[0].id;
		indePage->pageid[PageCount] = PageIdCount;
		PageCount++;
		PageIdCount++;
		if (PageCount == 16)
		{
			indexPagesCounted++;
			PageCount = 0;
			fileCurrent = ftell(wt);
			fseek(wt, 0, SEEK_END);
			rootIndexFseek = ftell(wt);
			fwrite(indePage, sizeof(Indexpage), 1, wt);
			fseek(wt, fileCurrent, SEEK_SET);
		}
	}
	if (PageCount != 16&&PageCount!=0)
	{
		indexPagesCounted++;
		while (PageCount != 16)
		{
			indePage->pageid[PageCount] = -1;
			indePage->key[PageCount - 1] = -1;
			PageCount++;
		}
		fileCurrent = ftell(wt);
		fseek(wt, 0, SEEK_END);
		rootIndexFseek = ftell(wt);
		fwrite(indePage, sizeof(Indexpage), 1, wt);
		fseek(wt, fileCurrent, SEEK_SET); 
	}
	if (indexPagesCounted == 1)
	{
		fclose(wt);
		freeThePage((char*)datapage);
		freeThePage((char*)indePage);
		freeThePage((char*)BufindexPage);
		*pageIdsCount = PageIdCount+indexPagesCounted;
		return rootIndexFseek;
	}
	fseek(wt, 0, SEEK_END);
	fileEnd = ftell(wt);
	indePage->pagePointingTo = 1;
	fseek(wt, fileCurrent, SEEK_SET);
	while (1)
	{
		startFlag = 0;
		indexPagesCounted = 0;
		PageCount = 0;
		while (ftell(wt) != fileEnd)
		{
			fread(BufindexPage, 1, sizeof(Indexpage), wt);
			//PageCount++;
			if (startFlag == 0)
			{
				startFlag++;
				PageIdCount++;
				PageCount++;
				continue;
			}
			if (PageCount == 1)
			{
				indePage->pageid[0] = PageIdCount - 1;
			}
			indePage->key[PageCount - 1] = BufindexPage->key[0];
			indePage->pageid[PageCount] = PageIdCount;
			PageCount++;
			PageIdCount++;
			if (PageCount == 16)
			{
				indexPagesCounted++;
				PageCount = 0;
				startFlag = 0;
				fileCurrent = ftell(wt);
				fseek(wt, 0, SEEK_END);
				rootIndexFseek = ftell(wt);
				fwrite(indePage, sizeof(Indexpage), 1, wt);
				fseek(wt, fileCurrent, SEEK_SET);
			}
		}
		if (PageCount != 16)
		{
			indexPagesCounted++;
			while (PageCount != 16)
			{
				indePage->pageid[PageCount] = -1;
				indePage->key[PageCount - 1] = -1;
				PageCount++;
			}
			fileCurrent = ftell(wt);
			fseek(wt, 0, SEEK_END);
			rootIndexFseek = ftell(wt);
			fwrite(indePage, sizeof(Indexpage), 1, wt);
			fseek(wt, fileCurrent, SEEK_SET);
		}
		fseek(wt, 0, SEEK_END);
		fileEnd = ftell(wt);
		fseek(wt, fileCurrent, SEEK_SET);
		*pageIdsCount += indexPagesCounted;
		if (indexPagesCounted == 1)
			break;
	}
	*pageIdsCount += PageIdCount;
	fclose(wt);
	freeThePage((char*)indePage);
	freeThePage((char*)BufindexPage);
	freeThePage((char*)datapage);
	//*pageIdsCount = PageIdCount;
	return rootIndexFseek;
}
	//if (recCount == maxRec)
		//{
		//	recCount = 0;
		//	//fwrite(&datapage, 1, sizeof(DataPage), wt);
		//	//initialiseDataPage(&datapage, maxRec); 
		//	indexUpdateFlag = 1;
		//	PageCount++;
		//	PageIdCount++;
		//}
		//strings = splitStr(buf, ',', &num);
		//copyStudentFromString(strings, &datapage.records[recCount]);
		/*if (indexUpdateFlag == 1)
		{
			indexUpdateFlag = 0;
			indePage.pageid[PageIdCount-1] = PageCount;
			if (PageIdCount == 16)
			{
				PageIdCount = 0;
				fwrite(&indePage, 1, sizeof(IndexPage), wt);
			}
			else
			{
				indePage.key[PageIdCount - 1] = datapage.records[0].id;
			}
		}
		recCount++;*/
	/*if (recCount != maxRec)
	{
		while (recCount != maxRec)
		{
			datapage.records[recCount].id = -1;
			for (i = 0; i < 32; i++)
				datapage.records[recCount].name[i] = '\0';
			recCount++;
		}
	}*/
	//fwrite(&datapage, 1, sizeof(DataPage), wt);

int print(Student **students,int num)
{
	int i = 0;
	for (i = 0; i < num; i++)
	{
		printf("%d %s\n", students[num]->id, students[num]->name);
	}
	return 0;
}



void** getAll(int start, Indexpage *index, FILE *wt,int *num)
{
	int temp = 0;
	Student *st=(Student*)search(index, -1, wt, &temp);
	int end = st->id;
	return (void**)getRange(1, end, index, wt, num);
}

void** getRange(int start, int end,Indexpage *rootindex,FILE *wt,int *num)
{
	Student **students;
	int count = 0,temp;
	students = (Student**)malloc(sizeof(Student*)*(end - start + 1));
	while (start <= end)
	{
		students[count]=(Student*)search(rootindex, start, wt, &temp);
		if (students[count] != NULL)
			count++;
		start++;
	}
	*num = count;
	return (void**)students;
}

//int selectFun(char **strings, int numOfStrings, FILE *wt,long rootNode)
//{
//	Indexpage *rootIndex=(Indexpage*)malloc(sizeof(Indexpage));
//	int seekPos,num;
//	fseek(wt, rootNode, SEEK_SET);
//	fread(rootIndex, 1, sizeof(Indexpage), wt);
//	if (numOfStrings == 2 &&str_cmp(strings[1],"*")==0)
//	{
//		Student **stus;
//		stus = getAll(1, rootIndex, wt, &num);
//		print(stus, num);
//	}
//	else if (str_cmp(strings[2], "WHERE") == 0)
//	{
//
//	}
//
//}

int UI(FILE *WT, long StudentRootNode,long marksRootNode)
{
	char buf[40], **strings, **toPrintStrs, *subs[4] = { "SUB1", "SUB2", "SUB3", "SUB4" };
	int numOfTokens,i,frompos,*toprint,printCount;
	Indexpage ind;
	while (1)
	{
		printf("$>");
		scanf("%[^\n]s",buf);
		getchar();
		strings = splitStr(buf, ' ', &numOfTokens);
		if (str_cmp(strings[0], "select") == 0)
		{
			for (i = 0; i < numOfTokens;i++)
				if (str_cmp(strings[i], "FROM") == 0)
				{
					frompos = i+1;
					break;
				}
			toPrintStrs = splitStr(strings[1], ',', &printCount);
			toprint = (int*)malloc(sizeof(int)*(1+printCount));
			for (i = 0; i < printCount; i++)
			{
				if (str_cmp(toPrintStrs[i], "name") == 0)
					toprint[i] = 1;
				else if (str_cmp(toPrintStrs[i], "SID") == 0)
					toprint[i] = 2;
				else if (str_cmp(toPrintStrs[i], "MID") == 0)
					toprint[i] = 3;
				else
				{
					for (int j = 0; j < 4; j++)
						if (str_cmp(toPrintStrs[i], subs[j]) == 0)
						{
							toprint[i] = 4 + j;
							break;
						}
				}
			}
			toprint[i] = StopPrint;
			int stseek;
			int maseek;
			fseek(WT, StudentRootNode, SEEK_SET);
			fread(&ind, 1, sizeof(Indexpage), WT);
			search(&ind, 1, WT, &stseek);
			fseek(WT, marksRootNode, SEEK_SET);
			fread(&ind, 1, sizeof(Indexpage), WT);
			search(&ind, 1, WT, &maseek);
			whereCon(stseek,maseek,WT,toprint,frompos,strings,numOfTokens,subs,4);
		}
	}
}

int main()
{
	FILE *fp = fopen("sample.bin", "wb");
	FILE *rd = fopen("randomuserdata.csv", "r");
	char **strings, buf[201];
	int counter = 0;
	long rootOfStudents,rootOfMArks,startPos;
	/*DataPage b;
	IndexPage inP;*/
	//copyDataPageFromFile(rd, fp); 
	Table_page *tb;
	void *memory = malloc(128 * 10);
	getPageFromPool((char*)memory, 1);
	fgets(buf, 200, rd);
	copyStudentDataPageFromFile(rd, fp);
	fp = fopen("sample.bin", "a+b");
	fclose(rd);
	rootOfStudents=copyStudentIndexesToFile(fp,&counter);
	fp = fopen("sample.bin", "a+b");
	fclose(fp);
	rd = fopen("SudentMarksDataSet_Version_2.csv", "r");//edit file
	fgets(buf, 200, rd);
	fp = fopen("sample.bin", "a+b");
	fseek(fp, 0, SEEK_END);
	startPos = ftell(fp);
	copyMarksDataPageFromFile(rd, fp);
	fp = fopen("sample.bin", "a+b");
	fseek(fp, startPos, SEEK_SET);
	rootOfMArks = copyMarksIndexesToFile(fp,&counter);
	tb = (Table_page*)getPageFromPool(NULL, 0);
	tb->table_id = 3;
	str_cpy("stu", tb->table_name[0]);
	tb->root_page_id[0] = rootOfStudents;
	str_cpy("marks", tb->table_name[1]);
	tb->root_page_id[1] = rootOfMArks;
	tb->table_type = 2;
	fp = fopen("sample.bin", "a+b");
	fseek(fp, 0, SEEK_END);
	fwrite(tb, 1, sizeof(Table_page), fp);
	fclose(fp);
	freeThePage((char*)tb);
	fp = fopen("sample.bin", "rb");
	UI(fp, rootOfStudents,rootOfMArks);

	fclose(fp);
	


	//copyIndexesToFile(fp);
	//struct ch
	//{
	//	//short b;
	//	int a;
	//	char na[32];
	//	char diatrict[20];
	//};
	//ch b[6];
	//while ((fgets(buf, 49, rd) != NULL) && i != 6)
	//{
	//	int num = 0;
	//	strings = splitStr(buf, ',', &num);
	//	b[i].a = str_to_num(strings[0]);
	//	str_cpy(strings[1], b[i].na);
	//	str_cpy(strings[2], b[i].diatrict);
	//	printf("%d %s %s\n", b[i].a, b[i].diatrict, b[i].na);
	//	i++;
	//}
	//fwrite(b, sizeof(struct ch), 6, fp);
	//fclose(fp);
	//fp = fopen("sample.bin", "rb");
	/*fp = fopen("sample.bin", "rb");
	int pageCount = 0;
	while (1)
	{
		if (pageCount != 16)
		{
			if (fread(&b, 1, sizeof(DataPage), fp) != 0)
			{
				for (int i = 0; i < 3; i++)
				{
					if (b.records[i].id == -1)
					{
						pageCount = 15;
					}
					printf("%d %s\n", b.records[i].id, b.records[i].name);
				}
			}
			else
				break;
			pageCount++;
		}
		else
		{
			pageCount = 0;
			if (fread(&inP, 1, sizeof(IndexPage), fp) != 0)
			{
				printf("keys: ");
				for (int i = 0; i < 15; i++)
					printf("%d ", inP.key[i]);
				printf("PageIds: ");
				for (int i = 0; i < 16; i++)
					printf("%d ", inP.pageid[i]);
				printf("\n");
			}
			else
				break;
		}
	}
	*/
	fclose(fp);
	//fp=fopen("sample2.bin", "w");
	//fwrite(&b, sizeof(v), 1, fp);
	system("pause");
}

void* goToDataPage(int startingPage, int search_key,FILE *wt,int table)
{
	Student *st;
	Marks *mrk;
	Student_datapage *dp1;
	Marks_data_page *dp2;
	int i = 2;
	if (table == StudentTable)
	{
		dp1 = (Student_datapage*)getPageFromPool(NULL, 0);
		st = (Student*)getPageFromPool(NULL, 0);
		fseek(wt, (startingPage)* 128, SEEK_SET);
		fread(dp1, sizeof(Student_datapage), 1, wt);
		if (search_key == -1)
		{
			while (dp1->records[i].id == -1){ i--; }
			*st = dp1->records[i];
			freeThePage((char*)dp1);
			//freeThePage((char*)st);
			return st;
		}
		for (i = 0; i < 3; i++)
			if (dp1->records[i].id == search_key)
			{
				*st = dp1->records[i];
				freeThePage((char*)dp1);
				//freeThePage((char*)st);
				return st;
			}
	}
	if (table == MarksTable)
	{
		dp2 = (Marks_data_page*)getPageFromPool(NULL, 0);
		mrk = (Marks*)getPageFromPool(NULL, 0);
		fseek(wt, (startingPage)* 128, SEEK_SET);
		fread(&dp2, sizeof(Marks_data_page), 1, wt);
		if (search_key == -1)
		{
			while (dp2->records[i].id == -1){ i--; }
			*mrk = dp2->records[i];
			freeThePage((char*)dp2);
			return mrk;
		}
		for (i = 0; i < 6; i++)
			if (dp2->records[i].id == search_key)
			{
				freeThePage((char*)dp2);
				*mrk = dp2->records[i];
				return mrk;
			}
	}
	return NULL;
}

void* search(Indexpage *ind,int searchKey,FILE *wt,int *fseekPos)
{
	int i = 0,count=14;
	Indexpage *nextPage=NULL;
	int temp,tableid;
	if (searchKey == -1)
	{
		while (ind->key[count] == -1){ count--; };
		if (ind->pagePointingTo == 0)
		{
			*fseekPos = ind->pageid[i + 1] * 128;
			temp=ind->pageid[i + 1];
			tableid = ind->table_id;
			free((char*)ind);
			return goToDataPage(temp, searchKey, wt,tableid);
		}
		nextPage = (Indexpage*)getPageFromPool(NULL, 0);
		fseek(wt, ind->pageid[i + 1], SEEK_SET);
		freeThePage((char*)ind);
		fread(nextPage, sizeof(Indexpage), 1, wt);
		return search(nextPage, searchKey, wt, fseekPos);
	}
	for (i = 0; i < 14;i++)
		if (ind->key[i] <= searchKey&&ind->key[i + 1] > searchKey)
		{
			if (ind->pagePointingTo == 0)
			{
				//freeThePage((char*)nextPage);
				*fseekPos = ind->pageid[i + 1] * 128;
				temp = ind->pageid[i + 1];
				tableid = ind->table_id;
				free((char*)ind);
				return goToDataPage(temp, searchKey, wt, tableid);
			}
			fseek(wt, ind->pageid[i + 1], SEEK_SET);
			//fread(nextPage, sizeof(Indexpage), 1, wt);
			//return search(nextPage, searchKey, wt,fseekPos);
		}
	if (ind->key[0] > searchKey)
	{
		if (ind->pagePointingTo == 0)
		{
			freeThePage((char*)nextPage);
			*fseekPos = ind->pageid[0] * 128;
			temp = ind->pageid[0];
			tableid = ind->table_id;
			free((char*)ind);
			return goToDataPage(temp, searchKey, wt, tableid);
			//return goToDataPage(ind->pageid[0], searchKey, wt,ind->table_id);
		}
		nextPage = (Indexpage*)getPageFromPool(NULL, 0);
		fseek(wt, ind->pageid[0], SEEK_SET);
		freeThePage((char*)ind);
		fread(nextPage, sizeof(Indexpage), 1, wt);
		return search(nextPage, searchKey, wt,fseekPos);
	}
	else
	{
		if (ind->pagePointingTo == 0)
		{
			*fseekPos = ind->pageid[15] * 128;
			temp = ind->pageid[15];
			tableid = ind->table_id;
			free((char*)ind);
			return goToDataPage(temp, searchKey, wt, tableid);
			
			//return goToDataPage(ind->pageid[15], searchKey, wt,ind->table_id);
		}
		nextPage = (Indexpage*)getPageFromPool(NULL, 0);
		fseek(wt, ind->pageid[15], SEEK_SET);
		freeThePage((char*)ind);
		fread(nextPage, sizeof(Indexpage), 1, wt);
		return search(nextPage, searchKey, wt,fseekPos);
	}
	return NULL;
}

void printData(int *printArray,Student *stu,Marks *mark)
{
	int i;
	for (i = 0;; i++)
	{
		if (printArray[i]==StopPrint)
		{
			printf("\n");
			break;
		}
		else if (printArray[i] == 1)//name
		{
			printf("%s ", stu->name);
		}
		else if (printArray[i] == 2)//sid
		{
			printf("%d ", stu->id);
		}
		else if (printArray[i] == 3)//mid
		{
			printf("%d ", mark->id);
		}
		else//scores
		{
			printf("%d ", mark->marks[printArray[i]-4]);
		}
	}
}

void whereCon(int fseekStudent, int fseekMarks, FILE *wt, int *printArray, int fromPos, char **Strings, int numOfStrings, char **subs, int numberofSubs)
{
	FILE *marksIterater, *stuIterator;
	Student_datapage Sdata;
	Marks_data_page Mdata;
	Student studentBuf;
	Marks marksBuf;
	int studentCounter = 0, marksCounter = 0, stFlag = 0, MrFlag = 0;
	marksIterater = wt;
	stuIterator = wt;
	stFlag = 0;
	MrFlag = 0;
	if (str_cmp(Strings[fromPos], "StuTable") == 0 || str_cmp(Strings[fromPos+1], "StuTable") == 0)
	{
		stFlag = 1;
		Sdata.page_type = 0;
		fseek(wt, fseekStudent, SEEK_SET);
		fread(&Sdata, 1, sizeof(Student_datapage), stuIterator);
		fseekStudent = ftell(wt);
		fromPos++;
	}
	else if (str_cmp(Strings[fromPos], "MarTable") == 0 || str_cmp(Strings[fromPos-1], "MarTable") == 0)
	{
		MrFlag = 1;
		Mdata.page_type = 0;
		fseek(wt, fseekMarks, SEEK_SET);
		fread(&Mdata, 1, sizeof(Marks_data_page), marksIterater);
		fseekMarks = ftell(wt);
		fromPos++;
	}
	while (1)
	{
		if (stFlag == 1 && Sdata.page_type == 1)
			break;
		if (MrFlag == 1 && Mdata.page_type == 1)
			break;
		if (studentCounter == 3&&stFlag==1)
		{
			fseek(wt, fseekStudent, SEEK_SET);
			fread(&Sdata, 1, sizeof(Student_datapage), stuIterator);
			fseekStudent = ftell(wt);
			studentCounter = 0;
		}
		if (marksCounter == 6&&MrFlag==1)
		{
			fseek(wt, fseekMarks, SEEK_SET);
			fread(&Mdata, 1, sizeof(Marks_data_page), marksIterater);
			fseekMarks = ftell(wt);
			marksCounter = 0;
		}
		if (stFlag == 1 && Sdata.page_type == 1)
			break;
		if (MrFlag == 1 && Mdata.page_type == 1)
			break;
		if ((MrFlag==1&& Mdata.records[marksCounter].id == -1 )|| (stFlag==1&& Sdata.records[studentCounter].id == -1))
			break;
		if (computeQuery(Strings, fromPos+1, numOfStrings,(stFlag==1)?&Sdata.records[studentCounter]:NULL,(MrFlag==1)?&Mdata.records[marksCounter]:NULL,numberofSubs,subs)==1)
		{
			printData(printArray, &Sdata.records[studentCounter], &Mdata.records[marksCounter]);
		}
		studentCounter++;
		marksCounter++;
	}
}



int GE(float a, float b)
{
	if (a >= b)
		return 1;
	else
		return 0;
}
int LE(float a, float b)
{
	if (a <= b)
		return 1;
	else
		return 0;
}
int GT(float a, float b)
{
	if (a > b)
		return 1;
	else
		return 0;
}
int LT(float a, float b)
{
	if (a < b)
		return 1;
	else
		return 0;
}
int EQ(float a, float b)
{
	if (a == b)
		return 1;
	else
		return 0;
}

int NEQ(float a, float b)
{
	if (a != b)
		return 1;
	else
		return 0;
}

int get_subject_numer(char **subs, char *str, int num_of_subs)
{
	int i = 0;
	for (i = 0; i < num_of_subs; i++)
	{
		if (str_cmp(subs[i], str) == 0)
			return i;
	}
	return -1;
}
 
int computeQuery(char **strings, int startindex, int no_of_strs, Student *srec,Marks *mrec, int noOfSubs, char **subNames)
{
	// numof subs 4
	int queryResult = 1, conditionVar, total = 0, i, orReturn = 0;
	int(*comparefun)(float, float) = NULL;
	float Compare_val;
	//startindex++;
	while (startindex < no_of_strs)
	{
		conditionVar = get_subject_numer(subNames, strings[startindex], noOfSubs);//complete this
		if (conditionVar == -1)
		{
			if (str_cmp(strings[startindex], "name") == 0)
			{
				if (srec == NULL)
				{
					printf("syntax Error cnnot apply the comparision as there is no 'name' in table\n", strings[startindex]);
					return -1;
				}
				if (strings[++startindex][0] != '=')
				{
					printf("syntax Error cnnot apply the comparision '%s' on name\n", strings[startindex]);
					return -1;
				}
				if (str_cmp(srec->name, strings[++startindex]) == 0)
				{
					queryResult *= 1;
				}
				else
					queryResult = 0;
			}
			else if (str_cmp(strings[startindex], "total") == 0)
			{
				if (mrec == NULL)
				{
					printf("syntax Error cnnot apply the comparision as there is no 'total' in table\n", strings[startindex]);
					return -1;
				}
				conditionVar = -4;//for total;
			}
			else if ((str_cmp(strings[startindex], "SID") == 0) && (str_cmp(strings[startindex + 2], "MID") == 0))
			{
				if (srec->id == mrec->id)
					queryResult *= 1;
				else
					queryResult = 0;
				startindex += 2;
			}
			else if (str_cmp(strings[startindex], "SID") == 0)
			{
				conditionVar = -5;
			}
			else if (str_cmp(strings[startindex], "MID") == 0)
			{
				conditionVar = -6;
			}
		}
		if (conditionVar != -1)//for 'total' to access the operations dont use 'else'
		{
			startindex++;
			//getting the complarasion;
			if (strings[startindex][0] == '=')
				comparefun = EQ;
			else if (str_cmp(strings[startindex], "!=")==0)
				comparefun = NEQ;
			else if (str_cmp(strings[startindex], ">=")==0)
				comparefun = GE;
			else if (str_cmp(strings[startindex], "<=")==0)
				comparefun = LE;
			else if (strings[startindex][0] == '>')
				comparefun = GT;
			else if (strings[startindex][0] == '<')
				comparefun = LT;
			else
			{
				printf("syntax Error cnnot apply the comparision '%s' on numbers\n", strings[startindex]);
				return -1;
			}
			startindex++;
			Compare_val = str_to_num(strings[startindex]);
			if (conditionVar >= 0)
			{
				queryResult *= comparefun(mrec->marks[conditionVar], Compare_val);
			}
			else if (conditionVar == -4)
			{
				for (i = 0; i < noOfSubs; i++)
					total += mrec->marks[i];
				queryResult *= comparefun(mrec->marks[conditionVar], total);
			}
			else if (conditionVar == -5)
			{
				queryResult *= comparefun(srec->id,Compare_val);
			}
			else if (conditionVar == -6)
			{
				queryResult *= comparefun(srec->id, Compare_val);
			}
		}
		startindex++;
		if (startindex < no_of_strs)
		{
			if (str_cmp(strings[startindex], "or") == 0)
			{
				orReturn = computeQuery(strings, startindex + 1, no_of_strs, srec,mrec, noOfSubs, subNames);
				if (orReturn == 1)
				{
					queryResult = 1;
					break;
				}
			}
			else if (str_cmp(strings[startindex], "and") == 1)
				startindex++;
		}
	}
	return queryResult;
}
