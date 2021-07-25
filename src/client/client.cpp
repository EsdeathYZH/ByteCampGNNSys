#include "glog/logging.h"

int main(int argc, char **argv) {
  // Initialize Google’s logging library.
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;

  // ...
  LOG(INFO) << "Found " << 1 << " cookies";
  FLAGS_logtostderr = 0;
  LOG(INFO) << "Found " << 2 << " cookies";
  FLAGS_logtostderr = 1;
  LOG(INFO) << "Found " << 3 << " cookies";
    return 0;
}
