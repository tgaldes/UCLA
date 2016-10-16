type awksub_nonterminals =
  | Expr | Lvalue | Incrop | Binop | Num;;
let my_computed_fixed_point_test0 = computed_fixed_point (=) sqrt 55. = 1.
let my_computed_periodic_point_test0 = computed_periodic_point (=) (fun x -> x *. x -. 1.) 2 1. = 0.
let my_while_away_test0 = equal_sets (while_away ((+) 1) ((>) 2) (-1)) [1;0;-1]
let my_rle_decode_test0 = equal_sets (rle_decode [2,0;1,1;1,4]) [0;0;1;4]
let my_filter_blind_alleys_test0 = filter_blind_alleys (Expr, [Expr, [T"("; N Num]; Num, [T"9"]; Expr, [N Lvalue]; Lvalue, [N Lvalue]]) =
(Expr,  [Expr, [T"("; N Num]; Num, [T"9"];]) 
let my_subset_test0 = subset [4;5;6] [5;6;7;4;14]
let my_equal_sets_test0 = equal_sets [4;5;2;1] [2;5;4;1;1;2]
let my_set_union_test0 = equal_sets (set_union [1] [1;2]) [1;2]
let my_set_intersection_test0 = equal_sets (set_intersection [1] [2;3;4]) []
let my_set_diff_test0 = equal_sets (set_diff [1;2;3;4] [2;4]) [1;3]
