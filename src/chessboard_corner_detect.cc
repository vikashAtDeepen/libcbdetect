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

void save_board_as_json(std::vector<cbdetect::Board> &boards, cbdetect::Corner &corners, const char* outputFileName);

void detect(const char* str, cbdetect::CornerType corner_type, const char* outputFilePath) {
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
  save_board_as_json(boards, corners, outputFilePath);
}

void save_board_as_json(std::vector<cbdetect::Board> &boards, cbdetect::Corner &corners, const char* outputFileName) {
  Json::Value root;
  Json::Value _boards(Json::arrayValue);
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

int main(int argc, char* argv[]) {
  // printf("chessboards...");
  if (argc < 3) {
    std::cout << "Invalid Usage" << std::endl;
    return 1;
  }
  char* imageFilePath = argv[1];
  char* outputFilePath = argv[2];
  detect(imageFilePath, cbdetect::SaddlePoint, outputFilePath);
  // printf("deltilles...");
  // detect("../../example_data/e6.png", cbdetect::MonkeySaddlePoint);
  return 0;
}
