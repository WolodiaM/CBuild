# Spec for parser flag definitions

## General format

Flag definition consists of a list of blocks. Blocks are separated by block separator - `\t`, except first one. There are no separator before first block and after last block.

This are defined blocks (in order):
 * Option identifier (will be used as a long flag option).
 * Short option name (optional).
 * Metadata block. Described in [Metadata](#metadata) section.
 * Description (in theory optional, code does not check its length).
 
First separator (after ID) could be one of theese:
 * `\t` - This will be flag with long and short variants
 * `\n` - This will be a flag with long option only. Short option block should be ommited.

Also, flag definition could be used to specify command to parser internals. This commands start from `-` character.

List of commands:
 * `separator` -> Using this will push `--` separator (if found) into a list of positional arguments. By defulat is not pushed into that list.

## Metadata

This block consistsi of `key=value` pairs. They are separated by metadata pair separator - `;`. Last pair is terminated by ablock separator. For any ommited key default value will be used. This section could be left empty in any flag type except *positional argument*.

Supported keys:
 * `arg` - Specifies type of argument expected for this flag or type of positional argument. Default value is **no argument**. `?` could be used at the end of arument type to mark arguments as optional. Allowed arguments:
    - `arg` - Single argument expected
    - `list` - List of arguments expected. List will terminate on first word starting from `-` or for positonal arguments - **never**.
    - `tlist` - Terminated list. List will terminate only on terminator symbol.
 * `len` - Set expected number of arguments for *list* and *tlist* argument types. ***0*** indicates any number of arguments.
 * `thint` - Type hint. Simply a string that will be displayed as a type of argument.
 * `tdelim` - Delimeter character for a *tlist* argument type.
 
## Limitations

*TList* delimeter could be 1 character only.

*TList* and *List* maximum expected length could only be 255. More arguments could be parsed, but only on ***0*** expected length of a list (value provided by a user in metadata is truncated to a byte).
