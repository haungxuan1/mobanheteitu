#pragma once
#include <afxwin.h>
#include <iostream>

using namespace std;

class TiffUtil
{
public:
	static bool readTiffFileToMat(Mat &tTiffMat, std::string tFileName);
	static bool writeMatToTiffFile(std::string tFileReadName, std::string tFileWriteName, Mat &tImgMat);
	static bool checkTiff(std::string tFileName);
	static bool getTiffInfo(std::string tFileName, int &iWid, int &iHei, int &iChn, int &iBitSample);

private:
	TiffUtil();
	~TiffUtil();
};
