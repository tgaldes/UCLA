;
; CS161 Hw3: Sokoban
; 
; *********************
;    READ THIS FIRST
; ********************* 
;
; All functions that you need to modify are marked with 'EXERCISE' in their header comments.
; Do not modify a-star.lsp.
; This file also contains many helper functions. You may call any of them in your functions.
;
; *Warning*: The provided A* code only supports the maximum cost of 4999 for any node.
; That is f(n)=g(n)+h(n) < 5000. So, be careful when you write your heuristic functions.
; Do not make them return anything too large.
;
; For Allegro Common Lisp users: The free version of Allegro puts a limit on memory.
; So, it may crash on some hard sokoban problems and there is no easy fix (unless you buy 
; Allegro). 
; Of course, other versions of Lisp may also crash if the problem is too hard, but the amount
; of memory available will be relatively more relaxed.
; Improving the quality of the heuristic will mitigate this problem, as it will allow A* to
; solve hard problems with fewer node expansions.
; 
; In either case, this limitation should not significantly affect your grade.
; 
; Remember that most functions are not graded on efficiency (only correctness).
; Efficiency can only influence your heuristic performance in the competition (which will
; affect your score).
;  
;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; General utility functions
; They are not necessary for this homework.
; Use/modify them for your own convenience.
;

;
; For reloading modified code.
; I found this easier than typing (load "filename") every time. 
;
(defun reload()
  (load "hw3.lsp")
  )

;
; For loading a-star.lsp.
;
(defun load-a-star()
  (load "a-star.lsp"))

;
; Reloads hw3.lsp and a-star.lsp
;
(defun reload-all()
  (reload)
  (load-a-star)
  )

;
; A shortcut function.
; goal-test and next-states stay the same throughout the assignment.
; So, you can just call (sokoban <init-state> #'<heuristic-name>).
; 
;
(defun sokoban (s h)
  (a* s #'goal-test #'next-states h)
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; end general utility functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; We now begin actual Sokoban code
;

; Define some global variables
(setq blank 0)
(setq wall 1)
(setq box 2)
(setq keeper 3)
(setq star 4)
(setq boxstar 5)
(setq keeperstar 6)

; Some helper functions for checking the content of a square
(defun isBlank (v)
  (= v blank)
  )

(defun isWall (v)
  (= v wall)
  )

(defun isBox (v)
  (= v box)
  )

(defun isKeeper (v)
  (= v keeper)
  )

(defun isStar (v)
  (= v star)
  )

(defun isBoxStar (v)
  (= v boxstar)
  )

(defun isKeeperStar (v)
  (= v keeperstar)
  )

;
; Helper function of getKeeperPosition
;
(defun getKeeperColumn (r col)
  (cond ((null r) nil)
	(t (if (or (isKeeper (car r)) (isKeeperStar (car r)))
	       col
	     (getKeeperColumn (cdr r) (+ col 1))
	     );end if
	   );end t
	);end cond
  )

;
; getKeeperPosition (s firstRow)
; Returns a list indicating the position of the keeper (c r).
; 
; Assumes that the keeper is in row >= firstRow.
; The top row is the zeroth row.
; The first (right) column is the zeroth column.
;
(defun getKeeperPosition (s row)
  (cond ((null s) nil)
	(t (let ((x (getKeeperColumn (car s) 0)))
	     (if x
		 ;keeper is in this row
		 (list x row)
		 ;otherwise move on
		 (getKeeperPosition (cdr s) (+ row 1))
		 );end if
	       );end let
	 );end t
	);end cond
  );end defun

;
; cleanUpList (l)
; returns l with any NIL element removed.
; For example, if l is '(1 2 NIL 3 NIL), returns '(1 2 3).
;
(defun cleanUpList (L)
  (cond ((null L) nil)
	(t (let ((cur (car L))
		 (res (cleanUpList (cdr L)))
		 )
	     (if cur 
		 (cons cur res)
		  res
		 )
	     );end let
	   );end t
	);end cond
  );end 

; EXERCISE: Modify this function to return true (t)
; if and only if s is a goal state of a Sokoban game.
; (no box is on a non-goal square)
;
; Currently, it always returns NIL. If A* is called with
; this function as the goal testing function, A* will never
; terminate until the whole search space is exhausted.
;
(defun goal-test (s)
  ;we'll return true if there is no "2" on the board
  (cond
   ((null s) T)
   ((atom s) (null (= 2 s)));check if we've got a single two
   ((null (car s)) T);we've gotten to the end without finding a two, so return true
   
   
   ((and (goal-test (car s))
         (goal-test (cdr s))) T));end cond
  );end defun

; EXERCISE: Modify this function to return the list of 
; sucessor states of s.
;
; This is the top-level next-states (successor) function.
; Some skeleton code is provided below.
; You may delete them totally, depending on your approach.
; 
; If you want to use it, you will need to set 'result' to be 
; the set of states after moving the keeper in each of the 4 directions.
; A pseudo-code for this is:
; 
; ...
; (result (list (try-move s UP) (try-move s DOWN) (try-move s LEFT) (try-move s RIGHT)))
; ...
; 
; You will need to define the function try-move and decide how to represent UP,DOWN,LEFT,RIGHT.
; Any NIL result returned from try-move can be removed by cleanUpList.
; 
;
(defun next-states (s)
  (let* ((pos (getKeeperPosition s 0))
	 (x (car pos))
	 (y (cadr pos))
	 ;x and y are now the coordinate of the keeper in s.
	 (result (list (try-move s 1 x y) (try-move s 2 x y) (try-move s 3 x y) (try-move s 4 x y)))
	 )
    (cleanUpList result);end
   );end let
  );

;tries to move keeper in board s direction dir (1 north 2 east etc)
;returns the new state if the specified move is possible, nil otherwise
;x and y are the x and y coordinates of the keeper in the new board
(defun try-move (s dir x y) (cond
                             ((on-path (get-tiles s dir x y) '((3 0 0)(3 0 1)(3 0 2)(3 0 4)(3 0 5)(3 0 nil))) 
                              (set-tiles s (list 0 3 (caddr (get-tiles s dir x y))) dir x y));moves onto a blank space
                             ((on-path (get-tiles s dir x y) '((3 1 0)(3 1 1)(3 1 2)(3 1 4)(3 1 5)(3 1 nil)(3 2 1)(3 2 2)(3 2 5)(3 2 nil)(3 5 1)(3 5 2)(3 5 5)(3 5 nil)(3 nil nil)))
                              nil);try to move onto a wall, push box into wall/box/box on star, push box on star into wall/box/box on star
                             ((on-path (get-tiles s dir x y) '((3 2 0)))
                              (set-tiles s (list 0 3 2) dir x y));move box onto a blank space
                             ((on-path (get-tiles s dir x y) '((3 2 4)))
                              (set-tiles s (list 0 3 5) dir x y));push a box onto a star!!
                             ((on-path (get-tiles s dir x y) '((3 4 0)(3 4 1)(3 4 2)(3 4 4)(3 4 5)(3 4 nil)))
                              (set-tiles s (list 0 6 (caddr (get-tiles s dir x y))) dir x y));move onto a star
                             ((on-path (get-tiles s dir x y) '((3 5 4)))
                              (set-tiles s (list 0 6 5) dir x y));move box from star to star and now keeper is on a star

                             ;below case are for when keeper is on a star to begin the turn
                             ((on-path (get-tiles s dir x y) '((6 0 0)(6 0 1)(6 0 2)(6 0 4)(6 0 5)(6 0 nil))) 
                              (set-tiles s (list 4 3 (caddr (get-tiles s dir x y))) dir x y));moves onto a blank space from a star
                             ((on-path (get-tiles s dir x y) '((6 1 0)(6 1 1)(6 1 2)(6 1 4)(6 1 5)(6 1 nil)(6 2 1)(6 2 2)(6 2 5)(6 2 nil)(6 5 1)(6 5 2)(6 5 5)(6 5 nil)(6 nil nil)))
                              nil);try to move onto a wall from star, push box into wall/box/box on star from star, push box on star into wall/box/box on star from star
                             ((on-path (get-tiles s dir x y) '((6 2 0)))
                              (set-tiles s (list 4 3 2) dir x y));move box onto a blank space when keeper starts on a star
                             ((on-path (get-tiles s dir x y) '((6 2 4)))
                              (set-tiles s (list 4 3 5) dir x y));push a box onto a star!! when keeper starts on a star
                             ((on-path (get-tiles s dir x y) '((6 4 0)(6 4 1)(6 4 2)(6 4 4)(6 4 5)(6 4 nil)))
                              (set-tiles s (list 4 6 (caddr (get-tiles s dir x y))) dir x y));move onto a star from a star
                             ((on-path (get-tiles s dir x y) '((6 5 4)))
                              (set-tiles s (list 4 6 5) dir x y));move box from star to star and now keeper is on a star when keeper started on a star
                             (T nil)))






;scenarios
;keeper would move into wall:nil
;keeper moves box into wall:nil
;keeper moves box into another box
;keeper moves from blank to blank
;keeper moves from blank to a star
;keeper moves from star to blank
;keeper moves from star to star
;keeper moves box into empty space from empty space
;keeper moves box onto goal state
;keeper moves box off of goal state and moves onto goal state

;helper function for try move
;takes state, direction, and coordinates of the keeper
;returns a list of (keeper tile, tile in given direction closest to keeper, tile in dir two spaces from keeper)
;last element of the list is nil if there is no tile two spaces from the keeper in that direction
;returns nil if not called with a dir = 1 2 3 or 4
(defun get-tiles (s dir x y) (cond
                              ((= 1 dir) (list (get-tile s x y) (get-tile s x (- y 1)) (get-tile s x (- y 2))));going north
                              ((= 3 dir) (list (get-tile s x y) (get-tile s x (+ y 1)) (get-tile s x (+ y 2))));south
                              ((= 2 dir) (list (get-tile s x y) (get-tile s (+ x 1) y) (get-tile s (+ x 2) y)));going east
                              ((= 4 dir) (list (get-tile s x y) (get-tile s (- x 1) y) (get-tile s (- x 2) y)));west
                              (T nil)));bad input

;helper for get-tiles
;gets the tile at the x y coordinates in state s
;handles negative inputs by returning nil
;inputs that are too positive are handled fine by nthcdr func
(defun get-tile(s x y) (cond
                        ((< x 0) nil)
                        ((< y 0) nil)
                        (T (car (nthcdr x (car (nthcdr y s)))))))

;helper function for try-move
;returns true if s is equal to any list in states, nil otherwise
(defun on-path (S STATES) (cond
                           ((null STATES) nil)
                           ((and (or (not (listp (car STATES)))
                                     (null (car STATES)))
                                 (eq (car S) (car STATES))
                                 (eq (cadr S) (cadr STATES))
                                 (eq (caddr S) (caddr STATES)))
                            T)
                           ((or (not (listp (car STATES)))
                               (null (car STATES))) nil)
                           ((and (eq (car S) (caar STATES))
                                 (eq (cadr S) (cadar STATES))
                                 (eq (caddr S) (caddar STATES)))
                            T);found a state, stop recursion and return true
                           ((= 0 0)
                            (ON-PATH S (cdr STATES)))));wasn't this state, look at the next one

;helper for try-move
;sets three spaces in s to the spaces in tiles, starting from x,y and going in direction dir
;returns the modified state
(defun set-tiles (s tiles dir x y) (cond
                                    ((= 1 dir) (set-square (set-square (set-square s (car tiles) x y) (cadr tiles) x (- y 1)) (caddr tiles) x (- y 2)))
                                    ((= 3 dir) (set-square (set-square (set-square s (car tiles) x y) (cadr tiles) x (+ y 1)) (caddr tiles) x (+ y 2)))
                                    ((= 2 dir) (set-square (set-square (set-square s (car tiles) x y) (cadr tiles) (+ x 1) y) (caddr tiles) (+ x 2) y))
                                    ((= 4 dir) (set-square (set-square (set-square s (car tiles) x y) (cadr tiles) (- x 1) y) (caddr tiles) (- x 2) y))
                                    (T nil)))

;helper for set-tiles
;will return a state with x,y set to tile
;this operates on a list of lists
;checks bounds of x and y
(defun set-square (s tile x y) (cond
                                ((or (> (+ y 1) (length s))
                                     (< y 0)) s);check that y is in bounds, return unmodified state if it's not
                                 ((= y 0) (append (list (set-tile (car s) tile x)) (cdr s)))
                                 (T (append (list(car s)) (set-square (cdr s) tile x (- y 1))))))

;helper for set-square
;set the yth element of the list s to tile and returns it
;this operates on a list of atoms
(defun set-tile (s tile x) (cond
                            ((or (> (+ x 1) (length s))
                                 (< x 0)) s);check that x is in bounds, return unmodified row if it's not
                            ((= 0 x) (append (list tile) (cdr s)))
                            (T (append (list (car s)) (set-tile (cdr s) tile (- x 1))))))

; EXERCISE: Modify this function to compute the trivial 
; admissible heuristic.
;
(defun h0 (s)
  (cond (T (+)))
  )

; EXERCISE: Modify this function to compute the 
; number of misplaced boxes in s.
;
(defun h1 (s) (cond
   ((null s) (+))
   ((and (atom s) 
         (= 2 s)) 1);add one when we find a two
   ((atom s) (+));when we have an atom not = 2 we return 0
   ((null (car s)) (+))
   
   
   (T (+ (h1 (car s)) (h1 (cdr s))));add number in first list with number in the rest
  );end cond
  );end defun

; EXERCISE: Change the name of this function to h<UID> where
; <UID> is your actual student ID number. Then, modify this 
; function to compute an admissible heuristic value of s. 
; 
; This function will be entered in the competition.
; Objective: make A* solve problems as fast as possible.
; The Lisp 'time' function can be used to measure the 
; running time of a function call.
;
(defun h204275320 (s) 
  (helper s))

;a helper for my h func
(defun helper(s)
  (- (goal-to-block (getKeeperPosition s 0) (get-blocks s) (get-goals s)) 1))

;helper for my h func
;returns a list of coordinates of every block that is not on a goal state
(defun get-blocks(s)
  (get-coords s 2 0 0))

;helper for my h func
;returns a list of coordinates (x y x y....) of every goal state that does not have a block on it
(defun get-goals(s) 
  (append (get-coords s 4 0 0) (get-coords s 6 0 0)))

;helper for get-blocks and get-goals
;returns a list of coordinates for every square in s that is equal to t
;called from above with x and y = 0 to start with
(defun get-coords(s v x y) 
  (let ((n (car (nthcdr x (car (nthcdr y s))))));get value of square at these coordinates
  (cond ((null n) '())
        ((and (= 0 y)
              (= v n)) (append (list (list x y)) (get-coords s v (+ 1 x) y) (get-coords s v x (+ 1 y))));start two new searches when y = 0, here we add these coords to the list because the we foudn a match
        ((= 0 y) (append (get-coords s v (+ 1 x) y) (get-coords s v x (+ 1 y))));start two new searches
        ((= v n) (append (list (list x y)) (get-coords s v x (+ 1 y))));continue with one search and add this coord
        (T (get-coords s v x (+ 1 y))))));no match and continue only this search

;helper for my h func
;takes a list of 
(defun block-to-goal(start blocks goals)
  (let* ((l (next-move-dist 999 start goals nil nil))
        (d (car l))
        (n (car (nthcdr 4 l)))
        (g (cadddr l)))
    (cond ((= 0 (length blocks)) d);we're out of goals, so return the distance from final block to the goal
          (t (+ d (goal-to-block n blocks g))))));add the distance of this block to the nextStart goal to the rest of the distances

(defun goal-to-block (start blocks goals)
  (let* ((l (next-move-dist 999 start blocks nil nil))
        (d (car l))
        (n (car (nthcdr 4 l)))
        (b (cadddr l)))
    (cond (T (+ d (block-to-goal n b goals))))));we'll always have another block to move


;helper for my h func
;computes the smallest manhattan dist between start and any coords in end
;returns a list of (dist, (startx starty), (null) <-since we've check every point, (list of all remaining end points), (coords of closest endpoint))
;this complicated return list will allow us to keep track of which blocks/goals we have
;calculated a move towards in our heuristic
(defun next-move-dist(dist start ends visited bestEnd) (cond
                                   ((and (null ends)
                                        (null visited)
                                        (null bestEnd)) (list 0 start ends visited bestEnd))
                                   ((null ends) (list dist start ends visited bestEnd));end of recursion, we've checked everything so return all the values
                                   (( < (manhattan-dist start (car ends)) dist) 
                                    (next-move-dist (manhattan-dist start (car ends)) start (cdr ends) (append (cleanUpList  (list bestEnd)) visited) (car ends)));front of ends is best so far
                                   (T (next-move-dist dist start (cdr ends) (append (list(car ends))  visited) bestEnd))));we've seen better

;helper for next-move-dist
;computes the manhattan distance bewteen start and end
;returns (dist, (endx, endy))
(defun manhattan-dist(start end)
  (cond ((and (<= 0 (- (car end) (car start)));each case is four one of four possible sign combos we can get from subtraction
              (<= 0 (- (cadr end) (cadr start)))) (+ (- (car end) (car start)) (- (cadr end) (cadr start))) );down to the right
        ((and (<= 0 (- (car end) (car start)))
              (> 0 (- (cadr end) (cadr start))))  (+ (- (car end) (car start)) (- (cadr start) (cadr end))));up to the right
        ((and (> 0 (- (car end) (car start)))
              (<= 0 (- (cadr end) (cadr start)))) (+ (- (car start) (car end)) (- (cadr end) (cadr start))));down to the left
        ((and (> 0 (- (car end) (car start)))
              (> 0 (- (cadr end) (cadr start))))  (+ (- (car start) (car end)) (- (cadr start) (cadr end))));up to the left
        (T nil)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#|
 | Some predefined problems.
 | Each problem can be visualized by calling (printstate <problem>). For example, (printstate p1).
 | Problems are ordered roughly by their difficulties.
 | For most problems, we also privide 2 additional number per problem:
 |    1) # of nodes expanded by A* using our next-states and h0 heuristic.
 |    2) the depth of the optimal solution.
 | These numbers are located at the comments of the problems. For example, the first problem below 
 | was solved by 80 nodes expansion of A* and its optimal solution depth is 7.
 | 
 | Your implementation may not result in the same number of nodes expanded, but it should probably
 | give something in the same ballpark. As for the solution depth, any admissible heuristic must 
 | make A* return an optimal solution. So, the depths of the optimal solutions provided could be used
 | for checking whether your heuristic is admissible.
 |
 | Warning: some problems toward the end are quite hard and could be impossible to solve without a good heuristic!
 | 
 |#

;(80,7)
(setq p1 '((1 1 1 1 1 1)
	   (1 0 3 0 0 1)
	   (1 0 2 0 0 1)
	   (1 1 0 1 1 1)
	   (1 0 0 0 0 1)
	   (1 0 0 0 4 1)
	   (1 1 1 1 1 1)))

;(110,10)
(setq p2 '((1 1 1 1 1 1 1)
	   (1 0 0 0 0 0 1) 
	   (1 0 0 0 0 0 1) 
	   (1 0 0 2 1 4 1) 
	   (1 3 0 0 1 0 1)
	   (1 1 1 1 1 1 1)))

;(211,12)
(setq p3 '((1 1 1 1 1 1 1 1 1)
	   (1 0 0 0 1 0 0 0 1)
	   (1 0 0 0 2 0 3 4 1)
	   (1 0 0 0 1 0 0 0 1)
	   (1 0 0 0 1 0 0 0 1)
	   (1 1 1 1 1 1 1 1 1)))

;(300,13)
(setq p4 '((1 1 1 1 1 1 1)
	   (0 0 0 0 0 1 4)
	   (0 0 0 0 0 0 0)
	   (0 0 1 1 1 0 0)
	   (0 0 1 0 0 0 0)
	   (0 2 1 0 0 0 0)
	   (0 3 1 0 0 0 0)))

;(551,10)
(setq p5 '((1 1 1 1 1 1)
	   (1 1 0 0 1 1)
	   (1 0 0 0 0 1)
	   (1 4 2 2 4 1)
	   (1 0 0 0 0 1)
	   (1 1 3 1 1 1)
	   (1 1 1 1 1 1)))

;(722,12)
(setq p6 '((1 1 1 1 1 1 1 1)
	   (1 0 0 0 0 0 4 1)
	   (1 0 0 0 2 2 3 1)
	   (1 0 0 1 0 0 4 1)
	   (1 1 1 1 1 1 1 1)))

;(1738,50)
(setq p7 '((1 1 1 1 1 1 1 1 1 1)
	   (0 0 1 1 1 1 0 0 0 3)
	   (0 0 0 0 0 1 0 0 0 0)
	   (0 0 0 0 0 1 0 0 1 0)
	   (0 0 1 0 0 1 0 0 1 0)
	   (0 2 1 0 0 0 0 0 1 0)
	   (0 0 1 0 0 0 0 0 1 4)))

;(1763,22)
(setq p8 '((1 1 1 1 1 1)
	   (1 4 0 0 4 1)
	   (1 0 2 2 0 1)
	   (1 2 0 1 0 1)
	   (1 3 0 0 4 1)
	   (1 1 1 1 1 1)))

;(1806,41)
(setq p9 '((1 1 1 1 1 1 1 1 1) 
	   (1 1 1 0 0 1 1 1 1) 
	   (1 0 0 0 0 0 2 0 1) 
	   (1 0 1 0 0 1 2 0 1) 
	   (1 0 4 0 4 1 3 0 1) 
	   (1 1 1 1 1 1 1 1 1)))

;(10082,51)
(setq p10 '((1 1 1 1 1 0 0)
	    (1 0 0 0 1 1 0)
	    (1 3 2 0 0 1 1)
	    (1 1 0 2 0 0 1)
	    (0 1 1 0 2 0 1)
	    (0 0 1 1 0 0 1)
	    (0 0 0 1 1 4 1)
	    (0 0 0 0 1 4 1)
	    (0 0 0 0 1 4 1)
	    (0 0 0 0 1 1 1)))

;(16517,48)
(setq p11 '((1 1 1 1 1 1 1)
	    (1 4 0 0 0 4 1)
	    (1 0 2 2 1 0 1)
	    (1 0 2 0 1 3 1)
	    (1 1 2 0 1 0 1)
	    (1 4 0 0 4 0 1)
	    (1 1 1 1 1 1 1)))

;(22035,38)
(setq p12 '((0 0 0 0 1 1 1 1 1 0 0 0)
	    (1 1 1 1 1 0 0 0 1 1 1 1)
	    (1 0 0 0 2 0 0 0 0 0 0 1)
	    (1 3 0 0 0 0 0 0 0 0 0 1)
	    (1 0 0 0 2 1 1 1 0 0 0 1)
	    (1 0 0 0 0 1 0 1 4 0 4 1)
	    (1 1 1 1 1 1 0 1 1 1 1 1)))

;(26905,28)
(setq p13 '((1 1 1 1 1 1 1 1 1 1)
	    (1 4 0 0 0 0 0 2 0 1)
	    (1 0 2 0 0 0 0 0 4 1)
	    (1 0 3 0 0 0 0 0 2 1)
	    (1 0 0 0 0 0 0 0 0 1)
	    (1 0 0 0 0 0 0 0 4 1)
	    (1 1 1 1 1 1 1 1 1 1)))

;(41715,53)
(setq p14 '((0 0 1 0 0 0 0)
	    (0 2 1 4 0 0 0)
	    (0 2 0 4 0 0 0)	   
	    (3 2 1 1 1 0 0)
	    (0 0 1 4 0 0 0)))

;(48695,44)
(setq p15 '((1 1 1 1 1 1 1)
	    (1 0 0 0 0 0 1)
	    (1 0 0 2 2 0 1)
	    (1 0 2 0 2 3 1)
	    (1 4 4 1 1 1 1)
	    (1 4 4 1 0 0 0)
	    (1 1 1 1 0 0 0)
	    ))

;(91344,111)
(setq p16 '((1 1 1 1 1 0 0 0)
	    (1 0 0 0 1 0 0 0)
	    (1 2 1 0 1 1 1 1)
	    (1 4 0 0 0 0 0 1)
	    (1 0 0 5 0 5 0 1)
	    (1 0 5 0 1 0 1 1)
	    (1 1 1 0 3 0 1 0)
	    (0 0 1 1 1 1 1 0)))

;(3301278,76)
(setq p17 '((1 1 1 1 1 1 1 1 1 1)
	    (1 3 0 0 1 0 0 0 4 1)
	    (1 0 2 0 2 0 0 4 4 1)
	    (1 0 2 2 2 1 1 4 4 1)
	    (1 0 0 0 0 1 1 4 4 1)
	    (1 1 1 1 1 1 0 0 0 0)))

;(??,25)
(setq p18 '((0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0)
	    (0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0)
	    (1 1 1 1 1 0 0 0 0 0 0 1 1 1 1 1)
	    (0 0 0 0 0 1 0 0 0 0 1 0 0 0 0 0)
	    (0 0 0 0 0 0 1 0 0 1 0 0 0 0 0 0)
	    (0 0 0 0 0 0 0 0 3 0 0 0 0 0 0 0)
	    (0 0 0 0 0 0 1 0 0 1 0 0 0 0 0 0)
	    (0 0 0 0 0 1 0 0 0 0 1 0 0 0 0 0)
	    (1 1 1 1 1 0 0 0 0 0 0 1 1 1 1 1)
	    (0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0)
	    (0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0)
	    (0 0 0 0 1 0 0 0 0 0 4 1 0 0 0 0)
	    (0 0 0 0 1 0 2 0 0 0 0 1 0 0 0 0)	    
	    (0 0 0 0 1 0 2 0 0 0 4 1 0 0 0 0)
	    ))
;(??,21)
(setq p19 '((0 0 0 1 0 0 0 0 1 0 0 0)
	    (0 0 0 1 0 0 0 0 1 0 0 0)
	    (0 0 0 1 0 0 0 0 1 0 0 0)
	    (1 1 1 1 0 0 0 0 1 1 1 1)
	    (0 0 0 0 1 0 0 1 0 0 0 0)
	    (0 0 0 0 0 0 3 0 0 0 2 0)
	    (0 0 0 0 1 0 0 1 0 0 0 4)
	    (1 1 1 1 0 0 0 0 1 1 1 1)
	    (0 0 0 1 0 0 0 0 1 0 0 0)
	    (0 0 0 1 0 0 0 0 1 0 0 0)
	    (0 0 0 1 0 2 0 4 1 0 0 0)))

;(??,??)
(setq p20 '((0 0 0 1 1 1 1 0 0)
	    (1 1 1 1 0 0 1 1 0)
	    (1 0 0 0 2 0 0 1 0)
	    (1 0 0 5 5 5 0 1 0)
	    (1 0 0 4 0 4 0 1 1)
	    (1 1 0 5 0 5 0 0 1)
	    (0 1 1 5 5 5 0 0 1)
	    (0 0 1 0 2 0 1 1 1)
	    (0 0 1 0 3 0 1 0 0)
	    (0 0 1 1 1 1 1 0 0)))

;(??,??)
(setq p21 '((0 0 1 1 1 1 1 1 1 0)
	    (1 1 1 0 0 1 1 1 1 0)
	    (1 0 0 2 0 0 0 1 1 0)
	    (1 3 2 0 2 0 0 0 1 0)
	    (1 1 0 2 0 2 0 0 1 0)
	    (0 1 1 0 2 0 2 0 1 0)
	    (0 0 1 1 0 2 0 0 1 0)
	    (0 0 0 1 1 1 1 0 1 0)
	    (0 0 0 0 1 4 1 0 0 1)
	    (0 0 0 0 1 4 4 4 0 1)
	    (0 0 0 0 1 0 1 4 0 1)
	    (0 0 0 0 1 4 4 4 0 1)
	    (0 0 0 0 1 1 1 1 1 1)))

;(??,??)
(setq p22 '((0 0 0 0 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0)
	    (0 0 0 0 1 0 0 0 1 0 0 0 0 0 0 0 0 0 0)
	    (0 0 0 0 1 2 0 0 1 0 0 0 0 0 0 0 0 0 0)
	    (0 0 1 1 1 0 0 2 1 1 0 0 0 0 0 0 0 0 0)
	    (0 0 1 0 0 2 0 2 0 1 0 0 0 0 0 0 0 0 0)
	    (1 1 1 0 1 0 1 1 0 1 0 0 0 1 1 1 1 1 1)
	    (1 0 0 0 1 0 1 1 0 1 1 1 1 1 0 0 4 4 1)
	    (1 0 2 0 0 2 0 0 0 0 0 0 0 0 0 0 4 4 1)
	    (1 1 1 1 1 0 1 1 1 0 1 3 1 1 0 0 4 4 1)
	    (0 0 0 0 1 0 0 0 0 0 1 1 1 1 1 1 1 1 1)
	    (0 0 0 0 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#|
 | Utility functions for printing states and moves.
 | You do not need to understand any of the functions below this point.
 |#

;
; Helper function of prettyMoves
; from s1 --> s2
;
(defun detectDiff (s1 s2)
  (let* ((k1 (getKeeperPosition s1 0))
	 (k2 (getKeeperPosition s2 0))
	 (deltaX (- (car k2) (car k1)))
	 (deltaY (- (cadr k2) (cadr k1)))
	 )
    (cond ((= deltaX 0) (if (> deltaY 0) 'DOWN 'UP))
	  (t (if (> deltaX 0) 'RIGHT 'LEFT))
	  );end cond
    );end let
  );end defun

;
; Translates a list of states into a list of moves.
; Usage: (prettyMoves (a* <problem> #'goal-test #'next-states #'heuristic))
;
(defun prettyMoves (m)
  (cond ((null m) nil)
	((= 1 (length m)) (list 'END))
	(t (cons (detectDiff (car m) (cadr m)) (prettyMoves (cdr m))))
	);end cond
  );

;
; Print the content of the square to stdout.
;
(defun printSquare (s)
  (cond ((= s blank) (format t " "))
	((= s wall) (format t "#"))
	((= s box) (format t "$"))
	((= s keeper) (format t "@"))
	((= s star) (format t "."))
	((= s boxstar) (format t "*"))
	((= s keeperstar) (format t "+"))
	(t (format t "|"))
	);end cond
  )

;
; Print a row
;
(defun printRow (r)
  (dolist (cur r)
    (printSquare cur)    
    )
  );

;
; Print a state
;
(defun printState (s)
  (progn    
    (dolist (cur s)
      (printRow cur)
      (format t "~%")
      )
    );end progn
  )

;
; Print a list of states with delay.
;
(defun printStates (sl delay)
  (dolist (cur sl)
    (printState cur)
    (sleep delay)
    );end dolist
  );end defun
