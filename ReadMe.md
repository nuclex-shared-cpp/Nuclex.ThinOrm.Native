Nuclex.ThinOrm.Native ![Developed on Linux, should work on Windows](./Documents/images/platforms-linux-windows-badge.svg) ![Design is not final and some parts of the code are still work in progress](./Documents/images/status-design-still-settling-badge.svg)
======================

This is a C++20 library that provides an
[ADO.NET](https://learn.microsoft.com/en-us/dotnet/framework/data/adonet/retrieving-and-modifying-data)-like
database abstraction layer that you can use to open database connections,
pool connections and run queries with bound parameters.

On top of this layer, a "thin" ORM inspired by
[LinqToDb](https://linq2db.github.io/) lets you construct queries in type-safe
C++20 code. It avoids high-level abstractions such as lazy-loading proxy
collections, active record or automatic cascading.

There's also a schema migration system inspired by .NET's
[Fluent Migrator](https://github.com/fluentmigrator/fluentmigrator) where you
implement each schema migrations as a class with an `Up()` and, optionally,
a `Down()` method and your database can be automatically up- and downgraded to
any schema version you want.

The entire library is designed to be usable in a dependency injection
environment (i.e. [Google Fruit](https://github.com/google/fruit)) and uses
unit tests to verify its functionality.


Basic Setup
-----------

First, you need a `ConnectionFactory` which is what establishes new
database connections for you. The `QtSqlConnectionFactory` is available if
you enabled Qt support during build, otherwise, the
`DriverBasedconnectionFactory` should be your tool of choice:

```cpp
auto connectionFactory = std::make_shared<DriverBasedConnectionFactory>();
connectionFactory->RegisterAllBuiltInDrivers();
```

Notice that all classes are designed to work well with dependency injectors,
so you usually have an interface (`ConnectionFactory` here) and constructors
only take dependencies (and any configuration options start with sensible
defaults that can also be changed after creation).

Next, you need to specify how to connect to your database. This can be done
through an ADO.NET-like connection string or a JDBC-like connection url:

```cpp
// Using a connection URL (JDBC-like)
ConnectionUrl connectionProperties = ConnectionUrl::Parse(
  u8"mariadb://johndoe:swordfish@localhost/ExampleDatabase"
);

// Using a connection string (ADO.NET-like)
ConnectionString alternative = ConnectionString::Parse(
  u8"Driver=mariadb; Host=localhost; Database=ExampleDatabase; "
  u8"User=johndoe; Password=swordfish"
);
```

The "driver" part depends entirely on the connection factory. For the
`QtSqlConnectionFactory`, you can specify any of the drivers provided by your
Qt SQL library (i.e. "SQLITE") whilst in the `DriverBasedConnectionFactory`,
they pick amonst its registered drivers.

Your `ConnectionFactory` can now create new connections to the database
using these connection properties:

```cpp
std::shared_ptr<Connection> databaseConnection = connectionFactory->Connect(
  connectionProperties // both ConnectionUrl and ConnectionString work here
);
```

For good measure, instead of passing the connection properties around
everywhere and creating new connections all the time, you can use the built-in
`ConnectionPool` class:

```cpp
// Assumes a ProjectDataContext class exists which simply acts as a "key" to
// distinguish between databases in case your application uses more than one.
auto connectionPool = std::make_shared<
  Nuclex::ThinOrm::Connections::StandardConnectionPool<ProjectDataContext>
>(connectionFactory, connectionProperties);

// Now simply request a connection from the pool
{
  std::shared_ptr<Connection> databaseConnection = connectionPool->BorrowConnection();
  // ...do something with the connection...
  connectionPool->ReturnConnection(connection);
}
```


Schema Migrations
-----------------

In order to initialize the database schema of a new database, as well as when
a new version of your applications needs to modify the database schema, there
is a schema migration system inpspired by FluentMigrator for .NET.

First, you write a migration class:

```cpp
/// <summary>Sets up a settings table for simple project-wide settings</summary>
class M001_ProjectSettings : public Nuclex::ThinOrm::Migrations::Migration {

  /// <summary>Initializes the schema migration</summary>
  public: M001_ProjectSettings() : Migration(1, u8"Project Settings") {}

  /// <summary>Upgrades the database schema</summary>
  /// <param name="connection">
  ///   Database connection through which the schema can be upgraded
  /// </param>
  public: void Up(Nuclex::ThinOrm::Connections::Connection &connection) override;

  /// <summary>Downgrades the database schema</summary>
  /// <param name="connection">
  ///   Database connection through which the schema can be downgraded
  /// </param>
  public: void Down(Nuclex::ThinOrm::Connections::Connection &connection) override;

};

// ------------------------------------------------------------------------------------------- //

void M001_ProjectSettings::Up(Nuclex::ThinOrm::Connections::Connection &connection) {
  Nuclex::ThinOrm::Query createTableQuery(
    u8"CREATE TABLE Settings(\n"
    u8"  Name  VARCHAR(63) NOT NULL,\n"
    u8"  Value  VARCHAR(255) NULL,\n"
    u8"  PRIMARY KEY (Name)\n"
    u8")"
  );
  connection.RunStatement(createTableQuery);

  Nuclex::ThinOrm::Query createIndexQuery(
    u8"CREATE INDEX SettingByName ON Settings(Name)"
  );
  connection.RunStatement(createIndexQuery);
}

// ------------------------------------------------------------------------------------------- //

void M001_ProjectSettings::Down(Nuclex::ThinOrm::Connections::Connection &connection) {
  Nuclex::ThinOrm::Query dropIndexQuery(
    u8"DROP INDEX SettingByName\n"
  );
  connection.RunStatement(dropIndexQuery);

  Nuclex::ThinOrm::Query dropTableQuery(
    u8"DROP TABLE Settings\n"
  );
  connection.RunStatement(dropTableQuery);
}
```

Next you need the `MigrationRunner`. To make your migration class known to
the `MigrationRunner`, either register it explicitly or use an initterm class
to register it on the `GlobalMigrationRepository`. We'll demonstrate the first
approach here because it avoids initterms and global instances:

```cpp
// Create a temporary migration runner and ask it to upgrade the database schema
// to the highest schema version number
{
  Nuclex::ThinOrm::Migrations::MigrationRunner runner(connectionPool);
  runner.AddMigration<Database::Migrations::M001_ProjectSettings>();

  runner.UpgradeToLatestSchema();
}
```


Querying (string-based)
-----------------------

The `Query` class lets you build a query. It is not an interface but
a concrete class, so you could create a repository of pre-parsed queries
in one place. It's fine to use them in an ad-hoc fashion, too.

Here's an example:

```cpp
Query testQuery(u8"SELECT Name, Birthday FROM Users");

std::shared_ptr<RowReader> reader = (
  databaseConnection->RunRowQuery(testQuery);
);

while(reader->MoveToNextResult()) {
  std::u8string name = reader->GetColumnValue(u8"Name").AsString();
  DateTime birthday = reader->GetColumnValue(u8"Birthday").AsDate();

  // ...do something with the name and birth date...
}
```

The `DataReader` instead is created by the `Connection` class but remains
independent of the `Query`. When a `Query` is executed on the connection,
the connection *can* prepare the query on the actual database connection and
retain it for repeated runs inside the `Query` instance. So if you keep your
queries around, they have a very good chance of coming to executing faster
when they are used next.

So that's the most basic way to use the library to run raw SQL queries.
Parameter binding is supported, of course and you can add
a `StandardConnectionPool` to the mix for situations where multiple queries
may be running simultanenously or where `DataReader`s need to be interleaved.


Querying (fluent)
-----------------

This feature is not ready yet. The fluent query system lets you register your
entity classes and then allows you to write queries within C++20 with full
type safety and protected from typos:

```cpp
// This may become the second global, following GlobalMigrationRepository
GlobalEntityRegistry r;

r.RegisterTable<TestEntity>(u8"users").
  WithColumn<&TestEntity::Id>(u8"id").NotNull().AutoGenerated().PrimaryKey().
  WithColumn<&TestEntity::Name>(u8"name").NotNull().
  WithColumn<&TestEntity::PasswordHash>(u8"passwordHash");
```

Then, in your `DataContext` class, you expose your tables with their entity
type to allow their usage in fluent queries:

```cpp
/// <summary>Sets up a settings table for simple project-wide settings</summary>
class ProjectDataContext : public Nuclex::ThinOrm::DataContext {

  /// <summary>
  ///   Initializes a new project data context using the specified conection pool to
  ///   access its database
  /// </summary>
  /// <param name="pool">Pool from which connections will be borrowed as needed</param>
  public: ProjectDataContext(
    const std::shared_ptr<Connections::ConnectionPool> &pool
  );

  /// <summary>Users that are allowed to view the project</summary>
  public: Table<User> Users;

};

// ------------------------------------------------------------------------------------------- //

void ProjectDataContext::ProjectDataContext(
  const std::shared_ptr<Connections::ConnectionPool> &pool
) :
  DataContext(pool),
  Users(*this) {}
```

Then, either follow the popular "repository" pattern where each repository
holds on to a reference of the `ProjectDataContext` or use your data context
directly, like this:

```cpp
std::optional<User> johnUser = myProjectDataContext.Users.
  Where(Column(&User::Name) == u8"John")).
  SingleOrDefault();
```

The query, honoring the connected database's SQL dialect peculiarities, will
then be constructed and executed for you. Note the complete absence of any
SQL strings or manual named parameter management.

Lists can also be fetched:

```cpp
std::vector<User> firstTwentyUsers = myProjectDataContext.Users.
  Skip(0).Take(20).ToVector();
```

**The fluent query feature is still in the design phase and not usable yet.**