#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <wchar.h>
#include "Shlwapi.h"
#include <locale.h>
#include<libxml/xmlmemory.h>
//#include "zlib.h"
#include <contrib/minizip\unzip.h>

#define USER_LEN 512
#define FILE_LEN 1024
#define ARR_LEN 1024
#define DOCUMENTS 0
#define DOWNLOADS 1
#define DESKTOP 2
#define BUF_MAX 1024
#define CHUNK 16384
#define ZIP_DEFAULT_COMPRESSION_LEVEL 1

#pragma comment(lib, "shlwapi.lib")
//#pragma comment(lib, "libzip.lib")

typedef struct {

	wchar_t ORIGIN[FILE_LEN];

	wchar_t TXT[FILE_LEN];
	wchar_t DOCX[FILE_LEN];

	wchar_t *TXT_ARR[ARR_LEN];
	wchar_t *DOCX_ARR[ARR_LEN];

	wint_t count_txt;
	wint_t count_docx;

}Locate;

void ChangeExt(wchar_t* path, wchar_t* newext)
{
	wchar_t drive[100];
	wchar_t dir[100];
	wchar_t fname[100];
	wchar_t ext[100];

	_wsplitpath(path, drive, dir, fname, ext);

	swprintf(path, L"%ws%ws%ws%ws", drive, dir, fname, newext);
	wcscat(path, dir);
	wcscat(path, fname);
	wcscat(path, newext);

	wprintf(L"%ws\n", path);
}


int main(void) {

	DWORD dwSize = USER_LEN;
	wchar_t strUserName[USER_LEN] = { 0, };
	wchar_t PATH[FILE_LEN] = L"C:\\Users\\";
	wchar_t ONE[FILE_LEN] = L"C:\\Users\\";

	//document = 0, downloads = 1, desktop = 2
	Locate locate[3];
	wchar_t* LPATH;

	int nError = GetUserNameW(strUserName, &dwSize);

	if (!nError) {
		printf("GetUserName() Error Code : %d\n", GetLastError());
		return 0;
	}
	printf("UserName : %ws\n", strUserName);

	setlocale(LC_ALL, "korean");
	_wsetlocale(LC_ALL, L"korean");

	wcscat(PATH, strUserName);

	//DOCUMENT
	wcscpy(locate[DOCUMENTS].DOCX, PATH);
	wcscat(locate[DOCUMENTS].DOCX, L"\\Documents\\");
	wcscpy(locate[DOCUMENTS].ORIGIN, locate[DOCUMENTS].DOCX);
	wcscat(locate[DOCUMENTS].DOCX, L"*.docx");
	wcscpy(locate[DOCUMENTS].TXT, locate[DOCUMENTS].ORIGIN);
	wcscat(locate[DOCUMENTS].TXT, L"*.txt");
	wprintf(L"Document ==> %s\n", locate[DOCUMENTS].TXT);

	//DOWNLOAD
	wcscpy(locate[DOWNLOADS].DOCX, PATH);
	wcscat(locate[DOWNLOADS].DOCX, L"\\Downloads\\");
	wcscpy(locate[DOWNLOADS].ORIGIN, locate[DOWNLOADS].DOCX);
	wcscat(locate[DOWNLOADS].DOCX, L"*.docx");
	wcscpy(locate[DOWNLOADS].TXT, PATH);
	wcscat(locate[DOWNLOADS].TXT, L"\\*.txt");
	wprintf(L"Download ==> %s\n", locate[DOWNLOADS].TXT);
	//DESKTOP
	wcscat(ONE, strUserName);
	wcscat(ONE, L"\\OneDrive");

	LPATH = ONE;

	int nOne = PathFileExistsW(LPATH); // �����ϸ� 1 ������ 0�� ��ȯ
	if (!nOne) { //OneDrive ��ΰ� ���� ���� ��Ȳ
		wcscat(locate[DESKTOP].DOCX, strUserName);
		wcscpy(locate[DESKTOP].ORIGIN, locate[DESKTOP].DOCX);
		wcscat(locate[DESKTOP].DOCX, L"\\Desktop");
		wcscat(locate[DESKTOP].TXT, strUserName);
		wcscat(locate[DESKTOP].TXT, L"\\Desktop");
		wprintf(L"Desktop ==> %s\n", locate[DESKTOP].TXT);
	}
	else {
		wcscpy(locate[DESKTOP].DOCX, ONE);
		wcscpy(locate[DESKTOP].ORIGIN, locate[DESKTOP].DOCX);
		wcscat(locate[DESKTOP].ORIGIN, L"\\���� ȭ��\\");
		wcscat(locate[DESKTOP].DOCX, L"\\���� ȭ��\\*.docx");
		wcscpy(locate[DESKTOP].TXT, ONE);
		wcscat(locate[DESKTOP].TXT, L"\\���� ȭ��\\*.txt");
		wprintf(L"Desktop ==> %s\n", locate[DESKTOP].TXT);
	}
	

	HANDLE hFind_txt = NULL;
	
	for (int idx=0; idx < 3; idx++) {

		locate[idx].count_txt= 0;

		WIN32_FIND_DATA FindData;

		printf("#### %d #### \n", idx); //document = 0, downloads = 1, desktop = 2

		hFind_txt = FindFirstFileW(locate[idx].TXT, &FindData);
		//HANDLE hFind_txt = FindFirstFileW(L"C:\\Users\\dayeo\\Desktop\\source\\", &FindData);
		printf("%ws\n", locate[idx].TXT);

		//txt ������ ��ĵ
		if (hFind_txt == INVALID_HANDLE_VALUE) { // exist no File in Direct
			FindClose(hFind_txt);
			printf("TXT�� �������� �ʴ� ����Դϴ�.\n");
		}
		else {
			do {
				if (FindData.dwFileAttributes && FILE_ATTRIBUTE_DIRECTORY) {
					locate[idx].TXT_ARR[locate[idx].count_txt] = (wchar_t*)malloc(sizeof(wchar_t) * FILE_LEN);
					ZeroMemory(locate[idx].TXT_ARR[locate[idx].count_txt], FILE_LEN);
					wcscpy(locate[idx].TXT_ARR[locate[idx].count_txt], FindData.cFileName);
					_tprintf(_T("[+] %ws\n"), locate[idx].TXT_ARR[locate[idx].count_txt++]);
				}
			} while (FindNextFile(hFind_txt, &FindData));
			FindClose(hFind_txt);
		}
		
		HANDLE hFind_docx = FindFirstFileW(locate[idx].DOCX, &FindData);
		printf("%ws\n", locate[idx].DOCX);

		locate[idx].count_docx = 0;

		//docs ������ ��ĵ
		if (hFind_docx == INVALID_HANDLE_VALUE) { // exist no File in Direct
			FindClose(hFind_docx);
			printf("DOCX�� �������� �ʴ� ����Դϴ�.\n");
		}
		else {
			do {
				if (FindData.dwFileAttributes && FILE_ATTRIBUTE_DIRECTORY) {
					locate[idx].DOCX_ARR[locate[idx].count_docx] = (wchar_t*)malloc(sizeof(wchar_t) * FILE_LEN);
					ZeroMemory(locate[idx].DOCX_ARR[locate[idx].count_docx], FILE_LEN);
					wcscpy(locate[idx].DOCX_ARR[locate[idx].count_docx], FindData.cFileName);
					_tprintf(_T("[+] %ws\n"), locate[idx].DOCX_ARR[locate[idx].count_docx++]);
				}
			} while (FindNextFile(hFind_docx, &FindData));
			FindClose(hFind_docx);
		}
		
	}

	// Docx ���� ����
	for (int i = 0; i < locate[DOCUMENTS].count_docx; i++) {
		wchar_t temp[MAX_PATH];
		ZeroMemory(temp, sizeof(temp));
		wcscpy(temp, locate[DOCUMENTS].ORIGIN);
		wcscat(temp, locate[DOCUMENTS].DOCX_ARR[i]);
		wprintf(L"%ws\n", temp);

		wchar_t temp2[MAX_PATH];
		ZeroMemory(temp2, sizeof(temp));
		wcscpy(temp2, locate[DOCUMENTS].ORIGIN);
		wcscat(temp2, locate[DOCUMENTS].DOCX_ARR[i]);

		ChangeExt(temp, L".zip");
		wprintf(L"��� ���� = %ws\n", temp);
		int result = _wrename(temp2, temp);
		printf("%d\n", result);
	}

	//���� ����
	//C:\Users\dayeo\Documents\example.zip
	wchar_t* filename = L"C:\\Users\\dayeo\\Documents\\example.zip";

	FILE* file = _wfopen(filename, L"rb");

	if (file == NULL)
	{
		wprintf(L"Failed to open file: %ls\n", filename);
		return 1;
	}

	fseek(file, 0, SEEK_END);
	long filesize = ftell(file);
	fseek(file, 0, SEEK_SET);

	//���� ��ŭ�� �޸� �Ҵ�
	wchar_t* filedata = (wchar_t*)malloc(filesize);

	if (filedata == NULL)
	{
		wprintf(L"Failed to allocate memory\n");
		fclose(file);
		return 1;
	}

	//���� �о����
	if (fread(filedata, sizeof(wchar_t), filesize / sizeof(wchar_t), file) != filesize / sizeof(wchar_t))
	{
		wprintf(L"Failed to read file\n");
		fclose(file);
		free(filedata);
		return 1;
	}

	fclose(file);

	z_stream zstream;
	zstream.zalloc = Z_NULL;
	zstream.zfree = Z_NULL;
	zstream.opaque = Z_NULL;
	zstream.avail_in = (uInt)filesize;
	zstream.next_in = (Bytef*)filedata;

	if (inflateInit2(&zstream, -MAX_WBITS) != Z_OK)
	{
		wprintf(L"Failed to initialize zlib\n");
		free(filedata);
		return 1;
	}

	char outbuf[CHUNK];

	do
	{
		zstream.avail_out = CHUNK;
		zstream.next_out = (Bytef*)outbuf;

		if (inflate(&zstream, Z_NO_FLUSH) != Z_OK && zstream.avail_out != 0)
		{
			wprintf(L"Failed to decompress data\n");
			inflateEnd(&zstream);
			free(filedata);
			return 1;
		}

		int outlen = CHUNK - zstream.avail_out;

		if (outlen > 0)
		{
			fwrite(outbuf, 1, outlen, stdout);
		}

	} while (zstream.avail_out == 0);

	inflateEnd(&zstream);

	free(filedata);



	
	
	return 0;
}