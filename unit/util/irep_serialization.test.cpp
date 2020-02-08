/*******************************************************************
 Module: irep serialization unit test

 Author: Rafael Sá Menezes

 Date: February 2020

 Test Plan:
   - Constructors
   - Conversions
 \*******************************************************************/

#define BOOST_TEST_MODULE "irep Serialization"

#include <util/irep_serialization.h>
#include <util/irep2.h>
#include <util/irep2_type.h>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/framework.hpp>

namespace btf = boost::unit_test::framework;
// THIS APPROACH IS NOT THREAD SAFE!!!!
void write_serialization(
  irep_serializable &original,
  const std::string &file_name)
{
  std::ofstream ofs(file_name, std::ofstream::binary);
  original.serialize(ofs);
}

std::unique_ptr<std::istream> load_serialization(const std::string &file_name)
{
  auto ptr = std::make_unique<std::ifstream>(file_name, std::ifstream::binary);
  return std::move(ptr);
}

// ******************** TESTS ********************

// ** type deduction
// Check whether the object is reconstructed with correct type

BOOST_AUTO_TEST_SUITE(type_deduction)

BOOST_AUTO_TEST_CASE(type2tc_deduction_ok)
{
  std::string file_name(
    btf::current_test_case().p_name);

  type2tc obj;
  write_serialization(obj, file_name);
  auto istream = load_serialization(file_name);

  std::shared_ptr<type2tc> ptr = type2tc::unserialize(*istream);
  // No exception should have been thrown
}

BOOST_AUTO_TEST_CASE(type2tc_deduction_fail)
{
  std::string file_name(
    btf::current_test_case().p_name);

  expr2tc obj;
  write_serialization(obj, file_name);
  auto istream = load_serialization(file_name);

  // This is an invalid cast
  BOOST_CHECK_THROW(
    type2tc::unserialize(*istream),
    std::bad_cast
    );
}

BOOST_AUTO_TEST_CASE(expr2tc_deduction_ok)
{
  std::string file_name(
    btf::current_test_case().p_name);

  expr2tc obj;
  write_serialization(obj, file_name);
  auto istream = load_serialization(file_name);

  std::shared_ptr<expr2tc> ptr = expr2tc::unserialize(*istream);
  // No exception should have been thrown
}

BOOST_AUTO_TEST_CASE(expr2tc_deduction_fail)
{
  std::string file_name(
    btf::current_test_case().p_name);

  type2tc obj;
  write_serialization(obj, file_name);
  auto istream = load_serialization(file_name);

  // This is an invalid cast
  BOOST_CHECK_THROW(
    expr2tc::unserialize(*istream),
    std::bad_cast
  );
}
BOOST_AUTO_TEST_SUITE_END();

// ** irep2_container
// Check whether the container is reconstructed correctly

BOOST_AUTO_TEST_SUITE(irep2_container)

BOOST_AUTO_TEST_SUITE_END();