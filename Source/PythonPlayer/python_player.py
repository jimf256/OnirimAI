import random
from parse_game_state import ParseGameState, GetColorByte

debug_printing = False

def DebugPrint(s):
    if debug_printing:
        print(s)

def HandleGameStarted(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: game started:\n{state}')
    return None

def HandleGameEnded(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: game ended:\n{state}')
    return None


# resolve a turn action
# must return a bytearray of length 2
# first byte is a value 0/1: play card/discard card
# second byte is a value 0-4: hand card index
def HandleResolveTurn(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: resolve turn:\n{state}')

    # pick action and card at random, however if we pick play we must make sure the card we
    # choose doesn't match the last card in the labrynth.  If we can't play anything valid
    # just discard a random card
    choice = random.randrange(0, 2)
    index = random.randrange(0, 5)
    if choice == 0:
        if state.labrynth:
            suitable_card = None
            for i,card in enumerate(state.hand):
                if card[1] != state.labrynth[-1][1]:
                    suitable_card = card
                    index = i
                    break
            if suitable_card is None:
                choice = 1
                index = random.randrange(0, 5)            

    return bytearray([choice, index])


# resolve a nightmare action
# must return a bytearray of length 2
# first byte is a value 0-3: destroy a door/discard a key/discard hand/discard from deck
# second byte is a value 0-4: hand card index
def HandleResolveNightmare(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: resolve nightmare:\n{state}')

    # if we have a key in hand, discard it, otherwise discard the hand
    key_color = None
    for card in state.hand:
        if card[1] == 'K':
            choice = 1
            key_color = GetColorByte(card[0])
            break

    if key_color is None:
        choice = 2
        key_color = 0

    return bytearray([choice, key_color])


# resolve a door card action
# must return a bytearray of length 1
# first byte is a value 0/1: use key/keep key
def HandleResolveDoorCard(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: resolve door card:\n{state}')

    # always use a key
    choice = 0;
    return bytearray([choice])


# resolve a premonition action
# must return a bytearray of length len(state.premonition)
# state.premonition contains 0 to 5 cards which are the cards on the top of the deck
# the returned values are card indexes, from 0 to 4, representing the new ordering of those cards
# for example, to reverse the order of the cards, return [4,3,2,1,0]
# the last card in the list is the one that will be discarded
def HandleResolvePremonition(data):
    state = ParseGameState(data)
    DebugPrint(f'python AI: resolve premonition:\n{state}')

    # prioritize keys, then doors, but avoid discarding doors
    keys = []
    doors = []
    nightmares = []
    others = []
    for i,card in enumerate(state.premonition):
        if card[1] is 'K':
            keys.append((card, i))
        elif card[1] is 'D':
            doors.append((card, i))
        elif card == 'NM':
            nightmares.append((card, i))
        else:
            others.append((card, i))

    order = []
    if len(doors) == len(state.premonition) or len(keys) == len(state.premonition):
        order = list(range(len(state.premonition)))
    elif len(doors) + len(keys) == len(state.premonition):
        order = [x[1] for x in keys[1:]] + [x[1] for x in doors] + [keys[0][1]]
    elif len(nightmares) > 0:
        order = [x[1] for x in keys] + [x[1] for x in others] + [x[1] for x in doors] + [x[1] for x in nightmares]
    else:
        order = [x[1] for x in keys] + [x[1] for x in others[1:]] + [x[1] for x in doors] + [others[0][1]]

    DebugPrint(order)
    DebugPrint([state.premonition[x] for x in order])
    return bytearray(order)
