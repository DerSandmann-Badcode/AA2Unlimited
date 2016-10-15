#pragma once

#include <Windows.h>
#include <vector>

namespace AAEdit {

/*
 * The little dialog that shows up in the editor to select stuff.
 */
	class UnlimitedDialog
	{
	public:
		UnlimitedDialog();
		~UnlimitedDialog();

		void Initialize();
		void Hide();
		void Destroy();

		void Refresh();

		inline bool IsVisible() const {
			return m_visible;
		}

		inline bool IsSaveFilesSet() const {
			return SendMessage(m_gnDialog.m_cbSaveFiles,BM_GETCHECK,0,0) == BST_CHECKED;
		}
		inline void SetSaveFiles(BOOL state) {
			SendMessage(m_gnDialog.m_cbSaveFiles,BM_SETCHECK,state ? BST_CHECKED : BST_UNCHECKED,0);
		}
	private:
	struct Dialog {
		HWND m_dialog;
		inline void Show(bool state) {
			ShowWindow(m_dialog, state ? SW_SHOW : SW_HIDE);
		}
		virtual void Refresh() = 0;
	};
	struct GNDialog : public Dialog {
		HWND m_cbSaveFiles;

		void Refresh();
		static INT_PTR CALLBACK DialogProc(_In_ HWND hwndDlg,_In_ UINT msg,_In_ WPARAM wparam,_In_ LPARAM lparam);
	} m_gnDialog;
	struct MODialog : public Dialog {
		HWND m_cbOverride;
		HWND m_edOverrideWith;
		HWND m_lbOverrides;
		
		void RefreshRuleList();
		void RefreshTextureList();
		void Refresh();
		static INT_PTR CALLBACK DialogProc(_In_ HWND hwndDlg, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam);
	} m_moDialog;
	struct AODialog : public Dialog {
		HWND m_edArchive;
		HWND m_edArchiveFile;
		HWND m_edOverrideFile;
		HWND m_btBrowse;
		HWND m_btApply;
		HWND m_lbOverrides;
		
		void RefreshRuleList();
		void Refresh();
		static INT_PTR CALLBACK DialogProc(_In_ HWND hwndDlg, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam);
	} m_aoDialog;
	struct ARDialog : public Dialog {
		HWND m_edArFrom;
		HWND m_edArTo;
		HWND m_edFileFrom;
		HWND m_edFileTo;
		HWND m_btApply;
		HWND m_lbOverrides;

		void RefreshRuleList();
		void Refresh();
		static INT_PTR CALLBACK DialogProc(_In_ HWND hwndDlg, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam);
	} m_arDialog;
	struct ETDialog : public Dialog {
		struct {
			HWND cbActive;
			HWND cbSaveInside;
			HWND edFile;
			HWND btBrowse;
		} m_eyes[2];

		void RefreshEnableState();
		void Refresh();
		static INT_PTR CALLBACK DialogProc(_In_ HWND hwndDlg, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam);
	} m_etDialog;
	struct TSDialog : public Dialog {
		HWND m_cbSelect;

		void LoadTanList();
		void Refresh();
		static INT_PTR CALLBACK DialogProc(_In_ HWND hwndDlg, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam);
	} m_tsDialog;
	struct HRDialog : public Dialog {
		HWND m_rbKind[4];
		HWND m_edSlot;
		HWND m_edAdjustment;
		HWND m_cbFlip;
		HWND m_lstHairs;

		HWND m_edHighlight;

		void RefreshHairList();
		void Refresh();
		static INT_PTR CALLBACK DialogProc(_In_ HWND hwndDlg, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam);
	} m_hrDialog;
	struct BDDialog : public Dialog {
		HWND m_cbOutlineColor;
		HWND m_edOutlineColorRed;
		HWND m_edOutlineColorGreen;
		HWND m_edOutlineColorBlue;

		HWND m_bmBtnAdd;
		HWND m_bmCbSelect;
		HWND m_bmList;
		HWND m_bmSldHeight;
		HWND m_bmSldWidth;
		HWND m_bmEdMatrix[4][4];
		
		void LoadData(int listboxId);
		void ApplyInput();
		void Refresh();
		static INT_PTR CALLBACK DialogProc(_In_ HWND hwndDlg,_In_ UINT msg,_In_ WPARAM wparam,_In_ LPARAM lparam);
	} m_bdDialog;


	HWND m_dialog;
	HWND m_tabs;

	bool m_visible;

	void AddDialog(int resourceName, Dialog* dialog, int tabN, const TCHAR* tabName, 
			RECT targetRct, INT_PTR (CALLBACK *DialogProc)(HWND,UINT,WPARAM,LPARAM));
	LPARAM GetCurrTabItemData();
private:

	static INT_PTR CALLBACK MainDialogProc(_In_ HWND hwndDlg, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam);
};

extern UnlimitedDialog g_AAUnlimitDialog;
/*
 * Fed every time the original notification function of the system dialog is called
 */
LRESULT __stdcall SystemDialogNotification(void* internclass, HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

}