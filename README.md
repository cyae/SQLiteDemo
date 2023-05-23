# How Does a Database Work?

- What format is data saved in? (in memory and on disk)
- When does it move from memory to disk?
- Why can there only be one primary key per table?
- How does rolling back a transaction work?
- How are indexes formatted?
- When and how does a full table scan happen?
- What format is a prepared statement saved in?

## Structure

A query goes through a chain of components in order to retrieve or modify data.

**Query** -> **Front-end**: [Tokenizer, Parser, Code Generator] -> **ByteCode** -> **Back-end**: [Virtual Machine, Indexing, Pager, OS Interface]
