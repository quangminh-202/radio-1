#define _USE_MATH_DEFINES
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "fftw3.h"

using namespace std;

#define FFT_POINTS 10000 // so luong diem du lieu
#define FFT_POINTS2 ((double)FFT_POINTS * (double)FFT_POINTS)
#define FS 1.0E+6 //tan so lay mau

const double F = 3.0E+4; // Frequency of the input signal
const double FMOD = 200000;
const double DT = 1.0 / FS; // Sampling interval
const double DF = FS / FFT_POINTS; // Frequency step
const double Mag = 1.0; // Magnitude of the input signal
OPENFILENAMEA ofn;
HANDLE hFile;
fftw_complex* In, * Out; // Arrays for FFT
fftw_plan pDir;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	char Txt[512];
	sprintf_s(Txt, "samples.txt");
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "Data Files(*.dat)\0*.dat;\0Any Files(*.*)\0\*.*\0";
	ofn.lpstrFile = Txt;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = sizeof(Txt);
	ofn.lpstrTitle = "Открыть файл";
	ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
	if (!GetSaveFileNameA(&ofn)) return FALSE;
	hFile = CreateFileA(ofn.lpstrFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxW(NULL, L"File is not created", L"FFT testing", MB_OK);
		return FALSE;
	}
	In = (fftw_complex*)VirtualAlloc(NULL, FFT_POINTS * sizeof(fftw_complex), MEM_COMMIT,
		PAGE_READWRITE);
	Out = (fftw_complex*)VirtualAlloc(NULL, FFT_POINTS * sizeof(fftw_complex), MEM_COMMIT,
		PAGE_READWRITE);
	if ((In == NULL) || (Out == NULL))
	{
		MessageBoxW(NULL, L"Not enough memory", L"FFT testing", MB_OK);
		return FALSE;
	}
	pDir = fftw_plan_dft_1d(FFT_POINTS, In, Out, FFTW_FORWARD, FFTW_ESTIMATE);
	if (pDir == NULL)
	{
		MessageBoxW(NULL, L"FFTW plan was not created", L"FFT testing", MB_OK);
		return FALSE;
	}
	double S[FFT_POINTS];
	ZeroMemory(In, FFT_POINTS * sizeof(fftw_complex));
	for (int i = 0; i < FFT_POINTS; i++)
		In[i][0] = S[i] = Mag * (1.0 + 0.75 * cos(2 * M_PI * FMOD * DT * i)) * cos(2 * M_PI * F * DT * i);
	fftw_execute(pDir);
	char buffer[256];
	DWORD ByteNum;
	double P;
	for (int i = 0; i < FFT_POINTS; i++)
	{
		P = (Out[i][0] * Out[i][0] + Out[i][1] * Out[i][1]) / FFT_POINTS2;
		sprintf_s(buffer, "%.8g\t%.8g\t%.8g\r\n", i * DF, P, S[i]);
		WriteFile(hFile, buffer, strlen(buffer), &ByteNum, NULL);
	}
	MessageBoxA(NULL, "Game over", "FFT testing", MB_OK);
	CloseHandle(hFile);
	VirtualFree(In, 0, MEM_RELEASE);
	VirtualFree(Out, 0, MEM_RELEASE);
	fftw_destroy_plan(pDir);
	return 0;
}