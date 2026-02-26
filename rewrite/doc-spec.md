# General

Starting comment: `///`. This is easy to parse.
Full pandoc markdown syntax support.

# Syntax extensions

By default, highlighted object will be printed.

## Param reference

`[p:<param_name>]` includes a parameter with automatic verbatim block.
`[pl:<param_name>]` automatically formats parameter and its type for usage in parameter list. 
`[pl:<param_name>:<param type>]` automatically formats parameter and its type for usage in parameter list. For macro functions because takes parameter name too. Does not create implicit list, just format starting text!

Maybe `p:` references will generate hover with type and comment from `pl:` references? If this can be done without too much (preferably no) js.

## Field references

`[f:<field_name>]` - same as `p:` reference but for a struct or enum field.
`[fl:<field_name>]` - same as `pl:` reference but for a struct or enum field. Does not create implicit list, just format starting text!

## Return reference

`[r:]` - Format return-name with its type and proper styling.
`[r:<return type>]` - Format return-name with give type and proper styling. For macro.

## General reference behavior

When referencing parameters (`[p:]`, `[pl:]`), fields (`[f:]`, `[fl:]`), or returns (`[r:]`), the builder will attempt to deduce the type and scope based on the nearest preceding C object (function, struct, enum, define, etc.) where the comment is located.

For `[pl:<name>]`, `[fl:<name>]`, and `[r:]`, the type will be automatically deduced from the C code. Only for macros or specific cases will the type need to be explicitly provided (e.g., `[pl:<name>:<type>]`, `[r:<type>]`).

If a reference `[...:]` cannot be resolved (e.g., parameter not found in scope), it will be replaced by **ERROR: Can't resolve types** in the generated Markdown, and a warning message will be printed during the build process.

## File-level comments

Comment should be directly on top of some C language object (struct, enum, typedef, function typedef, function, define, global constant).

If comments starts from `//!` is documents file. All of such comments are concatenated and put in the top of a page via paragraph breaks.

File-level comment can be generated between descriptions for language objects. This can be used to declare custom sections.

## Filepos

Comment with literal content of `//@ <name>` creates named location (spaces can be added around this block. It would be attached to next line of file.

Then it can be used by adding same `[line:<name>]` can be used to reference it.

## Default Value Evaluation

For documentation that references constants or enums, a small C helper program is built. This helper evaluates the "runtime" value of these constants (taking into account compiler constant folding and expressions involving other constants).

The helper works by generating a temporary C file that includes the project's headers and defines the necessary constants or enums. This file is then compiled and executed to obtain their values.

This compilation is designed to happen once per project (or per compilation unit) to optimize build time. Users can provide custom C includes or defines for the helper to accurately evaluate values that might depend on specific build configurations or target environments.

If a constant's value depends on a platform-specific feature not available during documentation build, it may need to be explicitly specified or marked as unknown. `[v:<val>]` can be used for this. Stray blocks of this kind cause error.

# Implementation details

## Blocks

Fenced divs are used for each block

```markdown
::: {#<object name> .<type>}
...
:::
```

Where type can be:
* function - Used for defines that define function-like macro
* function-typedef
* constant - Used for defines that define values too
* typedef
* struct
* enum
* macro - anything from preprocessor that can not be deduced

## Headings

Custom filter is used to subtract 2 from heading level. Level 5 and 6 headings are convert to bold and italic respectively text with newline at the end.  
Allows to write normal headings while not breaking global layout.

Level 1 headings is used as page-level headers and level 2 is used for object names at the top of a block. Both of this are added by C "parser" and "processor".

In file-level comments only 1 is subtracted from heading level.

## Template

Same as currently for wikimk, template is available. But here 2 templates are used - first one is for generated markdown. This allows to set metadata and some overal structure and second is for HTML. Default header and footer is present too. Search header is added.

## Hover

  1. The Generated HTML
  When your parser sees [p:my_param], it would wrap it in a span with a hidden "popup" span inside:
```html
<span class="doc-ref">
  <code>my_param</code>
  <span class="doc-popup">
    <strong>int</strong> - The count of items to process.
  </span>
</span>
```
  2. The CSS (The "No-JS" Magic)
  You can handle the visibility entirely through the :hover pseudo-class:

```css
/* The container */
.doc-ref {
    position: relative;
    display: inline-block;
    cursor: help;
    border-bottom: 1px dotted #666;
}

/* The popup (hidden by default) */
.doc-popup {
    visibility: hidden;
    position: absolute;
    bottom: 125%; /* Position above the text */
    left: 50%;
    transform: translateX(-50%);

    /* Styling */
    background-color: #333;
    color: #fff;
    padding: 8px;
    border-radius: 4px;
    width: 200px;
    z-index: 10;
    opacity: 0;
    transition: opacity 0.2s;
}

/* Show on hover */
.doc-ref:hover .doc-popup {
    visibility: visible;
    opacity: 1;
}

/* A small arrow at the bottom of the popup */
.doc-popup::after {
    content: "";
    position: absolute;
    top: 100%;
    left: 50%;
    margin-left: -5px;
    border-width: 5px;
    border-style: solid;
    border-color: #333 transparent transparent transparent;
}
```

# Used pandoc extensions (for a full wiki)

Extensions:

- smart
- auto\_identifiers
- Either ascii\_identifiers or gfm\_auto\_identifiers
- raw\_html - Disabled. Allow to build to other formats. And easier to manage. And harder to break.
-  all\_symbols\_escapable

Pandoc markdown:

- blank\_before\_header
- space\_in\_atx\_header
- header\_attributes - Used in cross-referencing
- implicit\_header\_references - Better to use explicit names but sometimes it is useful
- fenced\_code\_attributes - Syntax-highlighted
- intraword\_underscores - Highly useful for code.
- inline\_code\_attributes - Useful for code docs
- bracketed\_spans - Useful for custom styling. wikimk support colors here and pandoc supports "underline", "smallcaps" and "mark" (maybe need to be added to main wikimk css).

# Defined css

## Divs

### note

Same as in wikimk

### deprecated

Somethings like note but with deprecation indicator.