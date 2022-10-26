#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

int main(int argc, char** argv)
{
	HRESULT hres;
	//initialize COM?
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		cout << " Error code = 0x" << hex << hres << endl;
		return 1;               // Program has failed.
	}
	//initialize security?
	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);
	if (FAILED(hres))
	{
		cout << " Error code = 0x" << hex << hres << endl;
		CoUninitialize();
		return 1;               // Program has failed.
	}
	// locator for WMI?
	IWbemLocator* pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&pLoc);
	if (FAILED(hres))
	{
		cout << " Error code = 0x" << hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}
	// Connect to the local WMI
	IWbemServices* pSvc = NULL;

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);
	if (FAILED(hres))
	{
		cout << " Error code = 0x" << hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}
	cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);
	if (FAILED(hres))
	{
		cout << " Error code = 0x" << hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}
	// Call to get OS info
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_OperatingSystem"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		cout << " Error code = 0x" << hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}
	// Call to get CPU info
	IEnumWbemClassObject* pEnumerator2 = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_PerfRawData_PerfOS_Processor"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator2);

	if (FAILED(hres))
	{
		cout << " Error code = 0x" << hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}


	// start of program
	{
		string name;
		int choice = 1;
		int choice2 = 1;
		string confirm;
		string cpuusage = "N/A";
		IWbemClassObject* pclsObj = NULL;
		ULONG uReturn = 0; 
		IWbemClassObject* pclsObj2 = NULL;
		ULONG uReturn2 = 0;
		cout << "Hello, what is your name? " << "\n" << "\n";
		cout << "Enter your name here: ";
		cin >> name;
		if (name == "Christian") {
			cout << "Welcome " << name << "\n" << "\n";
		}
		else
			cout << "Are you sure you should be viewing this? Mr. " << name << "\n" << "\n" << "\n";
		while (true)
		{
			cout << "What would you like to view?" << "\n" << "\n" << "Press 1 for CPU / 2 for RAM / 3 for OS info" << "\n" << "\n" << "\n";
			cin >> choice;
			if(choice <= 3)
			switch (choice) {
			case 1:
				cout << "You chose CPU" << "\n" << "\n";
				cout << "Confirm? y/n " << "\n" << "\n";
				cin >> confirm;
				if (confirm == "y")
				{
					cout << "Choose one of the following: 1 for CPU usage / 2 to Quit" << "\n" << "\n";
					cin >> choice;
					switch (choice)
					{
					case 1:
						IWbemClassObject* pclsObj;
						while (1)
						{
							HRESULT hr = pEnumerator2->Next(WBEM_INFINITE, 1, &pclsObj2, &uReturn2);

							if (0 == uReturn2)
							{
								break;
							}

							VARIANT vtProp2;
							VariantInit(&vtProp2);

							hr = pclsObj2->Get(L"PercentProcessorTime", 0, &vtProp2, 0, 0);

							// Use it

							hr = pclsObj2->Get(L"TimeStamp_Sys100NS", 0, &vtProp2, 0, 0);

							// Use it
						}
					}
				}
			case 2:

			case 3:

				while (pEnumerator)
				{
					HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
						&pclsObj, &uReturn);

					if (0 == uReturn)
					{
						break;
					}

					VARIANT vtProp;

					VariantInit(&vtProp);
					// Get the value of the Name property
					hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
					wcout << " OS Name : " << vtProp.bstrVal << endl;
					VariantClear(&vtProp);

					pclsObj->Release();

				}
			default: 
			break;
			}
			else 
			pSvc->Release();
			pLoc->Release();
			pEnumerator->Release();
			CoUninitialize();

			return 0;

		}
	}


	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return 0;   
}
