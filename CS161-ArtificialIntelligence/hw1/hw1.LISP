;This function returns the number of addition calls required by PAD to compute the xth term in the Padovan sequence.
;x is the number of the term you'd like to find the number of additions for
;explanation: in the PAD function there is one addition for every two recursive calls for numbers that are greater than two. Hence, add one every time you make the call to smaller numbers.
(defun SUMS(x) (cond 
                ((= x 0) 0) 
                ((= x 1) 0) 
                ((= x 2) 0) 
                ((+ (SUMS(- x 2)) (SUMS(- x 3)) 1))))

;This function changes the values in a tree to ?s while preserving the structure of the tree.
;x is the tree you'd like converted
;explanation if x is an atom, just return a ?. If x is nil, return an empty list of values so you don't get a superfulous ?. If x is a list all you need to do is make a recursive call to ANON with the first element and cons it with a recursive call to the cdr of the list
(defun ANON(x) (cond 
                ((null x) (values))
                ((atom x) '?) 
                ((listp x) (cons (ANON (car x)) (ANON (cdr x))))))
;This function computes the xth term in the Padovian sequence
;x is the number of the term you'd like to compute
;explanation: there are three base cases (0, 1, and 2) where you return 1 and add it to the sum, otherwise you make a recursive call for a previous number in the sequence and add the two returned values
(defun PAD(x) (cond
               ((= x 0) 1)
               ((= x 1) 1)
               ((= x 2) 1)
               ((+ PAD(- x 2) PAD(- x 3)))))