#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/config.h"
#include "libcbdetect/find_corners.h"
#include "libcbdetect/plot_boards.h"
#include "libcbdetect/plot_corners.h"
#include <chrono>
#include <opencv2/opencv.hpp>
#include "json/json.h"
#include <vector>
#include <string>
#include <fstream>

using namespace std::chrono;

struct ImageDetectionConfig {
  std::vector<std::string> imageFilePaths;
  std::string outputFilePath;
};

struct ImageCornerDetectionResult {
  std::string imageFilePath;
  cbdetect::Corner corners;
  std::vector<cbdetect::Board> boards;
};

void save_board_as_json(std::vector<cbdetect::Board> &boards, cbdetect::Corner &corners, const char* outputFileName);
void readJsonFile(const char* jsonFilePath, Json::Value &jsonData);
void get_detection_result_as_json(ImageCornerDetectionResult &imageResult, Json::Value &root);
void format_detection_result_as_json(std::vector<cbdetect::Board> &boards, cbdetect::Corner &corners, Json::Value &root);

ImageCornerDetectionResult detect(const char* str, cbdetect::CornerType corner_type, const char* outputFilePath) {
  cbdetect::Corner corners;
  std::vector<cbdetect::Board> boards;
  cbdetect::Params params;
  params.corner_type = corner_type;

  cv::Mat img = cv::imread(str, cv::IMREAD_COLOR);
  // printf("reached here");
  // auto t1 = high_resolution_clock::now();
  cbdetect::find_corners(img, corners, params);
  // auto t2 = high_resolution_clock::now();
  // cbdetect::plot_corners(img, corners);
  // auto t3 = high_resolution_clock::now();
  cbdetect::boards_from_corners(img, corners, boards, params);
  // auto t4 = high_resolution_clock::now();
  // printf("Find corners took: %.3f ms\n", duration_cast<microseconds>(t2 - t1).count() / 1000.0);
  // printf("Find boards took: %.3f ms\n", duration_cast<microseconds>(t4 - t3).count() / 1000.0);
  // printf("Total took: %.3f ms\n", duration_cast<microseconds>(t2 - t1).count() / 1000.0 + duration_cast<microseconds>(t4 - t3).count() / 1000.0);
  // cbdetect::plot_boards(img, corners, boards, params);
  // save_board_as_json(boards, corners, outputFilePath);
  return { str, corners, boards };
}

void get_detection_result_as_json(ImageCornerDetectionResult &imageResult, Json::Value &root) {
  root["file_name"] = imageResult.imageFilePath;
  format_detection_result_as_json(imageResult.boards, imageResult.corners, root);
}

void format_detection_result_as_json(std::vector<cbdetect::Board> &boards, cbdetect::Corner &corners, Json::Value &root) {
  Json::Value _boards(Json::arrayValue);
  std::sort(boards.begin(), boards.end(),
    [](const cbdetect::Board& b1, const cbdetect::Board& b2) {
      return b1.num > b2.num;
    });
  for (cbdetect::Board &board: boards) {
    Json::Value _board(Json::arrayValue);
    for (std::vector<int> &indexes: board.idx) {
      Json::Value _boardCornerIndexes(Json::arrayValue);
      for (int val: indexes) {
        _boardCornerIndexes.append(Json::Value(val));
      }
      _board.append(_boardCornerIndexes);
    }
    _boards.append(_board);
  }
  Json::Value _points(Json::arrayValue);
  for (cv::Point2d& point: corners.p) {
    Json::Value _point(Json::arrayValue);
    _point.append(point.x);
    _point.append(point.y);
    _points.append(_point);
  }
  root["board_corner_indexes"] = _boards;
  root["detected_corner_points"] = _points;
}

void create_image_detection_result_json(std::vector<ImageCornerDetectionResult> &results, Json::Value &root) {
  for (auto& imageCornerDetectionResult: results) {
    Json::Value _result;
    get_detection_result_as_json(imageCornerDetectionResult, _result);
    root.append(_result);
  }
}

void save_image_detection_result_json(Json::Value &root, const char* outputFileName) {
  Json::FastWriter fastWriter;
  std::string jsonString = fastWriter.write(root);
  std::ofstream outdata;
  outdata.open(outputFileName);
  if (!outdata) {
    std::cout << "Output file opening failed!" << std::endl;
    exit(1);
  }
  outdata << jsonString;
  outdata.close();
}

void save_board_as_json(std::vector<cbdetect::Board> &boards, cbdetect::Corner &corners, const char* outputFileName) {
  Json::Value root;
  format_detection_result_as_json(boards, corners, root);
  Json::FastWriter fastWriter;
  std::string jsonString = fastWriter.write(root);
  // std::cout << jsonString << std::endl;
  std::ofstream outdata;
  outdata.open(outputFileName);
  if (!outdata) {
    std::cout << "Output file opening failed!" << std::endl;
    exit(1);
  }
  outdata << jsonString;
  outdata.close();
}

void readJsonFile(const char* jsonFilePath, Json::Value &jsonData) {
  std::ifstream file(jsonFilePath);
  file >> jsonData;
}

void run_process(const char* configFilePath, const char* outputFilePath) {
  Json::Value filesConfig;
  readJsonFile(configFilePath, filesConfig);
  Json::Value filesList = filesConfig["files"];
  Json::Value output(Json::arrayValue);
  std::vector<ImageCornerDetectionResult> cornersResults;
  for (Json::Value filePath: filesList) {
    ImageCornerDetectionResult res = detect(filePath.asCString(), cbdetect::SaddlePoint, "random_file");
    cornersResults.emplace_back(res);
  }
  create_image_detection_result_json(cornersResults, output);
  save_image_detection_result_json(output, outputFilePath);
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Invalid Usage" << std::endl;
    exit(1);
  }
  char* configFilePath = argv[1];
  char* outputFilePath = argv[2];
  run_process(configFilePath, outputFilePath);
  return 0;
}

// int main(int argc, char* argv[]) {
//   // printf("chessboards...");
//   if (argc < 3) {
//     std::cout << "Invalid Usage" << std::endl;
//     return 1;
//   }
//   char* imageFilePath = argv[1];
//   char* outputFilePath = argv[2];
//   detect(imageFilePath, cbdetect::SaddlePoint, outputFilePath);
//   // printf("deltilles...");
//   // detect("../../example_data/e6.png", cbdetect::MonkeySaddlePoint);
//   return 0;
// }
