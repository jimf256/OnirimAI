import random
from parse_game_state import ParseGameState, GetColorByte

debug_printing = False

def DebugPrint(s):
    if debug_printing:
        print(s)


# called before the first turn of the game
def HandleGameStarted(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: game started:\n{state}')
    return None


# called when the game has ended
def HandleGameEnded(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: game ended:\n{state}')
    return None


# resolve a turn action
# must return a bytearray of length 2
# first byte is a value 0/1: play card/discard card
# second byte is a value 0-4: hand card index
def HandleResolveTurn(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: resolve turn:\n{state}')
    # discard first card in hand
    return bytearray([1, 0])


# resolve a nightmare action
# must return a bytearray of length 2
# first byte is a value 0-3: destroy a door/discard a key/discard hand/discard from deck
# second byte is a value 0-4: hand card index
def HandleResolveNightmare(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: resolve nightmare:\n{state}')
    # discard hand
    return bytearray([2, 0])


# resolve a door card action
# must return a bytearray of length 1
# first byte is a value 0/1: use key/keep key
def HandleResolveDoorCard(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: resolve door card:\n{state}')
    # use key
    return bytearray([0])


# resolve a premonition action
# must return a bytearray of length len(state.premonition)
# state.premonition contains 0 to 5 cards which are the cards on the top of the deck
# the returned values are card indexes, from 0 to 4, representing the new ordering of those cards
# for example, to reverse the order of the cards, return [4,3,2,1,0]
# the last card in the list is the one that will be discarded
def HandleResolvePremonition(data):
    state = ParseGameState(data)
    DebugPrint(f'\npython AI: resolve premonition:\n{state}')
    # keep original order
    return bytearray(list(range(len(state.premonition))))
