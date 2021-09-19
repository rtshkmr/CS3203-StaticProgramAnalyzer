#include <util/TimeUtil.h>
#include <util/Logger.h>
#include <iostream>
#include <model/Entity.h>
#include <filesystem>
#include "catch.hpp"

TEST_CASE("0.Utils.Testing time util") {
  std::string date_time = TimeUtil::GetDateTimeStr();
  std::string file_name_date_time = TimeUtil::GetDateTimeFileNameStr();
  std::cout << ("date time is: " + date_time + "\n");
  std::cout << ("file name version of date time is:" + file_name_date_time + "\n");
  int x = 1;
}

TEST_CASE("0.Testing basic LOG function") {
  LoggerSettings logger_settings;
  logger_settings.can_overwrite_file_ = true;
  logger_settings.out_to_stdout_ = true;
  logger_settings.out_to_file_ = true;

  LoggerInit(logger_settings);
  LOG
  (spa_logger << "sup boisss i'm a message" << std::endl);
  auto* dummy_proc = new Procedure(new ProcedureName("SexyProc"));
  LOG
  (spa_logger << "sup boiss i'm a procedure newww: " << (* dummy_proc).GetName());
  LoggerTerminate();
  int x = 1;
}

