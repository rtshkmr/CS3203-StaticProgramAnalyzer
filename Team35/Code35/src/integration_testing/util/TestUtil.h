#ifndef AUTOTESTER_CODE35_SRC_INTEGRATION_TESTING_UTIL_TESTUTIL_H_
#define AUTOTESTER_CODE35_SRC_INTEGRATION_TESTING_UTIL_TESTUTIL_H_

namespace test_util {
std::vector<std::tuple<EntityEnum, std::string>> basic_source_tuples = {
    std::make_tuple(EntityEnum::kAssignEntity, "-1000000"), // placeholder
    std::make_tuple(EntityEnum::kReadEntity, "1"),
    std::make_tuple(EntityEnum::kPrintEntity, "2"),
    std::make_tuple(EntityEnum::kAssignEntity, "3"),
    std::make_tuple(EntityEnum::kAssignEntity, "4"),
    std::make_tuple(EntityEnum::kAssignEntity, "5")
};

std::vector<std::tuple<EntityEnum, std::string>> ml_source_tuples = {
    std::make_tuple(EntityEnum::kAssignEntity, "-1000000"), // placeholder
    std::make_tuple(EntityEnum::kAssignEntity, "1"),
    std::make_tuple(EntityEnum::kReadEntity, "2"),
    std::make_tuple(EntityEnum::kIfEntity, "3"),
    std::make_tuple(EntityEnum::kAssignEntity, "4"),
    std::make_tuple(EntityEnum::kAssignEntity, "5"),
    std::make_tuple(EntityEnum::kWhileEntity, "6"),
    std::make_tuple(EntityEnum::kReadEntity, "7"),
    std::make_tuple(EntityEnum::kIfEntity, "8"),
    std::make_tuple(EntityEnum::kAssignEntity, "9"),
    std::make_tuple(EntityEnum::kAssignEntity, "10"),
    std::make_tuple(EntityEnum::kIfEntity, "11"),
    std::make_tuple(EntityEnum::kAssignEntity, "12"),
    std::make_tuple(EntityEnum::kPrintEntity, "13"),
    std::make_tuple(EntityEnum::kAssignEntity, "14"),
    std::make_tuple(EntityEnum::kPrintEntity, "15"),
    std::make_tuple(EntityEnum::kAssignEntity, "16"),
    std::make_tuple(EntityEnum::kPrintEntity, "17"),
    std::make_tuple(EntityEnum::kPrintEntity, "18"),
    std::make_tuple(EntityEnum::kIfEntity, "19"),
    std::make_tuple(EntityEnum::kPrintEntity, "20"),
    std::make_tuple(EntityEnum::kPrintEntity, "21"),
    std::make_tuple(EntityEnum::kPrintEntity, "22"),
    std::make_tuple(EntityEnum::kAssignEntity, "23")
};

std::vector<std::tuple<EntityEnum, std::string>> ml2_source_tuples = {
    {EntityEnum::kAssignEntity, "-1000000"}, // placeholder
    {EntityEnum::kAssignEntity, "1"},
    {EntityEnum::kReadEntity, "2"},
    {EntityEnum::kIfEntity, "3"},
    {EntityEnum::kAssignEntity, "4"},
    {EntityEnum::kAssignEntity, "5"},
    {EntityEnum::kWhileEntity, "6"},
    {EntityEnum::kWhileEntity, "7"},
    {EntityEnum::kReadEntity, "8"},
    {EntityEnum::kIfEntity, "9"},
    {EntityEnum::kAssignEntity, "10"},
    {EntityEnum::kAssignEntity, "11"},
    {EntityEnum::kIfEntity, "12"},
    {EntityEnum::kAssignEntity, "13"},
    {EntityEnum::kPrintEntity, "14"},
    {EntityEnum::kAssignEntity, "15"},
    {EntityEnum::kPrintEntity, "16"},
    {EntityEnum::kAssignEntity, "17"},
    {EntityEnum::kPrintEntity, "18"},
    {EntityEnum::kIfEntity, "19"},
    {EntityEnum::kPrintEntity, "20"},
    {EntityEnum::kPrintEntity, "21"},
    {EntityEnum::kPrintEntity, "22"},
    {EntityEnum::kAssignEntity, "23"}
};
}
#endif //AUTOTESTER_CODE35_SRC_INTEGRATION_TESTING_UTIL_TESTUTIL_H_
