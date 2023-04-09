#include "Main.h"
#include "Includes.h"
#include "Utility.h"
#include "Driver.h"
#include "DX11Window.h"
#include "DX11.h"
#include "Data.h"
#include "Esp.h"
#include "Memory.h"
#include "Discord.h"
#include "Aimbot.h"
//#include "Others/termcolor.hpp"
#include"loadDrv.h"
#include <iostream> 
#include <windows.h> // WinApi header 
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"
#include "imgui-docking/imgui_internal.h"
Discord* g_Discord;

#include "ShObjIdl.h"
#include "xorstr.hpp"


#define DIRECTINPUT_VERSION 0x0800
#define XTREME_DRIVER_FILE  "rexuss.sys"
#define XTREME_SERVICE_NAME "rexuss"
#define XTREME_DEVICE_NAME  "\\Device\\rexuss"
//DRIVER* m_drv = nullptr;


//Discord* g_Discord;

inline bool existsDrv(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool WriteDataToFile(const UCHAR pBuffer[], const DWORD dwSize, const std::string& strFileName, const DWORD dwCreationDisposition = CREATE_NEW)
{
	const auto hFile = CreateFileA(strFileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) return false;
	DWORD dwNumberOfBytesWritten = NULL;
	const auto bWriteFile = WriteFile(hFile, pBuffer, dwSize, &dwNumberOfBytesWritten, nullptr);
	CloseHandle(hFile);
	return !(!bWriteFile || dwNumberOfBytesWritten != dwSize);
}

DRIVER* m_drv = nullptr;
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
void CustomImGUIStyle()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(4, 9);
	style.WindowRounding = 6.0f;
	style.FramePadding = ImVec2(3, 3);
	style.FrameRounding = 5.0f; // Make all elements (checkboxes, etc) circles
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(1, 6);
	style.CellPadding = ImVec2(5, 5);
	style.IndentSpacing = 23.0f;
	style.ScrollbarSize = 11.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 5.0f; // Make grab a circle
	style.GrabRounding = 3.0f;
	style.PopupRounding = 2.f;
	style.Alpha = 1.0;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.TabRounding = 7.0f;
	style.ChildRounding = 7.0f;
	style.ButtonTextAlign = ImVec2(0.50f, 0.50f);
	style.ChildBorderSize = 0.100f;
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.CurveTessellationTol = 1.f;
	style.TabBorderSize = 0.000f;
	ImVec4* colors = style.Colors;
	style.Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 235);
	style.Colors[ImGuiCol_Border] = ImColor(50, 50, 50, 255);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_TitleBg] = ImColor(66, 66, 66, 255);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_TitleBgActive] = ImColor(66, 66, 66, 255);
	style.Colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);
	style.Colors[ImGuiCol_MenuBarBg] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_SliderGrab] = ImColor(61, 133, 224, 250);
	style.Colors[ImGuiCol_SliderGrabActive] = ImColor(61, 133, 224, 250);
	style.Colors[ImGuiCol_Text] = ImVec4(0.98f, 0.97f, 0.93f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_HeaderActive] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_TabHovered] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_SliderGrabActive] = ImColor(61, 133, 224, 250);
	style.Colors[ImGuiCol_Button] = ImColor(40, 21, 110);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(158, 158, 158, 250);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_Header] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_HeaderHovered] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_HeaderActive] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_SeparatorHovered] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_SeparatorActive] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_ResizeGrip] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_ResizeGripHovered] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_ResizeGripActive] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_PlotHistogram] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_PlotLines] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_PlotHistogram] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_PlotHistogramHovered] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_PlotLinesHovered] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_PlotLines] = ImColor(66, 66, 66, 250);
	style.Colors[ImGuiCol_Tab] = ImColor(33, 33, 33, 255);
	style.Colors[ImGuiCol_TabHovered] = ImColor(66, 66, 66, 255);
	style.Colors[ImGuiCol_TabActive] = ImColor(66, 66, 66, 255);
	colors[ImGuiCol_TabUnfocusedActive] = ImColor(66, 66, 66, 250);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.98f, 0.26f, 0.26f, 0.35f);
	colors[ImGuiCol_NavHighlight] = ImColor(66, 66, 66, 250);
}


void print1(std::string text, int colorchoice)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// you can loop k higher to see more color choices
	// pick the colorattribute k you want
	SetConsoleTextAttribute(hConsole, 6);
	std::cout << text;
}

void print2(std::string text, int colorchoice)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// you can loop k higher to see more color choices
	// pick the colorattribute k you want
	SetConsoleTextAttribute(hConsole, 3);
	std::cout << text;
}

int main()
{
	/*print1(XorStr("{#} [MADE BY TEAM FORMULA |SAAIL AHMED|  ]\n"), 2);
	print1(XorStr("{#}  [CHEAT Version]\n"), 1);
	print1(XorStr("{#} 1.0\n"), 3);
	print1(XorStr("{#}  Status\n"), 4);
	print1(XorStr("{#} Safe\n"), 6);*/


	int choice = 2;
	/*HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);*/
	// 523 hertz (C5) for 500 milliseconds     
	g_Discord->Initialize();
	g_Discord->Update();
	string DriverPath = "C:\\rexuss.sys";
	string DriverName = "rexuss";
	;
	//system("pause");

	Sleep(1000);
	print1((""), 3);
	//system("title Formula Cheat 2.4.9");
	//std::cout << XorStr("Please Choose Your Emulator Version... \n\n");
	//print1(XorStr("-----------------------------------------------------------------------------------------------------------------------"), 4);
	//Sleep(1000);
	//print1(XorStr("{#} Connecting To The Auth Server\n"), 5);
	//print1(XorStr("{#} SUCCESS\n"), 7);
	//Sleep(500);
	//print1(XorStr("{#} CHEAT STATUS\n"), 6);
	//print1(XorStr("{#} SAFE\n"), 4);
	//Sleep(500);
	//print1(XorStr("{#} V1.0\n"), 9);
	//std::cout << termcolor::bright_yellow << "\n[>] Connecting To The Auth Server ";   Sleep(200);  std::cout << termcolor::bright_green << "\n Successs ";


	print1(XorStr("Checking loader\n"), 7);
	Sleep(1000);
	print1(XorStr("Checking version\n"), 7);
	Sleep(500);
	print1(XorStr("Download skipped*\n"), 7);
	Sleep(500);
	print1(XorStr(".....\n"), 7);

	Sleep(2000);

	//Sleep(3000); 


	//std::cout << XorStr("Please Choose Your Version... \n\n");
	//std::cout << XorStr("[+] 0. Free Version \n");
	//std::cout << XorStr("[+] 1. Pro Version \n");
	//std::cout << ("Enter Your Choice & Press Enter: \n");


	//std::cin >> choice;
	//if (choice == 0 || choice == 1);
	//std::cout << "\n";

	////if (choice == 0) {
	//	Setting::profree = false;
//
	//}
	//else if (choice == 1) {
	//	Setting::profree = true;

	//}

	m_drv = new DRIVER(XTREME_DRIVER_FILE, XTREME_DEVICE_NAME, XTREME_SERVICE_NAME, Driver, DriverSize);

	m_drv->Load();


	TCHAR procid[256] = L"aow_exe.exe";
	TCHAR procidss[256] = L"AndroidProcess.exe";
	Sleep(2000);
	Game::Id = Utility::GetTrueProcessId(procidss);
	if (Game::Id > 0)
	{
		Emulator::IsSmartGaGa = TRUE;
	}
	else
	{
		Game::Id = Utility::GetTrueProcessId(procid);
		if (Game::Id > 0)
		{
			Emulator::IsGameLoop = TRUE;
		}
	}
	if (Game::Id == 0)
	{
		m_drv->Unload();
		Sleep(500);

		MessageBoxA(NULL, "Can't find game process, Restart hack or Emulator", "Error", NULL);
		system("pause >nul");
		ExitProcess(0);
	}

	Game::hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Game::Id);

	if (Game::hProcess == 0)
	{
		m_drv->Unload();
		Sleep(500);


		MessageBoxA(NULL, "Can't find game process, Click ok to exit hack", "Error", NULL);
		system("pause >nul");
		ExitProcess(0);
	}

	if (Emulator::IsSmartGaGa == true)
	{
		EnumWindows(EnumWindowsProc, 0);
		if (IsIconic(Game::hWindow) != 0) ShowWindow(Game::hWindow, SW_RESTORE);
		Game::hWindow = FindWindowExW(Game::hWindow, 0, L"TitanRenderWindowClass", L"SmartGaGa RenderWindow");
		Game::hWindow = FindWindowEx(FindWindow(0, L"TitanRenderWindowClass"), NULL, L"TitanRenderWindowClass", L"SmartGaGa RenderWindow");

	}

	else if (Emulator::IsGameLoop == true)
	{
		EnumWindows(EnumWindowsProc, 0);
		if (IsIconic(Game::hWindow) != 0) ShowWindow(Game::hWindow, SW_RESTORE);
		Game::hWindow = FindWindowExW(Game::hWindow, 0, L"AEngineRenderWindowClass", L"AEngineRenderWindow");
	}

	if (Game::hWindow == 0)
	{
		//m_drv->Unload();
		MessageBox(NULL, L"Driver not Loader or Restart Emulator", L"REXUSS-HAX", MB_OKCANCEL);
		system("pause >nul");
		ExitProcess(0);
	}
	int scrWidthh = GetSystemMetrics(SM_CXSCREEN);
	int scrHeightt = GetSystemMetrics(SM_CYSCREEN);
	RECT Rect;
	GetWindowRect(Game::hWindow, &Rect);
	if (Rect.left < 0 || Rect.top < 0 || Rect.right < 0 || Rect.bottom < 0)
	{
		RECT tSize;
		GetWindowRect(Game::hWindow, &tSize);
		scrWidthh = tSize.right - tSize.left;
		scrHeightt = tSize.bottom - tSize.top;
		if (tSize.right > scrWidthh) {
			scrWidthh = tSize.right;
		}
		if (tSize.bottom > scrHeightt) {
			scrHeightt = tSize.bottom;
		}
		/*	DX11Window::Width = tSize.right - tSize.left;
			DX11Window::Height = tSize.bottom - tSize.top;*/
			//Driver::UnloadDriver(DriverName.c_str());
			/*cout << "Can't find game rect" << endl;
			system("pause >nul");
			ExitProcess(0);*/
		SetWindowPos(DX11Window::hWindow, HWND_TOPMOST, tSize.left, tSize.top, scrWidthh, scrHeightt, SWP_NOMOVE | SWP_NOSIZE);
	}

	if (Emulator::IsSmartGaGa == true)
	{
		Data::ViewMatrixBase = Data::GetViewMatrixBase(0x80000000, 0x90000000);

		if (Data::ViewMatrixBase == 0)
		{
			Data::ViewMatrixBase = Data::GetViewMatrixBase(0x90000000, 0xA0000000);
			Emulator::IsSmartGaGa4 = true;
		}
		else
		{
			Emulator::IsSmartGaGa7 = true;
		}
	}
	else if (Emulator::IsGameLoop == true)
	{
		Data::ViewMatrixBase = Data::GetViewMatrixBase(0x45000000, 0x60000000);

		if (Data::ViewMatrixBase == 0)
		{
			Data::ViewMatrixBase = Data::GetViewMatrixBase(0x25000000, 0x35000000);
			Emulator::IsGameLoop4 = true;
		}
		else
		{
			Emulator::IsGameLoop7 = true;
		}
	}

	if (Data::ViewMatrixBase == 0)
	{
		m_drv->Unload();
		MessageBox(NULL, L"Can't Find Game Base", L"INFO", MB_OKCANCEL);
		ExitProcess(0);
	}
	//	driver


	cout << "PID: " << dec << uppercase << Game::Id << " | " << hex << Game::Id << endl;
	m_drv->Unload();
	FreeConsole();

	// Main font and render font path here
	string MainFontPath = "C:\\Windows\\Fonts\\arial.ttf";
	string RenderFontPath = "C:\\Windows\\Fonts\\arial.ttf";

	DX11Window::Instantiate(Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top);
	DX11::Instantiate(DX11Window::hWindow, DX11Window::Width, DX11Window::Height);
	ImGui::CreateContext();
	ImGuiIO& Io = ImGui::GetIO();
	CustomImGUIStyle();


	Io.WantSaveIniSettings = true;

	//Io.Fonts->AddFontFromFileTTF("C:\\windows\\Fonts\\segoeuib.ttf", 14.5f);
	// Setup Dear ImGui style
	CustomImGUIStyle();

	static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
	ImFontConfig icons_config;

	ImFontConfig CustomFont;
	CustomFont.FontDataOwnedByAtlas = false;


	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 2.5;
	icons_config.OversampleV = 2.5;




	//
	Io.IniFilename = nullptr; //crutial for not leaving the imgui.ini file
	//Io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
	Esp::LoadConfig("‪C:\\rexusskey.ini"); //‪C:\khni_Key.ini
	Io.Fonts->AddFontFromFileTTF(MainFontPath.c_str(), 15.0f);


	Io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 16.0f, &icons_config, icons_ranges);

	ImGui_ImplWin32_Init(DX11Window::hWindow);
	ImGui_ImplDX11_Init(DX11::pDevice, DX11::pImmediateContext);
	DX11::ImGui_DX11::special = Io.Fonts->AddFontFromFileTTF(RenderFontPath.c_str(), 15.0f);
	DX11::ImGui_DX11::pRegularFont = Io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 13.f, &CustomFont);
	//DX11::ImGui_DX11::my_texture = Io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(logoo), sizeof(logoo), 13.f, &CustomFont);
	//Look here, all these are threads. Also this cheat source is badly made, many many threads wills slow your cheat\

	//you need to have 3-5 threads maximum here.koi na ap
	//Ab dekho yahan, Query thread mein hum jab tak enemy ka data find na karein, tab tak ESP display update nahi karega
	//So solution yeh hay ke apney threads decrease karo, 
	//and second thing, QueryThread ke andar se isPlayerEntity and PlayerData update ko separate thread mein le jao.
	//Kaam zyada hay lekin mein samjhata hu
	//dekho aisey:

	Thread::hQueryThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Data::QueryThread, 0, 0, 0);
	CloseHandle(Thread::hQueryThread);
	Thread::hEspThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Esp::EspThread, 0, 0, 0);
	CloseHandle(Thread::hEspThread);
	Thread::Target = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DX11Window::SetWindowToTarget, 0, 0, 0);
	CloseHandle(Thread::Target);
	Thread::hMemoryThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::MemoryThread, 0, 0, 0);
	CloseHandle(Thread::hMemoryThread);
	Thread::hAimbotThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Aimbot::AimbotThread, 0, 0, 0);
	CloseHandle(Thread::hAimbotThread);
	///mEMORY FUNCTİONS
	Thread::speedcarThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::FastCar, 0, 0, 0);
	CloseHandle(Thread::speedcarThread);
	//
	//Thread::norecoilThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::norecoil, 0, 0, 0);
	//CloseHandle(Thread::norecoilThread);
	//
	CloseHandle(Thread::xeffectThread);
	Thread::xeffectThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::MagicX, 0, 0, 0);
	CloseHandle(Thread::xeffectThread);
	////
	///
	CloseHandle(Thread::NoRecoilXThread);
	Thread::NoRecoilXThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::NoRecoilX, 0, 0, 0);
	CloseHandle(Thread::NoRecoilXThread);
	///
	Thread::RapidFireThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::RapidFire, 0, 0, 0);
	CloseHandle(Thread::RapidFireThread);
	//
	//Thread::RapidFireThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::RapidFire1, 0, 0, 0);
	//CloseHandle(Thread::RapidFireThread);
	//
	Thread::NoSpreadThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::NoSpread, 0, 0, 0);
	CloseHandle(Thread::NoSpreadThread);
	//
	Thread::CameraCatchThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::CameraCatch, 0, 0, 0);
	CloseHandle(Thread::CameraCatchThread);
	//
	//
	Thread::memoryaimbotThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::memoryaimbot, 0, 0, 0);
	CloseHandle(Thread::memoryaimbotThread);
	//
	Thread::FastSwitchWeaponThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::FastSwitchWeapon, 0, 0, 0);
	CloseHandle(Thread::FastSwitchWeaponThread);
	Thread::GodViewThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::GodView, 0, 0, 0);
	CloseHandle(Thread::GodViewThread);
	//
	CloseHandle(Thread::quicklootThread);
	Thread::quicklootThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::quickloot, 0, 0, 0);
	CloseHandle(Thread::quicklootThread);
	//
	//CloseHandle(Thread::nightmodeThread);
	//Thread::nightmodeThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::nightmode, 0, 0, 0);
	//CloseHandle(Thread::nightmodeThread);
	//
	//CloseHandle(Thread::recoilThread);
	//Thread::recoilThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::norecoil2, 0, 0, 0);
	//CloseHandle(Thread::recoilThread);

	CloseHandle(Thread::damagecounterThread);
	Thread::damagecounterThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::damagecounter, 0, 0, 0);
	CloseHandle(Thread::damagecounterThread);
	//
	CloseHandle(Thread::nogravityThread);
	Thread::nogravityThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::nogravity, 0, 0, 0);
	CloseHandle(Thread::nogravityThread);

	CloseHandle(Thread::blackskyThread);
	Thread::blackskyThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::blacksky, 0, 0, 0);
	CloseHandle(Thread::blackskyThread);

	//CloseHandle(Thread::nograssThread);
	//Thread::nograssThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Setting::NoGrass, 0, 0, 0);
	//CloseHandle(Thread::nograssThread);

	CloseHandle(Thread::FastLandingThread);
	Thread::FastLandingThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::FastLande, 0, 0, 0);
	CloseHandle(Thread::FastLandingThread);

	//CloseHandle(Thread::autohsThread);
	//Thread::autohsThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::autohs, 0, 0, 0);
	//CloseHandle(Thread::autohsThread);

	//CloseHandle(Thread::highdmgThread);
	//Thread::highdmgThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::highdmg, 0, 0, 0);
	//CloseHandle(Thread::highdmgThread);

	CloseHandle(Thread::noheadshotThread);
	Thread::noheadshotThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::noheadshot, 0, 0, 0);
	CloseHandle(Thread::noheadshotThread);

	CloseHandle(Thread::magicv1Thread);
	Thread::magicv1Thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::magicv1, 0, 0, 0);
	CloseHandle(Thread::magicv1Thread);
	//CloseHandle(Thread::nogravityThread);
	//Thread::nogravityThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::nogravity, 0, 0, 0);
	//CloseHandle(Thread::nogravityThread);

	//CloseHandle(Thread::magicv1Thread);
	//Thread::magicv1Thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::magicv1, 0, 0, 0);
	//CloseHandle(Thread::magicv1Thread);



	Thread::CarFlyThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Memory::flyCar, 0, 0, 0);
	CloseHandle(Thread::CarFlyThread);

	//Thread::gamespdThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Setting::gamespd, 0, 0, 0);
	//CloseHandle(Thread::gamespdThread);

	//Thread::gamespd1Thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Setting::gamespd1, 0, 0, 0);
	//CloseHandle(Thread::gamespd1Thread);

	//Memory::magnatbullot();
	MSG Message;
	while (true)
	{

		Sleep(10);

		if (PeekMessage(&Message, DX11Window::hWindow, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		if (Setting::ShowMenu)
		{
			SetForegroundWindow(DX11Window::hWindow);
		}

		//if (Setting::Legitrack)
		//{

		//	int randomNum = (rand() % 6) + 1;
		//	if (randomNum == 1)
		//	{
		//		Setting::curraim = 0;
		//	}
		//	if (randomNum == 2)
		//	{
		//		Setting::curraim = 1;
		//	}
		//	if (randomNum == 3)
		//	{
		//		Setting::curraim = 2;
		//	}

		//}

		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			if (DX11Window::IsClickable == true)
			{
				Utility::ChangeClickability(FALSE, DX11Window::hWindow);
				DX11Window::IsClickable = !DX11Window::IsClickable;
				Setting::ShowMenu = !Setting::ShowMenu;
			}
			else
			{
				Utility::ChangeClickability(TRUE, DX11Window::hWindow);
				DX11Window::IsClickable = !DX11Window::IsClickable;
				Setting::ShowMenu = !Setting::ShowMenu;
			}
		}
		if (GetAsyncKeyState(VK_HOME) & 1)
		{
			Setting::ShowMenu2 = !Setting::ShowMenu2;
		}
		if (GetAsyncKeyState(VK_END) & 1)
		{
			if (Setting::MagicBullet)
			{
				Memory::RestoreHook();
			}

			Sleep(100);
			m_drv->Unload();
			ExitProcess(1);
		}

	}

	return EXIT_SUCCESS;
}


BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	char Buffer[1000];
	GetClassNameA(hWnd, Buffer, 1000);

	string WindowClassName = Buffer;
	if (Emulator::IsSmartGaGa == TRUE)
	{
		if (WindowClassName.find("TitanRenderWindowClass") != string::npos)
		{
			wchar_t Name[1000];
			GetWindowTextW(hWnd, Name, 1000);
			Game::hWindow = FindWindowW(L"TitanRenderWindowClass", Name);
		}
	}
	else if (Emulator::IsGameLoop == TRUE)
	{
		if (WindowClassName.find("TXGuiFoundation") != string::npos)
		{
			wchar_t Name[1000];
			GetWindowTextW(hWnd, Name, 1000);
			Game::hWindow = FindWindowW(L"TXGuiFoundation", Name);
		}
	}

	return TRUE;
}