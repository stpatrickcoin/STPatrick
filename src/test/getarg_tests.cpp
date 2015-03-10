#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-STP");
    BOOST_CHECK(GetBoolArg("-STP"));
    BOOST_CHECK(GetBoolArg("-STP", false));
    BOOST_CHECK(GetBoolArg("-STP", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-STPo"));
    BOOST_CHECK(!GetBoolArg("-STPo", false));
    BOOST_CHECK(GetBoolArg("-STPo", true));

    ResetArgs("-STP=0");
    BOOST_CHECK(!GetBoolArg("-STP"));
    BOOST_CHECK(!GetBoolArg("-STP", false));
    BOOST_CHECK(!GetBoolArg("-STP", true));

    ResetArgs("-STP=1");
    BOOST_CHECK(GetBoolArg("-STP"));
    BOOST_CHECK(GetBoolArg("-STP", false));
    BOOST_CHECK(GetBoolArg("-STP", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noSTP");
    BOOST_CHECK(!GetBoolArg("-STP"));
    BOOST_CHECK(!GetBoolArg("-STP", false));
    BOOST_CHECK(!GetBoolArg("-STP", true));

    ResetArgs("-noSTP=1");
    BOOST_CHECK(!GetBoolArg("-STP"));
    BOOST_CHECK(!GetBoolArg("-STP", false));
    BOOST_CHECK(!GetBoolArg("-STP", true));

    ResetArgs("-STP -noSTP");  // -STP should win
    BOOST_CHECK(GetBoolArg("-STP"));
    BOOST_CHECK(GetBoolArg("-STP", false));
    BOOST_CHECK(GetBoolArg("-STP", true));

    ResetArgs("-STP=1 -noSTP=1");  // -STP should win
    BOOST_CHECK(GetBoolArg("-STP"));
    BOOST_CHECK(GetBoolArg("-STP", false));
    BOOST_CHECK(GetBoolArg("-STP", true));

    ResetArgs("-STP=0 -noSTP=0");  // -STP should win
    BOOST_CHECK(!GetBoolArg("-STP"));
    BOOST_CHECK(!GetBoolArg("-STP", false));
    BOOST_CHECK(!GetBoolArg("-STP", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--STP=1");
    BOOST_CHECK(GetBoolArg("-STP"));
    BOOST_CHECK(GetBoolArg("-STP", false));
    BOOST_CHECK(GetBoolArg("-STP", true));

    ResetArgs("--noSTP=1");
    BOOST_CHECK(!GetBoolArg("-STP"));
    BOOST_CHECK(!GetBoolArg("-STP", false));
    BOOST_CHECK(!GetBoolArg("-STP", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-STP", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-STP", "eleven"), "eleven");

    ResetArgs("-STP -bar");
    BOOST_CHECK_EQUAL(GetArg("-STP", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-STP", "eleven"), "");

    ResetArgs("-STP=");
    BOOST_CHECK_EQUAL(GetArg("-STP", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-STP", "eleven"), "");

    ResetArgs("-STP=11");
    BOOST_CHECK_EQUAL(GetArg("-STP", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-STP", "eleven"), "11");

    ResetArgs("-STP=eleven");
    BOOST_CHECK_EQUAL(GetArg("-STP", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-STP", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-STP", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-STP", 0), 0);

    ResetArgs("-STP -bar");
    BOOST_CHECK_EQUAL(GetArg("-STP", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-STP=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-STP", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-STP=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-STP", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--STP");
    BOOST_CHECK_EQUAL(GetBoolArg("-STP"), true);

    ResetArgs("--STP=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-STP", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noSTP");
    BOOST_CHECK(!GetBoolArg("-STP"));
    BOOST_CHECK(!GetBoolArg("-STP", true));
    BOOST_CHECK(!GetBoolArg("-STP", false));

    ResetArgs("-noSTP=1");
    BOOST_CHECK(!GetBoolArg("-STP"));
    BOOST_CHECK(!GetBoolArg("-STP", true));
    BOOST_CHECK(!GetBoolArg("-STP", false));

    ResetArgs("-noSTP=0");
    BOOST_CHECK(GetBoolArg("-STP"));
    BOOST_CHECK(GetBoolArg("-STP", true));
    BOOST_CHECK(GetBoolArg("-STP", false));

    ResetArgs("-STP --noSTP");
    BOOST_CHECK(GetBoolArg("-STP"));

    ResetArgs("-noSTP -STP"); // STP always wins:
    BOOST_CHECK(GetBoolArg("-STP"));
}

BOOST_AUTO_TEST_SUITE_END()
