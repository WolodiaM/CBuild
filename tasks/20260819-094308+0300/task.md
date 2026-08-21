---
priority: 70
status: closed
tags:
    - rglob
---

# glob `**` do not work.

Glob `src/**/*.c` should show all `.c` files in `src` recursively. Globs are not recursive by default.
Bug in CBuild. Probably `*` should be compiled to `[^/]*`, `**` to `.*` and `/` be ignored after `**`.