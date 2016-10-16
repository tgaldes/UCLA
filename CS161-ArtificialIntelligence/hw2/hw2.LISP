;;;;;;;;;;;;;;
; Homework 2 ;
;;;;;;;;;;;;;;

;;;;;;;;;;;;;;
; Question 1 ;
;;;;;;;;;;;;;;

; TODO: comment code
(defun BFS(FRINGE) (cond
               ((null FRINGE) (values))
               ((atom (car FRINGE)) (append  (cons (car FRINGE) '()) (BFS (cdr FRINGE))))
               ((listp (car FRINGE)) (BFS (append (cdr FRINGE) (car FRINGE) )))))

;;;;;;;;;;;;;;
; Question 2 ;
;;;;;;;;;;;;;;


; These functions implement a depth-first solver for the homer-baby-dog-poison
; problem. In this implementation, a state is represented by a single list
; (homer baby dog poison), where each variable is T if the respective entity is
; on the west side of the river, and NIL if it is on the east side.
; Thus, the initial state for this problem is (NIL NIL NIL NIL) (everybody 
; is on the east side) and the goal state is (T T T T).

; The main entry point for this solver is the function DFS, which is called
; with (a) the state to search from and (b) the path to this state. It returns
; the complete path from the initial state to the goal state: this path is a
; list of intermediate problem states. The first element of the path is the
; initial state and the last element is the goal state. Each intermediate state
; is the state that results from applying the appropriate operator to the
; preceding state. If there is no solution, DFS returns NIL.
; To call DFS to solve the original problem, one would call 
; (DFS '(NIL NIL NIL NIL) NIL) 
; However, it should be possible to call DFS with a different initial
; state or with an initial path.

; First, we define the helper functions of DFS.

; FINAL-STATE takes a single argument S, the current state, and returns T if it
; is the goal state (T T T T) and NIL otherwise.
;explanation; check every element of S and make sure they are all T, if yes, return T,
;return nil otherwise.
(defun FINAL-STATE (S) (cond
                        ((and (car S)
                              (cadr S)
                              (caddr S)
                              (car (cdddr S))) T)
                        ((= 0 0)  NIL)))

; NEXT-STATE returns the state that results from applying an operator to the
; current state. It takes three arguments: the current state (S), and which entity
; to move (A, equal to h for homer only, b for homer with baby, d for homer 
; with dog, and p for homer with poison). 
; It returns a list containing the state that results from that move.
; If applying this operator results in an invalid state (because the dog and baby,
; or poisoin and baby are left unsupervised on one side of the river), or when the
; action is impossible (homer is not on the same side as the entity) it returns NIL.
; NOTE that next-state returns a list containing the successor state (which is
; itself a list); the return should look something like ((NIL NIL T T)).
;explanation: Check to make sure that homer and the thing he is carrying across are 
;on the same side of the river, if not return nil (homer is always on the same side 
;as himself). Then check that flipping the corresponding elements of the state would
;result in a legal state. If yes, return that state, if no, return nil.
(defun NEXT-STATE (S A) (cond
                         ;homer and maggie together
                         ((and (eq A 'b)
                               (not (eq (first S) (second S)))) ;not on the same side
                          NIL)
                         ((and (eq A 'b);extra conditions will never happen since homer and baby end up on same side
                               (or (and (eq (null(second S)) (third S)) 
                                        (not (eq (null(first S)) (null(second S))))) ;dog eats baby
                                   (and (eq (null(second S)) (fourth S)) 
                                        (not (eq (null(first S)) (null(second S)))))));baby eats posion     
                          NIL)
                         ((and (eq A 'b)
                               (eq (first S) (second S)));on the same side
                           (list (null (first S)) (null (second S)) (third S) (fourth S)));flip sides of homer and maggie


                         ;homer and dog moving together
                         ((and (eq A 'd)
                               (not (eq (first S) (third S)))) ;not on the same side
                          NIL)
                         ((and (eq A 'd)
                               (or (and (eq (second S) (null(third S))) 
                                        (not (eq (null(first S)) (second S)))) ;dog eats baby
                                   (and (eq (second S) (fourth S)) 
                                        (not (eq (null(first S)) (second S))))));baby eats posion     
                          NIL)
                         ((and (eq A 'd)
                               (eq (first S) (third S)));on the same side
                           (list (null (first S)) (second S) (null(third S)) (fourth S)));flip side of homer and dog


                        ;homer and poison moving together
                         ((and (eq A 'p)
                               (not (eq (first S) (fourth S)))) ;not on the same side
                          NIL)
                         ((and (eq A 'p)
                               (or (and (eq (second S) (third S)) 
                                        (not (eq (null(first S)) (second S)))) ;dog eats baby
                                   (and (eq (second S) (null(fourth S))) 
                                        (not (eq (null(first S)) (second S))))));baby eats posion     
                          NIL)
                         ((and (eq A 'p)
                               (eq (first S) (fourth S)));on the same side
                           (list (null (first S)) (second S) (third S) (null(fourth S))));flip side of homer and poison


                         ;homer moving by himself, don't check if he is on the same side as himself
                         ((and (eq A 'h)
                               (or (and (eq (second S) (third S)) 
                                        (not (eq (null(first S)) (second S)))) ;dog eats baby
                                   (and (eq (second S) (fourth S)) 
                                        (not (eq (null(first S)) (second S))))));baby eats posion     
                          NIL)
                         ((eq A 'h) ;moving only homer doesn't violate any conditions
                           (list (null (first S)) (second S) (third S) (fourth S)));flip side of homer
))

; SUCC-FN returns all of the possible legal successor states to the current
; state. It takes a single argument (s), which encodes the current state, and
; returns a list of each state that can be reached by applying legal operators
; to the current state.
;explanation: There are 16 checks corresponding to the 16 possible values that
;a 4 digit binary number could have. Here the encoding is 1 if the next state reached 
;from the current state with the action is valid, 0 otherwise. When you find the right 
;4 digit number, return the states reached from the corresponding actions.
(defun SUCC-FN (S) (cond;I would have written this function far more elegantly if we were allowed to use setf :)
                     ((and (null (NEXT-STATE S 'h))
                           (null (NEXT-STATE S 'b))
                           (null (NEXT-STATE S 'd))
                           (null (NEXT-STATE S 'p)))
                          (list nil))

                    ((and (null (NEXT-STATE S 'h))
                           (null (NEXT-STATE S 'b))
                           (null (NEXT-STATE S 'd))
                           (not(null (NEXT-STATE S 'p))))
                          (list(NEXT-STATE S 'p)))

                    ((and (null (NEXT-STATE S 'h))
                           (null (NEXT-STATE S 'b))
                           (not(null (NEXT-STATE S 'd)))
                           (null (NEXT-STATE S 'p)))
                          (list(NEXT-STATE S 'd)))

                    ((and (null (NEXT-STATE S 'h))
                           (null (NEXT-STATE S 'b))
                           (not(null (NEXT-STATE S 'd)))
                           (not(null (NEXT-STATE S 'p))))
                          (list (NEXT-STATE S 'd) (NEXT-STATE S 'p)))



                    ((and (null (NEXT-STATE S 'h))
                           (not(null (NEXT-STATE S 'b)))
                           (null (NEXT-STATE S 'd))
                           (null (NEXT-STATE S 'p)))
                          (list (NEXT-STATE S 'b)))

                    ((and (null (NEXT-STATE S 'h))
                           (not(null (NEXT-STATE S 'b)))
                           (null (NEXT-STATE S 'd))
                           (not(null (NEXT-STATE S 'p))))
                          (list (NEXT-STATE S 'b) (NEXT-STATE S 'p)))

                    ((and (null (NEXT-STATE S 'h))
                           (not(null (NEXT-STATE S 'b)))
                           (not(null (NEXT-STATE S 'd)))
                           (null (NEXT-STATE S 'p)))
                          (list (NEXT-STATE S 'b) (NEXT-STATE S 'd)))

                    ((and (null (NEXT-STATE S 'h))
                           (not(null (NEXT-STATE S 'b)))
                           (not(null (NEXT-STATE S 'd)))
                           (not(null (NEXT-STATE S 'p))))
                          (list (NEXT-STATE S 'b) (NEXT-STATE S 'd) (NEXT-STATE S 'p)))




                    ((and (not(null (NEXT-STATE S 'h)))
                           (null (NEXT-STATE S 'b))
                           (null (NEXT-STATE S 'd))
                           (null (NEXT-STATE S 'p)))
                          (list (NEXT-STATE S 'h)))

                    ((and (not(null (NEXT-STATE S 'h)))
                           (null (NEXT-STATE S 'b))
                           (null (NEXT-STATE S 'd))
                           (not(null (NEXT-STATE S 'p))))
                          (list (NEXT-STATE S 'h)  (NEXT-STATE S 'p)))

                    ((and (not(null (NEXT-STATE S 'h)))
                           (null (NEXT-STATE S 'b))
                           (not(null (NEXT-STATE S 'd)))
                           (null (NEXT-STATE S 'p)))
                          (list (NEXT-STATE S 'h) (NEXT-STATE S 'd)))

                    ((and (not(null (NEXT-STATE S 'h)))
                           (null (NEXT-STATE S 'b))
                           (not(null (NEXT-STATE S 'd)))
                           (not(null (NEXT-STATE S 'p))))
                          (list (NEXT-STATE S 'h) (NEXT-STATE S 'd) (NEXT-STATE S 'p)))




                    ((and (not(null (NEXT-STATE S 'h)))
                           (not(null (NEXT-STATE S 'b)))
                           (null (NEXT-STATE S 'd))
                           (null (NEXT-STATE S 'p)))
                          (list (NEXT-STATE S 'h) (NEXT-STATE S 'b) ))

                    ((and (not(null (NEXT-STATE S 'h)))
                           (not(null (NEXT-STATE S 'b)))
                           (null (NEXT-STATE S 'd))
                           (not(null (NEXT-STATE S 'p))))
                          (list (NEXT-STATE S 'h) (NEXT-STATE S 'b) (NEXT-STATE S 'p)))

                    ((and (not(null (NEXT-STATE S 'h)))
                           (not(null (NEXT-STATE S 'b)))
                           (not(null (NEXT-STATE S 'd)))
                           (null (NEXT-STATE S 'p)))
                          (list (NEXT-STATE S 'h) (NEXT-STATE S 'b) (NEXT-STATE S 'd)))

                    ((and (not(null (NEXT-STATE S 'h)))
                           (not(null (NEXT-STATE S 'b)))
                           (not(null (NEXT-STATE S 'd)))
                           (not(null (NEXT-STATE S 'p))))
                          (list (NEXT-STATE S 'h) (NEXT-STATE S 'b) (NEXT-STATE S 'd) (NEXT-STATE S 'p)))

                    
    ))

; ON-PATH checks whether the current state is on the stack of states visited by
; this depth-first search. It takes two arguments: the current state (S) and the
; stack of states visited by DFS (STATES). It returns T if s is a member of
; states and NIL otherwise.
;explanation: Look through every element in STATES and check the individual elements of the
;list against the elements in S. If they are all the same, return true, otherwise return false.
(defun ON-PATH (S STATES) (cond
                           ((null STATES) nil)
                           ((and (or (not (listp (car STATES)))
                                     (null (car STATES)))
                                 (eq (car S) (car STATES))
                                 (eq (cadr S) (cadr STATES))
                                 (eq (caddr S) (caddr STATES))
                                 (eq (cadddr S) (cadddr STATES)))
                            T)
                           ((or (not (listp (car STATES)))
                               (null (car STATES))) nil)
                           ((and (eq (car S) (caar STATES))
                                 (eq (cadr S) (cadar STATES))
                                 (eq (caddr S) (caddar STATES))
                                 (eq (cadddr S) (car (cdddar STATES))))
                            T);found a state, stop recursion and return true
                           ((= 0 0)
                            (ON-PATH S (cdr STATES)))));wasn't this state, look at the next one
                        

; MULT-DFS is a helper function for DFS. It takes two arguments: a list of
; states from the initial state to the current state (PATH), and the legal
; successor states to the last, current state in the PATH (STATES). PATH is a
; first-in first-out list of states; that is, the first element is the initial
; state for the current search and the last element is the most recent state
; explored. MULT-DFS does a depth-first search on each element of STATES in
; turn. If any of those searches reaches the final state, MULT-DFS returns the
; complete path from the initial state to the goal state. Otherwise, it returns
; NIL.
;explanation: for every element of STATES, call DFS with it as the next state and PATH as the path. If
;any of these calls returns a non-nil value, we must have found the goal state through this search,
;so call it again and return its path.
(defun MULT-DFS (STATES PATH) (cond
                               ((null (car STATES)) nil);no legal moves from current state
                               ((DFS (car STATES) PATH) (DFS (car STATES) PATH))
                               ((null (cadr STATES)) nil);one element in states
                               ((DFS (cadr STATES) PATH) (DFS (cadr STATES) PATH))
                               ((null (caddr STATES)) nil);only two
                               ((DFS (caddr STATES) PATH) (DFS (caddr STATES) PATH))
                               ((null (cadddr STATES)) nil);only three
                               ((DFS (cadddr STATES) PATH) (DFS (cadddr STATES) PATH)))
    )

; DFS does a depth first search from a given state to the goal state. It
; takes two arguments: a state (S) and the path from the initial state to S
; (PATH). If S is the initial state in our search, PATH is set to NIL. DFS
; performs a depth-first search starting at the given state. It returns the path
; from the initial state to the goal state, if any, or NIL otherwise. DFS is
; responsible for checking if S is already the goal state, as well as for
; ensuring that the depth-first search does not revisit a node already on the
; search path.
;explanation: any time we check a state already on the path, return nil. If we find the goal state return
;the full path. Otherwise call mult-dfs with the total path we have traveled and the legal states we could
;get to from our current location as arguments.
(defun DFS (S PATH) (cond
                     ((ON-PATH S PATH) nil);check if S is already on the path, if yes return nil
                     ((FINAL-STATE S) (append PATH (list S)));check if S is goal state, if yes return True
                     ((null PATH) (MULT-DFS (SUCC-FN S) S));when path is nil the path is just S
                     ((and (= 4 (length PATH))
                           (atom (car PATH))) (MULT-DFS (SUCC-FN S) (append (list PATH) (list S))))
                     (T (MULT-DFS (SUCC-FN S) (append PATH (list S)))));otherwise call mult dfs after adding the state to our path and getting a list of valid next states
    )
