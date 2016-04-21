#pragma once

#include "SCPExtended.h"

namespace SCPUser {

	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	struct PAD_STATE 
	{	
		XINPUT_STATE     state;
		XINPUT_VIBRATION vibration;
		
		bool bConnected;
	};

	static XINPUT_VIBRATION l_Vibration  [XUSER_MAX_COUNT];
	static PAD_STATE	    l_Controller [XUSER_MAX_COUNT];
	static TCHAR			l_szMessage  [XUSER_MAX_COUNT][1024];

	public ref class SCPPad : public System::Windows::Forms::Form
	{
	protected:
		SCPExtended^ m_Extended;

	public:
		SCPPad(void)
		{
			InitializeComponent();

			m_Extended = gcnew SCPExtended();

			l_szMessage[0][0] = 0;
			l_szMessage[1][0] = 0;
			l_szMessage[2][0] = 0;
			l_szMessage[3][0] = 0;

			lbOutput_0->Text = _T("");
			lbOutput_1->Text = _T("");
			lbOutput_2->Text = _T("");
			lbOutput_3->Text = _T("");

			ZeroMemory(l_Controller, sizeof(PAD_STATE)        * XUSER_MAX_COUNT);
			ZeroMemory(l_Vibration,  sizeof(XINPUT_VIBRATION) * XUSER_MAX_COUNT);

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
			XInputEnable(true);
#endif
		}

	protected:
		~SCPPad(void)
		{
			XINPUT_VIBRATION vibration = { 0, 0 };

			for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
			{
				if (l_Controller[i].bConnected)
				{
					XInputSetState(i, &vibration);
				}
			}

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
			XInputEnable(false);
#endif

			if (components)
			{
				delete components;
			}
		}

	private: 
		HRESULT UpdateControllerState()
		{
			DWORD dwResult;

			for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
			{
				// Simply get the state of the controller from XInput.
				dwResult = XInputGetState(i, &l_Controller[i].state);

				if (dwResult == ERROR_SUCCESS)
				{
					l_Controller[i].bConnected = true;

					if ((l_Vibration[i].wLeftMotorSpeed  != l_Controller[i].vibration.wLeftMotorSpeed)
					||  (l_Vibration[i].wRightMotorSpeed != l_Controller[i].vibration.wRightMotorSpeed))
					{
						l_Vibration[i].wLeftMotorSpeed  = l_Controller[i].vibration.wLeftMotorSpeed;
						l_Vibration[i].wRightMotorSpeed = l_Controller[i].vibration.wRightMotorSpeed;

						XInputSetState(i, &l_Controller[i].vibration);
					}
				}
				else
				{
					l_Controller[i].bConnected = false;
				}
			}

			return S_OK;
		}

	private: 
		System::Void updateTimer_Tick(System::Object^ sender, System::EventArgs^ e)
		{
		    bool bRepaint = false;
			updateTimer->Enabled = false;

			TCHAR sz[4][1024] = { _T(""), _T(""), _T(""), _T("") };

			if (UpdateControllerState() == S_OK)
			{
				for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
				{
					if (l_Controller[i].bConnected)
					{
						WORD wButtons = l_Controller[i].state.Gamepad.wButtons;

						if (cbDeadZone->Checked)
						{
							if (-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE < l_Controller[i].state.Gamepad.sThumbLX
							&&  l_Controller[i].state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
							{
								l_Controller[i].state.Gamepad.sThumbLX = 0;
							}

							if (-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE < l_Controller[i].state.Gamepad.sThumbLY
							&&  l_Controller[i].state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
							{
								l_Controller[i].state.Gamepad.sThumbLY = 0;
							}

							if (-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE < l_Controller[i].state.Gamepad.sThumbRX
							&&  l_Controller[i].state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
							{
								l_Controller[i].state.Gamepad.sThumbRX = 0;
							}

							if (-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE < l_Controller[i].state.Gamepad.sThumbRY
							&&  l_Controller[i].state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
							{
								l_Controller[i].state.Gamepad.sThumbRY = 0;
							}

							if (l_Controller[i].state.Gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
							{
								l_Controller[i].state.Gamepad.bLeftTrigger = 0;
							}

							if (l_Controller[i].state.Gamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
							{
								l_Controller[i].state.Gamepad.bRightTrigger = 0;
							}
						}
						
						_stprintf_s(sz[i], 1024,
										  _T("Controller %d: Connected\n")
										  _T("  Buttons: %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n")
										  _T("  Left Trigger: %d\n")
										  _T("  Right Trigger: %d\n")
										  _T("  Left Thumbstick: %d/%d\n")
										_T("  Right Thumbstick: %d/%d\n")
									_T("  L/R Motor Speed: %d/%d\n"),
									i + 1,
										  ( wButtons & XINPUT_GAMEPAD_DPAD_UP ) ? _T("DPAD_UP ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) ? _T("DPAD_DOWN ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) ? _T("DPAD_LEFT ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) ? _T("DPAD_RIGHT ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_START ) ? _T("START ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_BACK ) ? _T("BACK ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) ? _T("LEFT_THUMB ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) ? _T("RIGHT_THUMB ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ) ? _T("LEFT_SHOULDER ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ) ? _T("RIGHT_SHOULDER ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_A ) ? _T("A ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_B ) ? _T("B ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_X ) ? _T("X ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_Y ) ? _T("Y ") : _T(""),
										  ( wButtons & XINPUT_GAMEPAD_GUIDE ) ? _T("GUIDE ") : _T(""),
										  l_Controller[i].state.Gamepad.bLeftTrigger,
										  l_Controller[i].state.Gamepad.bRightTrigger,
										  l_Controller[i].state.Gamepad.sThumbLX,
										  l_Controller[i].state.Gamepad.sThumbLY,
										  l_Controller[i].state.Gamepad.sThumbRX,
										  l_Controller[i].state.Gamepad.sThumbRY,
										l_Controller[i].vibration.wLeftMotorSpeed,
										l_Controller[i].vibration.wRightMotorSpeed
									);
					}
					else
					{
						_stprintf_s(sz[i], 1024, _T("Controller %d: Disconnected\n"), i + 1);
					}

					switch (i)
					{
					case 0:
						tbOutput_0_Large->Visible = l_Controller[i].bConnected;
						tbOutput_0_Small->Visible = l_Controller[i].bConnected;

						l_Controller[i].vibration.wLeftMotorSpeed  = (WORD)((float)tbOutput_0_Large->Value * 653.55f);				
						l_Controller[i].vibration.wRightMotorSpeed = (WORD)((float)tbOutput_0_Small->Value * 653.55f);

						Lable_R_0->Visible = l_Controller[i].bConnected;
						Lable_L_0->Visible = l_Controller[i].bConnected;
						break;

					case 1:
						tbOutput_1_Large->Visible = l_Controller[i].bConnected;
						tbOutput_1_Small->Visible = l_Controller[i].bConnected;

						l_Controller[i].vibration.wLeftMotorSpeed  = (WORD)((float)tbOutput_1_Large->Value * 653.55f);
						l_Controller[i].vibration.wRightMotorSpeed = (WORD)((float)tbOutput_1_Small->Value * 653.55f);

						Lable_R_1->Visible = l_Controller[i].bConnected;
						Lable_L_1->Visible = l_Controller[i].bConnected;
						break;

					case 2:
						tbOutput_2_Large->Visible = l_Controller[i].bConnected;
						tbOutput_2_Small->Visible = l_Controller[i].bConnected;

						l_Controller[i].vibration.wLeftMotorSpeed  = (WORD)((float)tbOutput_2_Large->Value * 653.55f);
						l_Controller[i].vibration.wRightMotorSpeed = (WORD)((float)tbOutput_2_Small->Value * 653.55f);

						Lable_R_2->Visible = l_Controller[i].bConnected;
						Lable_L_2->Visible = l_Controller[i].bConnected;
						break;

					case 3:
						tbOutput_3_Large->Visible = l_Controller[i].bConnected;
						tbOutput_3_Small->Visible = l_Controller[i].bConnected;

						l_Controller[i].vibration.wLeftMotorSpeed  = (WORD)((float)tbOutput_3_Large->Value * 653.55f);
						l_Controller[i].vibration.wRightMotorSpeed = (WORD)((float)tbOutput_3_Small->Value * 653.55f);

						Lable_R_3->Visible = l_Controller[i].bConnected;
						Lable_L_3->Visible = l_Controller[i].bConnected;
						break;
					}
				}
			}

			for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
			{
				if (_tcscmp(sz[i], l_szMessage[i]) != 0)
				{
					_tcscpy_s(l_szMessage[i], 1024, sz[i]);
			        bRepaint = true;
				}
			}

			if (bRepaint)
			{
				lbOutput_0->Text = gcnew String(l_szMessage[0]);
				lbOutput_1->Text = gcnew String(l_szMessage[1]);
				lbOutput_2->Text = gcnew String(l_szMessage[2]);
				lbOutput_3->Text = gcnew String(l_szMessage[3]);
			}

			updateTimer->Enabled = true;
		}

	private: 
		System::Void cbExtensions_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
		{
			if (cbExtensions->Checked)
			{
				m_Extended->Show(this);
			}
			else
			{
				m_Extended->Hide();
			}
		}

	#pragma region Windows Form Designer generated code
	private:
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(SCPPad::typeid));
			this->lbOutput_0 = (gcnew System::Windows::Forms::Label());
			this->lbOutput_1 = (gcnew System::Windows::Forms::Label());
			this->lbOutput_2 = (gcnew System::Windows::Forms::Label());
			this->lbOutput_3 = (gcnew System::Windows::Forms::Label());
			this->advPanel = (gcnew System::Windows::Forms::Panel());
			this->cbExtensions = (gcnew System::Windows::Forms::CheckBox());
			this->lblRumble = (gcnew System::Windows::Forms::Label());
			this->tbOutput_0_Large = (gcnew System::Windows::Forms::TrackBar());
			this->tbOutput_0_Small = (gcnew System::Windows::Forms::TrackBar());
			this->tbOutput_1_Large = (gcnew System::Windows::Forms::TrackBar());
			this->tbOutput_1_Small = (gcnew System::Windows::Forms::TrackBar());
			this->tbOutput_2_Large = (gcnew System::Windows::Forms::TrackBar());
			this->tbOutput_2_Small = (gcnew System::Windows::Forms::TrackBar());
			this->tbOutput_3_Large = (gcnew System::Windows::Forms::TrackBar());
			this->tbOutput_3_Small = (gcnew System::Windows::Forms::TrackBar());
			this->cbDeadZone = (gcnew System::Windows::Forms::CheckBox());
			this->statusBar = (gcnew System::Windows::Forms::StatusBar());
			this->updateTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->Lable_L_0 = (gcnew System::Windows::Forms::Label());
			this->Lable_L_2 = (gcnew System::Windows::Forms::Label());
			this->Lable_L_1 = (gcnew System::Windows::Forms::Label());
			this->Lable_L_3 = (gcnew System::Windows::Forms::Label());
			this->Lable_R_0 = (gcnew System::Windows::Forms::Label());
			this->Lable_R_1 = (gcnew System::Windows::Forms::Label());
			this->Lable_R_2 = (gcnew System::Windows::Forms::Label());
			this->Lable_R_3 = (gcnew System::Windows::Forms::Label());
			this->advPanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_0_Large))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_0_Small))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_1_Large))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_1_Small))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_2_Large))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_2_Small))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_3_Large))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_3_Small))->BeginInit();
			this->SuspendLayout();
			// 
			// lbOutput_0
			// 
			this->lbOutput_0->AutoSize = true;
			this->lbOutput_0->BackColor = System::Drawing::Color::Transparent;
			this->lbOutput_0->Font = (gcnew System::Drawing::Font(L"Consolas", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
																  static_cast<System::Byte>(0)));
			this->lbOutput_0->ForeColor = System::Drawing::SystemColors::ControlText;
			this->lbOutput_0->Location = System::Drawing::Point(13, 30);
			this->lbOutput_0->Name = L"lbOutput_0";
			this->lbOutput_0->Size = System::Drawing::Size(55, 13);
			this->lbOutput_0->TabIndex = 8;
			this->lbOutput_0->Text = L"<output>";
			// 
			// lbOutput_1
			// 
			this->lbOutput_1->AutoSize = true;
			this->lbOutput_1->BackColor = System::Drawing::Color::Transparent;
			this->lbOutput_1->Font = (gcnew System::Drawing::Font(L"Consolas", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
																  static_cast<System::Byte>(0)));
			this->lbOutput_1->ForeColor = System::Drawing::SystemColors::ControlText;
			this->lbOutput_1->Location = System::Drawing::Point(13, 120);
			this->lbOutput_1->Name = L"lbOutput_1";
			this->lbOutput_1->Size = System::Drawing::Size(55, 13);
			this->lbOutput_1->TabIndex = 9;
			this->lbOutput_1->Text = L"<output>";
			// 
			// lbOutput_2
			// 
			this->lbOutput_2->AutoSize = true;
			this->lbOutput_2->BackColor = System::Drawing::Color::Transparent;
			this->lbOutput_2->Font = (gcnew System::Drawing::Font(L"Consolas", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
																  static_cast<System::Byte>(0)));
			this->lbOutput_2->ForeColor = System::Drawing::SystemColors::ControlText;
			this->lbOutput_2->Location = System::Drawing::Point(13, 210);
			this->lbOutput_2->Name = L"lbOutput_2";
			this->lbOutput_2->Size = System::Drawing::Size(55, 13);
			this->lbOutput_2->TabIndex = 10;
			this->lbOutput_2->Text = L"<output>";
			// 
			// lbOutput_3
			// 
			this->lbOutput_3->AutoSize = true;
			this->lbOutput_3->BackColor = System::Drawing::Color::Transparent;
			this->lbOutput_3->Font = (gcnew System::Drawing::Font(L"Consolas", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
																  static_cast<System::Byte>(0)));
			this->lbOutput_3->ForeColor = System::Drawing::SystemColors::ControlText;
			this->lbOutput_3->Location = System::Drawing::Point(13, 300);
			this->lbOutput_3->Name = L"lbOutput_3";
			this->lbOutput_3->Size = System::Drawing::Size(55, 13);
			this->lbOutput_3->TabIndex = 11;
			this->lbOutput_3->Text = L"<output>";
			// 
			// advPanel
			// 
			this->advPanel->BackColor = System::Drawing::SystemColors::Control;
			this->advPanel->Controls->Add(this->Lable_L_1);
			this->advPanel->Controls->Add(this->Lable_L_3);
			this->advPanel->Controls->Add(this->Lable_L_2);
			this->advPanel->Controls->Add(this->Lable_R_3);
			this->advPanel->Controls->Add(this->Lable_R_2);
			this->advPanel->Controls->Add(this->Lable_R_1);
			this->advPanel->Controls->Add(this->Lable_R_0);
			this->advPanel->Controls->Add(this->Lable_L_0);
			this->advPanel->Controls->Add(this->cbExtensions);
			this->advPanel->Controls->Add(this->lblRumble);
			this->advPanel->Controls->Add(this->tbOutput_0_Large);
			this->advPanel->Controls->Add(this->tbOutput_0_Small);
			this->advPanel->Controls->Add(this->tbOutput_1_Large);
			this->advPanel->Controls->Add(this->tbOutput_1_Small);
			this->advPanel->Controls->Add(this->tbOutput_2_Large);
			this->advPanel->Controls->Add(this->tbOutput_2_Small);
			this->advPanel->Controls->Add(this->tbOutput_3_Large);
			this->advPanel->Controls->Add(this->tbOutput_3_Small);
			this->advPanel->Controls->Add(this->cbDeadZone);
			this->advPanel->Dock = System::Windows::Forms::DockStyle::Right;
			this->advPanel->Location = System::Drawing::Point(932, 0);
			this->advPanel->Name = L"advPanel";
			this->advPanel->Size = System::Drawing::Size(214, 491);
			this->advPanel->TabIndex = 13;
			// 
			// cbExtensions
			// 
			this->cbExtensions->AutoSize = true;
			this->cbExtensions->Location = System::Drawing::Point(50, 421);
			this->cbExtensions->Name = L"cbExtensions";
			this->cbExtensions->Size = System::Drawing::Size(77, 17);
			this->cbExtensions->TabIndex = 10;
			this->cbExtensions->Text = L"Extensions";
			this->cbExtensions->UseVisualStyleBackColor = true;
			this->cbExtensions->CheckedChanged += gcnew System::EventHandler(this, &SCPPad::cbExtensions_CheckedChanged);
			// 
			// lblRumble
			// 
			this->lblRumble->AutoSize = true;
			this->lblRumble->Location = System::Drawing::Point(72, 9);
			this->lblRumble->Name = L"lblRumble";
			this->lblRumble->Size = System::Drawing::Size(43, 13);
			this->lblRumble->TabIndex = 0;
			this->lblRumble->Text = L"Rumble";
			// 
			// tbOutput_0_Large
			// 
			this->tbOutput_0_Large->AutoSize = false;
			this->tbOutput_0_Large->BackColor = System::Drawing::SystemColors::Control;
			this->tbOutput_0_Large->Location = System::Drawing::Point(51, 30);
			this->tbOutput_0_Large->Maximum = 100;
			this->tbOutput_0_Large->Name = L"tbOutput_0_Large";
			this->tbOutput_0_Large->Size = System::Drawing::Size(150, 25);
			this->tbOutput_0_Large->TabIndex = 1;
			this->tbOutput_0_Large->TickFrequency = 10;
			this->tbOutput_0_Large->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// tbOutput_0_Small
			// 
			this->tbOutput_0_Small->AutoSize = false;
			this->tbOutput_0_Small->BackColor = System::Drawing::SystemColors::Control;
			this->tbOutput_0_Small->Location = System::Drawing::Point(51, 61);
			this->tbOutput_0_Small->Maximum = 100;
			this->tbOutput_0_Small->Name = L"tbOutput_0_Small";
			this->tbOutput_0_Small->Size = System::Drawing::Size(150, 25);
			this->tbOutput_0_Small->TabIndex = 2;
			this->tbOutput_0_Small->TickFrequency = 10;
			this->tbOutput_0_Small->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// tbOutput_1_Large
			// 
			this->tbOutput_1_Large->AutoSize = false;
			this->tbOutput_1_Large->BackColor = System::Drawing::SystemColors::Control;
			this->tbOutput_1_Large->Location = System::Drawing::Point(51, 120);
			this->tbOutput_1_Large->Maximum = 100;
			this->tbOutput_1_Large->Name = L"tbOutput_1_Large";
			this->tbOutput_1_Large->Size = System::Drawing::Size(150, 25);
			this->tbOutput_1_Large->TabIndex = 3;
			this->tbOutput_1_Large->TickFrequency = 10;
			this->tbOutput_1_Large->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// tbOutput_1_Small
			// 
			this->tbOutput_1_Small->AutoSize = false;
			this->tbOutput_1_Small->BackColor = System::Drawing::SystemColors::Control;
			this->tbOutput_1_Small->Location = System::Drawing::Point(51, 151);
			this->tbOutput_1_Small->Maximum = 100;
			this->tbOutput_1_Small->Name = L"tbOutput_1_Small";
			this->tbOutput_1_Small->Size = System::Drawing::Size(150, 25);
			this->tbOutput_1_Small->TabIndex = 4;
			this->tbOutput_1_Small->TickFrequency = 10;
			this->tbOutput_1_Small->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// tbOutput_2_Large
			// 
			this->tbOutput_2_Large->AutoSize = false;
			this->tbOutput_2_Large->BackColor = System::Drawing::SystemColors::Control;
			this->tbOutput_2_Large->Location = System::Drawing::Point(51, 210);
			this->tbOutput_2_Large->Maximum = 100;
			this->tbOutput_2_Large->Name = L"tbOutput_2_Large";
			this->tbOutput_2_Large->Size = System::Drawing::Size(150, 25);
			this->tbOutput_2_Large->TabIndex = 5;
			this->tbOutput_2_Large->TickFrequency = 10;
			this->tbOutput_2_Large->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// tbOutput_2_Small
			// 
			this->tbOutput_2_Small->AutoSize = false;
			this->tbOutput_2_Small->BackColor = System::Drawing::SystemColors::Control;
			this->tbOutput_2_Small->Location = System::Drawing::Point(51, 241);
			this->tbOutput_2_Small->Maximum = 100;
			this->tbOutput_2_Small->Name = L"tbOutput_2_Small";
			this->tbOutput_2_Small->Size = System::Drawing::Size(150, 25);
			this->tbOutput_2_Small->TabIndex = 6;
			this->tbOutput_2_Small->TickFrequency = 10;
			this->tbOutput_2_Small->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// tbOutput_3_Large
			// 
			this->tbOutput_3_Large->AutoSize = false;
			this->tbOutput_3_Large->BackColor = System::Drawing::SystemColors::Control;
			this->tbOutput_3_Large->Location = System::Drawing::Point(51, 300);
			this->tbOutput_3_Large->Maximum = 100;
			this->tbOutput_3_Large->Name = L"tbOutput_3_Large";
			this->tbOutput_3_Large->Size = System::Drawing::Size(150, 25);
			this->tbOutput_3_Large->TabIndex = 7;
			this->tbOutput_3_Large->TickFrequency = 10;
			this->tbOutput_3_Large->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// tbOutput_3_Small
			// 
			this->tbOutput_3_Small->AutoSize = false;
			this->tbOutput_3_Small->BackColor = System::Drawing::SystemColors::Control;
			this->tbOutput_3_Small->Location = System::Drawing::Point(51, 331);
			this->tbOutput_3_Small->Maximum = 100;
			this->tbOutput_3_Small->Name = L"tbOutput_3_Small";
			this->tbOutput_3_Small->Size = System::Drawing::Size(150, 25);
			this->tbOutput_3_Small->TabIndex = 8;
			this->tbOutput_3_Small->TickFrequency = 10;
			this->tbOutput_3_Small->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// cbDeadZone
			// 
			this->cbDeadZone->AutoSize = true;
			this->cbDeadZone->BackColor = System::Drawing::Color::Transparent;
			this->cbDeadZone->Checked = true;
			this->cbDeadZone->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbDeadZone->Location = System::Drawing::Point(50, 444);
			this->cbDeadZone->Name = L"cbDeadZone";
			this->cbDeadZone->Size = System::Drawing::Size(80, 17);
			this->cbDeadZone->TabIndex = 9;
			this->cbDeadZone->Text = L"Dead Zone";
			this->cbDeadZone->UseVisualStyleBackColor = false;
			// 
			// statusBar
			// 
			this->statusBar->Location = System::Drawing::Point(0, 491);
			this->statusBar->Name = L"statusBar";
			this->statusBar->Size = System::Drawing::Size(1146, 22);
			this->statusBar->TabIndex = 7;
			// 
			// updateTimer
			// 
			this->updateTimer->Enabled = true;
			this->updateTimer->Interval = 50;
			this->updateTimer->Tick += gcnew System::EventHandler(this, &SCPPad::updateTimer_Tick);
			// 
			// Lable_L_0
			// 
			this->Lable_L_0->AutoSize = true;
			this->Lable_L_0->Location = System::Drawing::Point(13, 30);
			this->Lable_L_0->Name = L"Lable_L_0";
			this->Lable_L_0->Size = System::Drawing::Size(13, 13);
			this->Lable_L_0->TabIndex = 11;
			this->Lable_L_0->Text = L"L";
			// 
			// Lable_L_2
			// 
			this->Lable_L_2->AutoSize = true;
			this->Lable_L_2->Location = System::Drawing::Point(13, 210);
			this->Lable_L_2->Name = L"Lable_L_2";
			this->Lable_L_2->Size = System::Drawing::Size(13, 13);
			this->Lable_L_2->TabIndex = 12;
			this->Lable_L_2->Text = L"L";
			// 
			// Lable_L_1
			// 
			this->Lable_L_1->AutoSize = true;
			this->Lable_L_1->Location = System::Drawing::Point(13, 120);
			this->Lable_L_1->Name = L"Lable_L_1";
			this->Lable_L_1->Size = System::Drawing::Size(13, 13);
			this->Lable_L_1->TabIndex = 13;
			this->Lable_L_1->Text = L"L";
			// 
			// Lable_L_3
			// 
			this->Lable_L_3->AutoSize = true;
			this->Lable_L_3->Location = System::Drawing::Point(13, 300);
			this->Lable_L_3->Name = L"Lable_L_3";
			this->Lable_L_3->Size = System::Drawing::Size(13, 13);
			this->Lable_L_3->TabIndex = 12;
			this->Lable_L_3->Text = L"L";
			// 
			// Lable_R_0
			// 
			this->Lable_R_0->AutoSize = true;
			this->Lable_R_0->Location = System::Drawing::Point(13, 61);
			this->Lable_R_0->Name = L"Lable_R_0";
			this->Lable_R_0->Size = System::Drawing::Size(15, 13);
			this->Lable_R_0->TabIndex = 11;
			this->Lable_R_0->Text = L"R";
			// 
			// Lable_R_1
			// 
			this->Lable_R_1->AutoSize = true;
			this->Lable_R_1->Location = System::Drawing::Point(11, 151);
			this->Lable_R_1->Name = L"Lable_R_1";
			this->Lable_R_1->Size = System::Drawing::Size(15, 13);
			this->Lable_R_1->TabIndex = 11;
			this->Lable_R_1->Text = L"R";
			// 
			// Lable_R_2
			// 
			this->Lable_R_2->AutoSize = true;
			this->Lable_R_2->Location = System::Drawing::Point(11, 241);
			this->Lable_R_2->Name = L"Lable_R_2";
			this->Lable_R_2->Size = System::Drawing::Size(15, 13);
			this->Lable_R_2->TabIndex = 11;
			this->Lable_R_2->Text = L"R";
			// 
			// Lable_R_3
			// 
			this->Lable_R_3->AutoSize = true;
			this->Lable_R_3->Location = System::Drawing::Point(11, 331);
			this->Lable_R_3->Name = L"Lable_R_3";
			this->Lable_R_3->Size = System::Drawing::Size(15, 13);
			this->Lable_R_3->TabIndex = 11;
			this->Lable_R_3->Text = L"R";
			// 
			// SCPPad
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::White;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->ClientSize = System::Drawing::Size(1146, 513);
			this->Controls->Add(this->lbOutput_0);
			this->Controls->Add(this->lbOutput_1);
			this->Controls->Add(this->lbOutput_2);
			this->Controls->Add(this->lbOutput_3);
			this->Controls->Add(this->advPanel);
			this->Controls->Add(this->statusBar);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->Name = L"SCPPad";
			this->Text = L"XInput Controller Tester";
			this->advPanel->ResumeLayout(false);
			this->advPanel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_0_Large))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_0_Small))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_1_Large))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_1_Small))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_2_Large))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_2_Small))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_3_Large))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tbOutput_3_Small))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}

	private: System::Windows::Forms::Label^  lbOutput_0;
	private: System::Windows::Forms::Label^  lbOutput_1;
	private: System::Windows::Forms::Label^  lbOutput_2;
	private: System::Windows::Forms::Label^  lbOutput_3;

	private: System::Windows::Forms::Panel^  advPanel;
	private: System::Windows::Forms::Label^  lblRumble;
	private: System::Windows::Forms::TrackBar^  tbOutput_0_Large;
	private: System::Windows::Forms::TrackBar^  tbOutput_0_Small;
	private: System::Windows::Forms::TrackBar^  tbOutput_1_Large;
	private: System::Windows::Forms::TrackBar^  tbOutput_1_Small;
	private: System::Windows::Forms::TrackBar^  tbOutput_2_Large;
	private: System::Windows::Forms::TrackBar^  tbOutput_2_Small;
	private: System::Windows::Forms::TrackBar^  tbOutput_3_Large;
	private: System::Windows::Forms::TrackBar^  tbOutput_3_Small;
	private: System::Windows::Forms::CheckBox^  cbDeadZone;
	private: System::Windows::Forms::StatusBar^  statusBar;
	private: System::Windows::Forms::Timer^  updateTimer;
	private: System::Windows::Forms::CheckBox^  cbExtensions;
private: System::Windows::Forms::Label^  Lable_L_0;
private: System::Windows::Forms::Label^  Lable_L_1;
private: System::Windows::Forms::Label^  Lable_L_3;
private: System::Windows::Forms::Label^  Lable_L_2;
private: System::Windows::Forms::Label^  Lable_R_3;
private: System::Windows::Forms::Label^  Lable_R_2;
private: System::Windows::Forms::Label^  Lable_R_1;
private: System::Windows::Forms::Label^  Lable_R_0;

	private: System::ComponentModel::IContainer^  components;
	#pragma endregion
	private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
	}
};
}
