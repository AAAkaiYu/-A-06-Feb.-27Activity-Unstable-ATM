#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif

#include "atm.hpp"
#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helper Definitions //
/////////////////////////////////////////////////////////////////////////////////////////////

bool CompareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1);
  std::ifstream f2(p2);

  if (f1.fail() || f2.fail()) {
    return false;  // file problem
  }

  std::string f1_read;
  std::string f2_read;
  while (f1.good() || f2.good()) {
    f1 >> f1_read;
    f2 >> f2_read;
    if (f1_read != f2_read || (f1.good() && !f2.good()) ||
        (!f1.good() && f2.good()))
      return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Test Cases
/////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Example: Create a new account", "[ex-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto accounts = atm.GetAccounts();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);

  Account sam_account = accounts[{12345678, 1234}];
  REQUIRE(sam_account.owner_name == "Sam Sepiol");
  REQUIRE(sam_account.balance == 300.30);

  auto transactions = atm.GetTransactions();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);
  std::vector<std::string> empty;
  REQUIRE(transactions[{12345678, 1234}] == empty);
}

TEST_CASE("Example: Simple widthdraw", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.WithdrawCash(12345678, 1234, 20);
  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];

  REQUIRE(sam_account.balance == 280.30);
}

TEST_CASE("Example: Print Prompt Ledger", "[ex-3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto& transactions = atm.GetTransactions();
  transactions[{12345678, 1234}].push_back(
      "Withdrawal - Amount: $200.40, Updated Balance: $99.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $40000.00, Updated Balance: $40099.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $32000.00, Updated Balance: $72099.90");
  atm.PrintLedger("./prompt.txt", 12345678, 1234);
  REQUIRE(CompareFiles("./ex-1.txt", "./prompt.txt"));
}
TEST_CASE("not enough deposit", "[ex-4]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 100.00);
  REQUIRE_THROWS_AS(atm.WithdrawCash(12345678, 1234, 12345690),
                    std::runtime_error);
  double balance = atm.CheckBalance(12345678, 1234);
  REQUIRE(balance == 100.00);
}
TEST_CASE("withdraw negative", "[ex-5]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 100.00);
  REQUIRE_THROWS_AS(atm.WithdrawCash(12345678, 1234, -50.0),
                    std::invalid_argument);
}
TEST_CASE("deposit negative", "[ex-6]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 100.00);
  REQUIRE_THROWS_AS(atm.DepositCash(12345678, 1234, -50.0),
                    std::invalid_argument);
}
TEST_CASE("deposit for user do not exist", "[ex-7]") {
  Atm atm;
  REQUIRE_THROWS_AS(atm.DepositCash(12345678, 1234, -50.0),
                    std::invalid_argument);
}
TEST_CASE("user already exist", "[ex-8]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 100.00);
  REQUIRE_THROWS_AS(atm.RegisterAccount(12345678, 1234, "another name", 100.00),
                    std::invalid_argument);
}
TEST_CASE("printing a ledger for an account that doesn't exist", "[ex-9]") {
  Atm atm;
  REQUIRE_THROWS_AS(atm.PrintLedger("test.txt", 99999999, 9999),
                    std::invalid_argument);
}
TEST_CASE("deposit appear as expected", "[ex-10]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 100.00);
  atm.DepositCash(12345678, 1234, 100.00);
  double balance = atm.CheckBalance(12345678, 1234);
  REQUIRE(balance == 200.00);
}