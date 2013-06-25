
#include <gtest/gtest.h>
#include <sqlitelib.h>

using namespace std;
using namespace sqlitelib;

class SqliteTest : public ::testing::Test
{
protected:
    SqliteTest() : db_("./test.db") {}

    virtual void SetUp()
	{
		EXPECT_EQ(true, db_.is_open());
	}

    Sqlite db_;
};

TEST_F(SqliteTest, ExecuteInt)
{
	auto sql = "SELECT age FROM people WHERE name='john'";
	auto val = db_.prepare(sql).execute_value<Int>();
	EXPECT_EQ(10, val);
}

TEST_F(SqliteTest, ExecuteText)
{
	auto sql = "SELECT name FROM people WHERE name='john'";
	auto val = db_.prepare(sql).execute_value<Text>();
	EXPECT_EQ("john", val);
}

TEST_F(SqliteTest, ExecuteBlob)
{
	auto sql = "SELECT data FROM people WHERE name='john'";
	auto val = db_.prepare(sql).execute_value<Blob>();
	EXPECT_EQ(4, val.size());
	EXPECT_EQ('A', val[0]);
	EXPECT_EQ('D', val[3]);
}

TEST_F(SqliteTest, ExecuteIntAndText)
{
	auto sql = "SELECT age, name FROM people";

	auto rows = db_.prepare(sql).execute<Int, Text>();
	EXPECT_EQ(4, rows.size());

	auto row = rows[3];
	EXPECT_EQ(25, std::get<0>(row));
	EXPECT_EQ("luke", std::get<1>(row));
}

TEST_F(SqliteTest, Bind)
{
	{
		auto sql = "SELECT name FROM people WHERE age>?";
		auto rows = db_.prepare(sql).bind(10).execute<Text>();
		EXPECT_EQ(rows.size(), 3); 
		EXPECT_EQ("paul", rows[0]);
	}

	{
		auto sql = "SELECT age FROM people WHERE name LIKE ?";
		auto val = db_.prepare(sql).bind("jo%").execute_value<Int>();
		EXPECT_EQ(10, val);
	}

	{
		auto sql = "SELECT id FROM people WHERE name=? AND age=?";
		auto val = db_.prepare(sql).bind("john", 10).execute_value<Int>();
		EXPECT_EQ(1, val);
	}
}

TEST_F(SqliteTest, ReusePreparedStatement)
{
	{
		auto stmt = db_.prepare("SELECT name FROM people WHERE age>?");
		auto rows = stmt.bind(10).execute<Text>();
		EXPECT_EQ(rows.size(), 3); 
		EXPECT_EQ("paul", rows[0]);

		rows = stmt.bind(20).execute<Text>();
		EXPECT_EQ(rows.size(), 1); 
		EXPECT_EQ("luke", rows[0]);
	}
}

// vim: et ts=4 sw=4 cin cino={1s ff=unix
