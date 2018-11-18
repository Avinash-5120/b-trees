//#include<stdio.h>
//#include<stdlib.h>
//#include"str2.h"
//#include<Windows.h>
//
//#define PageSize 128
//#define STUDENTRECSIZE 36
//#define MARKSRECSIZE 20
//#define StudentTable 1
//#define MarksTable 2
//
//struct Table_page
//{
//	byte table_type = 3;
//	byte table_id = 0;
//	char unused[6];
//	int root_page_id[10];
//	char table_name[10][8];
//};
//
//struct Marks
//{
//	int id;
//	int marks[4];
//};
//
//struct Student
//{
//	int id;
//	char name[32];
//};
//
//
////struct IndexPage
////{
////	int pageid[16];
////	int key[15];
////	char unused[4];
////};
//
////struct DataPage{
////	struct Student records[3];
////	char unused[20];
////};
//
//struct Student_datapage
//{
//	byte page_type;
//	byte table_id;
//	char unused[18];
//	Student records[3];
//};
//
//struct Marks_data_page
//{
//	byte page_type;
//	byte table_id;
//	char unused[6];
//	Marks records[6];
//};
//
//struct Indexpage
//{
//	byte pagetype;
//	byte table_id;
//	char unused[2];
//	int pageid[16];
//	int key[15];
//};
//
//int copyStudentFromString(char **strings, Student *student)
//{
//	student->id = str_to_num(strings[0]);
//	str_cpy(strings[1], student->name);
//	return 0;
//}
//
//int copyMarksFromString(char **strings, Marks *marks)
//{
//	marks->id = str_to_num(strings[0]);
//	for (int i = 0; i < 4; i++)
//		marks->marks[i] = str_to_num(strings[i + 1]);
//	return 0;
//}
//
//void initialiseDataPage(Student_datapage *dataPage, int maxRec)
//{
//	int recCount = 0;
//	while (recCount != maxRec)
//	{
//		dataPage->records[recCount].id = -1;
//		for (int i = 0; i < 32; i++)
//			dataPage->records[recCount].name[i] = '\0';
//		recCount++;
//	}
//	for (int i = 0; i < 18; i++)
//		dataPage->unused[i] = '\0';
//	dataPage->page_type = 0;//data
//	dataPage->table_id = StudentTable;
//}
//
//void initialiseMarksDataPage(Marks_data_page *dataPage, int maxRec)
//{
//	int recCount = 0;
//	while (recCount != maxRec)
//	{
//		dataPage->records[recCount].id = -1;
//		for (int i = 0; i < 4; i++)
//			dataPage->records[recCount].marks[i] = -1;
//		recCount++;
//	}
//	for (int i = 0; i < 16; i++)
//		dataPage->unused[i] = '\0';
//	dataPage->page_type = 0;//data
//	dataPage->table_id = MarksTable;
//}
//
//void initialiseIndex(Indexpage *indexp, int TableId)
//{
//	indexp->pagetype = 1;//index
//	for (int i = 0; i < 2; i++)
//		indexp->unused[i] = '\0';
//	indexp->table_id = TableId;
//}
//
//int copyStudentDataPageFromFile(FILE *fp, FILE *wt)
//{
//	char **strings, buf[250];
//	Student_datapage datapage;
//	//IndexPage indePage;
//	int i = 0, recCount, num = 0, maxRec, PageCount = 0, indexUpdateFlag = 0, PageIdCount = 0;
//	recCount = 0;
//	maxRec = PageSize / STUDENTRECSIZE;
//	initialiseDataPage(&datapage, maxRec);
//	while (fgets(buf, 249, fp) != NULL)
//	{
//		if (recCount == maxRec)
//		{
//			recCount = 0;
//			fwrite(&datapage, 1, sizeof(Student_datapage), wt);
//			initialiseDataPage(&datapage, maxRec); indexUpdateFlag = 1;
//			PageCount++; PageIdCount++;
//		}
//		strings = splitStr(buf, ',', &num);
//		copyStudentFromString(strings, &datapage.records[recCount]);
//		recCount++;
//	}
//	if (recCount != maxRec && recCount != 0)
//	{
//		while (recCount != maxRec)
//		{
//			datapage.records[recCount].id = -1;
//			for (int i = 0; i < 32; i++)
//				datapage.records[recCount].name[i] = '\0';
//			recCount++;
//		}
//	}
//	fwrite(&datapage, 1, sizeof(Student_datapage), wt);
//	fclose(wt);
//	return 0;
//}
//
//int copyMarksDataPageFromFile(FILE *fp, FILE *wt)
//{
//	char **strings, buf[250];
//	Marks_data_page datapage;
//	//IndexPage indePage;
//	int i = 0, recCount, num = 0, maxRec, PageCount = 0, indexUpdateFlag = 0, PageIdCount = 0;
//	recCount = 0;
//	maxRec = PageSize / MARKSRECSIZE;
//	initialiseMarksDataPage(&datapage, maxRec);
//	while (fgets(buf, 249, fp) != NULL)
//	{
//		if (recCount == maxRec)
//		{
//			recCount = 0;
//			fwrite(&datapage, 1, sizeof(Marks_data_page), wt);
//			initialiseMarksDataPage(&datapage, maxRec);
//			indexUpdateFlag = 1;
//			PageCount++; PageIdCount++;
//		}
//		strings = splitStr(buf, ',', &num);
//		copyMarksFromString(strings, &datapage.records[recCount]);
//		recCount++;
//	}
//	if (recCount != maxRec&&recCount != 0)
//	{
//		while (recCount != maxRec)
//		{
//			initialiseMarksDataPage(&datapage, maxRec);
//			recCount++;
//		}
//	}
//	fwrite(&datapage, 1, sizeof(Marks_data_page), wt);
//	fclose(wt);
//	return 0;
//}
//
//
//long createIndexes(FILE *wt,long fileCurrent,long fileEnd)
//{
//	long rootIndexFseek,;
//	fseek(wt, fileCurrent, SEEK_SET);
//	while (1)
//	{
//		startFlag = 0;
//		indexPagesCounted = 0;
//		PageIdCount = 0;
//		PageCount = 0;
//		while (ftell(wt) != fileEnd)
//		{
//			fread(&BufindexPage, 1, sizeof(Indexpage), wt);
//			//PageCount++;
//			if (startFlag == 0)
//			{
//				startFlag++;
//				PageIdCount++;
//				PageCount++;
//				continue;
//			}
//			if (PageCount == 1)
//			{
//				indePage.pageid[0] = PageIdCount - 1;
//			}
//			indePage.key[PageCount - 1] = BufindexPage.key[0];
//			indePage.pageid[PageCount] = PageIdCount;
//			PageCount++;
//			PageIdCount++;
//			if (PageCount == 16)
//			{
//				indexPagesCounted++;
//				PageCount = 0;
//				startFlag = 0;
//				fileCurrent = ftell(wt);
//				fseek(wt, 0, SEEK_END);
//				rootIndexFseek = ftell(wt);
//				fwrite(&indePage, sizeof(Indexpage), 1, wt);
//				fseek(wt, fileCurrent, SEEK_SET);
//			}
//		}
//		if (PageCount != 16)
//		{
//			indexPagesCounted++;
//			while (PageCount != 16)
//			{
//				indePage.pageid[PageCount] = -1;
//				indePage.key[PageCount - 1] = -1;
//				PageCount++;
//			}
//			fileCurrent = ftell(wt);
//			fseek(wt, 0, SEEK_END);
//			rootIndexFseek = ftell(wt);
//			fwrite(&indePage, sizeof(Indexpage), 1, wt);
//			fseek(wt, fileCurrent, SEEK_SET);
//		}
//		fseek(wt, 0, SEEK_END);
//		fileEnd = ftell(wt);
//		fseek(wt, fileCurrent, SEEK_SET);
//		if (indexPagesCounted == 1)
//			break;
//	}
//	fclose(wt);
//	return rootIndexFseek;
//}
//
//long copyMarksIndexesToFile(FILE *wt)
//{
//	char **strings, buf[250];
//	Marks_data_page datapage;
//	Indexpage indePage, BufindexPage;
//	int i = 0, recCount, num = 0, maxRec, PageCount = 0, indexUpdateFlag = 0, PageIdCount = 0;
//	long fileCurrent, fileEnd = 0, rootIndexFseek;
//	int startFlag = 0;
//	int indexPagesCounted = 0;
//	recCount = 0;
//	maxRec = PageSize / MARKSRECSIZE;
//	initialiseMarksDataPage(&datapage, maxRec);
//	PageCount = 0;
//	PageIdCount = 0;
//	fileCurrent = ftell(wt);
//	fseek(wt, 0, SEEK_END);
//	fileEnd = ftell(wt);
//	fseek(wt, fileCurrent, SEEK_SET);
//	initialiseIndex(&indePage, MarksTable);
//	while (ftell(wt) != fileEnd)
//	{
//		//PageCount++;
//		fread(&datapage, 1, sizeof(Marks_data_page), wt);
//		if (startFlag == 0)
//		{
//			startFlag++;
//			PageIdCount++;
//			PageCount++;
//			continue;
//		}
//		if (PageCount == 1)
//		{
//			indePage.pageid[0] = PageIdCount - 1;
//		}
//		indePage.key[PageCount - 1] = datapage.records[0].id;
//		indePage.pageid[PageCount] = PageIdCount;
//		PageCount++;
//		PageIdCount++;
//		if (PageCount == 16)
//		{
//			indexPagesCounted++;
//			PageCount = 0;
//			fileCurrent = ftell(wt);
//			fseek(wt, 0, SEEK_END);
//			rootIndexFseek = ftell(wt);
//			fwrite(&indePage, sizeof(Indexpage), 1, wt);
//			fseek(wt, fileCurrent, SEEK_SET);
//		}
//	}
//	if (PageCount != 16 && PageCount != 0)
//	{
//		indexPagesCounted++;
//		while (PageCount != 16)
//		{
//			indePage.pageid[PageCount] = -1;
//			indePage.key[PageCount - 1] = -1;
//			PageCount++;
//		}
//		fileCurrent = ftell(wt);
//		fseek(wt, 0, SEEK_END);
//		rootIndexFseek = ftell(wt);
//		fwrite(&indePage, sizeof(Indexpage), 1, wt);
//		fseek(wt, fileCurrent, SEEK_SET);
//	}
//	if (indexPagesCounted == 1)
//	{
//		fclose(wt);
//		return rootIndexFseek;
//	}
//	fseek(wt, 0, SEEK_END);
//	fileEnd = ftell(wt);
//	fseek(wt, fileCurrent, SEEK_SET);
//	while (1)
//	{
//		startFlag = 0;
//		indexPagesCounted = 0;
//		PageIdCount = 0;
//		PageCount = 0;
//		while (ftell(wt) != fileEnd)
//		{
//			fread(&BufindexPage, 1, sizeof(Indexpage), wt);
//			//PageCount++;
//			if (startFlag == 0)
//			{
//				startFlag++;
//				PageIdCount++;
//				PageCount++;
//				continue;
//			}
//			if (PageCount == 1)
//			{
//				indePage.pageid[0] = PageIdCount - 1;
//			}
//			indePage.key[PageCount - 1] = BufindexPage.key[0];
//			indePage.pageid[PageCount] = PageIdCount;
//			PageCount++;
//			PageIdCount++;
//			if (PageCount == 16)
//			{
//				indexPagesCounted++;
//				PageCount = 0;
//				startFlag = 0;
//				fileCurrent = ftell(wt);
//				fseek(wt, 0, SEEK_END);
//				rootIndexFseek = ftell(wt);
//				fwrite(&indePage, sizeof(Indexpage), 1, wt);
//				fseek(wt, fileCurrent, SEEK_SET);
//			}
//		}
//		if (PageCount != 16)
//		{
//			indexPagesCounted++;
//			while (PageCount != 16)
//			{
//				indePage.pageid[PageCount] = -1;
//				indePage.key[PageCount - 1] = -1;
//				PageCount++;
//			}
//			fileCurrent = ftell(wt);
//			fseek(wt, 0, SEEK_END);
//			rootIndexFseek = ftell(wt);
//			fwrite(&indePage, sizeof(Indexpage), 1, wt);
//			fseek(wt, fileCurrent, SEEK_SET);
//		}
//		fseek(wt, 0, SEEK_END);
//		fileEnd = ftell(wt);
//		fseek(wt, fileCurrent, SEEK_SET);
//		if (indexPagesCounted == 1)
//			break;
//	}
//	fclose(wt);
//	return rootIndexFseek;
//}
//
//long copyStudentIndexesToFile(FILE *wt)
//{
//	char **strings, buf[250];
//	Student_datapage datapage;
//	Indexpage indePage, BufindexPage;
//	int i = 0, recCount, num = 0, maxRec, PageCount = 0, indexUpdateFlag = 0, PageIdCount = 0;
//	long fileCurrent, fileEnd = 0, rootIndexFseek;
//	int startFlag = 0;
//	int indexPagesCounted = 0;
//	recCount = 0;
//	maxRec = PageSize / STUDENTRECSIZE;
//	initialiseDataPage(&datapage, maxRec);
//	PageCount = 0;
//	PageIdCount = 0;
//	fseek(wt, 0, SEEK_END);
//	fileEnd = ftell(wt);
//	fseek(wt, 0, SEEK_SET);
//	initialiseIndex(&indePage, StudentTable);
//	while (ftell(wt) != fileEnd)
//	{
//		//PageCount++;
//		fread(&datapage, 1, sizeof(Student_datapage), wt);
//		if (startFlag == 0)
//		{
//			startFlag++;
//			PageIdCount++;
//			PageCount++;
//			continue;
//		}
//		if (PageCount == 1)
//		{
//			indePage.pageid[0] = PageIdCount - 1;
//		}
//		indePage.key[PageCount - 1] = datapage.records[0].id;
//		indePage.pageid[PageCount] = PageIdCount;
//		PageCount++;
//		PageIdCount++;
//		if (PageCount == 16)
//		{
//			indexPagesCounted++;
//			PageCount = 0;
//			fileCurrent = ftell(wt);
//			fseek(wt, 0, SEEK_END);
//			rootIndexFseek = ftell(wt);
//			fwrite(&indePage, sizeof(Indexpage), 1, wt);
//			fseek(wt, fileCurrent, SEEK_SET);
//		}
//	}
//	if (PageCount != 16 && PageCount != 0)
//	{
//		indexPagesCounted++;
//		while (PageCount != 16)
//		{
//			indePage.pageid[PageCount] = -1;
//			indePage.key[PageCount - 1] = -1;
//			PageCount++;
//		}
//		fileCurrent = ftell(wt);
//		fseek(wt, 0, SEEK_END);
//		rootIndexFseek = ftell(wt);
//		fwrite(&indePage, sizeof(Indexpage), 1, wt);
//		fseek(wt, fileCurrent, SEEK_SET);
//	}
//	if (indexPagesCounted == 1)
//	{
//		fclose(wt);
//		return rootIndexFseek;
//	}
//	fseek(wt, 0, SEEK_END);
//	fileEnd = ftell(wt);
//	//createIndexes(FILE *wt,long fileCurrent,long fileEnd)
//	fseek(wt, fileCurrent, SEEK_SET);
//	while (1)
//	{
//		startFlag = 0;
//		indexPagesCounted = 0;
//		PageIdCount = 0;
//		PageCount = 0;
//		while (ftell(wt) != fileEnd)
//		{
//			fread(&BufindexPage, 1, sizeof(Indexpage), wt);
//			//PageCount++;
//			if (startFlag == 0)
//			{
//				startFlag++;
//				PageIdCount++;
//				PageCount++;
//				continue;
//			}
//			if (PageCount == 1)
//			{
//				indePage.pageid[0] = PageIdCount - 1;
//			}
//			indePage.key[PageCount - 1] = BufindexPage.key[0];
//			indePage.pageid[PageCount] = PageIdCount;
//			PageCount++;
//			PageIdCount++;
//			if (PageCount == 16)
//			{
//				indexPagesCounted++;
//				PageCount = 0;
//				startFlag = 0;
//				fileCurrent = ftell(wt);
//				fseek(wt, 0, SEEK_END);
//				rootIndexFseek = ftell(wt);
//				fwrite(&indePage, sizeof(Indexpage), 1, wt);
//				fseek(wt, fileCurrent, SEEK_SET);
//			}
//		}
//		if (PageCount != 16)
//		{
//			indexPagesCounted++;
//			while (PageCount != 16)
//			{
//				indePage.pageid[PageCount] = -1;
//				indePage.key[PageCount - 1] = -1;
//				PageCount++;
//			}
//			fileCurrent = ftell(wt);
//			fseek(wt, 0, SEEK_END);
//			rootIndexFseek = ftell(wt);
//			fwrite(&indePage, sizeof(Indexpage), 1, wt);
//			fseek(wt, fileCurrent, SEEK_SET);
//		}
//		fseek(wt, 0, SEEK_END);
//		fileEnd = ftell(wt);
//		fseek(wt, fileCurrent, SEEK_SET);
//		if (indexPagesCounted == 1)
//			break;
//	}
//	fclose(wt);
//	return rootIndexFseek;
//}
////if (recCount == maxRec)
////{
////	recCount = 0;
////	//fwrite(&datapage, 1, sizeof(DataPage), wt);
////	//initialiseDataPage(&datapage, maxRec); 
////	indexUpdateFlag = 1;
////	PageCount++;
////	PageIdCount++;
////}
////strings = splitStr(buf, ',', &num);
////copyStudentFromString(strings, &datapage.records[recCount]);
///*if (indexUpdateFlag == 1)
//{
//indexUpdateFlag = 0;
//indePage.pageid[PageIdCount-1] = PageCount;
//if (PageIdCount == 16)
//{
//PageIdCount = 0;
//fwrite(&indePage, 1, sizeof(IndexPage), wt);
//}
//else
//{
//indePage.key[PageIdCount - 1] = datapage.records[0].id;
//}
//}
//recCount++;*/
///*if (recCount != maxRec)
//{
//while (recCount != maxRec)
//{
//datapage.records[recCount].id = -1;
//for (i = 0; i < 32; i++)
//datapage.records[recCount].name[i] = '\0';
//recCount++;
//}
//}*/
////fwrite(&datapage, 1, sizeof(DataPage), wt);
//
//
//int othermain()
//{
//	FILE *fp = fopen("sample.bin", "wb");
//	FILE *rd = fopen("randomuserdata.csv", "r");
//	char **strings, buf[201];
//	long rootOfStudents, rootOfMArks, startPos;
//	/*DataPage b;
//	IndexPage inP;*/
//	//copyDataPageFromFile(rd, fp); 
//	Table_page tb;
//	fgets(buf, 200, rd);
//	copyStudentDataPageFromFile(rd, fp);
//	fp = fopen("sample.bin", "a+b");
//	fclose(rd);
//	rootOfStudents = copyStudentIndexesToFile(fp);
//	rd = fopen("SudentMarksDataSet_Version_2.csv", "r");//edit file
//	fgets(buf, 200, rd);
//	fp = fopen("sample.bin", "a+b");
//	fseek(fp, 0, SEEK_END);
//	startPos = ftell(fp);
//	copyMarksDataPageFromFile(rd, fp);
//	fp = fopen("sample.bin", "a+b");
//	fseek(fp, startPos, SEEK_SET);
//	rootOfMArks = copyMarksIndexesToFile(fp);
//	tb.table_id = 3;
//	str_cpy("stu", tb.table_name[0]);
//	tb.root_page_id[0] = rootOfStudents;
//	str_cpy("marks", tb.table_name[1]);
//	tb.root_page_id[1] = rootOfMArks;
//	tb.table_type = 2;
//	fp = fopen("sample.bin", "a+b");
//	fseek(fp, 0, SEEK_END);
//	fwrite(&tb, 1, sizeof(Table_page), fp);
//	fclose(fp);
//	fclose(fp); 
//	system("pause");
//}
//
//
//	//copyIndexesToFile(fp);
//	//struct ch
//	//{
//	//	//short b;
//	//	int a;
//	//	char na[32];
//	//	char diatrict[20];
//	//};
//	//ch b[6];
//	//while ((fgets(buf, 49, rd) != NULL) && i != 6)
//	//{
//	//	int num = 0;
//	//	strings = splitStr(buf, ',', &num);
//	//	b[i].a = str_to_num(strings[0]);
//	//	str_cpy(strings[1], b[i].na);
//	//	str_cpy(strings[2], b[i].diatrict);
//	//	printf("%d %s %s\n", b[i].a, b[i].diatrict, b[i].na);
//	//	i++;
//	//}
//	//fwrite(b, sizeof(struct ch), 6, fp);
//	//fclose(fp);
//	//fp = fopen("sample.bin", "rb");
//	/*fp = fopen("sample.bin", "rb");
//	int pageCount = 0;
//	while (1)
//	{
//	if (pageCount != 16)
//	{
//	if (fread(&b, 1, sizeof(DataPage), fp) != 0)
//	{
//	for (int i = 0; i < 3; i++)
//	{
//	if (b.records[i].id == -1)
//	{
//	pageCount = 15;
//	}
//	printf("%d %s\n", b.records[i].id, b.records[i].name);
//	}
//	}
//	else
//	break;
//	pageCount++;
//	}
//	else
//	{
//	pageCount = 0;
//	if (fread(&inP, 1, sizeof(IndexPage), fp) != 0)
//	{
//	printf("keys: ");
//	for (int i = 0; i < 15; i++)
//	printf("%d ", inP.key[i]);
//	printf("PageIds: ");
//	for (int i = 0; i < 16; i++)
//	printf("%d ", inP.pageid[i]);
//	printf("\n");
//	}
//	else
//	break;
//	}
//	}
//	*/
