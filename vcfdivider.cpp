/* File: vcfdivider.cpp
 * Author: CRE
 * Last Edited: Thu Nov 10 12:44:10 2016
 */

#include "vcfdivider.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <time.h>
#include <set>
using namespace std;
using namespace cre;

static inline void printMeta(FILE * InFile, char * Buffer, FILE* SFile, FILE* DFile)
{
	bool FirstLine=true;
	while(fgets(Buffer, LINE_BUFFER_SIZE, InFile))
	{
		uint Length=strlen(Buffer);
		if (Length==0) die("Read heads error");
		if (Length>=2&&Buffer[0]=='#'&&Buffer[1]=='#')
		{
			if (Length==LINE_BUFFER_SIZE-1) die("Buffer may not be enough!");
			if (Length>0&&Buffer[Length-1]=='\n')
			{
				Buffer[--Length]='\0';
			}
			if (!FirstLine)
			{
				fprintf(SFile,"\n");
				fprintf(DFile,"\n");
			}
			FirstLine=false;
			fprintf(SFile,"%s", Buffer);
			fprintf(DFile,"%s", Buffer);
		}
		else break;
	}
}

static inline uint printHeader(char* Buffer, set<string> &SelectedSamples, set<unsigned> &SelectedColumns, FILE* SFile, FILE* DFile)//return number of samples
{
	if (Buffer[0]!='#') die("Can't read left's header!");
	stringstream SS;
	SS.str(Buffer);
	char * SBuffer=myalloc(SMALL_BUFFER_SIZE, char);
	uint SampleSize=0;
	SS>>SBuffer>>SBuffer>>SBuffer>>SBuffer>>SBuffer>>SBuffer>>SBuffer>>SBuffer>>SBuffer;//They are #CHROM POS ID REF ALT QUAL FILTER INFO FORMAT
	fprintf(SFile, "\n#CHROM\tPOS\tID\tREF\tALT\tQUAL\tFILTER\tINFO\tFORMAT");
	fprintf(DFile, "\n#CHROM\tPOS\tID\tREF\tALT\tQUAL\tFILTER\tINFO\tFORMAT");
	while (SS>>Buffer)
	{
		if (SelectedSamples.count(Buffer)!=0)
		{
			SelectedColumns.insert(SampleSize);
			fprintf(SFile, "\t%s", Buffer);
		}
		else
		{
			fprintf(DFile, "\t%s", Buffer);
		}
		++SampleSize;
	}
	free (SBuffer);
	return SampleSize;
}

static inline void printBody(FILE * InFile, char * Buffer, uint SampleSize, set<unsigned> &SelectedColumns, FILE* SFile, FILE* DFile)
{
	uint DoneLine=0;
	while (!feof(InFile))
	{
		fscanf(InFile, "%s", Buffer);
		if (feof(InFile)) break;
		fprintf(SFile,"\n");
		fprintf(DFile,"\n");
		for (uint i=0;i<9;++i)
		{
			if (i!=0)
			{
				fprintf(SFile, "\t");
				fprintf(DFile, "\t");
				fscanf(InFile, "%s", Buffer);
			}
			fprintf(SFile, "%s", Buffer);
			fprintf(DFile, "%s", Buffer);
		}
		for (uint i=0;i<SampleSize;++i)
		{
			fscanf(InFile, "%s", Buffer);
			if (SelectedColumns.count(i))
			{
				fprintf(SFile, "\t%s", Buffer);
			}
			else
			{
				fprintf(DFile, "\t%s", Buffer);
			}
		}
		++DoneLine;
		if (DoneLine%10000==0) warn("%u lines have been proceeded.", DoneLine);
	}
}

static inline void getSSamples(FILE* SSampleFile, char * Buffer, set<string> &SelectedSamples)
{
	while (!feof(SSampleFile))
	{
		fscanf(SSampleFile, "%s", Buffer);
		if (feof(SSampleFile)) break;
		SelectedSamples.insert(Buffer);
	}
}

static inline void getRootName(const char * FileName, char * RootName)
{
	strcpy(RootName,FileName);
	if (strlen(FileName)>4)
	{
		if (strcmp(FileName+strlen(FileName)-4, ".vcf")==0)
		{
			RootName[strlen(FileName)-4]='\0';
		}
	}
}

void vcfdivide(const char* InFileName, const char * SSampleFileName)
{
	char * Buffer=myalloc(LINE_BUFFER_SIZE, char);
	char * RootName=myalloc(NAME_BUFFER_SIZE, char), *SName=myalloc(NAME_BUFFER_SIZE, char), *DName=myalloc(NAME_BUFFER_SIZE, char);
	getRootName(InFileName, RootName);
	strcpy(SName, RootName);
	strcpy(DName, RootName);
	strcat(SName, ".Selected.vcf");
	strcat(DName, ".Deleted.vcf");
	FILE *InFile, *SFile, *DFile, *SSampleFile;
	InFile=fopen(InFileName, "r");
	SFile=fopen(SName, "w");
	DFile=fopen(DName, "w");
	SSampleFile=fopen(SSampleFileName, "r");

	if (InFile==NULL) die ("Can't open %s for read!", InFileName);
	if (SFile==NULL) die ("Can't open %s for write!", SName);
	if (DFile==NULL) die ("Can't open %s for write!", DName);
	if (SSampleFile==NULL) die ("Can't open %s for read!", SSampleFileName);

	set<string> SelectedSamples;
	set<uint> SelectedColumns;
	uint SampleSize;
	getSSamples(SSampleFile, Buffer, SelectedSamples);
	fclose(SSampleFile);

	updateTime("Initializing", "Printing meta data and head...");
	printMeta(InFile, Buffer, SFile, DFile);
	SampleSize=printHeader(Buffer, SelectedSamples, SelectedColumns, SFile, DFile);
	updateTime("Printing meta data and head", "Printing body...");
	printBody(InFile, Buffer, SampleSize, SelectedColumns, SFile, DFile);
	updateTime("Done! All the work");

	fclose(InFile);
	fclose(SFile);
	fclose(DFile);
	free(Buffer);
	free(RootName);
	free(SName);
	free(DName);
}
