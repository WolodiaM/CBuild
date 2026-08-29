---
priority: 50
status: open
tags:
    - pool-alloc
---

# Allocator could use array for metadata

Metadata is not used outside of allocator, so simple dynamic array could be sued for it. It could be reallocated on each new block added and old data would be automatically copied (as padding in last byte should be `0`).