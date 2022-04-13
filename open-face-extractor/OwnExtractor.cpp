///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017, Carnegie Mellon University and University of Cambridge,
// all rights reserved.
//
// ACADEMIC OR NON-PROFIT ORGANIZATION NONCOMMERCIAL RESEARCH USE ONLY
//
// BY USING OR DOWNLOADING THE SOFTWARE, YOU ARE AGREEING TO THE TERMS OF THIS LICENSE AGREEMENT.  
// IF YOU DO NOT AGREE WITH THESE TERMS, YOU MAY NOT USE OR DOWNLOAD THE SOFTWARE.
//
// License can be found in OpenFace-license.txt

//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite at least one of the following works:
//
//       OpenFace 2.0: Facial Behavior Analysis Toolkit
//       Tadas Baltru�aitis, Amir Zadeh, Yao Chong Lim, and Louis-Philippe Morency
//       in IEEE International Conference on Automatic Face and Gesture Recognition, 2018  
//
//       Convolutional experts constrained local model for facial landmark detection.
//       A. Zadeh, T. Baltru�aitis, and Louis-Philippe Morency,
//       in Computer Vision and Pattern Recognition Workshops, 2017.    
//
//       Rendering of Eyes for Eye-Shape Registration and Gaze Estimation
//       Erroll Wood, Tadas Baltru�aitis, Xucong Zhang, Yusuke Sugano, Peter Robinson, and Andreas Bulling 
//       in IEEE International. Conference on Computer Vision (ICCV),  2015 
//
//       Cross-dataset learning and person-specific normalisation for automatic Action Unit detection
//       Tadas Baltru�aitis, Marwa Mahmoud, and Peter Robinson 
//       in Facial Expression Recognition and Analysis Challenge, 
//       IEEE International Conference on Automatic Face and Gesture Recognition, 2015 
//
///////////////////////////////////////////////////////////////////////////////
// FaceLandmarkImg.cpp : Defines the entry point for the console application for detecting landmarks in images.

// dlib
#include <dlib/image_processing/frontal_face_detector.h>

#include "LandmarkCoreIncludes.h"

#include <FaceAnalyser.h>
#include <GazeEstimation.h>

#include <ImageCapture.h>
#include <Visualizer.h>
#include <VisualizationUtils.h>
#include <RecorderOpenFace.h>
#include <RecorderOpenFaceParameters.h>

//zmq
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <zmq.hpp>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "base64.h"

#include <opencv2/imgproc/imgproc.hpp>



#ifndef CONFIG_DIR
#define CONFIG_DIR "~"
#endif

using namespace std;

std::vector<std::string> get_arguments(int argc, char **argv)
{

	std::vector<std::string> arguments;

	for (int i = 0; i < argc; ++i)
	{
		arguments.push_back(std::string(argv[i]));
	}
	return arguments;
}

string convertToJSON(vector<pair<string, double>> intensity, vector<pair<string, double>> presence){
		std::string json = "{ \"intensity\":  {";

		for(int i = 0; i < intensity.size(); i++){
			json = json + "\"" + intensity[i].first + "\":" + to_string(intensity[i].second);

			if(i < intensity.size()-1){
				json = json + ",";
			}
		}

		json = json + "}, \"presence\": {";

		for(int i = 0; i < presence.size(); i++){
			json = json + "\"" + presence[i].first + "\":" + to_string(presence[i].second);

			if(i < presence.size()-1){
				json = json + ",";
			}
		}

		json = json + "}}";

		return json;
}

int main(int argc, char **argv)
{
	//Convert arguments to more convenient vector form
	std::vector<std::string> arguments = get_arguments(argc, argv);

	// Load the models
	LandmarkDetector::FaceModelParameters det_parameters(arguments);

	// The modules that are being used for tracking
	std::cout << "Loading landmark model" << std::endl;
	LandmarkDetector::CLNF face_model(det_parameters.model_location);

	if (!face_model.loaded_successfully)
	{
		std::cout << "ERROR: Could not load the landmark detector" << std::endl;
		return 1;
	}

	std::cout << "Loading facial feature extractor" << std::endl;
	FaceAnalysis::FaceAnalyserParameters face_analysis_params(arguments);
	face_analysis_params.OptimizeForImages();
	FaceAnalysis::FaceAnalyser face_analyser(face_analysis_params);

	std::cout << "Everything loaded" << std::endl;

	// ZMQ preparation
	zmq::context_t ctx;
    zmq::socket_t sock(ctx, ZMQ_REP);
    sock.connect("tcp://localhost:5555");

	while(true){
		// wait for image
		std::cout << "Waiting for image..." << std::endl;
		zmq::message_t request;
		sock.recv (request, zmq::recv_flags::none);
		std::string rpl = std::string(static_cast<char*>(request.data()), request.size());
		//std::cout << rpl << std::endl;

		// decode image
		std::string dec_jpg =  base64_decode(rpl);
		std::vector<uchar> data(dec_jpg.begin(), dec_jpg.end());
		cv::Mat rgb_image = cv::imdecode(cv::Mat(data), 1);
		cv::Mat greyScale_image;
		cv::cvtColor(rgb_image, greyScale_image, cv::COLOR_BGR2GRAY);
		std::cout << "Image received" << std::endl;

		// cv::imshow("some", rgb_image);
		// cv::waitKey();
		

		// results will be stored in face_model
		LandmarkDetector::DetectLandmarksInImage(rgb_image, face_model, det_parameters, greyScale_image);
		face_analyser.PredictStaticAUsAndComputeFeatures(rgb_image, face_model.detected_landmarks);

		auto aus_intensity = face_analyser.GetCurrentAUsReg();
		auto aus_presence = face_analyser.GetCurrentAUsClass();

		string json = convertToJSON(aus_intensity, aus_presence);

		std::cout << json << std::endl;

		//  Send reply back to client
		zmq::message_t reply (json.length());
		memcpy (reply.data (), json.c_str(), json.length());
		sock.send (reply, zmq::send_flags::none);

		std::cout << "Reply sent" << std::endl;

		//Show image
		// for(int j = 0; j < face_model.detected_landmarks.rows / 2; j++){
		// 	float x = face_model.detected_landmarks[j][0];
		// 	float y = face_model.detected_landmarks[j + face_model.detected_landmarks.rows / 2][0];
		// 	// std::cout << "(" << x << "," << y << ")" << std::endl;
		// 	cv::circle(rgb_image, cv::Point2f(x,y), 8, cv::Scalar(255,0,0), cv::FILLED, cv::LINE_8);
		// }

		// cv::imshow("some", rgb_image);
		// cv::waitKey();
	}

	return 0;
}

