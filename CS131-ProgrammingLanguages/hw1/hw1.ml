(*print the text of boolean values*)
let to_string x = if x then "true\n" else "false\n";;
(*print contents of a list*)
let rec list_print a = match a with
[] -> ()
| h::t -> print_int h ; print_string " " ; list_print t;;

(*contains:: return true when e is in list l,
false otherwise*)
let rec contains e l = match l with
[] -> false
| h::t -> if h = e then true else contains e t;;

(*tests for contains*)
(*print_string "Unit tests for contains\n";;
let x = contains 3 [1;2];;
let y = if x = false then "P" else "FAILED";;
print_string y;;
let x = contains 3 [];;
let y = if x = false then "P" else "FAILED";;
print_string y;;
let x = contains 3 [1;3];;
let y = if x = true then "P" else "FAILED";;
print_string y;;
let x = contains 3 [3;3];;
let y = if x = true then "P" else "FAILED";;
print_string y;;
let x = contains 3 [1;2;1;4;5;45;3;4;5];;
let y = if x = true then "P" else "FAILED";;
print_string y;;*)

(*subset: returns true iff a is a subset of b*)
let rec subset a b = match a with
[] -> true
| h::t -> if contains h b then subset t b else false;;
(*unit tests for subset*)
(*print_string "\nUnit tests for subset\n";;
let x = subset [] [];;
let y = if x = true then "P" else "FAILED";;
print_string y;;
let x = subset [1] [1];;
let y = if x = true then "P" else "FAILED";;
print_string y;;
let x = subset [23;2] [2;5;7;23;4];;
let y = if x = true then "P" else "FAILED";;
print_string y;;
let x = subset [1;2] [2;4;3;2];;
let y = if x = false then "P" else "FAILED";;
print_string y;;
let x = subset [1;2;3;4] [1;2;3];;
let y = if x = false then "P" else "FAILED";;
print_string y;;
let x = subset [1;2;3] [2;3;1];;
let y = if x = true then "P" else "FAILED";;
print_string y;;*)

(*equal_sets: return true iff a and b are equal*)
let equal_sets a b = 
if subset a b = false then false else if subset b a = false then false else true;;
(*unit tests for equal sets*)
(*print_string "\nUnit tests for equal_sets\n";;
let x = equal_sets [] [];;
let y = if x = true then "P" else "FAILED";;
print_string y;;
let x = equal_sets [3] [3];;
let y = if x = true then "P" else "FAILED";;
print_string y;;
let x = equal_sets [4;6] [6;4];;
let y = if x = true then "P" else "FAILED";;
print_string y;;
let x = equal_sets [1;14;2] [14;2;1];;
let y = if x = true then "P" else "FAILED";;
print_string y;;
let x = equal_sets [] [2;3];;
let y = if x = false then "P" else "FAILED";;
print_string y;;
let x = equal_sets [3;4;5] [3;4;5;6];;
let y = if x = false then "P" else "FAILED";;
print_string y;;*)

(*set_union: return the union of two sets*)
let rec set_union a b = match a with
[] -> b
| h::t -> if contains h b then set_union t b else set_union t (b@[h]);;
(*unit tests for set_union*)
(*print_string "\nUnit tests for set_union\n";;
let x = set_union [] [];;
let y = if x = [] then "P" else "FAILED";;
print_string y;;
let x = set_union [1] [2];;
let y = if equal_sets x [2;1] then "P" else "FAILED";;
print_string y;;
let x = set_union [1;2;3] [1];;
let y = if equal_sets x [2;1;3] then "P" else "FAILED";;
print_string y;;
let x = set_union [4;5;6] [1;2;3;4];;
let y = if equal_sets x [2;1;3;4;5;6] then "P" else "FAILED";;
print_string y;;*)

(*set_intersection: return the intersection of the two sets*)
let rec set_intersection_rec a b c = match a with
[] -> c
| h::t -> if contains h b then set_intersection_rec t b (c@[h]) else
set_intersection_rec t b c;;
let set_intersection a b = set_intersection_rec a b [];;
(*unit tests for set_intersection*)
(*print_string "\nUnit tests for set_intersection\n";;
let x = set_intersection [] [];;
let y = if x = [] then "P" else "FAILED";;
print_string y;;
let x = set_intersection [1] [2];;
let y = if x = [] then "P" else "FAILED";;
print_string y;;
let x = set_intersection [1;2;3] [1;2];;
let y = if equal_sets x [1;2] then "P" else "FAILED";;
print_string y;;
let x = set_intersection [1;2] [1;2];;
let y = if equal_sets x [1;2] then "P" else "FAILED";;
print_string y;;
let x = set_intersection [1;2;23] [1;2;1;2];;
let y = if equal_sets x [1;2] then "P" else "FAILED";;
print_string y;;*)

(*set_diff: return all elements of a that do not appear in b*)
let rec set_diff_rec a b c = match a with
[] -> c
| h::t -> if contains h b then set_diff_rec t b c else set_diff_rec t b (c@[h]);;
let set_diff a b = set_diff_rec a b [];;
(*unit tests for set_diff*)
(*print_string "\nUnit tests for set_diff\n";;
let x = set_diff [] [];;
let y = if x = [] then "P" else "FAILED";;
print_string y;;
let x = set_diff [1;2;3] [1;2];;
let y = if equal_sets x [3] then "P" else "FAILED";;
print_string y;;
let x = set_diff [1;2;3;4] [1;2;2;1];;
let y = if equal_sets x [3;4] then "P" else "FAILED";;
print_string y;;
let x = set_diff [1;2] [1;2];;
let y = if equal_sets x [] then "P" else "FAILED";;
print_string y;;
let x = set_diff [1;2;3;1;3] [1;2];;
let y = if equal_sets x [3] then "P" else "FAILED";;
print_string y;;
let x = set_diff [1;2;3] [];;
let y = if equal_sets x [1;2;3] then "P" else "FAILED";;
print_string y;;*)

(*computed_fixed_point: returns the fixed point of the func
with respect to x*)
let rec computed_fixed_point eq f x = 
       if (eq (f x) x) then x 
       else computed_fixed_point eq f (f x);;
(*unit tests for computed_fixed_point*)
(*print_string "\nUnit tests for computed_fixed_point\n";;*)
(*let computed_fixed_point_test0 = computed_fixed_point (=) (fun z -> z / 2) 1000000000 = 0;;*)
(*let computed_fixed_point_test1 = computed_fixed_point (=) (fun x -> x *. 2.) 1. = infinity;;
print_string (to_string computed_fixed_point_test1);;
let computed_fixed_point_test2 = computed_fixed_point (=) (fun x -> x *. 2.) (-(1.)) = neg_infinity;;
print_string (to_string computed_fixed_point_test2);;
let computed_fixed_point_test3 = computed_fixed_point (=) sqrt 10. = 1.;;
print_string (to_string computed_fixed_point_test2);;
let computed_fixed_point_test4 = ((computed_fixed_point
 (fun x y -> abs_float (x -. y) < 1.) 
 (fun x -> x /. 2.)
 10.) = 1.25);;
print_string (to_string computed_fixed_point_test4);;*)

(*computed_periodic_point: returns the computedd periodic point with respect
to f and x and with period p*)
let rec computed_periodic_point eq f p x = match p with 
0 -> x
| _ -> if eq x (f (computed_periodic_point eq f (p - 1) (f x))) then x
        else computed_periodic_point eq f p (f x);;

(*unit tests for computed_periodic_point*)
(*print_string "Unit tests for computed periodic point\n";;
let computed_periodic_point_test0 = computed_periodic_point (=) (fun x -> x / 2) 0 (-1) = -1;;
print_string (to_string computed_periodic_point_test0);;
let computed_periodic_point_test1 =
  computed_periodic_point (=) (fun x -> x *. x -. 1.) 2 0.5 = -1.;;
print_string (to_string computed_periodic_point_test1);;*)

(*while_away: return a list of the maximum length that satisfies the given condition*)
let rec while_away_rec s p x r = if p x then while_away_rec s p (s x) (r@[x])
else r;;
let while_away s p x = while_away_rec s p x [];;
(*unit tests*)
(*print_string "Unit tests for while_away\n";;
let x = while_away ((+) 3) ((>) 10) 0;;
let y = if equal_sets x [0;3;6;9] then "P" else "FAILED";;
print_string y;;
let x = while_away ((+) 3) ((>) 13) 0;;
let y = if equal_sets x [0;3;6;9;12] then "P" else "FAILED";;
print_string y;;
let x = while_away ((+) 7) ((>) 34) 20;;
let y = if equal_sets x [20;27] then "P" else "FAILED";;
print_string y;;
let x = while_away ((+) 1) ((>) 2) (-1);;
let y = if equal_sets x [1;0;-1] then "P" else "FAILED";;
print_string y;;
let x = while_away ((+) (-3)) ((<) 10) 20;;
let y = if equal_sets x [20;17;14;11] then "P" else "FAILED";;
print_string y;;*)

(*rle_decode lp: returns the decoded version of lp, which is encoded in 
run length encoded form*)
let rec rle_decode_rec lp r = match lp with
[] -> r
| (l , e)::t -> if l = 0 then rle_decode_rec t r 
else let tuple = ((l - 1), e) in rle_decode_rec (tuple::t) (r@[e]);;
let rle_decode lp = rle_decode_rec lp [];;
(*unit tests for rle_decode*)
(*print_string "\nUnit tests for rle_decode\n";;
let x = rle_decode [2,0;1,1;3,3];;
let y = if equal_sets x [0;0;1;3;3;3] then "P" else "FAILED";;
print_string y;;
let x = rle_decode [];;
let y = if equal_sets x [] then "P" else "FAILED";;
print_string y;;
let x = rle_decode [5,0;1,0;1,1];;
let y = if equal_sets x [0;0;0;0;0;0;1] then "P" else "FAILED";;
print_string y;;
let x = rle_decode [1,"j";1,"k"];;
let y = if equal_sets x ["j";"k"] then "P" else "FAILED";;
print_string y;;*)

(*typedef for non/terminal symbols*)
type ('nonterminal, 'terminal) symbol = 
| N of 'nonterminal | T of 'terminal;;
(*let print_awk h = 
if h = Num then print_string "Num\n"
else if h = Expr then print_string "Expr\n" 
else if h = Incrop then print_string "Incrop\n" 
else if h = Lvalue then print_string "Lvalue\n" 
else if h = Binop then print_string "Binop\n";;
let rec list_print_awk a = match a with
[] -> ()
| h::t -> print_awk h; list_print_awk t;;
let rec print_types rules = match rules with
[] -> ()
|(a, rule)::t -> print_awk a; print_types t;;*)
let is_element_terminal e = match e with
N _ -> false
| T _ -> true;;
let rec reverse_list l r = match l with
[] -> r
| h::t -> reverse_list t r@[h];;
(*print_string "\nUnit tests for helpers of filterAlleys\n";;
let x = is_element_terminal (T"6");;
let y = if x then "P" else "FAILED";;
print_string y;;
let x = is_element_terminal (N Incrop);;
let y = if x then "FAILED" else "P";;
print_string y;;*)
(*return true iff the list contains only terminal symbols*)
(*let rec is_list_terminal l = match l with
[] -> true
| h::t -> if is_element_terminal h then is_list_terminal t
else false;;*)
(*return a list of all types that have only terminal symbols*)
(*let rec get_terminal_types l r = match l with
[] -> r
| (a, rule)::t -> if contains a r then get_terminal_types t r
else if is_list_terminal rule then get_terminal_types t (r@[a]) else get_terminal_types t r;;*)
(*some tests*)
(*let x = is_list_terminal [(T"6");N Incrop];;
let y = if x then "FAILED" else "P";;
print_string y;;
let x = is_list_terminal [(T"6");(T"5")];;
let y = if x then "P" else "FAILED";;
print_string y;;*)
let check_part_of_rule e terminals = match e with
N x -> if contains x terminals then true else false
| T x -> true;;
(*return true if all the symbols in the RHS of the rule
appear in the list of terminal symbols passed in*)
let rec is_rule_terminable rule terminals = match rule with
[] -> true
| h::t -> 
if is_element_terminal h then is_rule_terminable t terminals 
else if (check_part_of_rule h terminals) then is_rule_terminable t terminals else false;; 
(*feed rules to above function*)
let rec get_good_symbols_rec rules terminals = match rules with
[] -> terminals
|(a, rule)::t -> if contains a terminals then get_good_symbols_rec t terminals 
else if is_rule_terminable rule terminals then
get_good_symbols_rec t (terminals@[a]) else get_good_symbols_rec t terminals;;
(*return true iff the lhs is valid*)
let is_lhs_invalid lhs valid_lhs = if contains lhs valid_lhs
then false else true;;
(*return true iff the rhs is valid*)
let rec is_rhs_invalid rhs valid_rhs = match rhs with
[] -> false
| h::t -> if check_part_of_rule h valid_rhs
then is_rhs_invalid t valid_rhs
else true;;
(*helper function that removes rules from the grammar when
they don't appear in the input list valid_lhs*)
let rec remove_rules rules valid_lhs r = match rules with
[] -> r
| (a, rule)::t -> if is_lhs_invalid a valid_lhs then remove_rules t valid_lhs r
else if is_rhs_invalid rule valid_lhs then remove_rules t valid_lhs r
else remove_rules t valid_lhs r@[(a, rule)];;
(*the big mama*)
let filter_blind_alleys g = let terminables = (let wrapper =
        get_good_symbols_rec (snd g) in
        computed_fixed_point (equal_sets)
        wrapper []) in ((fst g), (reverse_list (remove_rules (snd g) terminables []) []));;
