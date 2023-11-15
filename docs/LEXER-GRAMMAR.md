# Lexical analysis

## Grammar explaination

**Keyword(keyword)** - specific string literals that are reserved for the language.
1. def
2. do
3. end
4. main

**Constant(const)** - any consecutive number sequence

**Operator(operator)** - Mathematical symbols - `+, -, *, /`

**Type annotation prefix(type_annot_prefix)** - The colon symbol - `:`

**Identifier(ident)** - Everything that starts with a ascii symbol and it does not contain any whitespace within it.

## Grammar

#### Keyword
$$ keyword \to \text{def \text{\textbar} do \text{\textbar} end \text{\textbar} main \text{\textbar} if \text{\textbar} else \text{\textbar} while \text{\textbar} puts \text{\textbar} reads \text{\textbar} integer} $$

#### Constant
$$const \to digit \\ $$
$$const \to const ~ digit \\ $$

#### Operator
$$operator \to \text{+} \\$$
$$operator \to \text{-} \\$$
$$operator \to \text{*} \\$$
$$operator \to \text{/} \\$$

#### Type annotation prefix
$$ type\_annot\_prefix \to \text{:} $$

#### Identifier
$$ident \to letter \\$$
$$ident \to ident ~ letter \\$$
$$ident \to ident ~ digit \\$$
$$digit \to \text{0} \\$$
$$digit \to \text{1} \\$$
$$digit \to \text{2} \\$$
$$digit \to \text{3} \\$$
$$digit \to \text{4} \\$$
$$digit \to \text{5} \\$$
$$digit \to \text{6} \\$$
$$digit \to \text{7} \\$$
$$digit \to \text{8} \\$$
$$digit \to \text{9} \\$$
$$letter \to \text{a} \\$$
$$letter \to \text{b} \\$$
$$letter \to \text{c} \\$$
$$letter \to \text{d} \\$$
$$letter \to \text{e} \\$$
$$letter \to \text{f} \\$$
$$letter \to \text{g} \\$$
$$letter \to \text{h} \\$$
$$letter \to \text{i} \\$$
$$letter \to \text{j} \\$$
$$letter \to \text{k} \\$$
$$letter \to \text{l} \\$$
$$letter \to \text{m} \\$$
$$letter \to \text{n} \\$$
$$letter \to \text{o} \\$$
$$letter \to \text{p} \\$$
$$letter \to \text{q} \\$$
$$letter \to \text{r} \\$$
$$letter \to \text{s} \\$$
$$letter \to \text{t} \\$$
$$letter \to \text{u} \\$$
$$letter \to \text{v} \\$$
$$letter \to \text{w} \\$$
$$letter \to \text{x} \\$$
$$letter \to \text{y} \\$$
$$letter \to \text{Z} \\$$
$$letter \to \text{A} \\$$
$$letter \to \text{B} \\$$
$$letter \to \text{C} \\$$
$$letter \to \text{D} \\$$
$$letter \to \text{E} \\$$
$$letter \to \text{F} \\$$
$$letter \to \text{G} \\$$
$$letter \to \text{H} \\$$
$$letter \to \text{I} \\$$
$$letter \to \text{J} \\$$
$$letter \to \text{K} \\$$
$$letter \to \text{L} \\$$
$$letter \to \text{M} \\$$
$$letter \to \text{N} \\$$
$$letter \to \text{O} \\$$
$$letter \to \text{P} \\$$
$$letter \to \text{Q} \\$$
$$letter \to \text{R} \\$$
$$letter \to \text{S} \\$$
$$letter \to \text{T} \\$$
$$letter \to \text{U} \\$$
$$letter \to \text{V} \\$$
$$letter \to \text{W} \\$$
$$letter \to \text{X} \\$$
$$letter \to \text{Y} \\$$
$$letter \to \text{Z} \\$$
