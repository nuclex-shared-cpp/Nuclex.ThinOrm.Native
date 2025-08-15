  - **Status:** Early development. Most of the planned features do not work,
    including message boxes, the task coordinator and half of the hardware info.

  - **Platforms:** Cross-platform, developed on Linux, will eventually
    receive testing on Windows.

Nuclex.ThinOrm.Native
======================

This library implements a "thin" ORM that grants you full control over how
queries will be constructed, avoiding convenience abstractions such as
active record, lazy-loading proxy collections or automatic cascading. It is
inspired by LinqToDb for .NET, though, as of now, it still requires you to
write out your SQL statements as strings.
