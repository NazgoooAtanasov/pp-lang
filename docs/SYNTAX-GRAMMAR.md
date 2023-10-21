# Syntactical analysis

<!-- <img src="syntax.png" /> -->
<!-- This is the original latex text for the syntax but since github does not preview it correct, we are stuck with images -->
$$Z \to \text{def main} \space block $$

$$block \to \text{do } stmt \space \{; stmt\} \space \text{end}$$

$$stmt \to blockless\_stmt  \ | \ blocked\_stmt $$

$$blocked\_stmt \to \ if\_stmt \ | \ while\_stmt   $$

$$blockless\_stmt \to variable\_assign \ | \ \text{puts} \ expression \ | \ \text{reads ident} $$

$$if\_stmt \to \text{if } booleanexpr \space block \space [\text{else } block] $$

$$while\_stmt \to  \text{while } booleanexpr \space block $$

$$ variable\_assign \to  \text{ident} \ = \ expression $$

$$expression \to computation \space \{ \text{*|/} \space computation \}$$

$$computation \to operand \space \{\text{+|-} \space operand\}$$

$$operand \to \text{ident | const | } (expression)$$

$$boolean \to expression \space [>|<|<=|>=|== expression]$$

$$booleanexpr \to boolean \space \{\text{\&\& | || } boolean\}$$
