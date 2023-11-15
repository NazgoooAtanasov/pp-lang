# Syntactical analysis

<!-- <img src="syntax.png" /> -->
<!-- This is the original latex text for the syntax but since github does not preview it correct, we are stuck with images -->
$$Z \to \text{def main} ~ block $$

$$block \to \text{do} \ stmt ~ \{stmt\} ~ \text{end}$$

$$stmt \to blockless\_stmt  \ \text{\textbar} \ blocked\_stmt $$

$$blocked\_stmt \to \ if\_stmt \ \text{\textbar} \ while\_stmt   $$

$$blockless\_stmt \to variable\_assign \ \text{\textbar} \ \text{puts} \ expression \ \text{\textbar} \ \text{reads ident} \ [;]$$

$$if\_stmt \to \text{if } booleanexpr ~ block ~ [\text{else } block] $$

$$while\_stmt \to  \text{while } booleanexpr ~ block $$

$$ variable\_assign \to  \text{ident} \ [type\_annot] \ = \ expression $$

$$ type\_anot \to \ \text{: integer} $$

$$expression \to operand ~ \{ \text{*\text{\textbar}/\text{\textbar}+\text{\textbar}-} ~ operand \}$$

$$operand \to \text{ident \text{\textbar} const \text{\textbar} } (expression)$$

$$boolean \to expression ~ [>\text{\textbar}<\text{\textbar}<=\text{\textbar}>=\text{\textbar}== expression]$$

$$booleanexpr \to boolean ~ \{\text{\&\& \text{\textbar} \text{\textbar}\text{\textbar} } boolean\}$$
