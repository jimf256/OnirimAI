from parse_game_state import ParseGameState

def HandleGameStarted(data):
    state = ParseGameState(data)
    #print(f'python AI: game started:\n{state}')
    return None

def HandleResolveTurn(data):
    state = ParseGameState(data)
    #print(f'python AI: resolve turn:\n{state}')
    return None

def HandleResolveNightmare(data):
    state = ParseGameState(data)
    #print(f'python AI: resolve nightmare:\n{state}')
    return None

def HandleResolveDoorCard(data):
    state = ParseGameState(data)
    #print(f'python AI: resolve door card:\n{state}')
    return None

def HandleResolvePremonition(data):
    state = ParseGameState(data)
    #print(f'python AI: resolve premonition:\n{state}')
    return None

def HandleGameEnded(data):
    state = ParseGameState(data)
    #print(f'python AI: game ended:\n{state}')
    return None
