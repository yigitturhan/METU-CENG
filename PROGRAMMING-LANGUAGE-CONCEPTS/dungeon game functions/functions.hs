-- Dungeon map is :: Tree Chamber [Encounter]
-- Each encounter is either a fight or a treasure
-- Fights deal you damage (reduce HP) but enemies drop some gold (add
-- gold)
-- Tresures just give gold, or potions (which give hp)
-- Nodes hold encounters, when you visit a node you go through all of them in order
-- You start with a certain amount of HP and 0 gold.
-- You lose HP and accumulate gold as you descend the tree and go through encounters

get_first (a,b) = a
get_second (a,b) = b
findindex e l = if (head l)==e then 0 else 1+findindex e (tail l)
swap (a,b) = (b,a)
max_of_list l = if length l == 1 then l!!0 else max (head l) (max_of_list(tail l))
sum_of_two_max l = if length(filter(\x->x==(max_of_list l))l)==1 then (max_of_list(filter(\x->x/=(max_of_list l))l)) + max_of_list l else 2*max_of_list l
sum_of_list [] = 0
sum_of_list l = (l!!0) + sum_of_list(tail l)

-- Polymorphic tree structure
data Tree a b = EmptyTree | Leaf a b | Node a b [Tree a b] deriving (Show, Eq)
get_tree_childs (Node a b l) = l
get_tree_childs (Leaf a b) = []
get_tree_childs (EmptyTree) = []
get_tree_encounters (Node a b l) = b
get_tree_encounters (Leaf a b) = b
get_tree_encounters (EmptyTree) = []

-- Every location in the tree is of some Chamber type.
data Chamber = Cavern |
               NarrowPassage |
               UndergroundRiver |
               SlipperyRocks deriving (Show, Eq)
               
-- An enemy has a name, an amount of damage that it deals
-- and an amount of gold that it drops (in that order).
data Enemy = Enemy String Integer Integer deriving (Show, Eq)
get_enemy_damage (Enemy s a b) = a
get_enemy_loot (Enemy s a b) = b

-- Gold n gives n amount of gold
-- Potion n heals n hp
data Loot = Gold Integer | Potion Integer deriving (Show, Eq)

-- An encounter is either a Fight with an Enemy, or a treasure where
-- you find Loot
data Encounter = Fight Enemy | Treasure Loot deriving (Show, Eq)

-- This is a type synonym for how we will represents our dungeons
type Dungeon = Tree Chamber [Encounter]


-- First argument is starting HP
-- Second argument is the dungeon map
-- Third argument is the path (each integer in the list shows what child
-- you descend into)
-- Calculate how much HP you have left and how much gold you've
-- accumulated after traversing the given path
traversePath :: Integer -> Dungeon -> [Int] -> (Integer, Integer)
traversePath h d l = swap(traverse_path_helper (0,h) l d)
traverse_path_helper (g,h) [] (Node a b p) = calculate_gold_health b g h
traverse_path_helper (g,h) l (Leaf a b) = calculate_gold_health b g h
traverse_path_helper (g,h) l (Node a b p) = traverse_path_helper (calculate_gold_health b g h) (tail l) (p!!(head l))
calculate_gold_health_helper (Fight (Enemy a b c)) g h = (g+c,h-b)
calculate_gold_health_helper (Treasure (Gold a)) g h = (g+a,h)
calculate_gold_health_helper (Treasure (Potion a)) g h = (g,h+a)
calculate_gold_health [] g h = (g,h)
calculate_gold_health x g h = calculate_gold_health (tail x) (get_first(calculate_gold_health_helper (head x) g h)) (get_second(calculate_gold_health_helper (head x) g h))
----------------------------------------------------------------------------------------------------------------------------------------------------------
-- First argument is starting HP
-- Second argument is dungeon map
-- Find which path down the tree yields the most gold for you
-- You cannot turn back, i.e. you'll find a non-branching path
-- You do not need to reach the bottom of the tree
-- Return how much gold you've accumulated
findMaximumGain :: Integer -> Dungeon -> Integer
findMaximumGain h d = find_max_gain_helper (0,h) d
find_max_gain_helper_v2 (g,h) []  = g
find_max_gain_helper_v2 (g,h) l  = max (find_max_gain_helper (g,h) (head l)) (find_max_gain_helper_v2 (g,h) (tail l))
find_max_gain_helper (g,h) (Leaf a b) = if get_second(calculate_gold_health b g h)>0 then get_first(calculate_gold_health b g h) else g
find_max_gain_helper (g,h) (Node a b l) =  if get_second(calculate_gold_health b g h)>0 then find_max_gain_helper_v2 (calculate_gold_health b g h) l else g
----------------------------------------------------------------------------------------------------------------------------------------------------------
-- First argument is starting HP
-- Second argument is the dungeon map
-- Remove paths that you cannot go thorugh with your starting HP. (By
-- removing nodes from tree).
-- Some internal nodes may become leafs during this process, make the
-- necessary changes in such a case.
does_et (Node a b l) = True
does_et (Leaf a b) = True
does_et (EmptyTree) = False
findViablePaths :: Integer -> Dungeon -> Dungeon
findViablePaths hp (Node a b l) = fvp_helper_v3(Node a b (fvp_helper_v2 l hp))
fvp_helper_v3_v1 l = map(\x->fvp_helper_v3 x)l
fvp_helper_v3 (Leaf a b) = (Leaf a b)
fvp_helper_v3 (Node a b l) = if length(filter(\x->does_et x)l)>0 then (Node a b (fvp_helper_v3_v1(filter(\x->does_et x)l))) else (Leaf a b)
fvp_helper_v2 l hp =  map(\x->fvp_helper_v1 x hp)l
fvp_helper_v1 (EmptyTree) hp = EmptyTree
fvp_helper_v1 (Leaf a b) hp = if get_second(calculate_gold_health b 0 hp)>0 then (Leaf a b) else EmptyTree
fvp_helper_v1 (Node a b l) hp = if get_second (calculate_gold_health b 0 hp)>0 then xs else EmptyTree
    where xs = if length(filter(\x->get_second(calculate_gold_health(get_tree_encounters x) 0 hp)>0)l)>0 then (Node a b (fvp_helper_v2 l (get_second(calculate_gold_health b 0 hp)))) else  (Leaf a b)
----------------------------------------------------------------------------------------------------------------------------------------------------------
-- First argument is starting HP
-- Second Argument is dungeon map
-- Find, among the viable paths in the tree (so the nodes you cannot
-- visit is already removed) the two most distant nodes, i.e. the two
-- nodes that are furthest awat from each other.
mostDistantPair :: Integer -> Dungeon -> (Integer, Dungeon)
mostDistantPair hp EmptyTree = (0,EmptyTree)
mostDistantPair hp (Leaf a b) = (0,(Leaf a b))
mostDistantPair hp (Node a b l) = (max(mdphelper1(findViablePaths hp (Node a b l))) (max_depth(Node a b l)),(findViablePaths hp (Node a b l)))
mdphelper1 (Node a b l) = if length l == 1 then mdphelper1(l!!0) else  2+sum_of_two_max(map(\x->max_depth x)l)
max_depth (Leaf a b) = 0
max_depth (Node a b l) = if length l == 0 then 0 else 1+max_depth_helper l
max_depth_helper [] = 0
max_depth_helper l = max (max_depth(head l)) (max_depth_helper(tail l))
----------------------------------------------------------------------------------------------------------------------------------------------------------
-- Find the subtree that has the highest total gold/damage ratio
-- Simply divide the total gold in the subtree by the total damage
-- in the subtree. You only take whole subtrees (i.e you can take a new
-- node as the root of your subtree, but you cannot remove nodes
-- below it). Note that the answer may be the whole tree.
mostEfficientSubtree :: Dungeon -> Dungeon
mostEfficientSubtree EmptyTree = EmptyTree
mostEfficientSubtree (Leaf a b) = (Leaf a b)
mostEfficientSubtree (Node a b l) = if check_negative_bool(Node a b l) then check_negative(Node a b l) else choose(Node a b l)
hp_cost_calculator (Leaf a b) = sum_of_list(map(\x->hpc_helper x)b)
hp_cost_calculator (Node a b l) = sum_of_list(map(\x->hpc_helper x)b)+sum_of_list(map(\x->hp_cost_calculator x)l)
hpc_helper (Treasure (Gold a)) = 0
hpc_helper (Treasure (Potion a)) = -a
hpc_helper (Fight (Enemy a b c)) = b
gold_calculator (Leaf a b) = sum_of_list(map(\x->gc_helper x)b)
gold_calculator (Node a b l) = sum_of_list(map(\x->gc_helper x)b)+sum_of_list(map(\x->gold_calculator x)l)
gc_helper (Treasure (Gold a)) = a
gc_helper (Treasure (Potion a)) = 0
gc_helper (Fight (Enemy a b c)) = c
choose (Leaf a b) = (Leaf a b)
choose (Node a b l) = if (efficiency_calculator(Node a b l) >= max_of_list(map(\x -> efficiency_calculator x)l)) then (Node a b l) else choose(l!!(findindex(max_of_list(map(\x -> efficiency_calculator x)l)) ((map(\x -> efficiency_calculator x)l))))
efficiency_calculator t = fromIntegral(gold_calculator t)/ fromIntegral(hp_cost_calculator t)
check_negative_bool (Leaf a b) = if hp_cost_calculator(Leaf a b)<=0 then True else False
check_negative_bool (Node a b l) = if (hp_cost_calculator(Node a b l) <=0) then True else contains_true(map(\x->check_negative_bool x)l)
contains_true l = if (length l)==0 then False else (head l)||(contains_true(tail l))
check_negative (Leaf a b) = (Leaf a b)
check_negative (Node a b l) = if hp_cost_calculator(Node a b l)<=0 then (Node a b l) else check_negative((filter(\x->check_negative_bool x)l)!!0)
