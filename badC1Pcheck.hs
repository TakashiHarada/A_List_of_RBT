import Data.List
import Data.Map as Map

isC1P :: [String] -> Bool
isC1P xss = (or . Prelude.map strongC1P . Prelude.map transpose . permutations . transpose) xss

strongC1P :: [String] -> Bool
strongC1P xss = (and . Prelude.map oneConsecutive) xss

oneConsecutive :: String -> Bool
oneConsecutive xs = 3 > fst (Prelude.foldr (\x (i,y) -> if x /= y then (i+1,x) else (i,y)) (0,'0') xs)

non_c1pList = nub $ m_I ++ m_II ++ m_III ++ m_IV ++ m_V
mapC1Plist = Map.fromList $ zip [1..] non_c1pList

ll = [ "110000",  -- 1
       "011000",  -- 2
       "101000",  -- 3
       "011100",  -- 4
       "110100",  -- 5
       "010100",  -- 6
       "001100",  -- 7
       "000011",  -- 8
       "101010",  -- 9
       "111100" ] -- 10

-- 1, 2, 3  
m_I   = [ "110000",
          "011000",
          "101000" ]

-- 1, 2, 4, 5
m_II  = [ "110000",
          "011000",
          "011100",
          "110100" ]

m_III = [ "110000",
          "011000",
          "010100" ]

m_IV  = [ "110000",
          "001100",
          "000011",
          "101010" ]
  
m_V   = [ "110000",
          "001100",
          "111100",
          "101010" ]
