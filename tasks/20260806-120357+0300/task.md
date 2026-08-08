---
priority: 90
status: closed
tags:
    - arena
---

# Alignment overallocate for multiples of 16

`align_up(x, 16) = (x + 15) & ~15`