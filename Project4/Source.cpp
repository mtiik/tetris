#define WriteConsoleOutputCharacter
//#include "stdafx.h"
#include <iostream>
#include <thread>
#include <vector>
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>

using namespace std;

// ====================================================================

int Width = 80;						// ������ ������� � ������
int Height = 30;					// ������ ������� � ������
wstring tetromino[7];				// �������
int FieldWidth = 12;				// ������ ���� 
int FieldHeight = 18;				// ������ ����
unsigned char* Field = nullptr;
bool Key[4];						// ���� ������
int CurrentPiece = 0;				// ������� �������
int CurrentRotation = 0;			// ������� �������
int CurrentX = FieldWidth / 2;		// ������� �� x
int CurrentY = 0;					// ������� �� y
int Speed = 20;						// ��������
int SpeedCount = 0;					// �������
bool ForceDown = false;				// ��� ������� ������� ����
bool RotateHold = true;				// ����� ��� �������� �����
int PieceCount = 0;					// �������
int Score = 0;						// ������� �������
vector<int> vLines;
bool Fail = false;

// ====================================================================

int Rotate(int px, int py, int r)
{
	int pi = 0;
	switch (r % 4)
	{
	case 0:
		pi = py * 4 + px;
		break;


	case 1:
		pi = 12 + py - (px * 4);
		break;


	case 2:
		pi = 15 - (py * 4) - px;
		break;


	case 3:
		pi = 3 - py + (px * 4);
		break;
	}

	return pi;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	// ���� ��� ������ ���� ������
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// �������� ������ �������
			int pi = Rotate(px, py, nRotation);

			// �������� ������ ������� � ����
			int fi = (nPosY + py) * FieldWidth + (nPosX + px);

			// �������� �� ����� �� ������� ����
			if (nPosX + px >= 0 && nPosX + px < FieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < FieldHeight)
				{
					// �������� �� ������� ����� � �������� ����
					if (tetromino[nTetromino][pi] != L'.' && Field[fi] != 0)
						return false;
				}
			}
		}

	return true;
}

int main()
{
	// ������� ����� �������
	wchar_t* screen = new wchar_t[Width * Height];
	for (int i = 0; i < Width * Height; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	tetromino[0].append(L"..X."); // �������
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

	Field = new unsigned char[FieldWidth * FieldHeight]; // ������� ����� ����
	for (int x = 0; x < FieldWidth; x++) // ������� ������� ����
		for (int y = 0; y < FieldHeight; y++)
			Field[y * FieldWidth + x] = (x == 0 || x == FieldWidth - 1 || y == FieldHeight - 1) ? 9 : 0;

	// ����

	while (!Fail)
	{
		// ����� ������� �����
		this_thread::sleep_for(50ms);
		SpeedCount++;
		ForceDown = (SpeedCount == Speed);

		// ����
		for (int k = 0; k < 4; k++)
			Key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0; // \x27 = A; \x25 = S; \x28 = D; Z

		// ������� ����

		// ������
		CurrentX += (Key[0] && DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX + 1, CurrentY)) ? 1 : 0;
		CurrentX -= (Key[1] && DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX - 1, CurrentY)) ? 1 : 0;
		CurrentY += (Key[2] && DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY + 1)) ? 1 : 0;

		// �������
		if (Key[3])
		{
			CurrentRotation += (RotateHold && DoesPieceFit(CurrentPiece, CurrentRotation + 1, CurrentX, CurrentY)) ? 1 : 0;
			RotateHold = false;
		}
		else
			RotateHold = true;

		// ������ ����� ����
		if (ForceDown)
		{
			// ���������
			SpeedCount = 0;
			PieceCount++;
			if (PieceCount % 50 == 0)
				if (Speed >= 10) Speed--;

			// �������� �� ���������� ������ ����
			if (DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY + 1))
				CurrentY++;
			else
			{
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[CurrentPiece][Rotate(px, py, CurrentRotation)] != L'.')
							Field[(CurrentY + py) * FieldWidth + (CurrentX + px)] = CurrentPiece + 1;

				// �������� ������
				for (int py = 0; py < 4; py++)
					if (CurrentY + py < FieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < FieldWidth - 1; px++)
							bLine &= (Field[(CurrentY + py) * FieldWidth + px]) != 0;

						if (bLine)
						{
							// ���� ����� ���������, �� �������� �� �� $
							for (int px = 1; px < FieldWidth - 1; px++)
								Field[(CurrentY + py) * FieldWidth + px] = 8;
							vLines.push_back(CurrentY + py);
						}
					}

				Score += 25;
				if (!vLines.empty())	Score += (1 << vLines.size()) * 100;

				// ����� ������ ����
				CurrentX = FieldWidth / 2;
				CurrentY = 0;
				CurrentRotation = 0;
				CurrentPiece = rand() % 7;

				// �������� �� ��������
				Fail = !DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY);
			}
		}

		// ���� �������


		// ��������� �������
		for (int x = 0; x < FieldWidth; x++)
			for (int y = 0; y < FieldHeight; y++) {
				screen[(y + 2) * Width + (x + 2)] = L" ABCDEFG$#"[Field[y * FieldWidth + x]];
			}
		// ��������� ������� ����������
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++) {
				if (tetromino[CurrentPiece][Rotate(px, py, CurrentRotation)] != L'.') {
					screen[(CurrentY + py + 2) * Width + (CurrentX + px + 2)] = CurrentPiece + 65;

				}
			}

		// ������
		swprintf_s(&screen[2 * Width + FieldWidth + 6], 16, L"SCORE: %8d", Score);

		// �������� ���������� �����
		if (!vLines.empty())
		{
			// ��������� ����� �������
			COORD c = { 0, 0 };
			WriteConsoleOutputCharacter(hConsole, screen, Width * Height, c, &dwBytesWritten);
			this_thread::sleep_for(400ms);

			for (auto& v : vLines)
				for (int px = 1; px < FieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						Field[py * FieldWidth + px] = Field[(py - 1) * FieldWidth + px];
					Field[px] = 0;
				}

			vLines.clear();
		}
		COORD l = { 0, 0 };
		WriteConsoleOutputCharacter(hConsole, screen, Width * Height, l, &dwBytesWritten);
	}

	CloseHandle(hConsole);
	std::cout << "Game Over!! Score:" << Score << endl;
	return 0;
}
