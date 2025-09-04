  - **Status:** Early development. Most of the planned features do not work,
    including even very basic things such as establishing connecitons

  - **Platforms:** Cross-platform, developed on Linux, will eventually
    receive testing on Windows.

Nuclex.ThinOrm.Native
======================

This library implements a "thin" ORM that grants you full control over how
queries will be constructed, avoiding convenience abstractions such as
active record, lazy-loading proxy collections or automatic cascading. It is
inspired by LinqToDb for .NET, though, as of now, it still requires you to
write out your SQL statements as strings.


Basic Setup
-----------

First, you need a `ConnectionFactory` which is what establishes new
database connections for you. The `QtSqlConnectionFactory` is available if
you enabled Qt support during build, otherwise, the
`DriverBasedconnectionFactory` should be your tool of choice:

```cpp
std::shared_ptr<DriverBasedConnectionFactory> connectionFactory = (
  std::make_shared<DriverBasedConnectionFactory>()
);

connectionFactory->RegisterAllBuiltInDrivers();
```

Notice that all these classes are designed to work well with dependency
injectors, so you usually have an interface (`ConnectionFactory` here)
and constructors only take dependencies (with optional configuration that
can also be changed after creation).

Next, you need to specify how to connect to your database. This can be done
through an ADO.NET-like connection string or a JDBC-like connection url:

```cpp
// Using a connection URL (JDBC-like)
ConnectionUrl connectionProperties = ConnectionUrl::Parse(
  u8"mariadb://johndoe:swordfish@localhost/ExampleDatabase
);

// Using a connection string (ADO.NET-like)
ConnectionString alternative = ConnectionString::Parse(
  u8"Driver=mariadb; Host=localhost; Database=ExampleDatabase; "
  u8"User=johndoe; Password=swordfish"
);
```

The "driver" part depends entirely on the connection factory. For the
`QtSqlConnectionFactory`, you can specify any of the drivers provided by your
Qt SQL library (i.e. "SQLITE") whilst the `DriverBasedConnectionFactory` has
a set of registered drivers (and you register custom ones, too).

Your `ConnectionFactory` can now create new connections to the database
using these connection properties:

```cpp
std::shared_ptr<Connection> databaseConnection = connectionFactory->Connect(
  connectionProperties // both ConnectionUrl and ConnectionString work here
);
```

Now you can already run raw queries on your database:

```cpp
Query testQuery(u8"SELECT Name, Birthday FROM Users");

std::shared_ptr<DataReader> reader = (
  databaseConnection->RunQueryWithResults(testQuery);
);

while(reader->MoveToNextResult()) {
  std::u8string name = reader->GetStringColumn(u8"Name");
  std::tm birthday = reader->GetDateTimeColumn(u8"Birthday");
}
```

Note two things here: first, the `Query` is an independent class (i.e. you
just create it rather than asking the database connection to set it up),
and second, the `DataReader` instead is created by the `Connection` class
but remains independent of the `Query`.

When a `Query` is executed on the connection, the connection *can* prepare
the query on the actual database connection and retain it for repeated runs
inside the `Query` instance. So you are free to use the `Query` for ad-hoc
queries, but you could also define your queries in a central place with
the added bonus them becoming faster after having been run once.

So that's the most basic way to use the library to run raw SQL queries.
Parameter binding is supported, of course and you can add
a `StandardConnectionPool` to the mix for situations where multiple queries
may be running simultanenously or where `DataReader`s need to be interleaved.
