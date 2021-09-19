#include "TestWrapper.h"
#include "component/SourceProcessor/SourceProcessor.h"
#include <component/QueryProcessor/QuerySystemController.h>
#include <sstream>
#include <util/Logger.h>

constexpr auto L = [](auto msg){
  LOG(spa_logger << Logger::Prettify(msg));
};

void Setup() {
  LoggerSettings logger_settings;
  logger_settings.can_overwrite_file_ = true;
  logger_settings.out_to_stdout_ = true;
  logger_settings.out_to_file_ = true;
  LoggerInit(logger_settings);
}

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
  Setup();
  L("[BEFORE] STARTING RUN");
  // create any objects here as instance variables of this class
  // as well as any initialization required for your spa program
  pkb = new PKB();
  L("... constructed TestWrapper");
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string file_name) {
  L("... Source Processor will process source file to create pkb\n");
  sp::SourceProcessor source_processor;
  pkb = sp::SourceProcessor::ProcessSourceFile(file_name);
  L("[DONE] FRONT END COMPLETED WITHOUT ERROR");

}

std::vector<std::string> split(std::string const& input) {
  std::istringstream buffer(input);
  std::vector<std::string> ret;

  std::copy(std::istream_iterator<std::string>(buffer),
            std::istream_iterator<std::string>(),
            std::back_inserter(ret));
  return ret;
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
  L("[BEFORE] EVALUATING  QUERY ");
// call your evaluator to evaluate the query here
  std::optional<std::string> s = QuerySystemController::Evaluate(& query, pkb);
  L("... Query System controller has evaluated the query");
  if (s) {
    std::vector<std::string> res_vec = split(s.value());

    for (std::string s : res_vec) {
      results.push_back(s);
    }
  }
  L("EVALUATED QUERY");
}


