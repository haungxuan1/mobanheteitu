#include "pch.h"
#include "Tif.h"
#include "PQTif.h"
#include "Implement.h"
#include "S2S.h"
#include "Tif2Mat.h"

namespace Tif
{
	CPQTif g_cTif;
	function<void()> g_fReadTifError;
	function<void(tuple<ImPtr::CImPtr, vector<wstring>, bool>&)> g_fTif2Mat;

	ImplementModule(Tif)

	unsigned int __stdcall OnRegThread(void* lpUesrParam)
	{
		g_cTif.Begin() ? g_fOnRegSucceed() : g_fOnRegFailed();

		return 0;
	}

	unsigned int __stdcall OnUnregThread(void* lpUesrParam)
	{
		g_cTif.End() ? g_fOnUnregSucceed() : g_fOnUnregFailed();

		return 0;
	}

	/*********************************************************************************************/
	void OnTif2Mat(std::string strTif)
	{
		g_cTif.Tif2Mat(strTif);
	}

	void OnTif2MatW(std::wstring strTif)
	{
		OnTif2Mat(S2S::W2S(strTif));
	}

	/*********************************************************************************************/
	void OnReadTifError()
	{
		g_fReadTifError();
	}

	void OnTif2Mat(tuple<ImPtr::CImPtr, vector<wstring>, bool>& cTif)
	{
		g_fTif2Mat(cTif);
	}

	/*********************************************************************************************/
	void SetTifReadTifErrorCallback(function<void()> fError)
	{
		g_fReadTifError = fError;
	}

	void SetTifTif2MatCallback(function<void(tuple<ImPtr::CImPtr, vector<wstring>, bool>&)> fTif2Mat)
	{
		g_fTif2Mat = fTif2Mat;
	}
	
	bool readTifChnNames(vector<wstring>& tVecProductChnNames, std::string tProductFile)
	{
		bool bRet = false;

		tVecProductChnNames.clear();

		TIFF* ptTiff = TIFFOpen(tProductFile.c_str(), "rb");

		if (ptTiff != nullptr) {
			uint16_t usPhotometric = 0;

			if (TIFFGetField(ptTiff, TIFFTAG_PHOTOMETRIC, &usPhotometric)) {
				if (PHOTOMETRIC_SEPARATED == usPhotometric) {
					tVecProductChnNames.push_back(L"C");
					tVecProductChnNames.push_back(L"M");
					tVecProductChnNames.push_back(L"Y");
					tVecProductChnNames.push_back(L"K");

					vector<wstring> tVecChnNames;

					bRet = Tif::GetChannels(ptTiff, tVecChnNames);
					if (bRet) {
						tVecProductChnNames.insert(tVecProductChnNames.end(), tVecChnNames.begin(), tVecChnNames.end());
					}
				}
			}

			TIFFClose(ptTiff);
		}

		return bRet;
	}
}
