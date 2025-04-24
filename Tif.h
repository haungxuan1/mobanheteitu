#pragma once
#include <tuple>
#include <string>
#include <vector>
#include <functional>
#include "Declare.h"
#include "ImPtr.h"

namespace Tif
{
	extern "C"
	{
		DeclareModule(Tif)

		// 读取Tif文件错误
		void SetTifReadTifErrorCallback(std::function<void()> fError);

		// 读取Tif完成
		void SetTifTif2MatCallback(std::function<void(std::tuple<ImPtr::CImPtr, std::vector<std::wstring>, bool>&)> fTif2Mat);

		/*************************************************Tif事件接口*************************************************/
		void OnTif2Mat(std::string strTif);
		void OnTif2MatW(std::wstring strTif);
		
		bool readTifChnNames(vector<wstring> &tVecProductChnNames, std::string tProductFile);
	}
}
