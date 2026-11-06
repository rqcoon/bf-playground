module Main where
    import Lexer
    import Parser

    import Control.Monad (when)
    import Control.Monad.IO.Class (liftIO)
    import Control.Monad.State.Strict
    import Data.Word (Word8)
    import qualified Data.IntMap.Strict as IM
    import Data.Char (ord, chr)
    import System.Environment (getArgs)
    import System.IO (isEOF)
    
    tapeSize :: Int
    tapeSize = 67000 -- ha ha 67

    data Machine = Machine{ 
        memory  :: IM.IntMap Word8,
        pointer :: Int
    }

    initialMachine :: Machine
    initialMachine = Machine{
        memory  = IM.empty,
        pointer = 0
    }

    currentCell :: StateT Machine IO Word8
    currentCell = do
        machine <- get
        pure $ IM.findWithDefault 0 (pointer machine) (memory machine)

    writeCell :: Word8 -> StateT Machine IO ()
    writeCell value = do
        machine <- get

        let ptr = pointer machine
            updatedMemory
                | value == 0 = IM.delete ptr (memory machine)
                | otherwise  = IM.insert ptr value (memory machine)
            
        put machine { memory = updatedMemory }

    executeProgram :: Program -> StateT Machine IO ()
    executeProgram = mapM_ executeInstruction

    executeInstruction :: Instruction -> StateT Machine IO ()

    executeInstruction (Move delta) = do
        machine <- get
        let next = pointer machine + delta

        when (next < 0 || next >= tapeSize) $ liftIO $ ioError $ userError "Data pointer out of bounds"
        modify' $ \m -> m { pointer = next }
    
    executeInstruction (Change delta) = do
        value <- currentCell
        writeCell (value + fromIntegral delta)
    
    executeInstruction Print = do
        value <- currentCell
        liftIO $ putChar (chr (fromIntegral value))

    executeInstruction Read = do
        eof <- liftIO isEOF
        if eof then writeCell 0
        else do
            character <- liftIO getChar
            writeCell (fromIntegral (ord character))

    executeInstruction (Loop body) = loop
        where
            loop = do
                value <- currentCell 
                when (value /= 0) $ do
                    executeProgram body
                    loop

    main :: IO ()
    main = do
        args <- getArgs

        case args of
            [file] -> do
                source <- readFile file
                case parse (lexer source) of
                    Left err -> putStrLn ("Parse error: " ++ err)
                    Right program -> do
                        _ <- runStateT (executeProgram program) initialMachine
                        pure ()
            _ -> putStrLn("your stupit idiot (usage: brainfuck <file>)")