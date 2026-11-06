module Lexer where

data Token
    = MoveRight
    | MoveLeft
    | Increment
    | Decrement
    | Output
    | Input
    | LoopStart
    | LoopEnd
    deriving (Show, Eq)

tokenize :: Char -> Maybe Token
tokenize '>' = Just MoveRight
tokenize '<' = Just MoveLeft
tokenize '+' = Just Increment
tokenize '-' = Just Decrement
tokenize '.' = Just Output
tokenize ',' = Just Input
tokenize '[' = Just LoopStart
tokenize ']' = Just LoopEnd
tokenize _   = Nothing

lexer :: String -> [Token]
lexer source = [token | character <- source, Just token <- [tokenize character]]