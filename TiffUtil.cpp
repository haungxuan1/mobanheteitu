#include "pch.h"
#include "FileUtil.h"
#include "TiffUtil.h"
#include "../Tif/Tif.h"

TiffUtil::TiffUtil()
{
}

TiffUtil::~TiffUtil()
{

}

bool TiffUtil::readTiffFileToMat(Mat& tTiffMat, std::string tFileName)
{
	bool bRet = false;

	TIFF* ptTiff = TIFFOpen(tFileName.c_str(), "rb");
	if (ptTiff != nullptr) {
		int iWid = 0;
		int iHei = 0;

		TIFFGetField(ptTiff, TIFFTAG_IMAGEWIDTH, &iWid);
		TIFFGetField(ptTiff, TIFFTAG_IMAGELENGTH, &iHei);

		if ((iWid > 0) && (iHei > 0)) {
			unsigned short usChn = 0;

			TIFFGetField(ptTiff, TIFFTAG_SAMPLESPERPIXEL, &usChn);

			if (usChn > 0) {
				unsigned char* pucLineBuf = new unsigned char[usChn * iWid];

				if (pucLineBuf != NULL) {
					int i;
					std::vector<cv::Mat> tProductMatVector;

					for (i = 0; i < usChn; i++) {
						tProductMatVector.push_back(Mat(iHei, iWid, CV_8UC1));
					}

					for (int y = 0; y < iHei; y++) {
						TIFFReadScanline(ptTiff, pucLineBuf, y);

						for (i = 0; i < usChn; i++) {
							for (int x = 0; x < iWid; x++) {
								tProductMatVector[i].at<uchar>(y, x) = *(pucLineBuf + usChn * x + i);
							}
						}
					}

					merge(tProductMatVector, tTiffMat);

					bRet = true;

					delete[] pucLineBuf;
				}
			}
		}

		TIFFClose(ptTiff);
	}

	return bRet;
}

bool TiffUtil::writeMatToTiffFile(std::string tFileReadName, std::string tFileWriteName, Mat& tImgMat)
{
	bool bRet = false;

	if (!tImgMat.empty() && !tFileReadName.empty() && !tFileWriteName.empty() && (CV_8U == tImgMat.depth()) && (tImgMat.channels() > 0)) {
		std::string tBkFileName = FileUtil::getBackupFileName(tFileReadName);

		FileUtil::backupFile(tBkFileName, tFileReadName);

		TIFF* ptTiffRead = TIFFOpen(tBkFileName.c_str(), "rb");

		if (ptTiffRead != nullptr) {
			TIFF* ptTiffWrite = TIFFOpen(tFileWriteName.c_str(), "w");

			if (ptTiffWrite != nullptr) {
				float fXRes = 72.0f;
				float fYRes = 72.0f;
				uint16 usSampleFormat = 0;
				uint16 usPhotoMetric;
				uint32 uPhotoshop = 0;
				uint8* pucPhotoshop = nullptr;
				bool bRet1 = TIFFGetField(ptTiffRead, TIFFTAG_PHOTOSHOP, &uPhotoshop, &pucPhotoshop);

				TIFFGetField(ptTiffRead, TIFFTAG_XRESOLUTION, &fXRes);
				TIFFGetField(ptTiffRead, TIFFTAG_YRESOLUTION, &fYRes);
				TIFFGetField(ptTiffRead, TIFFTAG_PHOTOMETRIC, &usPhotoMetric);
				TIFFGetField(ptTiffRead, TIFFTAG_SAMPLEFORMAT, &usSampleFormat);

				//printf("xres: %.2f, yres: %.2f, %hu, %hu\n", fXRes, fYRes, usPhotoMetric, usSampleFormat);

				if (bRet1) {
					TIFFSetField(ptTiffWrite, TIFFTAG_PHOTOSHOP, uPhotoshop, pucPhotoshop);
				}

				TIFFSetField(ptTiffWrite, TIFFTAG_IMAGEWIDTH,      tImgMat.cols);
				TIFFSetField(ptTiffWrite, TIFFTAG_IMAGELENGTH,     tImgMat.rows);
				TIFFSetField(ptTiffWrite, TIFFTAG_SAMPLESPERPIXEL, tImgMat.channels());
				TIFFSetField(ptTiffWrite, TIFFTAG_BITSPERSAMPLE,   8);
				TIFFSetField(ptTiffWrite, TIFFTAG_XRESOLUTION,     fXRes);
				TIFFSetField(ptTiffWrite, TIFFTAG_YRESOLUTION,     fYRes);
				TIFFSetField(ptTiffWrite, TIFFTAG_SAMPLEFORMAT,    usSampleFormat);
				TIFFSetField(ptTiffWrite, TIFFTAG_PHOTOMETRIC,     usPhotoMetric);
				TIFFSetField(ptTiffWrite, TIFFTAG_ORIENTATION,     ORIENTATION_TOPLEFT);
				TIFFSetField(ptTiffWrite, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);
				TIFFSetField(ptTiffWrite, TIFFTAG_COMPRESSION,     COMPRESSION_LZW);

				unsigned char* pucLineBuf = new unsigned char[tImgMat.channels() * tImgMat.cols];

				if (pucLineBuf != NULL) {
					std::vector<cv::Mat> tMatChnsVec;
					cv::split(tImgMat, tMatChnsVec);

					for (int y = 0; y < tImgMat.rows; y++) {
						for (int x = 0; x < tImgMat.cols; x++) {
							for (int j = 0; j < tImgMat.channels(); j++) {
								*(pucLineBuf + x * tImgMat.channels() + j) = tMatChnsVec[j].at<uchar>(y, x);
							}
						}

						TIFFWriteScanline(ptTiffWrite, pucLineBuf, y);
					}

					delete[] pucLineBuf;
					bRet = true;
				}
			
				TIFFClose(ptTiffRead);
			}

			TIFFClose(ptTiffWrite);
		}
	}

	return bRet;
}

bool TiffUtil::checkTiff(std::string tFileName)
{
	bool bRet = false;

	if (!tFileName.empty()) {
		int iChnCount = 0;

		TIFF* ptTiff = TIFFOpen(tFileName.c_str(), "rb");
		if (ptTiff != nullptr) {
			int iWid = 0;
			int iHei = 0;
			uint16_t usPhotometric = 0;
			uint16_t usTiffBitSample = 0;

			TIFFGetField(ptTiff, TIFFTAG_IMAGEWIDTH, &iWid);
			TIFFGetField(ptTiff, TIFFTAG_IMAGELENGTH, &iHei);

			if ((iWid > 0) && (iHei > 0)) {
				unsigned short usChn = 0;

				TIFFGetField(ptTiff, TIFFTAG_SAMPLESPERPIXEL, &usChn);
				TIFFGetField(ptTiff, TIFFTAG_BITSPERSAMPLE, &usTiffBitSample);

				iChnCount = usChn;
			}

			if (!TIFFGetField(ptTiff, TIFFTAG_PHOTOMETRIC, &usPhotometric)) {
				usPhotometric = 0;
			}

			TIFFClose(ptTiff);

			_cwprintf(L"%s: sample=%d, photometric=%d\r\n", __FUNCTIONW__, usTiffBitSample, usPhotometric);

			if ((usTiffBitSample != 8) || (usPhotometric != PHOTOMETRIC_SEPARATED)) {
				return false;
			}
		}

		if (iChnCount > 4) {
			vector<wstring> tVecChnNames;

			if (Tif::readTifChnNames(tVecChnNames, tFileName)) {
				bool bError = false;

				for (auto tItem : tVecChnNames) {
					if ((tItem != L"CyanLight") && (tItem != L"MagentaLight") &&
						(tItem != L"White") && (tItem != L"Varnish") &&
						(tItem != L"C") && (tItem != L"M") && (tItem != L"Y") && (tItem != L"K")) {
						//_cwprintf(L"%s£ºerror, %s\r\n", __FUNCTIONW__, tItem.c_str());

						bError = true;
						break;
					}
				}

				if (!bError) {
					bRet = true;
				}
			}
		}
		else if (iChnCount > 0) {
			bRet = true;
		}
	}

	//_cwprintf(L"%s end: %d\r\n", __FUNCTIONW__, bRet);
	return bRet;
}

bool TiffUtil::getTiffInfo(std::string tFileName, int& iWid, int& iHei, int& iChn, int& iBitSample)
{
	bool bRet = false;

	iWid = 0;
	iHei = 0;
	iChn = 0;
	iBitSample = 0;

	if (!tFileName.empty()) {
		int iChnCount = 0;

		TIFF* ptTiff = TIFFOpen(tFileName.c_str(), "rb");
		if (ptTiff != nullptr) {
			int iWid = 0;
			int iHei = 0;
			unsigned short usChn = 0;
			uint16_t uTiffBitSample = 0;

			TIFFGetField(ptTiff, TIFFTAG_IMAGEWIDTH,      &iWid);
			TIFFGetField(ptTiff, TIFFTAG_IMAGELENGTH,     &iHei);
			TIFFGetField(ptTiff, TIFFTAG_SAMPLESPERPIXEL, &usChn);
			TIFFGetField(ptTiff, TIFFTAG_BITSPERSAMPLE,   &uTiffBitSample);

			TIFFClose(ptTiff);

			iChn = usChn;
			iBitSample = uTiffBitSample;

			bRet = true;
		}
	}

	return bRet;
}
