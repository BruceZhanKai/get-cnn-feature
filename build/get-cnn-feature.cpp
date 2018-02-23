// train_bayes_caffe.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
	#include "stdafx.h"
	#include <io.h>
#else
	#include <unistd.h>
	#include <dirent.h>
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "libCaffe.h"
#include "basics.h"
#include <cstdlib>
#include <cstdio>


#ifdef _WIN32
std::vector<std::string> GetClassName(const std::string& InputFolder)
{
	std::vector<std::string> OutFolder;
	char path_find[100];
	std::string path_find_string = "./" + InputFolder + "/*";
	strcpy(path_find, path_find_string.c_str());
	struct _finddata_t c_file;
	intptr_t hFile;
	std::string path_in;
	if ((hFile = _findfirst(path_find, &c_file)) == -1L)
	{
		std::cout << "no file in " << path_find << std::endl;
		return OutFolder;
	}
	else
	{
		do
		{
			path_in = c_file.name;
			if (path_in == ".." || path_in == ".")
			{
				//
			}
			else
				OutFolder.push_back(path_in);

		} while (_findnext(hFile, &c_file) == 0);
		_findclose(hFile);
	}
	return OutFolder;
}





#else

std::vector<std::string> GetClassName(const std::string& InputFolder)
{
	std::vector<std::string> OutFolder;
	std::string path_find = "./" + InputFolder;
	DIR* dir;
	struct dirent * ptr;
	dir=opendir(path_find.c_str());
	if (dir != NULL)
	{
		while ((ptr = readdir(dir)) != NULL)
		{
			std::string path_in(ptr->d_name);
			if(path_in == "." || path_in == "..")
			{
				//
			}
			else
			{

				OutFolder.push_back(path_in);
			}
		}
	}
	closedir(dir);
	return OutFolder;
}
#endif


int main(int argc, char **argv)
{
	std::cout << "Welcome to use XYZ Robot Recognition Engine." << std::endl;

	std::cout << "============================================================" << std::endl;
	std::cout << "步驟一:" << std::endl;
	std::cout << "將 normalize 成 192x192 眼距 40 的照片放置" << std::endl;
	std::cout << "C:/feature_bayes_caffe/xxxxxx/data_input.csv 中" << std::endl;
	std::cout << "============================================================" << std::endl;
	std::cout << "步驟二:" << std::endl;
	std::cout << "執行 project 1,得到 feature vector.TXT 及 label.TXT" << std::endl;
	std::cout << "(project 1 : get-cnn-feature)" << std::endl;
	std::cout << "============================================================" << std::endl;
	std::cout << "步驟三:" << std::endl;
	std::cout << "丟 TXT 給 Gary 去 train bayes model (project 2)" << std::endl;
	std::cout << "============================================================" << std::endl;
	std::cout << "步驟四:" << std::endl;
	std::cout << "最後拿 train 好的 cnn model 及 bayes model 來 test 結果" << std::endl;
	std::cout << "並存成 roc.TXT (project 3 : cnn_bayes_roc)" << std::endl;
	std::cout << "============================================================" << std::endl;
	std::cout << "步驟五:" << std::endl;
	std::cout << "將 roc.TXT 丟給 LFW 提供的 matlab code 去算 ROC 曲線" << std::endl;
	std::cout << "============================================================" << std::endl;
	system("pause");

	FacialCaffe facialcaffe;


	if(!facialcaffe.LoadModel(argv))
    {
        std::cout << "[main] facialcaffe LoadModel fail\n";
		return false;
    }

	int SIZE;
	bool GetFeature = true;
	std::string cnn_str = "./Parameter.txt";
    BZ::IfstreamInfo<int> FR_int;
    FR_int.DoIfstreamItem(cnn_str,"feature_size:");
    if(FR_int.GetBool())SIZE=FR_int.GetValue();

	//const int SIZE = 4096;
	//const int SIZE = 256;
	char separator1 = ';';
	std::string filename, InputFolder;
	std::cout << "input floder name?" << std::endl;
	std::cout << "./InputFolder/*" << std::endl;
	std::cout << ">>";
	std::cin >> InputFolder;
	std::vector<std::string> FolderVector;

	FolderVector = GetClassName(InputFolder);

	std::cout << "FolderVector.size()=" << FolderVector.size() << std::endl;

	std::string feature_path = "./" + InputFolder + "/feature.txt";
	std::string label_path = "./" + InputFolder + "/label.txt";
	std::string check_path = "./" + InputFolder + "/check.txt";
	std::ofstream file_feature, file_label, flie_check;
	file_feature.open(feature_path, std::ios::out);
	file_feature.close();
	file_label.open(label_path, std::ios::out);
	file_label.close();
	flie_check.open(check_path, std::ios::out);
	flie_check.close();
	int CheckCount = 0;

	for (int i = 0; i < FolderVector.size(); i++)
	{
#ifdef _WIN32
		char Path_find[100];
		std::string Path_find_string = "./" + InputFolder + "/" + FolderVector[i] + "/*.jpg";
		strcpy(Path_find, Path_find_string.c_str());
		struct _finddata_t C_file;
		intptr_t HFile;
		std::string Path_in;
		if ((HFile = _findfirst(Path_find, &C_file)) == -1L)
		{
			std::cout << "no file in " << Path_find << std::endl;
		}
		else
		{
			do
			{
				Path_in = C_file.name;
				std::string CheckPath = "./" + InputFolder + "/" + FolderVector[i] + "/" + Path_in;
				cv::Mat img = cv::imread("./" + InputFolder + "/" + FolderVector[i] + "/" + Path_in, 1);
				if (!img.empty())
				{
					if (GetFeature)
					{
						const float* feature_vector = classifier.Classify(img, layer);
						file_feature.open(feature_path, ios::app);
						for (int j = 0; j < SIZE; j++) {
							file_feature << feature_vector[j] << "\t";
						}
						file_feature << "\n";
						file_feature.close();

						file_label.open(label_path, ios::app);
						file_label << i << "\n";
						file_label.close();
					}
					flie_check.open(check_path, ios::app);
					flie_check << CheckCount << "\t" << CheckPath << std::endl;
					flie_check.close();
					CheckCount++;
				}
			} while (_findnext(HFile, &C_file) == 0);
			_findclose(HFile);
		}
#else
		std::string path_find = "./" + InputFolder + "/" + FolderVector[i];
		DIR* dir;
		struct dirent * ptr;
		dir = opendir(path_find.c_str());
		if (dir != NULL)
		{
			while ((ptr = readdir(dir)) != NULL)
			{
				std::string path_in(ptr->d_name);
				if (path_in == "." || path_in == "..")
				{
					//
				}
				else
				{
					std::string CheckPath="./" + InputFolder + "/" + FolderVector[i] + "/" + path_in;
					cv::Mat img = cv::imread("./" + InputFolder + "/" + FolderVector[i] + "/" + path_in, 1);
					if (!img.empty())
					{
						if (GetFeature)
						{
							std::vector<float> feature_vector = facialcaffe.Predict(img);
							file_feature.open(feature_path, std::ios::app);
							for (int j = 0; j < SIZE; j++)
							{
								file_feature << feature_vector[j] << "\t";
							}
							file_feature << "\n";
							file_feature.close();

							file_label.open(label_path, std::ios::app);
							file_label << i << "\n";
							file_label.close();
						}
						flie_check.open(check_path, std::ios::app);
						flie_check << CheckCount << "\t" << CheckPath << std::endl;
						flie_check.close();
                        CheckCount++;
					}
				}
			}
		}
		closedir(dir);
#endif

	}

	std::cout << "complete" << std::endl;
	return 0;
}



/*
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
//#include <caffe/caffe.hpp>
#include "features.h"
using namespace std;
using namespace cv;
//using namespace caffe;

int main(int argc, char **argv)
{
	cout << "Welcome to use XYZ Robot Recognition Engine." << endl;
	cout << "Initialize..." << endl;

	//if (argc != 5)
	//if (argc != 9)
	if (argc != 1)
	{
		std::cerr << "Usage: " << argv[0]
			<< " deploy.prototxt network.caffemodel"
			<< " mean.binaryproto labels.txt " << std::endl;
		return 1;
	}
	::google::InitGoogleLogging(argv[0]);
	//string model_file   = argv[1];         //deploy_gender.prototxt
	//string trained_file = argv[2];         //gender_net.caffemodel
	//string mean_file    = argv[3];         //mean.binaryproto
	//string label_file   = argv[4];         //labels.txt
	string model_file = "./model1/VGG_FACE_deploy.prototxt";         //deploy_gender.prototxt
	cout << "model_file setting down..." << endl;

	string trained_file = "./model1/VGG_FACE.caffemodel";         //gender_net.caffemodel
	cout << "weights_file setting down..." << endl;

	string mean_file = "./model1/mean.binaryproto";         //mean.binaryproto
	cout << "mean_file setting down..." << endl;

	string label_file = "./model1/names.txt";         //labels.txt
	cout << "label_file setting down..." << endl;

	Classifier classifier(model_file, trained_file, mean_file, label_file);

	cout << "DNN loading... over" << endl;
	//======================================================

	const int SIZE = 4096;

	Mat img = imread("ak.png", -1);
	Mat dstMat;
	resize(img, dstMat, Size(224, 224), 0, 0, CV_INTER_LINEAR);
	cout << "extracting feature" << endl;
	const float* feature_vector = classifier.Classify(dstMat);
	cout << "feature extracting finished" << endl;
	//for (int d=0; d<SIZE; d++){
	//cout<<"feature_vector["<<d<<"]="<<feature_vector[d]<<endl;
	//}
	cout << "ouput to txt" << endl;
	ofstream fout("test1.txt");
	if (!fout) {
		cout << "無法寫入檔案\n";
		return 1;
	}

	for (int i = 0; i<SIZE; i++){
		fout << feature_vector[i] << "\t";
	}
	fout << endl;
	cout << "finish output" << endl;
	return 0;
}


*/




















