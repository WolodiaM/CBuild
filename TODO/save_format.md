# Target config
 * Per-file store
 * For each /home/user/project/src/file.cpp generate build/&lt;toolchain_id&gt;/config/.home.user.project.src.file.config
 * Migrating from pre 10.0 version - no troubles, it will simply threat it as a blank project, but better to remove all cached object files, also old build/&lt;toolchain_id&gt;/hash/ dir can be removed
 * Use -MM (if excludes only from /usr/lib and /lib or use -MD and manualy strip all from /usr/lib and /lib)