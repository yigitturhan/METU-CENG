{-# LANGUAGE FlexibleInstances #-}
import Data.List (sort, sortBy)
import Text.Printf (printf)

data Term = Const Integer | Pw Integer Power | Trig Integer Power Trigonometric | Exp Integer Power Exponential

data Power = Power Integer
data Polynomial = Polynomial [(Integer, Power)]
data Exponential = Exponential Polynomial
data Trigonometric = Sin Polynomial | Cos Polynomial

class Evaluable a where
    function :: a -> (Integer -> Double)

class Differentiable a where
    derivative :: a -> [Term]

-- You can use this as is
getRounded :: Double -> Double 
getRounded x = read s :: Double
               where s = printf "%.2f" x

-- INSTANCES FOR POWER

instance Show Power where
    show (Power a) = if a == 0 then "0" else if a == 1 then "x" else if a == -1 then "-x" else "x^"++(show a)

instance Eq Power where
    (Power a) == (Power b) = a == b

instance Ord Power where
    (Power a) <= (Power b) = a <= b
    (Power a) <(Power b) = a < b
    (Power a) >= (Power b) = a >= b
    (Power a) > (Power b) = a > b

instance Evaluable Power where
    function (Power a) b =fromIntegral b^a

instance Differentiable Power where
    derivative (Power a) = if a == 0 then [(Const 0)]
                           else [(Pw a (Power (a-1)))]

-- INSTANCES FOR POLYNOMIAL

instance Show Polynomial where
    show (Polynomial l) = if length l == 0 then "" 
                          else if getf(head l) == 0 then show(Polynomial (tail l))
                          else if getpint(gets(head l)) == 0 then show(getf(head l))++show (Polynomial (tail l))
                          else t++show(gets(head l))++w
                          where 
                                w = if show(Polynomial (tail l)) == "" then "" else " + "++show(Polynomial (tail l))
                                t = if (getf(head l)) == 1 then "" else if (getf(head l)) == -1 then "-" else show(getf(head l))

instance Evaluable Polynomial where
    function (Polynomial l) a = sum (map (\x->(fromIntegral (getf x))*(function (gets x) a)) l)

instance Differentiable Polynomial where
    derivative (Polynomial l) = derhelp l (map (\x->head (derivative (gets x))) l)

-- INSTANCES FOR TRIGONOMETRIC

instance Show Trigonometric where
    show (Sin (Polynomial l)) = if length l > 1 then "sin("++show(Polynomial l)++")"
                              else if getpint(gets(head l)) <= 1 && (getf (head l)) > 0 then "sin"++show(Polynomial [(head l)])
                              else "sin("++show(Polynomial [(head l)])++")"
    show (Cos (Polynomial l)) = if length l > 1 then "cos("++show(Polynomial l)++")"
                              else if getpint(gets(head l)) <= 1 && (getf (head l)) > 0 then "cos"++show(Polynomial [(head l)])
                              else "cos("++show(Polynomial [(head l)])++")"

instance Evaluable Trigonometric where
    function (Sin (Polynomial l)) a = getRounded (sin(function (Polynomial l) a))
    function (Cos (Polynomial l)) a = getRounded (cos(function (Polynomial l) a))

instance Differentiable Trigonometric where
    derivative (Sin p) = trihelp (derivative p) (Cos p)
    derivative (Cos p) = trihelp (derivative p) (Sin p)

-- INSTANCES FOR EXPONENTIAL

instance Show Exponential where
    show (Exponential p) = "e^("++show(p)++")"

instance Evaluable Exponential where
    function (Exponential p) a = getRounded (exp (function p a))

instance Differentiable Exponential where
    derivative (Exponential p) = exphelp (derivative p) (Exponential p)

-- INSTANCES FOR TERM

instance Show Term where
    show (Const a) = show a
    show (Pw a (Power p)) = if a == 0 then "0" else if a == 1 then show(Power p) else if a == -1 then "-"++show(Power p) else show(a)++show(Power p)
    show (Trig a (Power p1) t) = if a == 0 then "0" 
                                 else if p1 == 0 && a == 1 then show t
                                 else if p1 == 0 && a == -1 then "-"++show t
                                 else if p1 == 0 then show a ++ show t
                                 else if a == 1 then show(Power p1)++show t 
                                 else if a == -1 then "-"++show(Power p1)++show t 
                                 else show(a)++show(Power p1)++show t
    show (Exp a (Power p) e) = if a == 0 then "0"
                       else if a == 1 && p == 0 then show(e)
                       else if a == -1 && p == 0 then "-"++show(e)
                       else if p == 0 then show(a)++show(e)
                       else show(a)++show(Power p)++show(e)

instance Evaluable Term where
    function (Const a) b = fromIntegral a
    function (Pw a (Power b)) w = (fromIntegral a)*(function (Power b) w)
    function (Trig a (Power p1) t) w = (fromIntegral a)*(function(Power p1) w)*(function t w)
    function (Exp a p e) w = (fromIntegral a)*(function p w)*(function e w)

instance Differentiable Term where
    derivative (Const a) = [(Const 0)]
    derivative (Pw a (Power b)) = if b == 0 then [(Const 0)] 
                                  else if b == 1 then [(Const a)] 
                                  else [Pw (a*b) (Power (b-1))]
    derivative (Exp a (Power p) e) = if a == 0 then [(Const 0)]
                                     else if p == 0 then map (\x -> (sumas (Pw a (Power 0)) x)) (derivative e)
                                     else [expder ((derivative (Pw a (Power p)))!!0) e]++ exphel (Pw a (Power p)) (derivative e)
    derivative (Trig a (Power p) e) = if a == 0 then [(Const 0)]
                                      else if p == 0 then map (\x -> (sumas (Pw a (Power 0)) x)) (derivative e)
                                      else [trigder ((derivative (Pw a (Power p)))!!0) e]++exphel (Pw a (Power p)) (derivative e)
        
sumas (Pw f (Power t)) (Trig a (Power p) tr) = (Trig (a*f) (Power (p+t)) tr)
sumas  (Pw f (Power t)) (Exp a (Power p) e) = (Exp (f*a) (Power (p+t)) e)

exphel t l = if length l == 0 then []
                            else [sumas t (head l)]++exphel t (tail l)

expder (Pw a (Power b)) (Exponential p1) = (Exp a (Power b) (Exponential p1))

trigder (Pw a (Power b)) t = Trig a (Power b) t
-- INSTANCES FOR [TERM]

instance Evaluable [Term] where
    function l a = getRounded (sum (map (\x -> function x a ) l))

instance Differentiable [Term] where
    derivative l = simplify (derivative_helper l) []

derivative_helper l = if length l == 0 then []
                   else (derivative (head l))++(derivative_helper (tail l))

simplify l1 l2 = if length l1 == 0 then removezero l2
                 else simplify (tail l1) (simplify2 (head l1) l2)

simplify2 t l = if length l == 0 then [t]
                else if can_simp2 t (head l) then [simp t (head l)]++(tail l)
                else [head l]++simplify2 t (tail l)

simp (Pw a p) (Pw a1 p1) = if a+a1 == 0 then (Const 0) else (Pw (a+a1) p1)
simp (Const a) (Const b) = (Const (a+b))
simp (Exp a p e) (Exp a1 p1 e1) = if a+a1 == 0 then (Const 0) else (Exp (a+a1) p e)
simp (Trig a p t) (Trig a1 p1 t1) = if a+a1 == 0 then (Const 0) else (Trig (a+a1) p t)

are_eq_pol l1 l2 = sortBy sort_pol l1 == sortBy sort_pol l2

sort_pol (a1,p1) (a2,p2) = if a1 > a2 then GT else if a1 < a2 then LT else if p1 < p2 then LT else if p1 > p2 then GT else EQ

can_simp (Pw a (Power b)) (Pw a1 (Power b1)) = b == b1
can_simp (Const a) (Const b) = True
can_simp (Exp a (Power b) (Exponential (Polynomial pol1))) (Exp a1 (Power b1) (Exponential (Polynomial pol2))) = (b==b1) && (are_eq_pol pol1 pol2)
can_simp (Trig a (Power p) (Sin (Polynomial pol1))) (Trig a1 (Power p1) (Sin (Polynomial pol2))) = (p == p1) && (are_eq_pol pol1 pol2)
can_simp (Trig a (Power p) (Cos (Polynomial pol1))) (Trig a1 (Power p1) (Sin (Polynomial pol2))) = False
can_simp (Trig a (Power p) (Sin (Polynomial pol1))) (Trig a1 (Power p1) (Cos (Polynomial pol2))) = False
can_simp (Trig a (Power p) (Cos (Polynomial pol1))) (Trig a1 (Power p1) (Cos (Polynomial pol2))) = (p == p1) && (are_eq_pol pol1 pol2)

can_simp2 t1 t2 = if type_of_term t1 == type_of_term t2 then can_simp t1 t2 else False

removezero l = if length l == 0 then []
               else if isconst (head l) then removezero (tail l)
               else [(head l)] ++ removezero (tail l)
type_of_term (Pw a p) = 1
type_of_term (Const a) = 0
type_of_term (Trig a p (Sin p1)) = 2
type_of_term (Trig a p (Cos p1)) = 3
type_of_term (Exp a p e) = 4
getf (a,b) = a
gets (a,b) = b
isconst (Const a) = a == 0
isconst (Pw i p) = False
isconst (Trig a p t) = False
isconst (Exp a p e) = False
getpint (Power a) = a
getpoint (Pw i p) = i
getpoint2 (Pw i p) = -i
getpop (Pw i p) = p
derhelp l1 l2 = if length l1 == 0 then []
                else if (getf (head l1) == 0 || isconst (head l2)) then derhelp (tail l1) (tail l2)
                else [(Pw (getf (head l1)*(getpoint (head l2))) (getpop (head l2)))]++ (derhelp (tail l1) (tail l2))
exphelp l1 e = if length l1 == 0 then []
               else [Exp (getpoint (head l1)) (getpop (head l1)) e] ++ exphelp (tail l1) e
trihelp l1 (Sin p) = if length l1 == 0 then []
               else [Trig (getpoint2 (head l1)) (getpop (head l1)) (Sin p)] ++ trihelp (tail l1) (Sin p)
trihelp l1 (Cos p) = if length l1 == 0 then []
               else [Trig (getpoint (head l1)) (getpop (head l1)) (Cos p)] ++ trihelp (tail l1) (Cos p)   
