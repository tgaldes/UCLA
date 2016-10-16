(*hw2*)
type ('nonterminal, 'terminal) symbol = 
| N of 'nonterminal
| T of 'terminal;;
(*print the text of boolean values*)
(*let to_string x = if x then "true\n" else "false\n";;
(*print contents of a list*)
let rec list_print a = match a with
[] -> ()
| h::t -> print_int h ; print_string " " ; list_print t;;

let print_awk h = 
if h = Num then print_string "Num"
else if h = Expr then print_string "Expr" 
else if h = Incrop then print_string "Incrop" 
else if h = Lvalue then print_string "Lvalue" 
else if h = Binop then print_string "Binop"
else if h = Term then print_string "Term";;
(*let is_nonterminal e = match e with
N _ -> true
| T _ -> false;;
let remove_symbol e = match e with
N x -> x
| T x -> x;;
let get_string e = match e with
T x -> x
| _ -> "";;*)
let match_then_print e = match e with
N x -> print_awk x
| T x -> print_string x;;
let rec list_print_awk a = match a with
[] -> ()
| h::t -> match_then_print h;
list_print_awk t;;
let rec list_list_print_awk a = match a with
[] -> ()
| h::t -> list_print_awk h; print_string "\n"; list_list_print_awk t;;
let rec list_print l = match l with
[] -> ()
| h::t -> print_string h; print_string "\n"; list_print t;;
let rec print_derivation_list d = match d with
[] -> print_string "\n"
| (nt, lst)::t -> print_awk nt; list_print_awk lst;
print_derivation_list t;;
let print_derivation d = match d with
None -> print_string "No derivation\n"
| Some x -> print_string "Some derivation\n";
match x with
([], []) -> print_string "empty list\n"
| (a, _ ) -> print_derivation_list a;;
let awksub_rules =
   [Expr, [N Term; N Binop; N Expr];
    Expr, [N Term];
    Term, [N Num];
    Term, [N Lvalue];
    Term, [N Incrop; N Lvalue];
    Term, [N Lvalue; N Incrop];
    Term, [T"("; N Expr; T")"];
    Lvalue, [T"$"; N Expr];
    Incrop, [T"++"];
    Incrop, [T"--"];
    Binop, [T"+"];
    Binop, [T"-"];
    Num, [T"0"];
    Num, [T"1"];
    Num, [T"2"];
    Num, [T"3"];
    Num, [T"4"];
    Num, [T"5"];
    Num, [T"6"];
    Num, [T"7"];
    Num, [T"8"];
    Num, [T"9"]]
let pre_awkish_grammar = Expr , awksub_rules;;
let awkish_grammar =
  (Expr,
   function
     | Expr ->
         [[N Term; N Binop; N Expr];
          [N Term]]
     | Term ->
	 [[N Num];
	  [N Lvalue];
	  [N Incrop; N Lvalue];
	  [N Lvalue; N Incrop];
	  [T"("; N Expr; T")"]]
     | Lvalue ->
	 [[T"$"; N Expr]]
     | Incrop ->
	 [[T"++"];
	  [T"--"]]
     | Binop ->
	 [[T"+"];
	  [T"-"]]
     | Num ->
	 [[T"0"]; [T"1"]; [T"2"]; [T"3"]; [T"4"];
	  [T"5"]; [T"6"]; [T"7"]; [T"8"]; [T"9"]])*)
(*hw 2 stuff*)
let rec convert rules nt = match rules with
(l, r)::t -> if l = nt then r::(convert t nt)
        else convert t nt
|[] -> [];;
let convert_grammar gram1 = 
(fst gram1, fun nt -> (convert (snd gram1) nt));;



let rec match_nonterminal rules start_sym start_sym_rules acceptor prev suf =
(*grab the first rule, attempt to match it to the suffix*)
match start_sym_rules with
(*no rules remain so return None for no match*)
        [] -> None
(*try matching the first rule with match_terminal*)
        | h::t -> match (match_terminal rules h acceptor (prev@[start_sym, h]) suf) with
(*if match_terminal returns None, try the next rule*)
                None -> (*print_string "trying next rule in match_nonterminal\n";*) (match_nonterminal rules start_sym t acceptor prev suf)
                (*if we don't return none, return whatever was returned*)
                | return_some -> (*print_string "returning in match_nonterminal\n";*) return_some
and match_terminal rules rule acceptor prev suf = match rule with
(*no parts in the rule, return acceptor called on suf*)
[] -> (*print_string "used all of the rule\n"; list_print_awk rule;*) acceptor prev suf
| h::t -> (*print_string "rule remaining is:\n"; list_print_awk rule;*) match suf with
        (*nothing left in suf but stuff left in rule*)
        [] -> (*print_string "nothing in suf but rule remains\n rule is:\n"; list_print_awk rule;*) None
        (*get type of first symbol in rule*)
        | sym::suf_tail -> match h with
                (*if nonterm call match any to find a new rule, new acceptor must match end of rule and suffix*)
                (N nonterminal) -> (match_nonterminal rules nonterminal (rules nonterminal) (match_terminal rules t acceptor) prev suf)
                (*if terminal try to match with suf*)
                | (T terminal) -> 
                        if terminal = sym then match_terminal rules t acceptor prev suf_tail 
                        else None;;

let parse_prefix gram acceptor suf = match_nonterminal (snd gram) (fst gram) ((snd gram) (fst gram)) acceptor [] suf;;
