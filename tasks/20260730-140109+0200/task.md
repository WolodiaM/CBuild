---
priority: 40
status: open
tags:
    - platform
    - windows
---

# Windows support

Add Windows as a supported platform. Some features could not be supported on it thought.

# Notes

1. Windows does not support lazy-resolving symbols in dynamic libraries.
2. Windows can not overwrite file which is currently executed (but can move it?).
3. Windows have problems with spawning processes that are not external apps (e.g., fork without exec).