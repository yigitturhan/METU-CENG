
import Text.Printf

-- PE1: Recipe Calculator
-- A recipe calculator that
-- calculates: how much a recipe costs to make, what can be made with the
-- ingredients already available, and how much extra ingredients need to
-- be bought in order to make a recipe.

-- Recipe = Recipe Name [(Ingredient, Quantity)]
data Recipe = Recipe String [(String, Double)] deriving Show

-- Price = Price Ingredient Quantity Price
data Price = Price String Double Double deriving Show

getpricename :: Price -> String
getpricename (Price a b c) = a
getprice :: Price -> Double
getprice  (Price a b c) = c/b
getrecipelist :: Recipe -> [(String,Double)]
getrecipelist (Recipe st l) = l
getname :: (String,Double) -> String
getname (a,b) = a
getp :: (String, Double) -> Double
getp (a,b) = b
getRounded :: Double -> Double 
getRounded x = read s :: Double
               where s = printf "%.2f" x
----------------------------------------------------------------------------------------------------------------------------------------------------------
-- Calculate how much the given amount of the given ingredient costs
getIngredientCost :: (String, Double)->[Price]->Double
getIngredientCost (s,d) p=getRounded d*(getprice((filter(\x->getpricename x == s)p)!!0))
----------------------------------------------------------------------------------------------------------------------------------------------------------
-- Calculate how much it costs to buy all the ingredients of a recipe
recipeCost :: Recipe->[Price]->Double
recipeCost (Recipe s []) p = 0
recipeCost (Recipe s w) p = getRounded(getp(head(w)))*(getprice((filter(\x->getpricename x == (getname(head w)))p)!!0))+(recipeCost(Recipe s (tail(w)))p)
----------------------------------------------------------------------------------------------------------------------------------------------------------
-- Given a list of how much you already have of each ingredient,
-- calculate how much of which ingredients are missing for a recipe
missingIngredients :: Recipe->[(String, Double)]->[(String, Double)]
missingIngredients (Recipe st []) _ = []
missingIngredients (Recipe st m) s = help2([help (head m) s]++(missingIngredients(Recipe st (tail m))s))
help :: (String,Double) -> [(String,Double)] -> (String,Double)
help m s = (getname m,c)
    where c = if (length(filter(\x->getname m == (getname x))s)/=0) then ((getp m)-getp((filter(\x->getname m == (getname x))s)!!0)) else (getp m)
help2 :: [(String,Double)] -> [(String,Double)]
help2 a = filter(\x->(getp x)>0) a
----------------------------------------------------------------------------------------------------------------------------------------------------------
-- Given a list of ingredients in your kitchen, calculate what you would
-- have left after making the given recipe. If there isn't enough of an
-- ingredient, the recipe cannot be made! In that case the amount
-- of ingredient does not change.
makeRecipe :: [(String, Double)]->Recipe->[(String, Double)]
makeRecipe [] _ = []
makeRecipe s (Recipe st m) = a
    where a = if (isdone s (Recipe st m)) then makeRecipeh s (Recipe st m) else s
makeRecipeh :: [(String, Double)]->Recipe->[(String, Double)]
makeRecipeh [] _ = []
makeRecipeh s (Recipe st m) = [makehelper (head s) m]++(makeRecipeh (tail s) (Recipe st m))
makehelper :: (String,Double)->[(String,Double)]->(String,Double)
makehelper s1 m = ((getname s1),c)
    where c = if length(filter(\x->(getname x)==(getname s1))m)/=0 then (getp s1)-getp((filter(\x->(getname x) == (getname s1))m)!!0) else (getp s1)
isdone :: [(String,Double)]->Recipe->Bool
isdone _ (Recipe st []) = True
isdone s (Recipe st m ) = a && (isdone s (Recipe st (tail m)))
    where a = if length(filter(\x->(getname x)==(getname (head m))&&(getp x)>=(getp (head m)))s)/=0 then True else False
----------------------------------------------------------------------------------------------------------------------------------------------------------
-- Given a list of ingredients you already have, and a list of recipes,
-- make a shopping list showing how much of each ingredient you need
-- to buy, and its cost. Each ingredient mush appear in the shopping list
-- at most once (no duplicates!).
makelisttogether :: [Recipe]->[(String,Double)]
makelisttogether [] = []
makelisttogether re = makelisttogether (tail(re)) ++ (getrecipelist (head(re)))
updatestock :: [(String,Double)]->[(String,Double)]->[(String,Double)]
updatestock l [] = l
updatestock st re = updatestock ust (tail re)
    where ust = if length(filter(\x->getname x == getname (head re)) st)/=0 then map(\x->(updatestockhelp x (head re))) st else st++[(getname(head re),-1*getp(head re))]
updatestockhelp :: (String,Double) -> (String,Double) -> (String,Double)
updatestockhelp (a,b) (c,d) = if a == c then (a,b-d) else (a,b)
makeShoppingList :: [(String, Double)] -> [Recipe] -> [Price] -> [(String, Double, Double)]
makeShoppingList sto rec pri = mslhelp (updatestock sto (makelisttogether rec)) pri 
mslhelp :: [(String, Double)] -> [Price] -> [(String, Double, Double)]
mslhelp [] _ = []
mslhelp sto pri = l++(mslhelp (tail sto) pri)
    where l = if getp (head sto)<0 then [(getname (head sto),getRounded(-1*(getp(head sto))),getRounded(getprice((filter(\x->getpricename x == getname(head sto))pri)!!0)*(-1*getp(head sto))))] else []
