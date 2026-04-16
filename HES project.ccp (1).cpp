#include <windows.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

#define ID_WRITE  1
#define ID_READ   2
#define ID_CLEAR  3
#define ID_SAVE   4

HWND hAddr, hValue, hDisplay;
unsigned char memory[16] = { 0 };

void UpdateDisplay()
{
    std::stringstream ss;
    ss << std::uppercase << std::hex;
    for (int i = 0; i < 16; i++)
    {
        ss << "0x"
            << std::setw(2) << std::setfill('0') << i
            << " : "
            << std::setw(2) << std::setfill('0') << (int)memory[i]
            << "\r\n";
    }
    SetWindowTextA(hDisplay, ss.str().c_str());
}

void SaveMemoryToFile()
{
    std::ofstream file("memory_output.txt");
    if (!file)
    {
        MessageBoxA(NULL, "Unable to open file!", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    for (int i = 0; i < 16; i++)
    {
        file << "0x" << std::uppercase << std::hex
            << std::setw(2) << std::setfill('0') << i
            << " : "
            << std::setw(2) << (int)memory[i] << std::endl;
    }
    file.close();
    MessageBoxA(NULL, "Memory saved to memory_output.txt", "Success", MB_OK);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        CreateWindowA("STATIC", "Address (0-15):", WS_VISIBLE | WS_CHILD, 20, 20, 120, 20, hwnd, NULL, NULL, NULL);
        hAddr = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 20, 80, 20, hwnd, NULL, NULL, NULL);

        CreateWindowA("STATIC", "Hex Value (00-FF):", WS_VISIBLE | WS_CHILD, 20, 50, 120, 20, hwnd, NULL, NULL, NULL);
        hValue = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 50, 80, 20, hwnd, NULL, NULL, NULL);

        CreateWindowA("BUTTON", "WRITE", WS_VISIBLE | WS_CHILD, 20, 90, 70, 30, hwnd, (HMENU)ID_WRITE, NULL, NULL);
        CreateWindowA("BUTTON", "READ", WS_VISIBLE | WS_CHILD, 100, 90, 70, 30, hwnd, (HMENU)ID_READ, NULL, NULL);
        CreateWindowA("BUTTON", "CLEAR", WS_VISIBLE | WS_CHILD, 180, 90, 70, 30, hwnd, (HMENU)ID_CLEAR, NULL, NULL);
        CreateWindowA("BUTTON", "SAVE", WS_VISIBLE | WS_CHILD, 100, 360, 70, 30, hwnd, (HMENU)ID_SAVE, NULL, NULL);

        hDisplay = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY, 20, 140, 230, 200, hwnd, NULL, NULL, NULL);
        UpdateDisplay();
        break;

    case WM_COMMAND:
    {
        int id = LOWORD(wp);

        if (id == ID_WRITE || id == ID_READ)
        {
            char addrText[10];
            GetWindowTextA(hAddr, addrText, 10);

            if (strlen(addrText) == 0)
            {
                MessageBoxA(hwnd, "Please enter an address!", "Error", MB_OK | MB_ICONERROR);
                break;
            }

            int address = atoi(addrText);
            if (address < 0 || address > 15)
            {
                MessageBoxA(hwnd, "Invalid address! Must be 0-15.", "Error", MB_OK | MB_ICONERROR);
                break;
            }

            if (id == ID_WRITE)
            {
                char valText[10];
                GetWindowTextA(hValue, valText, 10);

                if (strlen(valText) == 0)
                {
                    MessageBoxA(hwnd, "Please enter a hex value!", "Error", MB_OK | MB_ICONERROR);
                    break;
                }

                int value = (int)strtol(valText, NULL, 16);
                if (value < 0 || value > 255)
                {
                    MessageBoxA(hwnd, "Invalid hex value! Must be 00-FF.", "Error", MB_OK | MB_ICONERROR);
                    break;
                }

                memory[address] = (unsigned char)value;
                UpdateDisplay();
            }
            else if (id == ID_READ)
            {
                char msg[50];
                wsprintfA(msg, "Address 0x%02X = %02X", address, memory[address]);
                MessageBoxA(hwnd, msg, "Read", MB_OK);
            }
        }
        else if (id == ID_CLEAR)
        {
            for (int i = 0; i < 16; i++) memory[i] = 0;
            UpdateDisplay();
        }
        else if (id == ID_SAVE)
        {
            SaveMemoryToFile();
        }

        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "HexEditor";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);

    CreateWindowA("HexEditor", "Hex Editor Simulation (COAL)", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 400, 150, 300, 430, NULL, NULL, hInst, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
