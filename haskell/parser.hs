module Parser where

    import Lexer
    
    data Instruction
        = Move Int
        | Change Int
        | Print
        | Read
        | Loop[Instruction]
        deriving (Show, Eq)

    type Program = [Instruction]

    parse :: [Token] -> Either String Program
    parse tokens = do
        (program, remaining) <- parseBody tokens

        case remaining of
            [] -> Right program
            _  -> Left  "Unexpected closing bracket"
        
    parseBody :: [Token] -> Either String (Program, [Token])
    parseBody [] = Right ([], [])
    parseBody (LoopEnd:tokens) = Right ([], LoopEnd:tokens)
    parseBody (token:tokens) = do
        (instruction, remaining) <- case token of
            MoveRight -> Right (Move 1, tokens)
            MoveLeft  -> Right (Move (-1), tokens)
            Increment -> Right (Change 1, tokens)
            Decrement -> Right (Change (-1), tokens)
            Input     -> Right (Read, tokens)
            Output    -> Right (Print, tokens)

            LoopStart -> do
                (body, remaining) <- parseBody tokens

                case remaining of
                    LoopEnd:rest -> Right (Loop body, rest)
                    _            -> Left "Unmatched open"

        (program, rest) <- parseBody remaining
        Right (instruction : program, rest)